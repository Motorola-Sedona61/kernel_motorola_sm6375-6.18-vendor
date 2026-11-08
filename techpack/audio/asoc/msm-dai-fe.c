// SPDX-License-Identifier: GPL-2.0-only
/* Copyright (c) 2012-2021, The Linux Foundation. All rights reserved.
 * Copyright (c) 2022 Qualcomm Innovation Center, Inc. All rights reserved.
 */


#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/platform_device.h>
#include <linux/of_device.h>
#include <sound/core.h>
#include <sound/pcm.h>
#include <sound/soc.h>
#include <fogos-audio-compat.h>

#include "msm-compress-q6-v2.h"

#include "msm-dai-fe.routes.h"

static int fe_dai_probe(struct snd_soc_dai *dai);

static struct snd_soc_dai_ops msm_fe_dai_ops = {
	.probe = fe_dai_probe,
};

/* Conventional and unconventional sample rate supported */
static unsigned int supported_sample_rates[] = {
	8000, 11025, 12000, 16000, 22050, 24000, 32000, 44100, 48000,
	88200, 96000, 176400, 192000, 352800, 384000
};

static struct snd_pcm_hw_constraint_list constraints_sample_rates = {
	.count = ARRAY_SIZE(supported_sample_rates),
	.list = supported_sample_rates,
	.mask = 0,
};

static int multimedia_startup(struct snd_pcm_substream *substream,
	struct snd_soc_dai *dai)
{
	snd_pcm_hw_constraint_list(substream->runtime, 0,
		SNDRV_PCM_HW_PARAM_RATE,
		&constraints_sample_rates);
	return 0;
}

static int fe_dai_probe(struct snd_soc_dai *dai)
{
	struct snd_soc_dapm_route intercon;
	struct snd_soc_dapm_context *dapm;

	if (!dai || !dai->driver) {
		pr_err("%s invalid params\n", __func__);
		return -EINVAL;
	}
	dapm = snd_soc_component_get_dapm(dai->component);
	memset(&intercon, 0, sizeof(intercon));
	if (dai->driver->playback.stream_name &&
		fogos_audio_aif_name(&dai->driver->playback)) {
		dev_dbg(dai->dev, "%s add route for widget %s",
			   __func__, dai->driver->playback.stream_name);
		intercon.source = dai->driver->playback.stream_name;
		intercon.sink = fogos_audio_aif_name(&dai->driver->playback);
		dev_dbg(dai->dev, "%s src %s sink %s\n",
			   __func__, intercon.source, intercon.sink);
		snd_soc_dapm_add_routes(dapm, &intercon, 1);
		snd_soc_dapm_ignore_suspend(dapm, intercon.source);
	}
	if (dai->driver->capture.stream_name &&
	   fogos_audio_aif_name(&dai->driver->capture)) {
		dev_dbg(dai->dev, "%s add route for widget %s",
			   __func__, dai->driver->capture.stream_name);
		intercon.sink = dai->driver->capture.stream_name;
		intercon.source = fogos_audio_aif_name(&dai->driver->capture);
		dev_dbg(dai->dev, "%s src %s sink %s\n",
			   __func__, intercon.source, intercon.sink);
		snd_soc_dapm_add_routes(dapm, &intercon, 1);
		snd_soc_dapm_ignore_suspend(dapm, intercon.sink);
	}
	return 0;
}

static int fe_dai_probe(struct snd_soc_dai *dai);

static struct snd_soc_dai_ops msm_fe_Multimedia_dai_ops = {
	.probe = fe_dai_probe,
	.startup	= multimedia_startup,
};

static const struct snd_soc_component_driver msm_fe_dai_component = {
	/* Linux 6.1: keep the 5.4 default single-DAI naming */
	.legacy_dai_naming = 1,
	.name		= "msm-dai-fe",
};

static const struct snd_soc_dai_ops msm_fe_Multimedia_dai_ops_compressed = {
	.compress_new = msm_compr_new,
	.probe = fe_dai_probe,
	.startup	= multimedia_startup,
};

