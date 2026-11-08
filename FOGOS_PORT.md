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

WALT checkpoint r5 imports Qualcomm kernel.lnx.6.12.r70-rel at
46f13428ae2d6e32ddfe2f3e5cb5046428ae73b4 into kernel/sched/walt.
It builds window tracking, placement, core control, input boost and the walt
cpufreq governor against ACK 6.18. Adaptations include runnable/queued entity
counts, pick_task rq flags, Binder transaction flags and restore hook, dynamic
task storage checks, and legacy Fogos cpufreq cycle-counter access. Minidump
reads WALT's dynamic tail through the current accessor. Full vendor compile
and modpost pass; runtime correctness and thermal improvements are untested.
See WALT.md in the matching private build repository for the boot contract.
