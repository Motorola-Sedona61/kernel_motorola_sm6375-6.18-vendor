/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * The vendor modules call the Qualcomm SCM driver built here
 * (drivers/firmware/qcom_scm.c), whose API is <linux/qcom_scm.h>. Code
 * taken from 6.6+ trees includes the upstream path; give it the same
 * declarations instead of the kernel's upstream SCM header.
 */
#include <linux/qcom_scm.h>
