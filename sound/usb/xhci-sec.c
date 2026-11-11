// SPDX-License-Identifier: GPL-2.0
/*
 * xHCI secondary ring APIs
 *
 * Copyright (c) 2019,2021 The Linux Foundation. All rights reserved.
 * Copyright (C) 2008 Intel Corp.
 * Copyright (c) 2022, Qualcomm Innovation Center, Inc. All rights reserved.
 */

#include <linux/iopoll.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/slab.h>
#include <linux/dma-mapping.h>
#include <linux/dmapool.h>

#include "xhci.h"

/* Let the 6.18 xHCI core own ring allocation, registers and teardown. */
struct xhci_sec {
	struct xhci_interrupter *ir;
	struct xhci_hcd *xhci;
	struct list_head list;
};

static LIST_HEAD(xhci_sec);
static DEFINE_MUTEX(xhci_sec_lock);

struct xhci_ring *xhci_sec_event_ring_setup(struct usb_device *udev,
					  unsigned int intr_num)
{
	struct usb_hcd *hcd = bus_to_hcd(udev->bus);
	struct xhci_hcd *xhci = hcd_to_xhci(hcd);
	struct xhci_sec *sec;
	struct xhci_ring *ring;

	if (udev->state == USB_STATE_NOTATTACHED || !HCD_RH_RUNNING(hcd))
		return ERR_PTR(-ENODEV);
	/* Firmware chooses the interrupter; zero is the primary ring. */
	if (!intr_num || intr_num >= xhci->max_interrupters) {
		xhci_err(xhci, "audio interrupter %u outside allocated count %u\n",
			 intr_num, xhci->max_interrupters);
		return ERR_PTR(-EINVAL);
	}
	if (xhci->xhc_state & XHCI_STATE_HALTED)
		return ERR_PTR(-ENODEV);

	mutex_lock(&xhci_sec_lock);
	list_for_each_entry(sec, &xhci_sec, list) {
		if (sec->xhci == xhci && sec->ir->intr_num == intr_num) {
			ring = sec->ir->event_ring;
			goto out;
		}
	}
	sec = kzalloc(sizeof(*sec), GFP_KERNEL);
	if (!sec) {
		ring = ERR_PTR(-ENOMEM);
		goto out;
	}
	sec->ir = xhci_create_secondary_interrupter(hcd, 1, 0, intr_num);
	if (!sec->ir) {
		kfree(sec);
		ring = ERR_PTR(-EBUSY);
		goto out;
	}
	sec->xhci = xhci;
	list_add_tail(&sec->list, &xhci_sec);
	ring = sec->ir->event_ring;
out:
	mutex_unlock(&xhci_sec_lock);
	return ring;
}

int xhci_sec_event_ring_cleanup(struct usb_device *udev, struct xhci_ring *ring)
{
	struct usb_hcd *hcd = bus_to_hcd(udev->bus);
	struct xhci_sec *sec;

	mutex_lock(&xhci_sec_lock);
	list_for_each_entry(sec, &xhci_sec, list) {
		if (sec->xhci == hcd_to_xhci(hcd) && sec->ir->event_ring == ring) {
			xhci_remove_secondary_interrupter(hcd, sec->ir);
			list_del(&sec->list);
			kfree(sec);
			break;
		}
	}
	mutex_unlock(&xhci_sec_lock);
	return 0;
}

phys_addr_t xhci_get_sec_event_ring_phys_addr(struct usb_device *udev,
	struct xhci_ring *ring, dma_addr_t *dma)
{
	struct usb_hcd *hcd = bus_to_hcd(udev->bus);
	struct xhci_hcd *xhci = hcd_to_xhci(hcd);
	struct device *dev = hcd->self.sysdev;
	struct sg_table sgt;
	phys_addr_t pa = 0;
	struct xhci_sec *sec;

	if (udev->state == USB_STATE_NOTATTACHED || !HCD_RH_RUNNING(hcd) ||
			(xhci->xhc_state & XHCI_STATE_HALTED))
		return 0;