static struct snd_soc_dai_driver msm_fe_dais[] = {
	{
		.playback = {
			.stream_name = "MultiMedia1 Playback",

			.rates = (SNDRV_PCM_RATE_8000_384000|
					SNDRV_PCM_RATE_KNOT),
			.formats = (SNDRV_PCM_FMTBIT_S16_LE |
						SNDRV_PCM_FMTBIT_S24_LE |
						SNDRV_PCM_FMTBIT_S24_3LE |
						SNDRV_PCM_FMTBIT_S32_LE),
			.channels_min = 1,
			.channels_max = 32,
			.rate_min =     8000,
			.rate_max =	384000,
		},
		.capture = {
			.stream_name = "MultiMedia1 Capture",

			.rates = (SNDRV_PCM_RATE_8000_384000|
					SNDRV_PCM_RATE_KNOT),
			.formats = (SNDRV_PCM_FMTBIT_S16_LE |
				    SNDRV_PCM_FMTBIT_S24_LE |
				    SNDRV_PCM_FMTBIT_S24_3LE |
				    SNDRV_PCM_FMTBIT_S32_LE),
			.channels_min = 1,
			.channels_max = 32,
			.rate_min =     8000,
			.rate_max =	48000,
		},
		.ops = &msm_fe_Multimedia_dai_ops,
		.name = "MultiMedia1",
		
	},
	{
		.playback = {
			.stream_name = "MultiMedia2 Playback",

			.rates = (SNDRV_PCM_RATE_8000_384000|
					SNDRV_PCM_RATE_KNOT),
			.formats = (SNDRV_PCM_FMTBIT_S16_LE |
						SNDRV_PCM_FMTBIT_S24_LE |
						SNDRV_PCM_FMTBIT_S24_3LE |
						SNDRV_PCM_FMTBIT_S32_LE),
			.channels_min = 1,
			.channels_max = 32,
			.rate_min =     8000,
			.rate_max =	384000,
		},
		.capture = {
			.stream_name = "MultiMedia2 Capture",

			.rates = (SNDRV_PCM_RATE_8000_384000|
					SNDRV_PCM_RATE_KNOT),
			.formats = (SNDRV_PCM_FMTBIT_S16_LE |
				    SNDRV_PCM_FMTBIT_S24_LE |
				    SNDRV_PCM_FMTBIT_S24_3LE |
				    SNDRV_PCM_FMTBIT_S32_LE),
			.channels_min = 1,
			.channels_max = 32,
			.rate_min =     8000,
			.rate_max =	48000,
		},
		.ops = &msm_fe_Multimedia_dai_ops,
		.name = "MultiMedia2",
		
	},
	{
		.playback = {
			.stream_name = "VoIP Playback",

			.rates = SNDRV_PCM_RATE_8000_48000,
			.formats = SNDRV_PCM_FMTBIT_S16_LE |
					SNDRV_PCM_FMTBIT_SPECIAL,
			.channels_min = 1,
			.channels_max = 2,
			.rate_min =	8000,
			.rate_max = 48000,
		},
		.capture = {
			.stream_name = "VoIP Capture",

			.rates = SNDRV_PCM_RATE_8000_48000,
			.formats = SNDRV_PCM_FMTBIT_S16_LE |
					SNDRV_PCM_FMTBIT_SPECIAL,
			.channels_min = 1,
			.channels_max = 2,
			.rate_min =	8000,
			.rate_max = 48000,
		},
		.ops = &msm_fe_dai_ops,
		.name = "VoIP",
		
	},
	{
		.playback = {
			.stream_name = "MultiMedia3 Playback",

			.rates = (SNDRV_PCM_RATE_8000_384000 |
					SNDRV_PCM_RATE_KNOT),
			.formats = (SNDRV_PCM_FMTBIT_S16_LE |
						SNDRV_PCM_FMTBIT_S24_LE |
						SNDRV_PCM_FMTBIT_S24_3LE |
						SNDRV_PCM_FMTBIT_S32_LE),
			.channels_min = 1,
			.channels_max = 32,
			.rate_min =	8000,
			.rate_max = 384000,
		},
		.capture = {
			.stream_name = "MultiMedia3 Capture",

			.rates = (SNDRV_PCM_RATE_8000_384000|
					SNDRV_PCM_RATE_KNOT),
			.formats = (SNDRV_PCM_FMTBIT_S16_LE |
						SNDRV_PCM_FMTBIT_S24_LE |
						SNDRV_PCM_FMTBIT_S24_3LE |
						SNDRV_PCM_FMTBIT_S32_LE),
			.channels_min = 1,
			.channels_max = 32,
			.rate_min =     8000,
			.rate_max =	48000,
		},
		.ops = &msm_fe_Multimedia_dai_ops,
		.name = "MultiMedia3",
		
	},
	{
		.playback = {
			.stream_name = "MultiMedia4 Playback",

			.rates = (SNDRV_PCM_RATE_8000_384000 |
					SNDRV_PCM_RATE_KNOT),
			.formats = (SNDRV_PCM_FMTBIT_S16_LE |
						SNDRV_PCM_FMTBIT_S24_LE |
						SNDRV_PCM_FMTBIT_S24_3LE |
						SNDRV_PCM_FMTBIT_S32_LE),
			.channels_min = 1,
			.channels_max = 32,
			.rate_min =	8000,
			.rate_max = 384000,
		},
		.ops = &msm_fe_Multimedia_dai_ops_compressed,
		.name = "MultiMedia4",
		
	},
	{
		.playback = {
			.stream_name = "MultiMedia5 Playback",

			.rates = (SNDRV_PCM_RATE_8000_384000 |
					SNDRV_PCM_RATE_KNOT),
			.formats = (SNDRV_PCM_FMTBIT_S16_LE |
						SNDRV_PCM_FMTBIT_S24_LE |
						SNDRV_PCM_FMTBIT_S24_3LE |
						SNDRV_PCM_FMTBIT_S32_LE),
			.channels_min = 1,
			.channels_max = 32,
			.rate_min =	8000,
			.rate_max = 384000,
		},
		.capture = {
			.stream_name = "MultiMedia5 Capture",

			.rates = (SNDRV_PCM_RATE_8000_48000|
					SNDRV_PCM_RATE_KNOT),
			.formats = (SNDRV_PCM_FMTBIT_S16_LE |
				    SNDRV_PCM_FMTBIT_S24_LE |
				    SNDRV_PCM_FMTBIT_S24_3LE |
				    SNDRV_PCM_FMTBIT_S32_LE),
			.channels_min = 1,
			.channels_max = 32,
			.rate_min =     8000,
			.rate_max =	48000,
		},
		.ops = &msm_fe_Multimedia_dai_ops,
		.name = "MultiMedia5",
		
	},
	{
		.playback = {
			.stream_name = "MultiMedia6 Playback",

			.rates = (SNDRV_PCM_RATE_8000_384000 |
					SNDRV_PCM_RATE_KNOT),
			.formats = (SNDRV_PCM_FMTBIT_S16_LE |
						SNDRV_PCM_FMTBIT_S24_LE |
						SNDRV_PCM_FMTBIT_S24_3LE |
						SNDRV_PCM_FMTBIT_S32_LE),
			.channels_min = 1,
			.channels_max = 32,
			.rate_min =	8000,
			.rate_max = 384000,
		},
		.capture = {
			.stream_name = "MultiMedia6 Capture",

			.rates = (SNDRV_PCM_RATE_8000_48000|
					SNDRV_PCM_RATE_KNOT),
			.formats = (SNDRV_PCM_FMTBIT_S16_LE |
				    SNDRV_PCM_FMTBIT_S24_LE |
				    SNDRV_PCM_FMTBIT_S24_3LE |
				    SNDRV_PCM_FMTBIT_S32_LE),
			.channels_min = 1,
			.channels_max = 32,
			.rate_min =     8000,
			.rate_max =	48000,
		},
		.ops = &msm_fe_Multimedia_dai_ops,
		.name = "MultiMedia6",
		
	},
	{
		.playback = {
			.stream_name = "MultiMedia7 Playback",

			.rates = (SNDRV_PCM_RATE_8000_384000 |
					SNDRV_PCM_RATE_KNOT),
			.formats = (SNDRV_PCM_FMTBIT_S16_LE |
						SNDRV_PCM_FMTBIT_S24_LE |
						SNDRV_PCM_FMTBIT_S24_3LE |
						SNDRV_PCM_FMTBIT_S32_LE),
			.channels_min = 1,
			.channels_max = 32,
			.rate_min =	8000,
			.rate_max = 384000,
		},
		.ops = &msm_fe_Multimedia_dai_ops_compressed,
		.name = "MultiMedia7",
		
	},
	{
		.playback = {
			.stream_name = "MultiMedia8 Playback",

			.rates = (SNDRV_PCM_RATE_8000_384000 |
					SNDRV_PCM_RATE_KNOT),
			.formats = (SNDRV_PCM_FMTBIT_S16_LE |
						SNDRV_PCM_FMTBIT_S24_LE |
						SNDRV_PCM_FMTBIT_S24_3LE |
						SNDRV_PCM_FMTBIT_S32_LE),
			.channels_min = 1,
			.channels_max = 32,
			.rate_min =	8000,
			.rate_max = 384000,
		},
		.capture = {
			.stream_name = "MultiMedia8 Capture",

			.rates = (SNDRV_PCM_RATE_8000_48000|
					SNDRV_PCM_RATE_KNOT),
			.formats = (SNDRV_PCM_FMTBIT_S16_LE |
				    SNDRV_PCM_FMTBIT_S24_LE |
				    SNDRV_PCM_FMTBIT_S24_3LE |
				    SNDRV_PCM_FMTBIT_S32_LE),
			.channels_min = 1,
			.channels_max = 32,
			.rate_min =     8000,
			.rate_max =	48000,
		},
		.ops = &msm_fe_Multimedia_dai_ops,
		.name = "MultiMedia8",
		
	},
	/* FE DAIs created for hostless operation purpose */
	{
		.playback = {
			.stream_name = "SLIMBUS0_HOSTLESS Playback",

			.rates = SNDRV_PCM_RATE_8000_384000,
			.formats = (SNDRV_PCM_FMTBIT_S16_LE |
				    SNDRV_PCM_FMTBIT_S24_LE),
			.channels_min = 1,
			.channels_max = 8,
			.rate_min =     8000,
			.rate_max =     384000,
		},
		.capture = {
			.stream_name = "SLIMBUS0_HOSTLESS Capture",

			.rates = SNDRV_PCM_RATE_8000_96000,
			.formats = (SNDRV_PCM_FMTBIT_S16_LE |
				    SNDRV_PCM_FMTBIT_S24_LE |
				    SNDRV_PCM_FMTBIT_S32_LE),
			.channels_min = 1,
			.channels_max = 8,
			.rate_min =     8000,
			.rate_max =     384000,
		},
		.ops = &msm_fe_dai_ops,
		.name = "SLIMBUS0_HOSTLESS",
		
	},
	{
		.playback = {
			.stream_name = "SLIMBUS1_HOSTLESS Playback",

			.rates = SNDRV_PCM_RATE_8000_384000,
			.formats = (SNDRV_PCM_FMTBIT_S16_LE |
						SNDRV_PCM_FMTBIT_S24_LE),
			.channels_min = 1,
			.channels_max = 2,
			.rate_min =     8000,
			.rate_max =     384000,
		},
		.capture = {
			.stream_name = "SLIMBUS1_HOSTLESS Capture",

			.rates = SNDRV_PCM_RATE_8000_48000,
			.formats = (SNDRV_PCM_FMTBIT_S16_LE |
				    SNDRV_PCM_FMTBIT_S24_LE),
			.channels_min = 1,
			.channels_max = 2,
			.rate_min =     8000,
			.rate_max =     48000,
		},
		.ops = &msm_fe_dai_ops,
		.name = "SLIMBUS1_HOSTLESS",
		
	},
	{
		.playback = {
			.stream_name = "SLIMBUS3_HOSTLESS Playback",

			.rates = SNDRV_PCM_RATE_8000_384000,
			.formats = (SNDRV_PCM_FMTBIT_S16_LE |
						SNDRV_PCM_FMTBIT_S24_LE),
			.channels_min = 1,
			.channels_max = 2,
			.rate_min =     8000,
			.rate_max =     384000,
		},
		.capture = {
			.stream_name = "SLIMBUS3_HOSTLESS Capture",

			.rates = SNDRV_PCM_RATE_8000_48000,
			.formats = (SNDRV_PCM_FMTBIT_S16_LE |
				    SNDRV_PCM_FMTBIT_S24_LE),
			.channels_min = 1,
			.channels_max = 2,
			.rate_min =     8000,
			.rate_max =     48000,
		},
		.ops = &msm_fe_dai_ops,
		.name = "SLIMBUS3_HOSTLESS",
		
	},
	{
		.playback = {
			.stream_name = "SLIMBUS4_HOSTLESS Playback",

			.rates = SNDRV_PCM_RATE_8000_384000,
			.formats = (SNDRV_PCM_FMTBIT_S16_LE |
						SNDRV_PCM_FMTBIT_S24_LE),
			.channels_min = 1,
			.channels_max = 2,
			.rate_min =     8000,
			.rate_max =     384000,
		},
		.capture = {
			.stream_name = "SLIMBUS4_HOSTLESS Capture",

			.rates = SNDRV_PCM_RATE_8000_48000,
			.formats = (SNDRV_PCM_FMTBIT_S16_LE |
				    SNDRV_PCM_FMTBIT_S24_LE),
			.channels_min = 1,
			.channels_max = 2,
			.rate_min =     8000,
			.rate_max =     48000,
		},
		.ops = &msm_fe_dai_ops,
		.name = "SLIMBUS4_HOSTLESS",
		
	},
	{
		.playback = {
			.stream_name = "SLIMBUS6_HOSTLESS Playback",

			.rates = SNDRV_PCM_RATE_8000_384000,
			.formats = (SNDRV_PCM_FMTBIT_S16_LE |
				    SNDRV_PCM_FMTBIT_S24_LE),
			.channels_min = 1,
			.channels_max = 8,
			.rate_min =     8000,
			.rate_max =     384000,
		},
		.ops = &msm_fe_dai_ops,
		.name = "SLIMBUS6_HOSTLESS",
		
	},
	{
		.playback = {
			.stream_name = "SLIMBUS7_HOSTLESS Playback",

			.rates = SNDRV_PCM_RATE_8000_384000,
			.formats = (SNDRV_PCM_FMTBIT_S16_LE |
				    SNDRV_PCM_FMTBIT_S24_LE),
			.channels_min = 1,
			.channels_max = 8,
			.rate_min =     8000,
			.rate_max =     384000,
		},
		.capture = {
			.stream_name = "SLIMBUS7_HOSTLESS Capture",

			.rates = SNDRV_PCM_RATE_8000_384000,
			.formats = (SNDRV_PCM_FMTBIT_S16_LE |
				    SNDRV_PCM_FMTBIT_S24_LE),
			.channels_min = 1,
			.channels_max = 8,
			.rate_min =     8000,
			.rate_max =     384000,
		},
		.ops = &msm_fe_dai_ops,
		.name = "SLIMBUS7_HOSTLESS",
		
	},
	{
		.playback = {
			.stream_name = "SLIMBUS8_HOSTLESS Playback",

			.rates = SNDRV_PCM_RATE_8000_384000,
			.formats = (SNDRV_PCM_FMTBIT_S16_LE |
				    SNDRV_PCM_FMTBIT_S24_LE),
			.channels_min = 1,
			.channels_max = 8,
			.rate_min =     8000,
			.rate_max =     384000,
		},
		.capture = {
			.stream_name = "SLIMBUS8_HOSTLESS Capture",

			.rates = SNDRV_PCM_RATE_8000_384000,
			.formats = (SNDRV_PCM_FMTBIT_S16_LE |
				    SNDRV_PCM_FMTBIT_S24_LE),
			.channels_min = 1,
			.channels_max = 8,
			.rate_min =     8000,
			.rate_max =     384000,
		},
		.ops = &msm_fe_dai_ops,
		.name = "SLIMBUS8_HOSTLESS",
		
	},
	{
		.playback = {
			.stream_name = "CDC_DMA_HOSTLESS Playback",

			.rates = SNDRV_PCM_RATE_8000 | SNDRV_PCM_RATE_11025 |
				SNDRV_PCM_RATE_16000 | SNDRV_PCM_RATE_22050 |
				SNDRV_PCM_RATE_32000 | SNDRV_PCM_RATE_44100 |
				SNDRV_PCM_RATE_48000 | SNDRV_PCM_RATE_88200 |
				SNDRV_PCM_RATE_96000 | SNDRV_PCM_RATE_176400 |
				SNDRV_PCM_RATE_192000 | SNDRV_PCM_RATE_352800 |
				SNDRV_PCM_RATE_384000,
			.formats = SNDRV_PCM_FMTBIT_S16_LE |
				   SNDRV_PCM_FMTBIT_S24_LE |
				   SNDRV_PCM_FMTBIT_S24_3LE |
				   SNDRV_PCM_FMTBIT_S32_LE,
			.channels_min = 1,
			.channels_max = 4,
			.rate_min = 8000,
			.rate_max = 384000,
		},
		.capture = {
			.stream_name = "CDC_DMA_HOSTLESS Capture",

			.rates = SNDRV_PCM_RATE_8000 | SNDRV_PCM_RATE_11025 |
				SNDRV_PCM_RATE_16000 | SNDRV_PCM_RATE_22050 |
				SNDRV_PCM_RATE_32000 | SNDRV_PCM_RATE_44100 |
				SNDRV_PCM_RATE_48000 | SNDRV_PCM_RATE_88200 |
				SNDRV_PCM_RATE_96000 | SNDRV_PCM_RATE_176400 |
				SNDRV_PCM_RATE_192000 | SNDRV_PCM_RATE_352800 |
				SNDRV_PCM_RATE_384000,
			.formats = SNDRV_PCM_FMTBIT_S16_LE |
				   SNDRV_PCM_FMTBIT_S24_LE |
				   SNDRV_PCM_FMTBIT_S24_3LE,
			.channels_min = 1,
			.channels_max = 8,
			.rate_min = 8000,
			.rate_max = 384000,
		},
		.ops = &msm_fe_dai_ops,
		.name = "CDC_DMA_HOSTLESS",
		
	},
	{
		.capture = {
			.stream_name = "TX3_CDC_DMA_HOSTLESS Capture",

			.rates = SNDRV_PCM_RATE_8000 | SNDRV_PCM_RATE_11025 |
				SNDRV_PCM_RATE_16000 | SNDRV_PCM_RATE_22050 |
				SNDRV_PCM_RATE_32000 | SNDRV_PCM_RATE_44100 |
				SNDRV_PCM_RATE_48000 | SNDRV_PCM_RATE_88200 |
				SNDRV_PCM_RATE_96000 | SNDRV_PCM_RATE_176400 |
				SNDRV_PCM_RATE_192000 | SNDRV_PCM_RATE_352800 |
				SNDRV_PCM_RATE_384000,
			.formats = SNDRV_PCM_FMTBIT_S16_LE |
				   SNDRV_PCM_FMTBIT_S24_LE |
				   SNDRV_PCM_FMTBIT_S24_3LE,
			.channels_min = 1,
			.channels_max = 8,
			.rate_min = 8000,
			.rate_max = 384000,
		},
		.ops = &msm_fe_dai_ops,
		.name = "TX3_CDC_DMA_HOSTLESS",
		
	},
	{
		.capture = {
			.stream_name = "TX4_CDC_DMA_HOSTLESS Capture",

			.rates = SNDRV_PCM_RATE_8000 | SNDRV_PCM_RATE_11025 |
				SNDRV_PCM_RATE_16000 | SNDRV_PCM_RATE_22050 |
				SNDRV_PCM_RATE_32000 | SNDRV_PCM_RATE_44100 |
				SNDRV_PCM_RATE_48000 | SNDRV_PCM_RATE_88200 |
				SNDRV_PCM_RATE_96000 | SNDRV_PCM_RATE_176400 |
				SNDRV_PCM_RATE_192000 | SNDRV_PCM_RATE_352800 |
				SNDRV_PCM_RATE_384000,
			.formats = SNDRV_PCM_FMTBIT_S16_LE |
				   SNDRV_PCM_FMTBIT_S24_LE |
				   SNDRV_PCM_FMTBIT_S24_3LE,
			.channels_min = 1,
			.channels_max = 8,
			.rate_min = 8000,
			.rate_max = 384000,
		},
		.ops = &msm_fe_dai_ops,
		.name = "TX4_CDC_DMA_HOSTLESS",
		
	},
	{
		.playback = {
			.stream_name = "INT_FM_HOSTLESS Playback",

			.rates = SNDRV_PCM_RATE_8000_48000,
			.formats = SNDRV_PCM_FMTBIT_S16_LE,
			.channels_min = 1,
			.channels_max = 2,
			.rate_min =     8000,
			.rate_max =     48000,
		},
		.capture = {
			.stream_name = "INT_FM_HOSTLESS Capture",

			.rates = SNDRV_PCM_RATE_8000_48000,
			.formats = SNDRV_PCM_FMTBIT_S16_LE,
			.channels_min = 1,
			.channels_max = 2,
			.rate_min =     8000,
			.rate_max =     48000,
		},
		.ops = &msm_fe_dai_ops,
		.name = "INT_FM_HOSTLESS",
		
	},
	{
		.playback = {
			.stream_name = "INT_HFP_BT Hostless Playback",

			.rates = SNDRV_PCM_RATE_8000 | SNDRV_PCM_RATE_16000,
			.formats = SNDRV_PCM_FMTBIT_S16_LE,
			.channels_min = 1,
			.channels_max = 2,
			.rate_min =     8000,
			.rate_max =     16000,
		},
		.capture = {
			.stream_name = "INT_HFP_BT Hostless Capture",

			.rates = SNDRV_PCM_RATE_8000 | SNDRV_PCM_RATE_16000,
			.formats = SNDRV_PCM_FMTBIT_S16_LE,
			.channels_min = 1,
			.channels_max = 2,
			.rate_min =     8000,
			.rate_max =     16000,
		},
		.ops = &msm_fe_dai_ops,
		.name = "INT_HFP_BT_HOSTLESS",
		
	},
	{
		.playback = {
			.stream_name = "USBAUDIO_HOSTLESS Playback",

			.rates = SNDRV_PCM_RATE_8000 | SNDRV_PCM_RATE_11025 |
				SNDRV_PCM_RATE_16000 | SNDRV_PCM_RATE_22050 |
				SNDRV_PCM_RATE_32000 | SNDRV_PCM_RATE_44100 |
				SNDRV_PCM_RATE_48000 | SNDRV_PCM_RATE_88200 |
				SNDRV_PCM_RATE_96000 | SNDRV_PCM_RATE_176400 |
				SNDRV_PCM_RATE_192000 | SNDRV_PCM_RATE_352800 |
				SNDRV_PCM_RATE_384000,
			.formats = SNDRV_PCM_FMTBIT_S16_LE |
				SNDRV_PCM_FMTBIT_S24_LE |
				SNDRV_PCM_FMTBIT_S24_3LE |
				SNDRV_PCM_FMTBIT_S32_LE,
			.channels_min = 1,
			.channels_max = 8,
			.rate_min =     8000,
			.rate_max =     384000,
		},
		.capture = {
			.stream_name = "USBAUDIO_HOSTLESS Capture",

			.rates = SNDRV_PCM_RATE_8000 | SNDRV_PCM_RATE_11025 |
				SNDRV_PCM_RATE_16000 | SNDRV_PCM_RATE_22050 |
				SNDRV_PCM_RATE_32000 | SNDRV_PCM_RATE_44100 |
				SNDRV_PCM_RATE_48000 | SNDRV_PCM_RATE_88200 |
				SNDRV_PCM_RATE_96000 | SNDRV_PCM_RATE_176400 |
				SNDRV_PCM_RATE_192000 | SNDRV_PCM_RATE_352800 |
				SNDRV_PCM_RATE_384000,
			.formats = SNDRV_PCM_FMTBIT_S16_LE |
				SNDRV_PCM_FMTBIT_S24_LE |
				SNDRV_PCM_FMTBIT_S24_3LE |
				SNDRV_PCM_FMTBIT_S32_LE,
			.channels_min = 1,
			.channels_max = 8,
			.rate_min =     8000,
			.rate_max =     384000,
		},
		.ops = &msm_fe_dai_ops,
		.name = "USBAUDIO_HOSTLESS",
		
	},
	{
		.playback = {
			.stream_name = "HDMI_HOSTLESS Playback",

			.rates = SNDRV_PCM_RATE_8000_48000,
			.formats = (SNDRV_PCM_FMTBIT_S16_LE |
				    SNDRV_PCM_FMTBIT_S24_LE),
			.channels_min = 1,
			.channels_max = 2,
			.rate_min = 8000,
			.rate_max = 48000,
		},
		.ops = &msm_fe_dai_ops,
		.name = "HDMI_HOSTLESS",
		
	},
	{
		.playback = {
			.stream_name = "DISPLAY_PORT_HOSTLESS Playback",

			.rates = SNDRV_PCM_RATE_8000_48000,
			.formats = (SNDRV_PCM_FMTBIT_S16_LE |
				    SNDRV_PCM_FMTBIT_S24_LE),
			.channels_min = 1,
			.channels_max = 2,
			.rate_min = 8000,
			.rate_max = 48000,
		},
		.ops = &msm_fe_dai_ops,
		.name = "DISPLAY_PORT_HOSTLESS",
		
	},
	{
		.playback = {
			.stream_name = "AUXPCM_HOSTLESS Playback",

			.rates = SNDRV_PCM_RATE_8000 | SNDRV_PCM_RATE_16000,
			.formats = SNDRV_PCM_FMTBIT_S16_LE,
			.channels_min = 1,
			.channels_max = 1,
			.rate_min =     8000,
			.rate_max =     16000,
		},
		.capture = {
			.stream_name = "AUXPCM_HOSTLESS Capture",

			.rates = SNDRV_PCM_RATE_8000 | SNDRV_PCM_RATE_16000,
			.formats = SNDRV_PCM_FMTBIT_S16_LE,
			.channels_min = 1,
			.channels_max = 1,
			.rate_min =     8000,
			.rate_max =    16000,
		},
		.ops = &msm_fe_dai_ops,
		.name = "AUXPCM_HOSTLESS",
		
	},
	{
		.playback = {
			.stream_name = "SEC_AUXPCM_HOSTLESS Playback",

			.rates = SNDRV_PCM_RATE_8000 | SNDRV_PCM_RATE_16000,
			.formats = SNDRV_PCM_FMTBIT_S16_LE,
			.channels_min = 1,
			.channels_max = 1,
			.rate_min =     8000,
			.rate_max =     16000,
		},
		.ops = &msm_fe_dai_ops,
		.name = "SEC_AUXPCM_RX_HOSTLESS",
		
	},
	{
		.capture = {
			.stream_name = "SEC_AUXPCM_HOSTLESS Capture",

			.rates = SNDRV_PCM_RATE_8000 | SNDRV_PCM_RATE_16000,
			.formats = SNDRV_PCM_FMTBIT_S16_LE,
			.channels_min = 1,
			.channels_max = 1,
			.rate_min =     8000,
			.rate_max =    16000,
		},
		.ops = &msm_fe_dai_ops,
		.name = "SEC_AUXPCM_TX_HOSTLESS",
		
	},
	{
		.playback = {
			.stream_name = "VOICE_STUB Playback",

			.rates = SNDRV_PCM_RATE_8000_48000,
			.formats = SNDRV_PCM_FMTBIT_S16_LE,
			.channels_min = 1,
			.channels_max = 2,
			.rate_min = 8000,
			.rate_max = 48000,
		},
		.capture = {
			.stream_name = "VOICE_STUB Capture",

			.rates = SNDRV_PCM_RATE_8000_48000,
			.formats = SNDRV_PCM_FMTBIT_S16_LE,
			.channels_min = 1,
			.channels_max = 2,
			.rate_min = 8000,
			.rate_max = 48000,
		},
		.ops = &msm_fe_dai_ops,
		.name = "VOICE_STUB",
		
	},
	{
		.playback = {
			.stream_name = "MI2S_RX_HOSTLESS Playback",

			.rates = SNDRV_PCM_RATE_8000_48000,
			.formats = SNDRV_PCM_FMTBIT_S16_LE,
			.channels_min = 1,
			.channels_max = 2,
			.rate_min = 8000,
			.rate_max = 48000,
		},
		.capture = {
			.stream_name = "MI2S_TX_HOSTLESS Capture",

			.rates = SNDRV_PCM_RATE_8000_48000,
			.formats = (SNDRV_PCM_FMTBIT_S16_LE |
				    SNDRV_PCM_FMTBIT_S24_LE),
			.channels_min = 1,
			.channels_max = 2,
			.rate_min = 8000,
			.rate_max = 48000,
		},
		.ops = &msm_fe_dai_ops,
		.name = "MI2S_TX_HOSTLESS",
		
	},
	{
		.playback = {
			.stream_name = "SEC_I2S_RX_HOSTLESS Playback",

			.rates = SNDRV_PCM_RATE_8000_48000,
			.formats = SNDRV_PCM_FMTBIT_S16_LE,
			.channels_min = 1,
			.channels_max = 2,
			.rate_min =     8000,
			.rate_max =    48000,
		},
		.ops = &msm_fe_dai_ops,
		.name = "SEC_I2S_RX_HOSTLESS",
		
	},
	{
		.capture = {
			.stream_name = "Primary MI2S_TX Hostless Capture",

			.rates = SNDRV_PCM_RATE_8000_48000,
			.formats = (SNDRV_PCM_FMTBIT_S16_LE |
				    SNDRV_PCM_FMTBIT_S24_LE),
			.channels_min = 1,
			.channels_max = 2,
			.rate_min = 8000,
			.rate_max = 48000,
		},
		.ops = &msm_fe_dai_ops,
		.name = "PRI_MI2S_TX_HOSTLESS",
		
	},
	{
		.playback = {
			.stream_name = "Primary MI2S_RX Hostless Playback",

			.rates = SNDRV_PCM_RATE_8000_384000,
			.formats = (SNDRV_PCM_FMTBIT_S16_LE |
				    SNDRV_PCM_FMTBIT_S24_LE),
			.channels_min = 1,
			.channels_max = 2,
			.rate_min =     8000,
			.rate_max =    384000,
		},
		.ops = &msm_fe_dai_ops,
		.name = "PRI_MI2S_RX_HOSTLESS",
		
	},
	{
		.capture = {
			.stream_name = "Secondary MI2S_TX Hostless Capture",

			.rates = SNDRV_PCM_RATE_8000_48000,
			.formats = (SNDRV_PCM_FMTBIT_S16_LE |
				    SNDRV_PCM_FMTBIT_S24_LE),
			.channels_min = 1,
			.channels_max = 2,
			.rate_min = 8000,
			.rate_max = 48000,
		},
		.ops = &msm_fe_dai_ops,
		.name = "SEC_MI2S_TX_HOSTLESS",
		
	},
	{
		.playback = {
			.stream_name = "Secondary MI2S_RX Hostless Playback",

			.rates = SNDRV_PCM_RATE_8000_384000,
			.formats = SNDRV_PCM_FMTBIT_S16_LE |
				    SNDRV_PCM_FMTBIT_S24_LE,
			.channels_min = 1,
			.channels_max = 2,
			.rate_min = 8000,
			.rate_max = 384000,
		},
		.ops = &msm_fe_dai_ops,
		.name = "SEC_MI2S_RX_HOSTLESS",
		
	},
	{
		.capture = {
			.stream_name = "Tertiary MI2S_TX Hostless Capture",

			.rates = SNDRV_PCM_RATE_8000_48000,
			.formats = (SNDRV_PCM_FMTBIT_S16_LE |
				    SNDRV_PCM_FMTBIT_S24_LE),
			.channels_min = 1,
			.channels_max = 2,
			.rate_min = 8000,
			.rate_max = 48000,
		},
		.ops = &msm_fe_dai_ops,
		.name = "TERT_MI2S_TX_HOSTLESS",
		
	},
	{
		.playback = {
			.stream_name = "Tertiary MI2S_RX Hostless Playback",

			.rates = SNDRV_PCM_RATE_8000_384000,
			.formats = SNDRV_PCM_FMTBIT_S16_LE |
				    SNDRV_PCM_FMTBIT_S24_LE,
			.channels_min = 1,
			.channels_max = 2,
			.rate_min =	8000,
			.rate_max =    384000,
		},
		.ops = &msm_fe_dai_ops,
		.name = "TERT_MI2S_RX_HOSTLESS",
		
	},
	{
		.capture = {
			.stream_name = "Quaternary MI2S_TX Hostless Capture",

			.rates = SNDRV_PCM_RATE_8000_48000,
			.formats = (SNDRV_PCM_FMTBIT_S16_LE |
				    SNDRV_PCM_FMTBIT_S24_LE),
			.channels_min = 1,
			.channels_max = 2,
			.rate_min = 8000,
			.rate_max = 48000,
		},
		.ops = &msm_fe_dai_ops,
		.name = "QUAT_MI2S_TX_HOSTLESS",
		
	},
	{
		.playback = {
			.stream_name = "Quaternary MI2S_RX Hostless Playback",

			.rates = SNDRV_PCM_RATE_8000_384000,
			.formats = SNDRV_PCM_FMTBIT_S16_LE |
				    SNDRV_PCM_FMTBIT_S24_LE,
			.channels_min = 1,
			.channels_max = 8,
			.rate_min = 8000,
			.rate_max = 384000,
		},
		.ops = &msm_fe_dai_ops,
		.name = "QUAT_MI2S_RX_HOSTLESS",
		
	},
	{
		.playback = {
			.stream_name = "INT0 MI2S_RX Hostless Playback",

			.rates = SNDRV_PCM_RATE_8000_192000,
			.formats = (SNDRV_PCM_FMTBIT_S16_LE |
				    SNDRV_PCM_FMTBIT_S24_LE),
			.channels_min = 1,
			.channels_max = 2,
			.rate_min =     8000,
			.rate_max =    192000,
		},
		.ops = &msm_fe_dai_ops,
		.name = "INT0_MI2S_RX_HOSTLESS",
		
	},
	{
		.playback = {
			.stream_name = "INT4 MI2S_RX Hostless Playback",

			.rates = SNDRV_PCM_RATE_8000_192000,
			.formats = (SNDRV_PCM_FMTBIT_S16_LE |
				    SNDRV_PCM_FMTBIT_S24_LE),
			.channels_min = 1,
			.channels_max = 4,
			.rate_min =     8000,
			.rate_max =    192000,
		},
		.ops = &msm_fe_dai_ops,
		.name = "INT4_MI2S_RX_HOSTLESS",
		
	},
	{
		.capture = {
			.stream_name = "INT3 MI2S_TX Hostless Capture",

			.rates = SNDRV_PCM_RATE_8000_48000,
			.formats = (SNDRV_PCM_FMTBIT_S16_LE |
				    SNDRV_PCM_FMTBIT_S24_LE),
			.channels_min = 1,
			.channels_max = 2,
			.rate_min = 8000,
			.rate_max = 48000,
		},
		.ops = &msm_fe_dai_ops,
		.name = "INT3_MI2S_TX_HOSTLESS",
		
	},
	/* TDM Hostless */
	{
		.capture = {
			.stream_name = "Primary TDM0 Hostless Capture",

			.rates = SNDRV_PCM_RATE_8000_48000,
			.formats = (SNDRV_PCM_FMTBIT_S16_LE |
				    SNDRV_PCM_FMTBIT_S24_LE),
			.channels_min = 1,
			.channels_max = 8,
			.rate_min = 8000,
			.rate_max = 48000,
		},
		.ops = &msm_fe_dai_ops,
		.name = "PRI_TDM_TX_0_HOSTLESS",
		
	},
	{
		.playback = {
			.stream_name = "Primary TDM0 Hostless Playback",

			.rates = SNDRV_PCM_RATE_8000_48000,
			.formats = (SNDRV_PCM_FMTBIT_S16_LE |
				    SNDRV_PCM_FMTBIT_S24_LE),
			.channels_min = 1,
			.channels_max = 8,
			.rate_min = 8000,
			.rate_max = 48000,
		},
		.ops = &msm_fe_dai_ops,
		.name = "PRI_TDM_RX_0_HOSTLESS",
		
	},
	{
		.capture = {
			.stream_name = "Primary TDM1 Hostless Capture",

			.rates = SNDRV_PCM_RATE_8000_48000,
			.formats = (SNDRV_PCM_FMTBIT_S16_LE |
				    SNDRV_PCM_FMTBIT_S24_LE),
			.channels_min = 1,
			.channels_max = 8,
			.rate_min = 8000,
			.rate_max = 48000,
		},
		.ops = &msm_fe_dai_ops,
		.name = "PRI_TDM_TX_1_HOSTLESS",
		
	},
	{
		.playback = {
			.stream_name = "Primary TDM1 Hostless Playback",

			.rates = SNDRV_PCM_RATE_8000_48000,
			.formats = (SNDRV_PCM_FMTBIT_S16_LE |
				    SNDRV_PCM_FMTBIT_S24_LE),
			.channels_min = 1,
			.channels_max = 8,
			.rate_min = 8000,
			.rate_max = 48000,
		},
		.ops = &msm_fe_dai_ops,
		.name = "PRI_TDM_RX_1_HOSTLESS",
		
	},
	{
		.capture = {
			.stream_name = "Primary TDM2 Hostless Capture",

			.rates = SNDRV_PCM_RATE_8000_48000,
			.formats = (SNDRV_PCM_FMTBIT_S16_LE |
				    SNDRV_PCM_FMTBIT_S24_LE),
			.channels_min = 1,
			.channels_max = 8,
			.rate_min = 8000,
			.rate_max = 48000,
		},
		.ops = &msm_fe_dai_ops,
		.name = "PRI_TDM_TX_2_HOSTLESS",
		
	},
	{
		.playback = {
			.stream_name = "Primary TDM2 Hostless Playback",

			.rates = SNDRV_PCM_RATE_8000_48000,
			.formats = (SNDRV_PCM_FMTBIT_S16_LE |
				    SNDRV_PCM_FMTBIT_S24_LE),
			.channels_min = 1,
			.channels_max = 8,
			.rate_min = 8000,
			.rate_max = 48000,
		},
		.ops = &msm_fe_dai_ops,
		.name = "PRI_TDM_RX_2_HOSTLESS",
		
	},
	{
		.capture = {
			.stream_name = "Primary TDM3 Hostless Capture",

			.rates = SNDRV_PCM_RATE_8000_48000,
			.formats = (SNDRV_PCM_FMTBIT_S16_LE |
				    SNDRV_PCM_FMTBIT_S24_LE),
			.channels_min = 1,
			.channels_max = 8,
			.rate_min = 8000,
			.rate_max = 48000,
		},
		.ops = &msm_fe_dai_ops,
		.name = "PRI_TDM_TX_3_HOSTLESS",
		
	},
	{
		.playback = {
			.stream_name = "Primary TDM3 Hostless Playback",

			.rates = SNDRV_PCM_RATE_8000_48000,
			.formats = (SNDRV_PCM_FMTBIT_S16_LE |
				    SNDRV_PCM_FMTBIT_S24_LE),
			.channels_min = 1,
			.channels_max = 8,
			.rate_min = 8000,
			.rate_max = 48000,
		},
		.ops = &msm_fe_dai_ops,
		.name = "PRI_TDM_RX_3_HOSTLESS",
		
	},
	{
		.capture = {
			.stream_name = "Primary TDM4 Hostless Capture",

			.rates = SNDRV_PCM_RATE_8000_48000,
			.formats = (SNDRV_PCM_FMTBIT_S16_LE |
				    SNDRV_PCM_FMTBIT_S24_LE),
			.channels_min = 1,
			.channels_max = 8,
			.rate_min = 8000,
			.rate_max = 48000,
		},
		.ops = &msm_fe_dai_ops,
		.name = "PRI_TDM_TX_4_HOSTLESS",
		
	},
	{
		.playback = {
			.stream_name = "Primary TDM4 Hostless Playback",

			.rates = SNDRV_PCM_RATE_8000_48000,
			.formats = (SNDRV_PCM_FMTBIT_S16_LE |
				    SNDRV_PCM_FMTBIT_S24_LE),
			.channels_min = 1,
			.channels_max = 8,
			.rate_min = 8000,
			.rate_max = 48000,
		},
		.ops = &msm_fe_dai_ops,
		.name = "PRI_TDM_RX_4_HOSTLESS",
		
	},
	{
		.capture = {
			.stream_name = "Primary TDM5 Hostless Capture",

			.rates = SNDRV_PCM_RATE_8000_48000,
			.formats = (SNDRV_PCM_FMTBIT_S16_LE |
				    SNDRV_PCM_FMTBIT_S24_LE),
			.channels_min = 1,
			.channels_max = 8,
			.rate_min = 8000,
			.rate_max = 48000,
		},
		.ops = &msm_fe_dai_ops,
		.name = "PRI_TDM_TX_5_HOSTLESS",
		
	},
	{
		.playback = {
			.stream_name = "Primary TDM5 Hostless Playback",

			.rates = SNDRV_PCM_RATE_8000_48000,
			.formats = (SNDRV_PCM_FMTBIT_S16_LE |
				    SNDRV_PCM_FMTBIT_S24_LE),
			.channels_min = 1,
			.channels_max = 8,
			.rate_min = 8000,
			.rate_max = 48000,
		},
		.ops = &msm_fe_dai_ops,
		.name = "PRI_TDM_RX_5_HOSTLESS",
		
	},
	{
		.capture = {
			.stream_name = "Primary TDM6 Hostless Capture",

			.rates = SNDRV_PCM_RATE_8000_48000,
			.formats = (SNDRV_PCM_FMTBIT_S16_LE |
				    SNDRV_PCM_FMTBIT_S24_LE),
			.channels_min = 1,
			.channels_max = 8,
			.rate_min = 8000,
			.rate_max = 48000,
		},
		.ops = &msm_fe_dai_ops,
		.name = "PRI_TDM_TX_6_HOSTLESS",
		
	},
	{
		.playback = {
			.stream_name = "Primary TDM6 Hostless Playback",

			.rates = SNDRV_PCM_RATE_8000_48000,
			.formats = (SNDRV_PCM_FMTBIT_S16_LE |
				    SNDRV_PCM_FMTBIT_S24_LE),
			.channels_min = 1,
			.channels_max = 8,
			.rate_min = 8000,
			.rate_max = 48000,
		},
		.ops = &msm_fe_dai_ops,
		.name = "PRI_TDM_RX_6_HOSTLESS",
		
	},
	{
		.capture = {
			.stream_name = "Primary TDM7 Hostless Capture",

			.rates = SNDRV_PCM_RATE_8000_48000,
			.formats = (SNDRV_PCM_FMTBIT_S16_LE |
				    SNDRV_PCM_FMTBIT_S24_LE),
			.channels_min = 1,
			.channels_max = 8,
			.rate_min = 8000,
			.rate_max = 48000,
		},
		.ops = &msm_fe_dai_ops,
		.name = "PRI_TDM_TX_7_HOSTLESS",
		
	},
	{
		.playback = {
			.stream_name = "Primary TDM7 Hostless Playback",

			.rates = SNDRV_PCM_RATE_8000_48000,
			.formats = (SNDRV_PCM_FMTBIT_S16_LE |
				    SNDRV_PCM_FMTBIT_S24_LE),
			.channels_min = 1,
			.channels_max = 8,
			.rate_min = 8000,
			.rate_max = 48000,
		},
		.ops = &msm_fe_dai_ops,
		.name = "PRI_TDM_RX_7_HOSTLESS",
		
	},
	{
		.capture = {
			.stream_name = "Secondary TDM0 Hostless Capture",

			.rates = SNDRV_PCM_RATE_8000_48000,
			.formats = (SNDRV_PCM_FMTBIT_S16_LE |
				    SNDRV_PCM_FMTBIT_S24_LE),
			.channels_min = 1,
			.channels_max = 8,
			.rate_min = 8000,
			.rate_max = 48000,
		},
		.ops = &msm_fe_dai_ops,
		.name = "SEC_TDM_TX_0_HOSTLESS",
		
	},
	{
		.playback = {
			.stream_name = "Secondary TDM0 Hostless Playback",

			.rates = SNDRV_PCM_RATE_8000_48000,
			.formats = (SNDRV_PCM_FMTBIT_S16_LE |
				    SNDRV_PCM_FMTBIT_S24_LE),
			.channels_min = 1,
			.channels_max = 8,
			.rate_min = 8000,
			.rate_max = 48000,
		},
		.ops = &msm_fe_dai_ops,
		.name = "SEC_TDM_RX_0_HOSTLESS",
		
	},
	{
		.capture = {
			.stream_name = "Secondary TDM1 Hostless Capture",

			.rates = SNDRV_PCM_RATE_8000_48000,
			.formats = (SNDRV_PCM_FMTBIT_S16_LE |
				    SNDRV_PCM_FMTBIT_S24_LE),
			.channels_min = 1,
			.channels_max = 8,
			.rate_min = 8000,
			.rate_max = 48000,
		},
		.ops = &msm_fe_dai_ops,
		.name = "SEC_TDM_TX_1_HOSTLESS",
		
	},
	{
		.playback = {
			.stream_name = "Secondary TDM1 Hostless Playback",

			.rates = SNDRV_PCM_RATE_8000_48000,
			.formats = (SNDRV_PCM_FMTBIT_S16_LE |
				    SNDRV_PCM_FMTBIT_S24_LE),
			.channels_min = 1,
			.channels_max = 8,
			.rate_min = 8000,
			.rate_max = 48000,
		},
		.ops = &msm_fe_dai_ops,
		.name = "SEC_TDM_RX_1_HOSTLESS",
		
	},
	{
		.capture = {
			.stream_name = "Secondary TDM2 Hostless Capture",

			.rates = SNDRV_PCM_RATE_8000_48000,
			.formats = (SNDRV_PCM_FMTBIT_S16_LE |
				    SNDRV_PCM_FMTBIT_S24_LE),
			.channels_min = 1,
			.channels_max = 8,
			.rate_min = 8000,
			.rate_max = 48000,
		},
		.ops = &msm_fe_dai_ops,
		.name = "SEC_TDM_TX_2_HOSTLESS",
		
	},
	{
		.playback = {
			.stream_name = "Secondary TDM2 Hostless Playback",

			.rates = SNDRV_PCM_RATE_8000_48000,
			.formats = (SNDRV_PCM_FMTBIT_S16_LE |
				    SNDRV_PCM_FMTBIT_S24_LE),
			.channels_min = 1,
			.channels_max = 8,
			.rate_min = 8000,
			.rate_max = 48000,
		},
		.ops = &msm_fe_dai_ops,
		.name = "SEC_TDM_RX_2_HOSTLESS",
		
	},
	{
		.capture = {
			.stream_name = "Secondary TDM3 Hostless Capture",

			.rates = SNDRV_PCM_RATE_8000_48000,
			.formats = (SNDRV_PCM_FMTBIT_S16_LE |
				    SNDRV_PCM_FMTBIT_S24_LE),
			.channels_min = 1,
			.channels_max = 8,
			.rate_min = 8000,
			.rate_max = 48000,
		},
		.ops = &msm_fe_dai_ops,
		.name = "SEC_TDM_TX_3_HOSTLESS",
		
	},
	{
		.playback = {
			.stream_name = "Secondary TDM3 Hostless Playback",

			.rates = SNDRV_PCM_RATE_8000_48000,
			.formats = (SNDRV_PCM_FMTBIT_S16_LE |
				    SNDRV_PCM_FMTBIT_S24_LE),
			.channels_min = 1,
			.channels_max = 8,
			.rate_min = 8000,
			.rate_max = 48000,
		},
		.ops = &msm_fe_dai_ops,
		.name = "SEC_TDM_RX_3_HOSTLESS",
		
	},
	{
		.capture = {
			.stream_name = "Secondary TDM4 Hostless Capture",

			.rates = SNDRV_PCM_RATE_8000_48000,
			.formats = (SNDRV_PCM_FMTBIT_S16_LE |
				    SNDRV_PCM_FMTBIT_S24_LE),
			.channels_min = 1,
			.channels_max = 8,
			.rate_min = 8000,
			.rate_max = 48000,
		},
		.ops = &msm_fe_dai_ops,
		.name = "SEC_TDM_TX_4_HOSTLESS",
		
	},
	{
		.playback = {
			.stream_name = "Secondary TDM4 Hostless Playback",

			.rates = SNDRV_PCM_RATE_8000_48000,
			.formats = (SNDRV_PCM_FMTBIT_S16_LE |
				    SNDRV_PCM_FMTBIT_S24_LE),
			.channels_min = 1,
			.channels_max = 8,
			.rate_min = 8000,
			.rate_max = 48000,
		},
		.ops = &msm_fe_dai_ops,
		.name = "SEC_TDM_RX_4_HOSTLESS",
		
	},
	{
		.capture = {
			.stream_name = "Secondary TDM5 Hostless Capture",

			.rates = SNDRV_PCM_RATE_8000_48000,
			.formats = (SNDRV_PCM_FMTBIT_S16_LE |
				    SNDRV_PCM_FMTBIT_S24_LE),
			.channels_min = 1,
			.channels_max = 8,
			.rate_min = 8000,
			.rate_max = 48000,
		},
		.ops = &msm_fe_dai_ops,
		.name = "SEC_TDM_TX_5_HOSTLESS",
		
	},
	{
		.playback = {
			.stream_name = "Secondary TDM5 Hostless Playback",

			.rates = SNDRV_PCM_RATE_8000_48000,
			.formats = (SNDRV_PCM_FMTBIT_S16_LE |
				    SNDRV_PCM_FMTBIT_S24_LE),
			.channels_min = 1,
			.channels_max = 8,
			.rate_min = 8000,
			.rate_max = 48000,
		},
		.ops = &msm_fe_dai_ops,
		.name = "SEC_TDM_RX_5_HOSTLESS",
		
	},
	{
		.capture = {
			.stream_name = "Secondary TDM6 Hostless Capture",

			.rates = SNDRV_PCM_RATE_8000_48000,
			.formats = (SNDRV_PCM_FMTBIT_S16_LE |
				    SNDRV_PCM_FMTBIT_S24_LE),
			.channels_min = 1,
			.channels_max = 8,
			.rate_min = 8000,
			.rate_max = 48000,
		},
		.ops = &msm_fe_dai_ops,
		.name = "SEC_TDM_TX_6_HOSTLESS",
		
	},
	{
		.playback = {
			.stream_name = "Secondary TDM6 Hostless Playback",

			.rates = SNDRV_PCM_RATE_8000_48000,
			.formats = (SNDRV_PCM_FMTBIT_S16_LE |
				    SNDRV_PCM_FMTBIT_S24_LE),
			.channels_min = 1,
			.channels_max = 8,
			.rate_min = 8000,
			.rate_max = 48000,
		},
		.ops = &msm_fe_dai_ops,
		.name = "SEC_TDM_RX_6_HOSTLESS",
		
	},
	{
		.capture = {
			.stream_name = "Secondary TDM7 Hostless Capture",

			.rates = SNDRV_PCM_RATE_8000_48000,
			.formats = (SNDRV_PCM_FMTBIT_S16_LE |
				    SNDRV_PCM_FMTBIT_S24_LE),
			.channels_min = 1,
			.channels_max = 8,
			.rate_min = 8000,
			.rate_max = 48000,
		},
		.ops = &msm_fe_dai_ops,
		.name = "SEC_TDM_TX_7_HOSTLESS",
		
	},
	{
		.playback = {
			.stream_name = "Secondary TDM7 Hostless Playback",

			.rates = SNDRV_PCM_RATE_8000_48000,
			.formats = (SNDRV_PCM_FMTBIT_S16_LE |
				    SNDRV_PCM_FMTBIT_S24_LE),
			.channels_min = 1,
			.channels_max = 8,
			.rate_min = 8000,
			.rate_max = 48000,
		},
		.ops = &msm_fe_dai_ops,
		.name = "SEC_TDM_RX_7_HOSTLESS",
		
	},
	{
		.capture = {
			.stream_name = "Tertiary TDM0 Hostless Capture",

			.rates = SNDRV_PCM_RATE_8000_48000,
			.formats = (SNDRV_PCM_FMTBIT_S16_LE |
				    SNDRV_PCM_FMTBIT_S24_LE),
			.channels_min = 1,
			.channels_max = 8,
			.rate_min = 8000,
			.rate_max = 48000,
		},
		.ops = &msm_fe_dai_ops,
		.name = "TERT_TDM_TX_0_HOSTLESS",
		
	},
	{
		.playback = {
			.stream_name = "Tertiary TDM0 Hostless Playback",

			.rates = SNDRV_PCM_RATE_8000_48000,
			.formats = (SNDRV_PCM_FMTBIT_S16_LE |
				    SNDRV_PCM_FMTBIT_S24_LE),
			.channels_min = 1,
			.channels_max = 8,
			.rate_min = 8000,
			.rate_max = 48000,
		},
		.ops = &msm_fe_dai_ops,
		.name = "TERT_TDM_RX_0_HOSTLESS",
		
	},
	{
		.capture = {
			.stream_name = "Tertiary TDM1 Hostless Capture",

			.rates = SNDRV_PCM_RATE_8000_48000,
			.formats = (SNDRV_PCM_FMTBIT_S16_LE |
				    SNDRV_PCM_FMTBIT_S24_LE),
			.channels_min = 1,
			.channels_max = 8,
			.rate_min = 8000,
			.rate_max = 48000,
		},
		.ops = &msm_fe_dai_ops,
		.name = "TERT_TDM_TX_1_HOSTLESS",
		
	},
	{
		.playback = {
			.stream_name = "Tertiary TDM1 Hostless Playback",

			.rates = SNDRV_PCM_RATE_8000_48000,
			.formats = (SNDRV_PCM_FMTBIT_S16_LE |
				    SNDRV_PCM_FMTBIT_S24_LE),
			.channels_min = 1,
			.channels_max = 8,
			.rate_min = 8000,
			.rate_max = 48000,
		},
		.ops = &msm_fe_dai_ops,
		.name = "TERT_TDM_RX_1_HOSTLESS",
		
	},
	{
		.capture = {
			.stream_name = "Tertiary TDM2 Hostless Capture",

			.rates = SNDRV_PCM_RATE_8000_48000,
			.formats = (SNDRV_PCM_FMTBIT_S16_LE |
				    SNDRV_PCM_FMTBIT_S24_LE),
			.channels_min = 1,
			.channels_max = 8,
			.rate_min = 8000,
			.rate_max = 48000,
		},
		.ops = &msm_fe_dai_ops,
		.name = "TERT_TDM_TX_2_HOSTLESS",
		
	},
	{
		.playback = {
			.stream_name = "Tertiary TDM2 Hostless Playback",

			.rates = SNDRV_PCM_RATE_8000_48000,
			.formats = (SNDRV_PCM_FMTBIT_S16_LE |
				    SNDRV_PCM_FMTBIT_S24_LE),
			.channels_min = 1,
			.channels_max = 8,
			.rate_min = 8000,
			.rate_max = 48000,
		},
		.ops = &msm_fe_dai_ops,
		.name = "TERT_TDM_RX_2_HOSTLESS",
		
	},
	{
		.capture = {
			.stream_name = "Tertiary TDM3 Hostless Capture",

			.rates = SNDRV_PCM_RATE_8000_48000,
			.formats = (SNDRV_PCM_FMTBIT_S16_LE |
				    SNDRV_PCM_FMTBIT_S24_LE),
			.channels_min = 1,
			.channels_max = 8,
			.rate_min = 8000,
			.rate_max = 48000,
		},
		.ops = &msm_fe_dai_ops,
		.name = "TERT_TDM_TX_3_HOSTLESS",
		
	},
	{
		.playback = {
			.stream_name = "Tertiary TDM3 Hostless Playback",

			.rates = SNDRV_PCM_RATE_8000_48000,
			.formats = (SNDRV_PCM_FMTBIT_S16_LE |
				    SNDRV_PCM_FMTBIT_S24_LE),
			.channels_min = 1,
			.channels_max = 8,
			.rate_min = 8000,
			.rate_max = 48000,
		},
		.ops = &msm_fe_dai_ops,
		.name = "TERT_TDM_RX_3_HOSTLESS",
		
	},
	{
		.capture = {
			.stream_name = "Tertiary TDM4 Hostless Capture",

			.rates = SNDRV_PCM_RATE_8000_48000,
			.formats = (SNDRV_PCM_FMTBIT_S16_LE |
				    SNDRV_PCM_FMTBIT_S24_LE),
			.channels_min = 1,
			.channels_max = 8,
			.rate_min = 8000,
			.rate_max = 48000,
		},
		.ops = &msm_fe_dai_ops,
		.name = "TERT_TDM_TX_4_HOSTLESS",
		
	},
	{
		.playback = {
			.stream_name = "Tertiary TDM4 Hostless Playback",

			.rates = SNDRV_PCM_RATE_8000_48000,
			.formats = (SNDRV_PCM_FMTBIT_S16_LE |
				    SNDRV_PCM_FMTBIT_S24_LE),
			.channels_min = 1,
			.channels_max = 8,
			.rate_min = 8000,
			.rate_max = 48000,
		},
		.ops = &msm_fe_dai_ops,
		.name = "TERT_TDM_RX_4_HOSTLESS",
		
	},
	{
		.capture = {
			.stream_name = "Tertiary TDM5 Hostless Capture",

			.rates = SNDRV_PCM_RATE_8000_48000,
			.formats = (SNDRV_PCM_FMTBIT_S16_LE |
				    SNDRV_PCM_FMTBIT_S24_LE),
			.channels_min = 1,
			.channels_max = 8,
			.rate_min = 8000,
			.rate_max = 48000,
		},
		.ops = &msm_fe_dai_ops,
		.name = "TERT_TDM_TX_5_HOSTLESS",
		
	},
	{
		.playback = {
			.stream_name = "Tertiary TDM5 Hostless Playback",

			.rates = SNDRV_PCM_RATE_8000_48000,
			.formats = (SNDRV_PCM_FMTBIT_S16_LE |
				    SNDRV_PCM_FMTBIT_S24_LE),
			.channels_min = 1,
			.channels_max = 8,
			.rate_min = 8000,
			.rate_max = 48000,
		},
		.ops = &msm_fe_dai_ops,
		.name = "TERT_TDM_RX_5_HOSTLESS",
		
	},
	{
		.capture = {
			.stream_name = "Tertiary TDM6 Hostless Capture",

			.rates = SNDRV_PCM_RATE_8000_48000,
			.formats = (SNDRV_PCM_FMTBIT_S16_LE |
				    SNDRV_PCM_FMTBIT_S24_LE),
			.channels_min = 1,
			.channels_max = 8,
			.rate_min = 8000,
			.rate_max = 48000,
		},
		.ops = &msm_fe_dai_ops,
		.name = "TERT_TDM_TX_6_HOSTLESS",
		
	},
	{
		.playback = {
			.stream_name = "Tertiary TDM6 Hostless Playback",

			.rates = SNDRV_PCM_RATE_8000_48000,
			.formats = (SNDRV_PCM_FMTBIT_S16_LE |
				    SNDRV_PCM_FMTBIT_S24_LE),
			.channels_min = 1,
			.channels_max = 8,
			.rate_min = 8000,
			.rate_max = 48000,
		},
		.ops = &msm_fe_dai_ops,
		.name = "TERT_TDM_RX_6_HOSTLESS",
		
	},
	{
		.capture = {
			.stream_name = "Tertiary TDM7 Hostless Capture",

			.rates = SNDRV_PCM_RATE_8000_48000,
			.formats = (SNDRV_PCM_FMTBIT_S16_LE |
				    SNDRV_PCM_FMTBIT_S24_LE),
			.channels_min = 1,
			.channels_max = 8,
			.rate_min = 8000,
			.rate_max = 48000,
		},
		.ops = &msm_fe_dai_ops,
		.name = "TERT_TDM_TX_7_HOSTLESS",
		
	},
	{
		.playback = {
			.stream_name = "Tertiary TDM7 Hostless Playback",

			.rates = SNDRV_PCM_RATE_8000_48000,
			.formats = (SNDRV_PCM_FMTBIT_S16_LE |
				    SNDRV_PCM_FMTBIT_S24_LE),
			.channels_min = 1,
			.channels_max = 8,
			.rate_min = 8000,
			.rate_max = 48000,
		},
		.ops = &msm_fe_dai_ops,
		.name = "TERT_TDM_RX_7_HOSTLESS",
		
	},
	{
		.capture = {
			.stream_name = "Quaternary TDM0 Hostless Capture",

			.rates = SNDRV_PCM_RATE_8000_48000,
			.formats = (SNDRV_PCM_FMTBIT_S16_LE |
				    SNDRV_PCM_FMTBIT_S24_LE),
			.channels_min = 1,
			.channels_max = 8,
			.rate_min = 8000,
			.rate_max = 48000,
		},
		.ops = &msm_fe_dai_ops,
		.name = "QUAT_TDM_TX_0_HOSTLESS",
		
	},
	{
		.playback = {
			.stream_name = "Quaternary TDM0 Hostless Playback",

			.rates = SNDRV_PCM_RATE_8000_48000,
			.formats = (SNDRV_PCM_FMTBIT_S16_LE |
				    SNDRV_PCM_FMTBIT_S24_LE),
			.channels_min = 1,
			.channels_max = 8,
			.rate_min = 8000,
			.rate_max = 48000,
		},
		.ops = &msm_fe_dai_ops,
		.name = "QUAT_TDM_RX_0_HOSTLESS",
		
	},
	{
		.capture = {
			.stream_name = "Quaternary TDM1 Hostless Capture",

			.rates = SNDRV_PCM_RATE_8000_48000,
			.formats = (SNDRV_PCM_FMTBIT_S16_LE |
				    SNDRV_PCM_FMTBIT_S24_LE),
			.channels_min = 1,
			.channels_max = 8,
			.rate_min = 8000,
			.rate_max = 48000,
		},
		.ops = &msm_fe_dai_ops,
		.name = "QUAT_TDM_TX_1_HOSTLESS",
		
	},
	{
		.playback = {
			.stream_name = "Quaternary TDM1 Hostless Playback",

			.rates = SNDRV_PCM_RATE_8000_48000,
			.formats = (SNDRV_PCM_FMTBIT_S16_LE |
				    SNDRV_PCM_FMTBIT_S24_LE),
			.channels_min = 1,
			.channels_max = 8,
			.rate_min = 8000,
			.rate_max = 48000,
		},
		.ops = &msm_fe_dai_ops,
		.name = "QUAT_TDM_RX_1_HOSTLESS",
		
	},
	{
		.capture = {
			.stream_name = "Quaternary TDM2 Hostless Capture",

			.rates = SNDRV_PCM_RATE_8000_48000,
			.formats = (SNDRV_PCM_FMTBIT_S16_LE |
				    SNDRV_PCM_FMTBIT_S24_LE),
			.channels_min = 1,
			.channels_max = 8,
			.rate_min = 8000,
			.rate_max = 48000,
		},
		.ops = &msm_fe_dai_ops,
		.name = "QUAT_TDM_TX_2_HOSTLESS",
		
	},
	{
		.playback = {
			.stream_name = "Quaternary TDM2 Hostless Playback",

			.rates = SNDRV_PCM_RATE_8000_48000,
			.formats = (SNDRV_PCM_FMTBIT_S16_LE |
				    SNDRV_PCM_FMTBIT_S24_LE),
			.channels_min = 1,
			.channels_max = 8,
			.rate_min = 8000,
			.rate_max = 48000,
		},
		.ops = &msm_fe_dai_ops,
		.name = "QUAT_TDM_RX_2_HOSTLESS",
		
	},
	{
		.capture = {
			.stream_name = "Quaternary TDM3 Hostless Capture",

			.rates = SNDRV_PCM_RATE_8000_48000,
			.formats = (SNDRV_PCM_FMTBIT_S16_LE |
				    SNDRV_PCM_FMTBIT_S24_LE),
			.channels_min = 1,
			.channels_max = 8,
			.rate_min = 8000,
			.rate_max = 48000,
		},
		.ops = &msm_fe_dai_ops,
		.name = "QUAT_TDM_TX_3_HOSTLESS",
		
	},
	{
		.playback = {
			.stream_name = "Quaternary TDM3 Hostless Playback",

			.rates = SNDRV_PCM_RATE_8000_48000,
			.formats = (SNDRV_PCM_FMTBIT_S16_LE |
				    SNDRV_PCM_FMTBIT_S24_LE),
			.channels_min = 1,
			.channels_max = 8,
			.rate_min = 8000,
			.rate_max = 48000,
		},
		.ops = &msm_fe_dai_ops,
		.name = "QUAT_TDM_RX_3_HOSTLESS",
		
	},
	{
		.capture = {
			.stream_name = "Quaternary TDM4 Hostless Capture",

			.rates = SNDRV_PCM_RATE_8000_48000,
			.formats = (SNDRV_PCM_FMTBIT_S16_LE |
				    SNDRV_PCM_FMTBIT_S24_LE),
			.channels_min = 1,
			.channels_max = 8,
			.rate_min = 8000,
			.rate_max = 48000,
		},
		.ops = &msm_fe_dai_ops,
		.name = "QUAT_TDM_TX_4_HOSTLESS",
		
	},
	{
		.playback = {
			.stream_name = "Quaternary TDM4 Hostless Playback",

			.rates = SNDRV_PCM_RATE_8000_48000,
			.formats = (SNDRV_PCM_FMTBIT_S16_LE |
				    SNDRV_PCM_FMTBIT_S24_LE),
			.channels_min = 1,
			.channels_max = 8,
			.rate_min = 8000,
			.rate_max = 48000,
		},
		.ops = &msm_fe_dai_ops,
		.name = "QUAT_TDM_RX_4_HOSTLESS",
		
	},
	{
		.capture = {
			.stream_name = "Quaternary TDM5 Hostless Capture",

			.rates = SNDRV_PCM_RATE_8000_48000,
			.formats = (SNDRV_PCM_FMTBIT_S16_LE |
				    SNDRV_PCM_FMTBIT_S24_LE),
			.channels_min = 1,
			.channels_max = 8,
			.rate_min = 8000,
			.rate_max = 48000,
		},
		.ops = &msm_fe_dai_ops,
		.name = "QUAT_TDM_TX_5_HOSTLESS",
		
	},
	{
		.playback = {
			.stream_name = "Quaternary TDM5 Hostless Playback",

			.rates = SNDRV_PCM_RATE_8000_48000,
			.formats = (SNDRV_PCM_FMTBIT_S16_LE |
				    SNDRV_PCM_FMTBIT_S24_LE),
			.channels_min = 1,
			.channels_max = 8,
			.rate_min = 8000,
			.rate_max = 48000,
		},
		.ops = &msm_fe_dai_ops,
		.name = "QUAT_TDM_RX_5_HOSTLESS",
		
	},
	{
		.capture = {
			.stream_name = "Quaternary TDM6 Hostless Capture",

			.rates = SNDRV_PCM_RATE_8000_48000,
			.formats = (SNDRV_PCM_FMTBIT_S16_LE |
				    SNDRV_PCM_FMTBIT_S24_LE),
			.channels_min = 1,
			.channels_max = 8,
			.rate_min = 8000,
			.rate_max = 48000,
		},
		.ops = &msm_fe_dai_ops,
		.name = "QUAT_TDM_TX_6_HOSTLESS",
		
	},
	{
		.playback = {
			.stream_name = "Quaternary TDM6 Hostless Playback",

			.rates = SNDRV_PCM_RATE_8000_48000,
			.formats = (SNDRV_PCM_FMTBIT_S16_LE |
				    SNDRV_PCM_FMTBIT_S24_LE),
			.channels_min = 1,
			.channels_max = 8,
			.rate_min = 8000,
			.rate_max = 48000,
		},
		.ops = &msm_fe_dai_ops,
		.name = "QUAT_TDM_RX_6_HOSTLESS",
		
	},
	{
		.capture = {
			.stream_name = "Quaternary TDM7 Hostless Capture",

			.rates = SNDRV_PCM_RATE_8000_48000,
			.formats = (SNDRV_PCM_FMTBIT_S16_LE |
				    SNDRV_PCM_FMTBIT_S24_LE),
			.channels_min = 1,
			.channels_max = 16,
			.rate_min = 8000,
			.rate_max = 48000,
		},
		.ops = &msm_fe_dai_ops,
		.name = "QUAT_TDM_TX_7_HOSTLESS",
		
	},
	{
		.playback = {
			.stream_name = "Quaternary TDM7 Hostless Playback",

			.rates = SNDRV_PCM_RATE_8000_48000,
			.formats = (SNDRV_PCM_FMTBIT_S16_LE |
				    SNDRV_PCM_FMTBIT_S24_LE),
			.channels_min = 1,
			.channels_max = 16,
			.rate_min = 8000,
			.rate_max = 48000,
		},
		.ops = &msm_fe_dai_ops,
		.name = "QUAT_TDM_RX_7_HOSTLESS",
		
	},
	{
		.capture = {
			.stream_name = "Quinary TDM7 Hostless Capture",

			.rates = SNDRV_PCM_RATE_8000_48000,
			.formats = (SNDRV_PCM_FMTBIT_S16_LE |
				    SNDRV_PCM_FMTBIT_S24_LE),
			.channels_min = 1,
			.channels_max = 16,
			.rate_min = 8000,
			.rate_max = 48000,
		},
		.ops = &msm_fe_dai_ops,
		.name = "QUIN_TDM_TX_7_HOSTLESS",
		
	},
	{
		.playback = {
			.stream_name = "Quinary TDM7 Hostless Playback",

			.rates = SNDRV_PCM_RATE_8000_48000,
			.formats = (SNDRV_PCM_FMTBIT_S16_LE |
				    SNDRV_PCM_FMTBIT_S24_LE),
			.channels_min = 1,
			.channels_max = 16,
			.rate_min = 8000,
			.rate_max = 48000,
		},
		.ops = &msm_fe_dai_ops,
		.name = "QUIN_TDM_RX_7_HOSTLESS",
		
	},
	{
		.playback = {
			.stream_name = "DTMF_RX_HOSTLESS Playback",

			.rates = SNDRV_PCM_RATE_8000_48000,
			.formats = SNDRV_PCM_FMTBIT_S16_LE,
			.channels_min = 1,
			.channels_max = 2,
			.rate_min =	8000,
			.rate_max = 48000,
		},
		.ops = &msm_fe_dai_ops,
		.name = "DTMF_RX_HOSTLESS",
		
	},
	{
		.capture = {
			.stream_name = "CPE Listen Audio capture",

			.rates = SNDRV_PCM_RATE_8000_48000,
			.formats = (SNDRV_PCM_FMTBIT_S16_LE |
				    SNDRV_PCM_FMTBIT_S24_LE),
			.channels_min = 1,
			.channels_max = 1,
			.rate_min = 8000,
			.rate_max = 48000,
		},
		.ops = &msm_fe_dai_ops,
		.name = "CPE_LSM_NOHOST",
	},
	{
		.playback = {
			.stream_name = "VOLTE_STUB Playback",

			.rates = SNDRV_PCM_RATE_8000_48000,
			.formats = SNDRV_PCM_FMTBIT_S16_LE,
			.channels_min = 1,
			.channels_max = 2,
			.rate_min = 8000,
			.rate_max = 48000,
		},
		.capture = {
			.stream_name = "VOLTE_STUB Capture",

			.rates = SNDRV_PCM_RATE_8000_48000,
			.formats = SNDRV_PCM_FMTBIT_S16_LE,
			.channels_min = 1,
			.channels_max = 2,
			.rate_min = 8000,
			.rate_max = 48000,
		},
		.ops = &msm_fe_dai_ops,
		.name = "VOLTE_STUB",
		
	},
	{
		.playback = {
			.stream_name = "VOICE2_STUB Playback",

			.rates = SNDRV_PCM_RATE_8000_48000,
			.formats = SNDRV_PCM_FMTBIT_S16_LE,
			.channels_min = 1,
			.channels_max = 2,
			.rate_min = 8000,
			.rate_max = 48000,
		},
		.capture = {
			.stream_name = "VOICE2_STUB Capture",

			.rates = SNDRV_PCM_RATE_8000_48000,
			.formats = SNDRV_PCM_FMTBIT_S16_LE,
			.channels_min = 1,
			.channels_max = 2,
			.rate_min = 8000,
			.rate_max = 48000,
		},
		.ops = &msm_fe_dai_ops,
		.name = "VOICE2_STUB",
		
	},
	{
		.playback = {
			.stream_name = "MultiMedia9 Playback",

			.rates = (SNDRV_PCM_RATE_8000_384000|
				  SNDRV_PCM_RATE_KNOT),
			.formats = (SNDRV_PCM_FMTBIT_S16_LE |
				    SNDRV_PCM_FMTBIT_S24_LE |
				    SNDRV_PCM_FMTBIT_S24_3LE),
			.channels_min = 1,
			.channels_max = 32,
			.rate_min =     8000,
			.rate_max =	384000,
		},
		.capture = {
			.stream_name = "MultiMedia9 Capture",

			.rates = (SNDRV_PCM_RATE_8000_48000|
				  SNDRV_PCM_RATE_KNOT),
			.formats = SNDRV_PCM_FMTBIT_S16_LE,
			.channels_min = 1,
			.channels_max = 32,
			.rate_min =     8000,
			.rate_max =	48000,
		},
		.ops = &msm_fe_Multimedia_dai_ops,
		.name = "MultiMedia9",
		
	},
	{
		.playback = {
			.stream_name = "QCHAT Playback",

			.rates = SNDRV_PCM_RATE_8000_48000,
			.formats = SNDRV_PCM_FMTBIT_S16_LE,
			.channels_min = 1,
			.channels_max = 2,
			.rate_min = 8000,
			.rate_max = 48000,
		},
		.capture = {
			.stream_name = "QCHAT Capture",

			.rates = SNDRV_PCM_RATE_8000_48000,
			.formats = SNDRV_PCM_FMTBIT_S16_LE,
			.channels_min = 1,
			.channels_max = 2,
			.rate_min = 8000,
			.rate_max = 48000,
		},
		.ops = &msm_fe_dai_ops,
		.name = "QCHAT",
		
	},
	{
		.capture = {
			.stream_name = "Listen 1 Audio Service Capture",

			.rates = (SNDRV_PCM_RATE_16000 |
				  SNDRV_PCM_RATE_48000),
			.formats = (SNDRV_PCM_FMTBIT_S16_LE |
				    SNDRV_PCM_FMTBIT_S24_LE),
			.channels_min = 1,
			.channels_max = 4,
			.rate_min = 16000,
			.rate_max = 48000,
		},
		.ops = &msm_fe_dai_ops,
		.name = "LSM1",
		
	},
	{
		.capture = {
			.stream_name = "Listen 2 Audio Service Capture",

			.rates = (SNDRV_PCM_RATE_16000 |
				  SNDRV_PCM_RATE_48000),
			.formats = (SNDRV_PCM_FMTBIT_S16_LE |
				    SNDRV_PCM_FMTBIT_S24_LE),
			.channels_min = 1,
			.channels_max = 4,
			.rate_min = 16000,
			.rate_max = 48000,
		},
		.ops = &msm_fe_dai_ops,
		.name = "LSM2",
		
	},
	{
		.capture = {
			.stream_name = "Listen 3 Audio Service Capture",

			.rates = (SNDRV_PCM_RATE_16000 |
				  SNDRV_PCM_RATE_48000),
			.formats = (SNDRV_PCM_FMTBIT_S16_LE |
				    SNDRV_PCM_FMTBIT_S24_LE),
			.channels_min = 1,
			.channels_max = 4,
			.rate_min = 16000,
			.rate_max = 48000,
		},
		.ops = &msm_fe_dai_ops,
		.name = "LSM3",
		
	},
	{
		.capture = {
			.stream_name = "Listen 4 Audio Service Capture",

			.rates = (SNDRV_PCM_RATE_16000 |
				  SNDRV_PCM_RATE_48000),
			.formats = (SNDRV_PCM_FMTBIT_S16_LE |
				    SNDRV_PCM_FMTBIT_S24_LE),
			.channels_min = 1,
			.channels_max = 4,
			.rate_min = 16000,
			.rate_max = 48000,
		},
		.ops = &msm_fe_dai_ops,
		.name = "LSM4",
		
	},
	{
		.capture = {
			.stream_name = "Listen 5 Audio Service Capture",

			.rates = (SNDRV_PCM_RATE_16000 |
				  SNDRV_PCM_RATE_48000),
			.formats = (SNDRV_PCM_FMTBIT_S16_LE |
				    SNDRV_PCM_FMTBIT_S24_LE),
			.channels_min = 1,
			.channels_max = 4,
			.rate_min = 16000,
			.rate_max = 48000,
		},
		.ops = &msm_fe_dai_ops,
		.name = "LSM5",
		
	},
	{
		.capture = {
			.stream_name = "Listen 6 Audio Service Capture",

			.rates = (SNDRV_PCM_RATE_16000 |
				  SNDRV_PCM_RATE_48000),
			.formats = (SNDRV_PCM_FMTBIT_S16_LE |
				    SNDRV_PCM_FMTBIT_S24_LE),
			.channels_min = 1,
			.channels_max = 4,
			.rate_min = 16000,
			.rate_max = 48000,
		},
		.ops = &msm_fe_dai_ops,
		.name = "LSM6",
		
	},
	{
		.capture = {
			.stream_name = "Listen 7 Audio Service Capture",

			.rates = (SNDRV_PCM_RATE_16000 |
				  SNDRV_PCM_RATE_48000),
			.formats = (SNDRV_PCM_FMTBIT_S16_LE |
				    SNDRV_PCM_FMTBIT_S24_LE),
			.channels_min = 1,
			.channels_max = 4,
			.rate_min = 16000,
			.rate_max = 48000,
		},
		.ops = &msm_fe_dai_ops,
		.name = "LSM7",
		
	},
	{
		.capture = {
			.stream_name = "Listen 8 Audio Service Capture",

			.rates = (SNDRV_PCM_RATE_16000 |
				  SNDRV_PCM_RATE_48000),
			.formats = (SNDRV_PCM_FMTBIT_S16_LE |
				    SNDRV_PCM_FMTBIT_S24_LE),
			.channels_min = 1,
			.channels_max = 4,
			.rate_min = 16000,
			.rate_max = 48000,
		},
		.ops = &msm_fe_dai_ops,
		.name = "LSM8",
		
	},
	/* FE DAIs created for multiple instances of offload playback */
	{
		.playback = {
			.stream_name = "MultiMedia10 Playback",

			.rates = (SNDRV_PCM_RATE_8000_384000 |
				  SNDRV_PCM_RATE_KNOT),
			.formats = (SNDRV_PCM_FMTBIT_S16_LE |
				    SNDRV_PCM_FMTBIT_S24_LE |
				    SNDRV_PCM_FMTBIT_S24_3LE |
				    SNDRV_PCM_FMTBIT_S32_LE),
			.channels_min = 1,
			.channels_max = 32,
			.rate_min =	8000,
			.rate_max = 384000,
		},
		.capture = {
			.stream_name = "MultiMedia10 Capture",

			.rates = (SNDRV_PCM_RATE_8000_48000 |
				  SNDRV_PCM_RATE_KNOT),
			.formats = (SNDRV_PCM_FMTBIT_S16_LE |
				    SNDRV_PCM_FMTBIT_S24_LE |
				    SNDRV_PCM_FMTBIT_S24_3LE),
			.channels_min = 1,
			.channels_max = 8,
			.rate_min = 8000,
			.rate_max = 48000,
		},
		.ops = &msm_fe_Multimedia_dai_ops,
		.name = "MultiMedia10",
		
	},
	{
		.playback = {
			.stream_name = "MultiMedia11 Playback",

			.rates = (SNDRV_PCM_RATE_8000_384000 |
				  SNDRV_PCM_RATE_KNOT),
			.formats = (SNDRV_PCM_FMTBIT_S16_LE |
				    SNDRV_PCM_FMTBIT_S24_LE |
				    SNDRV_PCM_FMTBIT_S24_3LE |
				    SNDRV_PCM_FMTBIT_S32_LE),
			.channels_min = 1,
			.channels_max = 32,
			.rate_min =	8000,
			.rate_max = 384000,
		},
		.ops = &msm_fe_Multimedia_dai_ops_compressed,
		.name = "MultiMedia11",
		
	},
	{
		.playback = {
			.stream_name = "MultiMedia12 Playback",

			.rates = (SNDRV_PCM_RATE_8000_384000 |
				  SNDRV_PCM_RATE_KNOT),
			.formats = (SNDRV_PCM_FMTBIT_S16_LE |
				    SNDRV_PCM_FMTBIT_S24_LE |
				    SNDRV_PCM_FMTBIT_S24_3LE |
				    SNDRV_PCM_FMTBIT_S32_LE),
			.channels_min = 1,
			.channels_max = 32,
			.rate_min =	8000,
			.rate_max = 384000,
		},
		.ops = &msm_fe_Multimedia_dai_ops_compressed,
		.name = "MultiMedia12",
		
	},
	{
		.playback = {
			.stream_name = "MultiMedia13 Playback",

			.rates = (SNDRV_PCM_RATE_8000_384000 |
				  SNDRV_PCM_RATE_KNOT),
			.formats = (SNDRV_PCM_FMTBIT_S16_LE |
				    SNDRV_PCM_FMTBIT_S24_LE |
				    SNDRV_PCM_FMTBIT_S24_3LE |
				    SNDRV_PCM_FMTBIT_S32_LE),
			.channels_min = 1,
			.channels_max = 32,
			.rate_min =	8000,
			.rate_max = 384000,
		},
		.ops = &msm_fe_Multimedia_dai_ops_compressed,
		.name = "MultiMedia13",
		
	},
	{
		.playback = {
			.stream_name = "MultiMedia14 Playback",

			.rates = (SNDRV_PCM_RATE_8000_384000 |
				  SNDRV_PCM_RATE_KNOT),
			.formats = (SNDRV_PCM_FMTBIT_S16_LE |
				    SNDRV_PCM_FMTBIT_S24_LE |
				    SNDRV_PCM_FMTBIT_S24_3LE |
				    SNDRV_PCM_FMTBIT_S32_LE),
			.channels_min = 1,
			.channels_max = 32,
			.rate_min =	8000,
			.rate_max = 384000,
		},
		.ops = &msm_fe_Multimedia_dai_ops_compressed,
		.name = "MultiMedia14",
		
	},
	{
		.playback = {
			.stream_name = "MultiMedia15 Playback",

			.rates = (SNDRV_PCM_RATE_8000_384000 |
				  SNDRV_PCM_RATE_KNOT),
			.formats = (SNDRV_PCM_FMTBIT_S16_LE |
				    SNDRV_PCM_FMTBIT_S24_LE |
				    SNDRV_PCM_FMTBIT_S24_3LE |
				    SNDRV_PCM_FMTBIT_S32_LE),
			.channels_min = 1,
			.channels_max = 32,
			.rate_min =	8000,
			.rate_max = 384000,
		},
		.ops = &msm_fe_Multimedia_dai_ops_compressed,
		.name = "MultiMedia15",
		
	},
	{
		.playback = {
			.stream_name = "MultiMedia16 Playback",

			.rates = (SNDRV_PCM_RATE_8000_384000 |
				  SNDRV_PCM_RATE_KNOT),
			.formats = (SNDRV_PCM_FMTBIT_S16_LE |
				    SNDRV_PCM_FMTBIT_S24_LE |
				    SNDRV_PCM_FMTBIT_S24_3LE |
				    SNDRV_PCM_FMTBIT_S32_LE),
			.channels_min = 1,
			.channels_max = 32,
			.rate_min =	8000,
			.rate_max = 384000,
		},
		.capture = {
			.stream_name = "MultiMedia16 Capture",

			.rates = (SNDRV_PCM_RATE_8000_48000|
					SNDRV_PCM_RATE_KNOT),
			.formats = (SNDRV_PCM_FMTBIT_S16_LE |
				    SNDRV_PCM_FMTBIT_S24_LE |
				    SNDRV_PCM_FMTBIT_S24_3LE |
				    SNDRV_PCM_FMTBIT_S32_LE),
			.channels_min = 1,
			.channels_max = 32,
			.rate_min =     8000,
			.rate_max =     48000,
		},
		.ops = &msm_fe_Multimedia_dai_ops,
		.name = "MultiMedia16",
		
	},
	{
		.playback = {
			.stream_name = "VoiceMMode1 Playback",

			.rates = SNDRV_PCM_RATE_8000_48000,
			.formats = SNDRV_PCM_FMTBIT_S16_LE,
			.channels_min = 1,
			.channels_max = 2,
			.rate_min =     8000,
			.rate_max =     48000,
	 },
		.capture = {
			.stream_name = "VoiceMMode1 Capture",

			.rates = SNDRV_PCM_RATE_8000_48000,
			.formats = SNDRV_PCM_FMTBIT_S16_LE,
			.channels_min = 1,
			.channels_max = 2,
			.rate_min =     8000,
			.rate_max =     48000,
		},
		.ops = &msm_fe_dai_ops,
		.name = "VoiceMMode1",
		
	},
	{
		.playback = {
			.stream_name = "VoiceMMode2 Playback",

			.rates = SNDRV_PCM_RATE_8000_48000,
			.formats = SNDRV_PCM_FMTBIT_S16_LE,
			.channels_min = 1,
			.channels_max = 2,
			.rate_min =     8000,
			.rate_max =     48000,
		},
		.capture = {
			.stream_name = "VoiceMMode2 Capture",

			.rates = SNDRV_PCM_RATE_8000_48000,
			.formats = SNDRV_PCM_FMTBIT_S16_LE,
			.channels_min = 1,
			.channels_max = 2,
			.rate_min =     8000,
			.rate_max =     48000,
		},
		.ops = &msm_fe_dai_ops,
		.name = "VoiceMMode2",
		
	},
	{
		.capture = {
			.stream_name = "MultiMedia17 Capture",

			.rates = (SNDRV_PCM_RATE_8000_192000|
					SNDRV_PCM_RATE_KNOT),
			.formats = (SNDRV_PCM_FMTBIT_S16_LE |
				    SNDRV_PCM_FMTBIT_S24_LE |
				    SNDRV_PCM_FMTBIT_S24_3LE),
			.channels_min = 1,
			.channels_max = 32,
			.rate_min =     8000,
			.rate_max =     192000,
		},
		.ops = &msm_fe_Multimedia_dai_ops_compressed,
		.name = "MultiMedia17",
		
	},
	{
		.capture = {
			.stream_name = "MultiMedia18 Capture",

			.rates = (SNDRV_PCM_RATE_8000_192000|
					SNDRV_PCM_RATE_KNOT),
			.formats = (SNDRV_PCM_FMTBIT_S16_LE |
				    SNDRV_PCM_FMTBIT_S24_LE |
				    SNDRV_PCM_FMTBIT_S24_3LE),
			.channels_min = 1,
			.channels_max = 32,
			.rate_min =     8000,
			.rate_max =     192000,
		},
		.ops = &msm_fe_Multimedia_dai_ops_compressed,
		.name = "MultiMedia18",
		
	},
	{
		.capture = {
			.stream_name = "MultiMedia19 Capture",

			.rates = (SNDRV_PCM_RATE_8000_192000|
					SNDRV_PCM_RATE_KNOT),
			.formats = (SNDRV_PCM_FMTBIT_S16_LE |
				    SNDRV_PCM_FMTBIT_S24_LE |
				    SNDRV_PCM_FMTBIT_S24_3LE),
			.channels_min = 1,
			.channels_max = 32,
			.rate_min =     8000,
			.rate_max =     192000,
		},
		.ops = &msm_fe_Multimedia_dai_ops_compressed,
		.name = "MultiMedia19",
		
	},
	{
		.playback = {
			.stream_name = "MultiMedia20 Playback",

			.rates = (SNDRV_PCM_RATE_8000_384000|
					SNDRV_PCM_RATE_KNOT),
			.formats = (SNDRV_PCM_FMTBIT_S16_LE |
				    SNDRV_PCM_FMTBIT_S24_LE |
				    SNDRV_PCM_FMTBIT_S24_3LE |
				    SNDRV_PCM_FMTBIT_S32_LE),
			.channels_min = 1,
			.channels_max = 32,
			.rate_min =     8000,
			.rate_max =     384000,
		},
		.capture = {
			.stream_name = "MultiMedia20 Capture",

			.rates = (SNDRV_PCM_RATE_8000_48000|
					SNDRV_PCM_RATE_KNOT),
			.formats = (SNDRV_PCM_FMTBIT_S16_LE |
				    SNDRV_PCM_FMTBIT_S24_LE |
				    SNDRV_PCM_FMTBIT_S24_3LE |
				    SNDRV_PCM_FMTBIT_S32_LE),
			.channels_min = 1,
			.channels_max = 32,
			.rate_min =     8000,
			.rate_max =     48000,
		},
		.ops = &msm_fe_Multimedia_dai_ops,
		.name = "MultiMedia20",
		
	},
	{
		.playback = {
			.stream_name = "MultiMedia21 Playback",

			.rates = (SNDRV_PCM_RATE_8000_384000 |
					SNDRV_PCM_RATE_KNOT),
			.formats = (SNDRV_PCM_FMTBIT_S16_LE |
				    SNDRV_PCM_FMTBIT_S24_LE |
				    SNDRV_PCM_FMTBIT_S24_3LE |
				    SNDRV_PCM_FMTBIT_S32_LE),
			.channels_min = 1,
			.channels_max = 32,
			.rate_min = 8000,
			.rate_max = 384000,
		},
		.capture = {
			.stream_name = "MultiMedia21 Capture",

			.rates = (SNDRV_PCM_RATE_8000_48000|
					SNDRV_PCM_RATE_KNOT),
			.formats = (SNDRV_PCM_FMTBIT_S16_LE |
				    SNDRV_PCM_FMTBIT_S24_LE |
				    SNDRV_PCM_FMTBIT_S24_3LE |
				    SNDRV_PCM_FMTBIT_S32_LE),
			.channels_min = 1,
			.channels_max = 32,
			.rate_min =     8000,
			.rate_max =     48000,
		},
		.ops = &msm_fe_Multimedia_dai_ops,
		.name = "MultiMedia21",
		
	},
	{
		.playback = {
			.stream_name = "MultiMedia22 Playback",

			.rates = (SNDRV_PCM_RATE_8000_384000 |
					SNDRV_PCM_RATE_KNOT),
			.formats = (SNDRV_PCM_FMTBIT_S16_LE |
				    SNDRV_PCM_FMTBIT_S24_LE |
				    SNDRV_PCM_FMTBIT_S24_3LE |
				    SNDRV_PCM_FMTBIT_S32_LE),
			.channels_min = 1,
			.channels_max = 32,
			.rate_min = 8000,
			.rate_max = 384000,
		},
		.capture = {
			.stream_name = "MultiMedia22 Capture",

			.rates = (SNDRV_PCM_RATE_8000_48000|
					SNDRV_PCM_RATE_KNOT),
			.formats = (SNDRV_PCM_FMTBIT_S16_LE |
				    SNDRV_PCM_FMTBIT_S24_LE |
				    SNDRV_PCM_FMTBIT_S24_3LE |
				    SNDRV_PCM_FMTBIT_S32_LE),
			.channels_min = 1,
			.channels_max = 32,
			.rate_min =     8000,
			.rate_max =     48000,
		},
		.ops = &msm_fe_Multimedia_dai_ops,
		.name = "MultiMedia22",
		
	},
	{
		.playback = {
			.stream_name = "MultiMedia23 Playback",

			.rates = (SNDRV_PCM_RATE_8000_384000 |
					SNDRV_PCM_RATE_KNOT),
			.formats = (SNDRV_PCM_FMTBIT_S16_LE |
				    SNDRV_PCM_FMTBIT_S24_LE |
				    SNDRV_PCM_FMTBIT_S24_3LE |
				    SNDRV_PCM_FMTBIT_S32_LE),
			.channels_min = 1,
			.channels_max = 32,
			.rate_min = 8000,
			.rate_max = 384000,
		},
		.capture = {
			.stream_name = "MultiMedia23 Capture",

			.rates = (SNDRV_PCM_RATE_8000_48000|
					SNDRV_PCM_RATE_KNOT),
			.formats = (SNDRV_PCM_FMTBIT_S16_LE |
				    SNDRV_PCM_FMTBIT_S24_LE |
				    SNDRV_PCM_FMTBIT_S24_3LE |
				    SNDRV_PCM_FMTBIT_S32_LE),
			.channels_min = 1,
			.channels_max = 32,
			.rate_min =     8000,
			.rate_max =     48000,
		},
		.ops = &msm_fe_Multimedia_dai_ops,
		.name = "MultiMedia23",
		
	},
	{
		.playback = {
			.stream_name = "MultiMedia24 Playback",

			.rates = (SNDRV_PCM_RATE_8000_384000 |
					SNDRV_PCM_RATE_KNOT),
			.formats = (SNDRV_PCM_FMTBIT_S16_LE |
				    SNDRV_PCM_FMTBIT_S24_LE |
				    SNDRV_PCM_FMTBIT_S24_3LE |
				    SNDRV_PCM_FMTBIT_S32_LE),
			.channels_min = 1,
			.channels_max = 8,
			.rate_min = 8000,
			.rate_max = 384000,
		},
		.capture = {
			.stream_name = "MultiMedia24 Capture",

			.rates = (SNDRV_PCM_RATE_8000_48000|
					SNDRV_PCM_RATE_KNOT),
			.formats = (SNDRV_PCM_FMTBIT_S16_LE |
				    SNDRV_PCM_FMTBIT_S24_LE |
				    SNDRV_PCM_FMTBIT_S24_3LE |
				    SNDRV_PCM_FMTBIT_S32_LE),
			.channels_min = 1,
			.channels_max = 8,
			.rate_min =     8000,
			.rate_max =     48000,
		},
		.ops = &msm_fe_Multimedia_dai_ops,
		.name = "MultiMedia24",
		
	},
	{
		.playback = {
			.stream_name = "MultiMedia25 Playback",

			.rates = (SNDRV_PCM_RATE_8000_384000 |
					SNDRV_PCM_RATE_KNOT),
			.formats = (SNDRV_PCM_FMTBIT_S16_LE |
				    SNDRV_PCM_FMTBIT_S24_LE |
				    SNDRV_PCM_FMTBIT_S24_3LE |
				    SNDRV_PCM_FMTBIT_S32_LE),
			.channels_min = 1,
			.channels_max = 8,
			.rate_min = 8000,
			.rate_max = 384000,
		},
		.capture = {
			.stream_name = "MultiMedia25 Capture",

			.rates = (SNDRV_PCM_RATE_8000_48000|
					SNDRV_PCM_RATE_KNOT),
			.formats = (SNDRV_PCM_FMTBIT_S16_LE |
				    SNDRV_PCM_FMTBIT_S24_LE |
				    SNDRV_PCM_FMTBIT_S24_3LE |
				    SNDRV_PCM_FMTBIT_S32_LE),
			.channels_min = 1,
			.channels_max = 8,
			.rate_min =     8000,
			.rate_max =     48000,
		},
		.ops = &msm_fe_Multimedia_dai_ops,
		.name = "MultiMedia25",
		
	},
	{
		.playback = {
			.stream_name = "MultiMedia26 Playback",

			.rates = (SNDRV_PCM_RATE_8000_384000|
					SNDRV_PCM_RATE_KNOT),
			.formats = (SNDRV_PCM_FMTBIT_S16_LE |
				    SNDRV_PCM_FMTBIT_S24_LE |
				    SNDRV_PCM_FMTBIT_S24_3LE |
				    SNDRV_PCM_FMTBIT_S32_LE),
			.channels_min = 1,
			.channels_max = 32,
			.rate_min = 8000,
			.rate_max = 384000,
		},
		.ops = &msm_fe_Multimedia_dai_ops_compressed,
		.name = "MultiMedia26",
		
	},
	{
		.capture = {
			.stream_name = "MultiMedia27 Capture",

			.rates = (SNDRV_PCM_RATE_8000_192000|
					SNDRV_PCM_RATE_KNOT),
			.formats = (SNDRV_PCM_FMTBIT_S16_LE |
				    SNDRV_PCM_FMTBIT_S24_LE |
				    SNDRV_PCM_FMTBIT_S24_3LE),
			.channels_min = 1,
			.channels_max = 32,
			.rate_min = 8000,
			.rate_max = 192000,
		},
		.ops = &msm_fe_Multimedia_dai_ops_compressed,
		.name = "MultiMedia27",
		
	},
	{
		.capture = {
		.stream_name = "MultiMedia28 Capture",

		.rates = (SNDRV_PCM_RATE_8000_192000|
			  SNDRV_PCM_RATE_KNOT),
		.formats = (SNDRV_PCM_FMTBIT_S16_LE |
			    SNDRV_PCM_FMTBIT_S24_LE |
			    SNDRV_PCM_FMTBIT_S24_3LE),
		.channels_min = 1,
		.channels_max = 32,
		.rate_min =     8000,
		.rate_max =     192000,
		},
		.ops = &msm_fe_Multimedia_dai_ops_compressed,
		.name = "MultiMedia28",
		
	},
	{
		.capture = {
			.stream_name = "MultiMedia29 Capture",

			.rates = (SNDRV_PCM_RATE_8000_192000|
				  SNDRV_PCM_RATE_KNOT),
			.formats = (SNDRV_PCM_FMTBIT_S16_LE |
				    SNDRV_PCM_FMTBIT_S24_LE |
				    SNDRV_PCM_FMTBIT_S24_3LE),
			.channels_min = 1,
			.channels_max = 32,
			.rate_min =     8000,
			.rate_max =     192000,
		},
		.ops = &msm_fe_Multimedia_dai_ops_compressed,
		.name = "MultiMedia29",
		
	},
	{
		.capture = {
			.stream_name = "MultiMedia30 Capture",

			.rates = (SNDRV_PCM_RATE_8000_192000|
				  SNDRV_PCM_RATE_KNOT),
			.formats = (SNDRV_PCM_FMTBIT_S16_LE |
				    SNDRV_PCM_FMTBIT_S24_LE |
				    SNDRV_PCM_FMTBIT_S24_3LE),
			.channels_min = 1,
			.channels_max = 32,
			.rate_min =     8000,
			.rate_max =     192000,
		},
		.ops = &msm_fe_Multimedia_dai_ops_compressed,
		.name = "MultiMedia30",
		
	},
	{
		.playback = {
			.stream_name = "MultiMedia31 Playback",

			.rates = (SNDRV_PCM_RATE_8000_384000|
				SNDRV_PCM_RATE_KNOT),
			.formats = (SNDRV_PCM_FMTBIT_S16_LE |
				SNDRV_PCM_FMTBIT_S24_LE |
				SNDRV_PCM_FMTBIT_S24_3LE |
				SNDRV_PCM_FMTBIT_S32_LE),
			.channels_min = 1,
			.channels_max = 32,
			.rate_min =     8000,
			.rate_max =     384000,
		},
		.capture = {
			.stream_name = "MultiMedia31 Capture",

			.rates = (SNDRV_PCM_RATE_8000_384000 |
				  SNDRV_PCM_RATE_KNOT),
			.formats = (SNDRV_PCM_FMTBIT_S16_LE |
				    SNDRV_PCM_FMTBIT_S24_LE |
				    SNDRV_PCM_FMTBIT_S24_3LE |
				    SNDRV_PCM_FMTBIT_S32_LE),
			.channels_min = 1,
			.channels_max = 32,
			.rate_min = 8000,
			.rate_max = 48000,
		},
		.ops = &msm_fe_Multimedia_dai_ops,
		.name = "MultiMedia31",
		
	},
	{
		.playback = {
			.stream_name = "MultiMedia32 Playback",

			.rates = (SNDRV_PCM_RATE_8000_384000|
				SNDRV_PCM_RATE_KNOT),
			.formats = (SNDRV_PCM_FMTBIT_S16_LE |
				SNDRV_PCM_FMTBIT_S24_LE |
				SNDRV_PCM_FMTBIT_S24_3LE |
				SNDRV_PCM_FMTBIT_S32_LE),
			.channels_min = 1,
			.channels_max = 32,
			.rate_min =     8000,
			.rate_max =     384000,
		},
		.capture = {
			.stream_name = "MultiMedia32 Capture",

			.rates = (SNDRV_PCM_RATE_8000_384000 |
				  SNDRV_PCM_RATE_KNOT),
			.formats = (SNDRV_PCM_FMTBIT_S16_LE |
				    SNDRV_PCM_FMTBIT_S24_LE |
				    SNDRV_PCM_FMTBIT_S24_3LE |
				    SNDRV_PCM_FMTBIT_S32_LE),
			.channels_min = 1,
			.channels_max = 32,
			.rate_min = 8000,
			.rate_max = 48000,
		},
		.ops = &msm_fe_Multimedia_dai_ops,
		.name = "MultiMedia32",
		
	},
	{
		.playback = {
			.stream_name = "MultiMedia33 Playback",

			.rates = (SNDRV_PCM_RATE_8000_384000 |
				  SNDRV_PCM_RATE_KNOT),
			.formats = (SNDRV_PCM_FMTBIT_S16_LE |
				    SNDRV_PCM_FMTBIT_S24_LE |
				    SNDRV_PCM_FMTBIT_S24_3LE |
				    SNDRV_PCM_FMTBIT_S32_LE),
			.channels_min = 1,
			.channels_max = 32,
			.rate_min = 8000,
			.rate_max = 384000,
		},
		.capture = {
			.stream_name = "MultiMedia33 Capture",

			.rates = (SNDRV_PCM_RATE_8000_384000 |
				  SNDRV_PCM_RATE_KNOT),
			.formats = (SNDRV_PCM_FMTBIT_S16_LE |
				    SNDRV_PCM_FMTBIT_S24_LE |
				    SNDRV_PCM_FMTBIT_S24_3LE |
				    SNDRV_PCM_FMTBIT_S32_LE),
			.channels_min = 1,
			.channels_max = 32,
			.rate_min = 8000,
			.rate_max = 48000,
		},
		.ops = &msm_fe_Multimedia_dai_ops,
		.name = "MultiMedia33",
		
	},
	{
		.playback = {
			.stream_name = "MultiMedia34 Playback",

			.rates = (SNDRV_PCM_RATE_8000_384000 |
				  SNDRV_PCM_RATE_KNOT),
			.formats = (SNDRV_PCM_FMTBIT_S16_LE |
				    SNDRV_PCM_FMTBIT_S24_LE |
				    SNDRV_PCM_FMTBIT_S24_3LE |
				    SNDRV_PCM_FMTBIT_S32_LE),
			.channels_min = 1,
			.channels_max = 32,
			.rate_min = 8000,
			.rate_max = 384000,
		},
		.capture = {
			.stream_name = "MultiMedia34 Capture",

			.rates = (SNDRV_PCM_RATE_8000_384000 |
				  SNDRV_PCM_RATE_KNOT),
			.formats = (SNDRV_PCM_FMTBIT_S16_LE |
				    SNDRV_PCM_FMTBIT_S24_LE |
				    SNDRV_PCM_FMTBIT_S24_3LE |
				    SNDRV_PCM_FMTBIT_S32_LE),
			.channels_min = 1,
			.channels_max = 32,
			.rate_min = 8000,
			.rate_max = 48000,
		},
		.ops = &msm_fe_Multimedia_dai_ops,
		.name = "MultiMedia34",
		
	},
};

