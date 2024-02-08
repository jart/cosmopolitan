#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#── vi: set noet ft=make ts=8 sw=8 fenc=utf-8 :vi ────────────────────┘
#
# SYNOPSIS
#
#   Cosmopolitan C Runtime.
#
# OVERVIEW
#
#   This is a special object library defining the Cosmopolitan SysV
#   userspace runtime entrypoint, i.e. _start(). The $(CRT) argument
#   should come first in the link order when building ELF binaries;
#   becauseo it's the lowest level function; it wraps main(); and it
#   decides which runtime support libraries get schlepped into builds.
#
#   Please note that, when building an αcτµαlly pδrταblε εxεcµταblε,
#   lower-level entrypoints exist and as such, $(CRT) will generally
#   come second in link-order after $(APE).

PKGS += CRT

CRT_ARTIFACTS += CRT
CRT = o/$(MODE)/libc/crt/crt.o
CRT_FILES = libc/crt/crt.S
CRT_SRCS = libc/crt/crt.S
CRT_OBJS = o/$(MODE)/libc/crt/crt.o
$(CRT_OBJS): $(BUILD_FILES) libc/crt/BUILD.mk

# these assembly files are safe to build on aarch64
o/$(MODE)/libc/crt/crt.o: libc/crt/crt.S
	@$(COMPILE) -AOBJECTIFY.S $(OBJECTIFY.S) $(OUTPUT_OPTION) -c $<

.PHONY: o/$(MODE)/libc/crt
o/$(MODE)/libc/crt: $(CRT)
