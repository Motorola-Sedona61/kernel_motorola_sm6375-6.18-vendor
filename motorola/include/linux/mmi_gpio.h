/* SPDX-License-Identifier: GPL-2.0-only */
#ifndef _LINUX_MMI_GPIO_H
#define _LINUX_MMI_GPIO_H

#include <linux/device.h>
#include <linux/gpio.h>

/* Preserve the legacy Motorola DT GPIO names with managed ownership. */
static inline void mmi_gpio_release(void *data)
{
	gpio_free((unsigned long)data);
}

static inline int mmi_devm_gpio_request(struct device *dev, unsigned int gpio,
				      const char *label)
{
	int ret = gpio_request(gpio, label);

	if (ret)
		return ret;
	return devm_add_action_or_reset(dev, mmi_gpio_release,
				       (void *)(unsigned long)gpio);
}

static inline void mmi_devm_gpio_free(struct device *dev, unsigned int gpio)
{
	devm_release_action(dev, mmi_gpio_release, (void *)(unsigned long)gpio);
}

#endif
