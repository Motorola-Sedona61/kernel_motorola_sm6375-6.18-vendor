# Fogos 6.18 vendor modules

Qualcomm and Motorola drivers ported from the existing Fogos 6.1 sources for
an ACK 6.18 kernel. Keep this source with the matching private kernel/build
repositories. The build wrappers supply a separate vendor configuration.

The r4 checkpoint boots Android. Main driver groups build together: display,
KGSL, APR/Q6 legacy audio, Bluetooth, WLAN, camera/CCI, IPA/RMNET, video,
FastRPC/CDSP and secure services. Runtime validation remains incomplete.

mmi_annotate and mmi_info are required early modem support. Fingerprint drivers
remain blocklisted for explicit selection by init. moto_binder is built but
is omitted from automatic loading while runtime need remains unconfirmed.

The user reports lag, dead audio and heating on r4. Investigation and fixes
follow this source checkpoint. Raw bugreports and device identifiers are not
part of the source publication. Import revisions and adaptation notes are in
the private build repository.
