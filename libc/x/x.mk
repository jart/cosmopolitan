#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#───vi: set et ft=make ts=8 tw=8 fenc=utf-8 :vi───────────────────────┘
#
# SYNOPSIS
#
#   Cosmopolitan Extended Memory & Formatting Functions
#
# DESCRIPTION
#
#   This package implements nonstandard APIs that've been spotted in a
#   substantial number of independent codebases.

PKGS += LIBC_X

LIBC_X_ARTIFACTS += LIBC_X_A
LIBC_X = $(LIBC_X_A_DEPS) $(LIBC_X_A)
LIBC_X_A = o/$(MODE)/libc/x/x.a
LIBC_X_A_FILES := $(wildcard libc/x/*)
LIBC_X_A_HDRS = $(filter %.h,$(LIBC_X_A_FILES))
LIBC_X_A_SRCS_S = $(filter %.S,$(LIBC_X_A_FILES))
LIBC_X_A_SRCS_C = $(filter %.c,$(LIBC_X_A_FILES))

LIBC_X_A_SRCS =					\
	$(LIBC_X_A_SRCS_S)			\
	$(LIBC_X_A_SRCS_C)

LIBC_X_A_OBJS =					\
	$(LIBC_X_A_SRCS:%=o/$(MODE)/%.zip.o)	\
	$(LIBC_X_A_SRCS_S:%.S=o/$(MODE)/%.o)	\
	$(LIBC_X_A_SRCS_C:%.c=o/$(MODE)/%.o)

LIBC_X_A_CHECKS =				\
	$(LIBC_X_A).pkg				\
	$(LIBC_X_A_HDRS:%=o/$(MODE)/%.ok)

LIBC_X_A_DIRECTDEPS =				\
	LIBC_CALLS				\
	LIBC_FMT				\
	LIBC_MEM				\
	LIBC_NEXGEN32E				\
	LIBC_RUNTIME				\
	LIBC_STDIO				\
	LIBC_STR				\
	LIBC_STUBS				\
	LIBC_SYSV				\
	THIRD_PARTY_GDTOA

LIBC_X_A_DEPS :=				\
	$(call uniq,$(foreach x,$(LIBC_X_A_DIRECTDEPS),$($(x))))

$(LIBC_X_A):	libc/x/				\
		$(LIBC_X_A).pkg			\
		$(LIBC_X_A_OBJS)

$(LIBC_X_A).pkg:				\
		$(LIBC_X_A_OBJS)		\
		$(foreach x,$(LIBC_X_A_DIRECTDEPS),$($(x)_A).pkg)

LIBC_X_LIBS = $(foreach x,$(LIBC_X_ARTIFACTS),$($(x)))
LIBC_X_SRCS = $(foreach x,$(LIBC_X_ARTIFACTS),$($(x)_SRCS))
LIBC_X_HDRS = $(foreach x,$(LIBC_X_ARTIFACTS),$($(x)_HDRS))
LIBC_X_BINS = $(foreach x,$(LIBC_X_ARTIFACTS),$($(x)_BINS))
LIBC_X_CHECKS = $(foreach x,$(LIBC_X_ARTIFACTS),$($(x)_CHECKS))
LIBC_X_OBJS = $(foreach x,$(LIBC_X_ARTIFACTS),$($(x)_OBJS))
LIBC_X_TESTS = $(foreach x,$(LIBC_X_ARTIFACTS),$($(x)_TESTS))
$(LIBC_X_OBJS): $(BUILD_FILES) libc/x/x.mk

.PHONY: o/$(MODE)/libc/x
o/$(MODE)/libc/x: $(LIBC_X_CHECKS)