	mutex_lock(&xhci_sec_lock);
	list_for_each_entry(sec, &xhci_sec, list) {
		if (sec->xhci == xhci && sec->ir->event_ring == ring) {
			if (dma_get_sgtable(dev, &sgt, ring->first_seg->trbs,
				ring->first_seg->dma, TRB_SEGMENT_SIZE))
				break;

			*dma = ring->first_seg->dma;

			pa = page_to_phys(sg_page(sgt.sgl));
			sg_free_table(&sgt);

			break;
		}
	}

	mutex_unlock(&xhci_sec_lock);
	return pa;
}

/* Returns 1 if the arguments are OK;
 * returns 0 this is a root hub; returns -EINVAL for NULL pointers.
 */
static int xhci_check_args(struct usb_hcd *hcd, struct usb_device *udev,
		struct usb_host_endpoint *ep, int check_ep, bool check_virt_dev,
		const char *func)
{
	struct xhci_hcd	*xhci;
	struct xhci_virt_device	*virt_dev;

	if (!hcd || (check_ep && !ep) || !udev) {
		pr_debug("xHCI %s called with invalid args\n", func);
		return -EINVAL;
	}
	if (!udev->parent) {
		pr_debug("xHCI %s called for root hub\n", func);
		return 0;
	}

	xhci = hcd_to_xhci(hcd);
	if (check_virt_dev) {
		if (!udev->slot_id || !xhci->devs[udev->slot_id]) {
			xhci_dbg(xhci, "xHCI %s called with unaddressed device\n",
					func);
			return -EINVAL;
		}

		virt_dev = xhci->devs[udev->slot_id];
		if (virt_dev->udev != udev) {
			xhci_dbg(xhci, "xHCI %s called with udev and virt_dev does not match\n",
					func);
			return -EINVAL;
		}
	}

	if (xhci->xhc_state & XHCI_STATE_HALTED)
		return -ENODEV;

	return 1;
}

phys_addr_t xhci_get_xfer_ring_phys_addr(struct usb_device *udev,
		struct usb_host_endpoint *ep, dma_addr_t *dma)
{
	int ret;
	unsigned int ep_index;
	struct xhci_virt_device *virt_dev;
	struct usb_hcd *hcd = bus_to_hcd(udev->bus);
	struct device *dev = hcd->self.sysdev;
	struct xhci_hcd *xhci = hcd_to_xhci(hcd);
	struct sg_table sgt;
	phys_addr_t pa;

	if (udev->state == USB_STATE_NOTATTACHED || !HCD_RH_RUNNING(hcd))
		return 0;

	ret = xhci_check_args(hcd, udev, ep, 1, true, __func__);
	if (ret <= 0) {
		xhci_err(xhci, "%s: invalid args\n", __func__);
		return 0;
	}

	virt_dev = xhci->devs[udev->slot_id];
	ep_index = xhci_get_endpoint_index(&ep->desc);

	if (virt_dev->eps[ep_index].ring &&
		virt_dev->eps[ep_index].ring->first_seg) {

		if (dma_get_sgtable(dev, &sgt,
			virt_dev->eps[ep_index].ring->first_seg->trbs,
			virt_dev->eps[ep_index].ring->first_seg->dma,
			TRB_SEGMENT_SIZE))
			return 0;

		*dma = virt_dev->eps[ep_index].ring->first_seg->dma;

		pa = page_to_phys(sg_page(sgt.sgl));
		sg_free_table(&sgt);

		return pa;
	}

	return 0;
}

/* Use the core's synchronous stop, including command timeout handling. */
int xhci_stop_endpoint(struct usb_device *udev, struct usb_host_endpoint *ep)
{
	struct usb_hcd *hcd = bus_to_hcd(udev->bus);
	struct xhci_hcd *xhci = hcd_to_xhci(hcd);
	unsigned int ep_index;
	int ret;

	ret = xhci_check_args(hcd, udev, ep, 1, true, __func__);
	if (ret <= 0)
		return ret;
	ep_index = xhci_get_endpoint_index(&ep->desc);
	return xhci_stop_endpoint_sync(xhci, &xhci->devs[udev->slot_id]->eps[ep_index],
				       0, GFP_NOIO);
}
