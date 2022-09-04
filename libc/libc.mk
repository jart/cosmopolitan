#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#───vi: set et ft=make ts=8 tw=8 fenc=utf-8 :vi───────────────────────┘

PKGS += LIBC

LIBC_HDRS = $(filter %.h,$(LIBC_FILES))
LIBC_INCS = $(filter %.inc,$(LIBC_FILES))
LIBC_FILES := $(wildcard libc/*) $(wildcard libc/isystem/*)
LIBC_CHECKS = $(LIBC_HDRS:%=o/$(MODE)/%.ok)

.PHONY:		o/$(MODE)/libc
o/$(MODE)/libc:	o/$(MODE)/libc/calls		\
		o/$(MODE)/libc/crt		\
		o/$(MODE)/libc/dns		\
		o/$(MODE)/libc/elf		\
		o/$(MODE)/libc/fmt		\
		o/$(MODE)/libc/intrin		\
		o/$(MODE)/libc/linux		\
		o/$(MODE)/libc/log		\
		o/$(MODE)/libc/mem		\
		o/$(MODE)/libc/nexgen32e	\
		o/$(MODE)/libc/nt		\
		o/$(MODE)/libc/runtime		\
		o/$(MODE)/libc/sock		\
		o/$(MODE)/libc/stdio		\
		o/$(MODE)/libc/str		\
		o/$(MODE)/libc/stubs		\
		o/$(MODE)/libc/sysv		\
		o/$(MODE)/libc/testlib		\
		o/$(MODE)/libc/thread		\
		o/$(MODE)/libc/time		\
		o/$(MODE)/libc/tinymath		\
		o/$(MODE)/libc/vga		\
		o/$(MODE)/libc/x		\
		o/$(MODE)/libc/zipos		\
		$(LIBC_CHECKS)
