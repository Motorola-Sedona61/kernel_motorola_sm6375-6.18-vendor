# SPDX-License-Identifier: GPL-2.0-only
# Out-of-tree Qualcomm/Motorola modules for the fogos 6.18 kernel.

# The 6.1 kernel these drivers come from did not build with
# -Wmissing-prototypes; 6.18 enables it by default.
subdir-ccflags-y += -Wno-missing-prototypes
# 6.18 builds with -fstrict-flex-arrays=3, so with the UBSAN bounds trap any
# trailing [0]/[1] array the 6.1 code uses as a flexible array would panic.
# Build the vendor code as 6.1 did (the compiler default); kernel structures
# keep their __counted_by checks.
subdir-ccflags-y += -fstrict-flex-arrays=0

obj-y += drivers/soc/qcom/
obj-y += drivers/firmware/
obj-y += drivers/pinctrl/qcom/
obj-y += drivers/soc/qcom/mem_buf/
obj-y += drivers/iommu/
obj-y += drivers/dma-buf/heaps/
obj-y += kernel/trace/
obj-y += drivers/irqchip/
obj-y += drivers/mailbox/
obj-y += net/qrtr/
obj-y += drivers/rpmsg/
obj-y += drivers/cpufreq/
obj-y += drivers/remoteproc/
obj-y += drivers/input/misc/
obj-y += drivers/power/reset/
obj-y += drivers/interconnect/qcom/
obj-y += drivers/regulator/
obj-y += drivers/clk/qcom/
obj-y += drivers/mfd/
obj-y += drivers/slimbus/
obj-y += drivers/iommu/arm/arm-smmu/
obj-y += drivers/phy/qualcomm/
obj-y += drivers/ufs/host/
obj-y += drivers/spmi/
obj-y += drivers/nvmem/
obj-y += drivers/soc/qcom/dcvs/
obj-y += drivers/rtc/
obj-y += drivers/perf/
obj-y += drivers/mmc/host/
obj-y += drivers/dma/qcom/
obj-y += drivers/i2c/busses/
obj-y += drivers/spi/
obj-y += drivers/tty/serial/
obj-y += drivers/pwm/
obj-y += drivers/iio/adc/
obj-y += drivers/thermal/qcom/
obj-y += drivers/power/supply/qcom/
obj-y += drivers/usb/phy/
obj-y += drivers/usb/pd/
obj-y += drivers/usb/dwc3/
obj-y += motorola/
obj-y += techpack/display/
obj-y += drivers/char/
obj-y += drivers/cpuidle/governors/
obj-y += drivers/edac/
obj-y += drivers/i3c/master/
obj-y += drivers/interconnect/
obj-y += drivers/leds/
obj-y += drivers/soc/qcom/memshare/
obj-y += drivers/soc/qcom/sps/
obj-y += drivers/uio/msm_sharedmem/
obj-y += drivers/usb/gadget/function/
obj-y += kernel/
obj-y += sound/usb/

obj-y += techpack/graphics/

obj-y += techpack/dsp/
obj-y += techpack/securemsm/
obj-y += techpack/bt/
obj-y += techpack/mmrm/

obj-y += techpack/audio/

obj-y += techpack/wlan/

obj-y += techpack/dataipa/ techpack/datarmnet/ techpack/datarmnet-ext/
obj-y += techpack/camera/ techpack/video/
