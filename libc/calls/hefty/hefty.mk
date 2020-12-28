#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#───vi: set et ft=make ts=8 tw=8 fenc=utf-8 :vi───────────────────────┘
#
# SYNOPSIS
#
#   Cosmopolitan System Call Compatibility Layer
#
# DESCRIPTION
#
#   This subpackage exports functions traditionally understood as system
#   calls that Cosmopolitan needs to wrap in a nontrivial way, requiring
#   things like dynamic memory allocation.

PKGS += LIBC_CALLS_HEFTY

LIBC_CALLS_HEFTY_ARTIFACTS += LIBC_CALLS_HEFTY_A
LIBC_CALLS_HEFTY = $(LIBC_CALLS_HEFTY_A_DEPS) $(LIBC_CALLS_HEFTY_A)
LIBC_CALLS_HEFTY_A = o/$(MODE)/libc/calls/hefty/hefty.a
LIBC_CALLS_HEFTY_A_FILES := $(wildcard libc/calls/hefty/*)
LIBC_CALLS_HEFTY_A_HDRS = $(filter %.h,$(LIBC_CALLS_HEFTY_A_FILES))
LIBC_CALLS_HEFTY_A_SRCS_S = $(filter %.S,$(LIBC_CALLS_HEFTY_A_FILES))
LIBC_CALLS_HEFTY_A_SRCS_C = $(filter %.c,$(LIBC_CALLS_HEFTY_A_FILES))

LIBC_CALLS_HEFTY_A_SRCS =				\
	$(LIBC_CALLS_HEFTY_A_SRCS_S)			\
	$(LIBC_CALLS_HEFTY_A_SRCS_C)

LIBC_CALLS_HEFTY_A_OBJS =				\
	$(LIBC_CALLS_HEFTY_A_SRCS:%=o/$(MODE)/%.zip.o)	\
	$(LIBC_CALLS_HEFTY_A_SRCS_S:%.S=o/$(MODE)/%.o)	\
	$(LIBC_CALLS_HEFTY_A_SRCS_C:%.c=o/$(MODE)/%.o)

LIBC_CALLS_HEFTY_A_CHECKS =				\
	$(LIBC_CALLS_HEFTY_A).pkg			\
	$(LIBC_CALLS_HEFTY_A_HDRS:%=o/$(MODE)/%.ok)

LIBC_CALLS_HEFTY_A_DIRECTDEPS =				\
	LIBC_ALG					\
	LIBC_CALLS					\
	LIBC_FMT					\
	LIBC_MEM					\
	LIBC_NEXGEN32E					\
	LIBC_NT_KERNEL32				\
	LIBC_RUNTIME					\
	LIBC_STR					\
	LIBC_STUBS					\
	LIBC_SYSV					\
	LIBC_SYSV_CALLS

LIBC_CALLS_HEFTY_A_DEPS :=				\
	$(call uniq,$(foreach x,$(LIBC_CALLS_HEFTY_A_DIRECTDEPS),$($(x))))

$(LIBC_CALLS_HEFTY_A):					\
		libc/calls/hefty/			\
		$(LIBC_CALLS_HEFTY_A).pkg		\
		$(LIBC_CALLS_HEFTY_A_OBJS)

$(LIBC_CALLS_HEFTY_A).pkg:				\
		$(LIBC_CALLS_HEFTY_A_OBJS)		\
		$(foreach x,$(LIBC_CALLS_HEFTY_A_DIRECTDEPS),$($(x)_A).pkg)

LIBC_CALLS_HEFTY_LIBS = $(foreach x,$(LIBC_CALLS_HEFTY_ARTIFACTS),$($(x)))
LIBC_CALLS_HEFTY_SRCS = $(foreach x,$(LIBC_CALLS_HEFTY_ARTIFACTS),$($(x)_SRCS))
LIBC_CALLS_HEFTY_HDRS = $(foreach x,$(LIBC_CALLS_HEFTY_ARTIFACTS),$($(x)_HDRS))
LIBC_CALLS_HEFTY_BINS = $(foreach x,$(LIBC_CALLS_HEFTY_ARTIFACTS),$($(x)_BINS))
LIBC_CALLS_HEFTY_CHECKS = $(foreach x,$(LIBC_CALLS_HEFTY_ARTIFACTS),$($(x)_CHECKS))
LIBC_CALLS_HEFTY_OBJS = $(foreach x,$(LIBC_CALLS_HEFTY_ARTIFACTS),$($(x)_OBJS))
LIBC_CALLS_HEFTY_TESTS = $(foreach x,$(LIBC_CALLS_HEFTY_ARTIFACTS),$($(x)_TESTS))
$(LIBC_CALLS_HEFTY_OBJS): $(BUILD_FILES) libc/calls/hefty/hefty.mk

.PHONY: o/$(MODE)/libc/calls/hefty
o/$(MODE)/libc/calls/hefty: $(LIBC_CALLS_HEFTY_CHECKS)
