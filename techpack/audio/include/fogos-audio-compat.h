/* SPDX-License-Identifier: GPL-2.0-only */
#ifndef _FOGOS_AUDIO_COMPAT_H
#define _FOGOS_AUDIO_COMPAT_H
#include <sound/soc.h>
/* Qualcomm mixer controls retained from the working 6.1 APR stack. */
struct soc_multi_mixer_control {
	int min, max, platform_max, count;
	unsigned int reg, rreg, shift, rshift, invert;
};
static inline int fogos_soc_info_multi_ext(struct snd_kcontrol *kcontrol,
	struct snd_ctl_elem_info *uinfo)
{
	struct soc_multi_mixer_control *mc =
		(struct soc_multi_mixer_control *)kcontrol->private_value;
	int platform_max;

	if (!mc->platform_max)
		mc->platform_max = mc->max;
	platform_max = mc->platform_max;

	if (platform_max == 1 && !strnstr(kcontrol->id.name, " Volume", 30))
		uinfo->type = SNDRV_CTL_ELEM_TYPE_BOOLEAN;
	else
		uinfo->type = SNDRV_CTL_ELEM_TYPE_INTEGER;

	uinfo->count = mc->count;
	uinfo->value.integer.min = 0;
	uinfo->value.integer.max = platform_max;
	return 0;
}

#define SOC_SINGLE_MULTI_EXT(xname, xreg, xshift, xmax, xinvert, xcount,\
	xhandler_get, xhandler_put) \
{	.iface = SNDRV_CTL_ELEM_IFACE_MIXER, .name = xname, \
	.info = fogos_soc_info_multi_ext, \
	.get = xhandler_get, .put = xhandler_put, \
	.private_value = (unsigned long)&(struct soc_multi_mixer_control) \
		{.reg = xreg, .shift = xshift, .rshift = xshift, .max = xmax, \
		.count = xcount, .platform_max = xmax, .invert = xinvert} }

/* Preserve copy_user's uncopied-byte return convention with iov_iter. */
#include <linux/uio.h>
static inline unsigned long fogos_audio_copy_from_iter(void *dest,
		struct iov_iter *iter, unsigned long bytes)
{
	return bytes - copy_from_iter(dest, bytes, iter);
}

static inline unsigned long fogos_audio_copy_to_iter(struct iov_iter *iter,
		const void *src, unsigned long bytes)
{
	return bytes - copy_to_iter(src, bytes, iter);
}

#define SND_JACK_OC_HPHL BIT(16)
#define SND_JACK_OC_HPHR BIT(17)
#define SND_JACK_UNSUPPORTED BIT(18)

#endif
