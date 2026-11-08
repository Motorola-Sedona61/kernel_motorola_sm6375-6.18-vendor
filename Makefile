# SPDX-License-Identifier: GPL-2.0-only
# Build the vendor modules against a configured and built 6.18 kernel:
#   make KERNEL_SRC=<kernel> KERNEL_OUT=<kernel O=> VENDOR_OUT=<dir> [modules]
# plus the usual LLVM=1 ARCH=arm64 toolchain variables.
#
# The kernel .config stays a GKI configuration. The vendor options come
# from configs/fogos.config: they are passed to Kbuild as make variables
# and, for the C code, as a generated autoconf header.

VENDOR_SRC := $(abspath $(dir $(lastword $(MAKEFILE_LIST))))
KERNEL_SRC ?= $(VENDOR_SRC)/../kernel
KERNEL_OUT ?= $(KERNEL_SRC)
VENDOR_OUT ?= $(VENDOR_SRC)/../out/vendor
VENDOR_CONFIG ?= $(VENDOR_SRC)/configs/fogos.config
VENDOR_MAKE_CONFIG ?= $(VENDOR_SRC)/configs/fogos-make.config

autoconf := $(VENDOR_OUT)/include/generated/vendor_autoconf.h
config_vars := $(shell sed -n 's/^\(CONFIG_[A-Za-z0-9_]*=.*\)$$/\1/p' $(VENDOR_CONFIG))
make_vars := $(shell sed -n 's/^\(CONFIG_[A-Za-z0-9_]*=.*\)$$/\1/p' $(VENDOR_MAKE_CONFIG))

vendor_includes := -I$(VENDOR_SRC)/include -I$(VENDOR_SRC)/include/uapi \
	-include $(autoconf)

kbuild := $(MAKE) -C $(KERNEL_SRC) O=$(KERNEL_OUT) M=$(VENDOR_SRC) \
	MO=$(VENDOR_OUT) NOSTDINC_FLAGS="$(vendor_includes)" $(config_vars) $(make_vars) \
	DISPLAY_ROOT=$(VENDOR_SRC)/techpack/display

modules: $(autoconf) wlan-config
	+$(kbuild) modules

modules_install: $(autoconf) wlan-config
	+$(kbuild) modules_install

clean:
	+$(kbuild) clean
	rm -f $(autoconf)

$(autoconf): $(VENDOR_CONFIG) $(VENDOR_SRC)/scripts/gen-autoconf.sh $(KERNEL_OUT)/.config
	mkdir -p $(dir $@)
	sh $(VENDOR_SRC)/scripts/gen-autoconf.sh $(VENDOR_CONFIG) \
		$(KERNEL_OUT)/.config > $@.tmp
	@if cmp -s $@.tmp $@; then rm -f $@.tmp; else mv $@.tmp $@; fi

.PHONY: modules modules_install clean

# qcacld uses the same generated Kconfig defaults as its DDK build.
wlan-config:
	python3 $(VENDOR_SRC)/../tools/gen-qcacld-config.py \
		--wlan-root $(VENDOR_SRC)/techpack/wlan/qcacld-3.0 \
		--kernel $(KERNEL_SRC) --defconfig blair_gki_wlan_defconfig \
		--out $(VENDOR_OUT)/include/generated/wlan \
		--set FEATURE_BLACKLIST_MGR=n --set MPC_UT_FRAMEWORK=n
.PHONY: wlan-config
