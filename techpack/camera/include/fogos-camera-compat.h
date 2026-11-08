/* SPDX-License-Identifier: GPL-2.0-only */
#ifndef FOGOS_CAMERA_COMPAT_H
#define FOGOS_CAMERA_COMPAT_H
#include <linux/gpio.h>
#include <linux/of_gpio.h>
struct cam_gpio {
	unsigned gpio;
	unsigned long flags;
	const char *label;
};
static inline void cam_gpio_free_array(const struct cam_gpio *array, size_t num)
{
	while (num--)
		gpio_free(array[num].gpio);
}
#endif
