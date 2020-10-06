#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#───vi: set et ft=make ts=8 tw=8 fenc=utf-8 :vi───────────────────────┘

PKGS += LIBC_LINUX

LIBC_LINUX_HDRS = $(filter %.h,$(LIBC_LINUX_FILES))
LIBC_LINUX_FILES := $(wildcard libc/linux/*)
LIBC_LINUX_CHECKS = $(LIBC_LINUX_HDRS:%=o/$(MODE)/%.ok)

.PHONY: o/$(MODE)/libc/linux
o/$(MODE)/linux: $(LIBC_LINUX_CHECKS)
