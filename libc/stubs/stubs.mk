#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#───vi: set et ft=make ts=8 tw=8 fenc=utf-8 :vi───────────────────────┘
#
# SYNOPSIS
#
#   Universally Linkable Weak Functions
#
# DESCRIPTION
#
#   This package contains no-op implementations for (1) synthetic
#   compiler surprises and (2) mission-critical core functions, e.g.
#   abort(). These functions are designed to work in all addressing
#   modes, including real mode. Every package links this. It's at the
#   base of the topological order of things.

PKGS += LIBC_STUBS

LIBC_STUBS_ARTIFACTS += LIBC_STUBS_A
LIBC_STUBS = $(LIBC_STUBS_A)
LIBC_STUBS_A = o/$(MODE)/libc/stubs/stubs.a
LIBC_STUBS_A_FILES := $(wildcard libc/stubs/*.S)

LIBC_STUBS_A_HDRS =					\
	$(filter %.h,$(LIBC_STUBS_A_FILES))

LIBC_STUBS_A_SRCS =					\
	$(filter %.S,$(LIBC_STUBS_A_FILES))

LIBC_STUBS_A_OBJS = 					\
	$(LIBC_STUBS_A_SRCS:%=o/$(MODE)/%.zip.o)	\
	$(LIBC_STUBS_A_SRCS:%.S=o/$(MODE)/%.o)

LIBC_STUBS_A_CHECKS =					\
	$(LIBC_STUBS_A).pkg				\
	$(LIBC_STUBS_A_HDRS:%=o/%.ok)

$(LIBC_STUBS_A):					\
		libc/stubs/				\
		$(LIBC_STUBS_A).pkg			\
		$(LIBC_STUBS_A_OBJS)

$(LIBC_STUBS_A).pkg:					\
		$(LIBC_STUBS_A_OBJS)			\
		$(foreach x,$(LIBC_STUBS_A_DIRECTDEPS),$($(x)_A).pkg)

LIBC_STUBS_LIBS = $(foreach x,$(LIBC_STUBS_ARTIFACTS),$($(x)))
LIBC_STUBS_SRCS = $(foreach x,$(LIBC_STUBS_ARTIFACTS),$($(x)_SRCS))
LIBC_STUBS_CHECKS = $(foreach x,$(LIBC_STUBS_ARTIFACTS),$($(x)_CHECKS))
LIBC_STUBS_OBJS = $(foreach x,$(LIBC_STUBS_ARTIFACTS),$($(x)_OBJS))

.PHONY: o/$(MODE)/libc/stubs
o/$(MODE)/libc/stubs:					\
		$(LIBC_STUBS_CHECKS)
