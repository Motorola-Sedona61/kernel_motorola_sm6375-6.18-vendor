/* SPDX-License-Identifier: GPL-2.0-only */
/* Original APR route endpoints, keyed by the unchanged ALSA stream names. */
static const char *fogos_audio_aif_name(const struct snd_soc_pcm_stream *stream)
{
	static const struct { const char *stream; const char *aif; } routes[] = {
		{ "HDMI Playback", "HDMI" },
		{ "HDMI MS Playback", "HDMI_MS" },
		{ "Display Port Playback", "DISPLAY_PORT" },
		{ "Display Port1 Playback", "DISPLAY_PORT1" },
	};
	unsigned int i;

	if (!stream->stream_name)
		return NULL;
	for (i = 0; i < ARRAY_SIZE(routes); i++)
		if (!strcmp(stream->stream_name, routes[i].stream))
			return routes[i].aif;
	return NULL;
}
