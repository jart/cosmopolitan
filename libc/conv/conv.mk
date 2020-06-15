#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#───vi: set et ft=make ts=8 tw=8 fenc=utf-8 :vi───────────────────────┘
#
# SYNOPSIS
#
#   Cosmopolitan Data Conversions
#
# DESCRIPTION
#
#   This package provides functions that convert between data types.
#   It's intended to be lower level than the memory and formatting
#   libraries.

PKGS += LIBC_CONV

LIBC_CONV_ARTIFACTS += LIBC_CONV_A
LIBC_CONV = $(LIBC_CONV_A_DEPS) $(LIBC_CONV_A)
LIBC_CONV_A = o/$(MODE)/libc/conv/conv.a
LIBC_CONV_A_FILES := $(wildcard libc/conv/*)
LIBC_CONV_A_HDRS = $(filter %.h,$(LIBC_CONV_A_FILES))
LIBC_CONV_A_SRCS_S = $(filter %.S,$(LIBC_CONV_A_FILES))
LIBC_CONV_A_SRCS_C = $(filter %.c,$(LIBC_CONV_A_FILES))

LIBC_CONV_A_SRCS =				\
	$(LIBC_CONV_A_SRCS_S)			\
	$(LIBC_CONV_A_SRCS_C)

LIBC_CONV_A_OBJS =				\
	$(LIBC_CONV_A_SRCS:%=o/$(MODE)/%.zip.o)	\
	$(LIBC_CONV_A_SRCS_S:%.S=o/$(MODE)/%.o)	\
	$(LIBC_CONV_A_SRCS_C:%.c=o/$(MODE)/%.o)

LIBC_CONV_A_CHECKS =				\
	$(LIBC_CONV_A).pkg			\
	$(LIBC_CONV_A_HDRS:%=o/$(MODE)/%.ok)

LIBC_CONV_A_DIRECTDEPS =			\
	LIBC_STUBS				\
	LIBC_NEXGEN32E				\
	LIBC_TINYMATH				\
	LIBC_SYSV

LIBC_CONV_A_DEPS :=				\
	$(call uniq,$(foreach x,$(LIBC_CONV_A_DIRECTDEPS),$($(x))))

$(LIBC_CONV_A):	libc/conv/			\
		$(LIBC_CONV_A).pkg		\
		$(LIBC_CONV_A_OBJS)

$(LIBC_CONV_A).pkg:				\
		$(LIBC_CONV_A_OBJS)		\
		$(foreach x,$(LIBC_CONV_A_DIRECTDEPS),$($(x)_A).pkg)

#o/$(MODE)/libc/conv/strtoimax.o: CC = clang-10
#o/$(MODE)/libc/conv/strtoumax.o: CC = clang-10

o/$(MODE)/libc/conv/itoa64radix10.o		\
o/$(MODE)/libc/conv/timetofiletime.o		\
o/$(MODE)/libc/conv/filetimetotime.o		\
o/$(MODE)/libc/conv/filetimetotimespec.o	\
o/$(MODE)/libc/conv/filetimetotimeval.o:	\
		OVERRIDE_COPTS +=		\
			-O3

LIBC_CONV_LIBS = $(foreach x,$(LIBC_CONV_ARTIFACTS),$($(x)))
LIBC_CONV_SRCS = $(foreach x,$(LIBC_CONV_ARTIFACTS),$($(x)_SRCS))
LIBC_CONV_HDRS = $(foreach x,$(LIBC_CONV_ARTIFACTS),$($(x)_HDRS))
LIBC_CONV_BINS = $(foreach x,$(LIBC_CONV_ARTIFACTS),$($(x)_BINS))
LIBC_CONV_CHECKS = $(foreach x,$(LIBC_CONV_ARTIFACTS),$($(x)_CHECKS))
LIBC_CONV_OBJS = $(foreach x,$(LIBC_CONV_ARTIFACTS),$($(x)_OBJS))
LIBC_CONV_TESTS = $(foreach x,$(LIBC_CONV_ARTIFACTS),$($(x)_TESTS))
$(LIBC_CONV_OBJS): $(BUILD_FILES) libc/conv/conv.mk

.PHONY: o/$(MODE)/libc/conv
o/$(MODE)/libc/conv: $(LIBC_CONV_CHECKS)