static int msm_fe_dai_dev_probe(struct platform_device *pdev)
{

	dev_dbg(&pdev->dev, "%s: dev name %s\n", __func__,
		dev_name(&pdev->dev));
	return snd_soc_register_component(&pdev->dev, &msm_fe_dai_component,
		msm_fe_dais, ARRAY_SIZE(msm_fe_dais));
}

static void msm_fe_dai_dev_remove(struct platform_device *pdev)
{
	snd_soc_unregister_component(&pdev->dev);
	return;
}

static const struct of_device_id msm_dai_fe_dt_match[] = {
	{.compatible = "qcom,msm-dai-fe"},
	{}
};

static struct platform_driver msm_fe_dai_driver = {
	.probe  = msm_fe_dai_dev_probe,
	.remove = msm_fe_dai_dev_remove,
	.driver = {
		.name = "msm-dai-fe",
		.owner = THIS_MODULE,
		.of_match_table = msm_dai_fe_dt_match,
		.suppress_bind_attrs = true,
	},
};

int __init msm_fe_dai_init(void)
{
	return platform_driver_register(&msm_fe_dai_driver);
}

void msm_fe_dai_exit(void)
{
	platform_driver_unregister(&msm_fe_dai_driver);
}

/* Module information */
MODULE_DESCRIPTION("MSM Frontend DAI driver");
MODULE_LICENSE("GPL v2");
