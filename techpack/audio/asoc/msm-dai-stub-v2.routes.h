/* SPDX-License-Identifier: GPL-2.0-only */
/* Original APR route endpoints, keyed by the unchanged ALSA stream names. */
static const char *fogos_audio_aif_name(const struct snd_soc_pcm_stream *stream)
{
	static const struct { const char *stream; const char *aif; } routes[] = {
		{ "Stub Playback", "STUB_RX" },
		{ "Stub Capture", "STUB_TX" },
		{ "Stub1 Capture", "STUB_1_TX" },
		{ "DTMF TX", "STUB_DTMF_TX" },
		{ "CS-VOICE HOST RX CAPTURE", "STUB_HOST_RX_CAPTURE_TX" },
		{ "CS-VOICE HOST TX CAPTURE", "STUB_HOST_TX_CAPTURE_TX" },
		{ "CS-VOICE HOST RX PLAYBACK", "STUB_HOST_RX_PLAYBACK_RX" },
		{ "CS-VOICE HOST TX PLAYBACK", "STUB_HOST_TX_PLAYBACK_RX" },
	};
	unsigned int i;

	if (!stream->stream_name)
		return NULL;
	for (i = 0; i < ARRAY_SIZE(routes); i++)
		if (!strcmp(stream->stream_name, routes[i].stream))
			return routes[i].aif;
	return NULL;
}
