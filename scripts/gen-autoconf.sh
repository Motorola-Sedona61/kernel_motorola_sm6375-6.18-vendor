#!/bin/sh
# SPDX-License-Identifier: GPL-2.0-only
# gen-autoconf.sh <vendor config> <kernel .config>
# Print an autoconf.h-style header for the vendor options. The kernel
# .config must not set any of them: the vendor build never overrides the
# kernel's own configuration.
set -eu
vendor=$1
kernel=$2

echo "/* Generated from $(basename "$vendor") by scripts/gen-autoconf.sh. */"
echo "#ifndef __VENDOR_AUTOCONF_H"
echo "#define __VENDOR_AUTOCONF_H"
sed -n 's/^\(CONFIG_[A-Za-z0-9_]*\)=\(.*\)$/\1 \2/p' "$vendor" |
while read -r sym val; do
    if grep -q "^$sym=" "$kernel"; then
        echo "gen-autoconf.sh: $sym is also set by the kernel .config" >&2
        exit 1
    fi
    case $val in
        y) echo "#define $sym 1" ;;
        m) echo "#define ${sym}_MODULE 1" ;;
        n) ;;
        *) echo "#define $sym $val" ;;
    esac
done
echo "#endif"
