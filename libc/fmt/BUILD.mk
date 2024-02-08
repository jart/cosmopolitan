#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#── vi: set noet ft=make ts=8 sw=8 fenc=utf-8 :vi ────────────────────┘
#
# SYNOPSIS
#
#   Cosmopolitan String Formatting
#
# DESCRIPTION
#
#   This package provides incredible domain-specific languages for
#   encoding and decoding strings.

PKGS += LIBC_FMT

LIBC_FMT_ARTIFACTS += LIBC_FMT_A
LIBC_FMT = $(LIBC_FMT_A_DEPS) $(LIBC_FMT_A)
LIBC_FMT_A = o/$(MODE)/libc/fmt/fmt.a
LIBC_FMT_A_FILES := $(wildcard libc/fmt/*)
LIBC_FMT_A_HDRS = $(filter %.h,$(LIBC_FMT_A_FILES))
LIBC_FMT_A_INCS = $(filter %.inc,$(LIBC_FMT_A_FILES))
LIBC_FMT_A_SRCS_S = $(filter %.S,$(LIBC_FMT_A_FILES))
LIBC_FMT_A_SRCS_C = $(filter %.c,$(LIBC_FMT_A_FILES))

LIBC_FMT_A_SRCS =				\
	$(LIBC_FMT_A_SRCS_S)			\
	$(LIBC_FMT_A_SRCS_C)

LIBC_FMT_A_OBJS =				\
	$(LIBC_FMT_A_SRCS_S:%.S=o/$(MODE)/%.o)	\
	$(LIBC_FMT_A_SRCS_C:%.c=o/$(MODE)/%.o)

LIBC_FMT_A_CHECKS =				\
	$(LIBC_FMT_A).pkg			\
	$(LIBC_FMT_A_HDRS:%=o/$(MODE)/%.ok)

LIBC_FMT_A_DIRECTDEPS =				\
	LIBC_INTRIN				\
	LIBC_NEXGEN32E				\
	LIBC_NT_KERNEL32			\
	LIBC_STR				\
	LIBC_SYSV				\
	LIBC_TINYMATH				\
	THIRD_PARTY_COMPILER_RT

LIBC_FMT_A_DEPS :=				\
	$(call uniq,$(foreach x,$(LIBC_FMT_A_DIRECTDEPS),$($(x))))

$(LIBC_FMT_A):	libc/fmt/			\
		$(LIBC_FMT_A).pkg		\
		$(LIBC_FMT_A_OBJS)

$(LIBC_FMT_A).pkg:				\
		$(LIBC_FMT_A_OBJS)		\
		$(foreach x,$(LIBC_FMT_A_DIRECTDEPS),$($(x)_A).pkg)

$(LIBC_FMT_A_OBJS): private			\
		CFLAGS +=			\
			-fno-jump-tables

o/$(MODE)/libc/fmt/formatint64.o		\
o/$(MODE)/libc/fmt/formatint64thousands.o	\
o/$(MODE)/libc/fmt/dosdatetimetounix.o		\
o/$(MODE)/libc/fmt/itoa64radix10.greg.o: private\
		CFLAGS +=			\
			-O3

o/$(MODE)/libc/fmt/atoi.o			\
o/$(MODE)/libc/fmt/strtol.o			\
o/$(MODE)/libc/fmt/strtoul.o			\
o/$(MODE)/libc/fmt/wcstol.o			\
o/$(MODE)/libc/fmt/wcstoul.o			\
o/$(MODE)/libc/fmt/strtoimax.o			\
o/$(MODE)/libc/fmt/strtoumax.o			\
o/$(MODE)/libc/fmt/wcstoimax.o			\
o/$(MODE)/libc/fmt/wcstoumax.o: private		\
		CFLAGS +=			\
			-Os

LIBC_FMT_LIBS = $(foreach x,$(LIBC_FMT_ARTIFACTS),$($(x)))
LIBC_FMT_SRCS = $(foreach x,$(LIBC_FMT_ARTIFACTS),$($(x)_SRCS))
LIBC_FMT_HDRS = $(foreach x,$(LIBC_FMT_ARTIFACTS),$($(x)_HDRS))
LIBC_FMT_INCS = $(foreach x,$(LIBC_FMT_ARTIFACTS),$($(x)_INCS))
LIBC_FMT_CHECKS = $(foreach x,$(LIBC_FMT_ARTIFACTS),$($(x)_CHECKS))
LIBC_FMT_OBJS = $(foreach x,$(LIBC_FMT_ARTIFACTS),$($(x)_OBJS))
$(LIBC_FMT_OBJS): $(BUILD_FILES) libc/fmt/BUILD.mk

.PHONY: o/$(MODE)/libc/fmt
o/$(MODE)/libc/fmt: $(LIBC_FMT_CHECKS)
