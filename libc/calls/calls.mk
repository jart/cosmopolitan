#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#───vi: set et ft=make ts=8 tw=8 fenc=utf-8 :vi───────────────────────┘
#
# SYNOPSIS
#
#   System Call Compatibility Layer
#
# DESCRIPTION
#
#   This package exports a familiar system call interface that works
#   across platforms.

PKGS += LIBC_CALLS

LIBC_CALLS_ARTIFACTS += LIBC_CALLS_A
LIBC_CALLS = $(LIBC_CALLS_A_DEPS) $(LIBC_CALLS_A)
LIBC_CALLS_A = o/$(MODE)/libc/calls/syscalls.a
LIBC_CALLS_A_FILES :=					\
	$(wildcard libc/calls/typedef/*)		\
	$(wildcard libc/calls/thunks/*)			\
	$(wildcard libc/calls/struct/*)			\
	$(wildcard libc/calls/*)
LIBC_CALLS_A_HDRS = $(filter %.h,$(LIBC_CALLS_A_FILES))
LIBC_CALLS_A_SRCS_S = $(filter %.S,$(LIBC_CALLS_A_FILES))
LIBC_CALLS_A_SRCS_C = $(filter %.c,$(LIBC_CALLS_A_FILES))

LIBC_CALLS_A_SRCS =					\
	$(LIBC_CALLS_A_SRCS_S)				\
	$(LIBC_CALLS_A_SRCS_C)

LIBC_CALLS_A_OBJS =					\
	$(LIBC_CALLS_A_SRCS:%=o/$(MODE)/%.zip.o)	\
	$(LIBC_CALLS_A_SRCS_S:%.S=o/$(MODE)/%.o)	\
	$(LIBC_CALLS_A_SRCS_C:%.c=o/$(MODE)/%.o)

LIBC_CALLS_A_CHECKS =					\
	$(LIBC_CALLS_A).pkg				\
	$(LIBC_CALLS_A_HDRS:%=o/$(MODE)/%.ok)

LIBC_CALLS_A_DIRECTDEPS =				\
	LIBC_FMT					\
	LIBC_NEXGEN32E					\
	LIBC_NT_ADVAPI32				\
	LIBC_NT_KERNEL32				\
	LIBC_NT_NTDLL					\
	LIBC_RAND					\
	LIBC_STR					\
	LIBC_STUBS					\
	LIBC_SYSV_CALLS					\
	LIBC_SYSV

LIBC_CALLS_A_DEPS :=					\
	$(call uniq,$(foreach x,$(LIBC_CALLS_A_DIRECTDEPS),$($(x))))

$(LIBC_CALLS_A):					\
		libc/calls/				\
		$(LIBC_CALLS_A).pkg			\
		$(LIBC_CALLS_A_OBJS)

$(LIBC_CALLS_A).pkg:					\
		$(LIBC_CALLS_A_OBJS)			\
		$(foreach x,$(LIBC_CALLS_A_DIRECTDEPS),$($(x)_A).pkg)

o/$(MODE)/libc/calls/raise.o:				\
		OVERRIDE_COPTS +=			\
			$(NO_MAGIC)

o/$(MODE)/libc/calls/siggy.o:				\
		OVERRIDE_COPTS +=			\
			-ffunction-sections

o/$(MODE)/libc/calls/xnutrampoline.o			\
o/$(MODE)/libc/calls/ntcontext2linux.o:			\
		OVERRIDE_COPTS +=			\
			-O3

LIBC_CALLS_LIBS = $(foreach x,$(LIBC_CALLS_ARTIFACTS),$($(x)))
LIBC_CALLS_SRCS = $(foreach x,$(LIBC_CALLS_ARTIFACTS),$($(x)_SRCS))
LIBC_CALLS_HDRS = $(foreach x,$(LIBC_CALLS_ARTIFACTS),$($(x)_HDRS))
LIBC_CALLS_BINS = $(foreach x,$(LIBC_CALLS_ARTIFACTS),$($(x)_BINS))
LIBC_CALLS_CHECKS = $(foreach x,$(LIBC_CALLS_ARTIFACTS),$($(x)_CHECKS))
LIBC_CALLS_OBJS = $(foreach x,$(LIBC_CALLS_ARTIFACTS),$($(x)_OBJS))
LIBC_CALLS_TESTS = $(foreach x,$(LIBC_CALLS_ARTIFACTS),$($(x)_TESTS))

.PHONY: o/$(MODE)/libc/calls
o/$(MODE)/libc/calls:					\
		o/$(MODE)/libc/calls/hefty		\
		$(LIBC_CALLS_CHECKS)
