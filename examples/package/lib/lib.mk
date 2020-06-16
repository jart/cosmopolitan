#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#───vi: set et ft=make ts=8 tw=8 fenc=utf-8 :vi───────────────────────┘
#
# SYNOPSIS
#
#   Your package static library build config
#
# DESCRIPTION
#
#   Your library doesn't have a main() function and can be compromised
#   of sources written in multiple languages.

PKGS += EXAMPLES_PACKAGE_LIB

EXAMPLES_PACKAGE_LIB_ARTIFACTS += EXAMPLES_PACKAGE_LIB_A
EXAMPLES_PACKAGE_LIB = $(EXAMPLES_PACKAGE_LIB_A_DEPS) $(EXAMPLES_PACKAGE_LIB_A)
EXAMPLES_PACKAGE_LIB_A = o/$(MODE)/examples/package/lib/lib.a
EXAMPLES_PACKAGE_LIB_A_HDRS = $(filter %.h,$(EXAMPLES_PACKAGE_LIB_A_FILES))
EXAMPLES_PACKAGE_LIB_A_SRCS_S = $(filter %.S,$(EXAMPLES_PACKAGE_LIB_A_FILES))
EXAMPLES_PACKAGE_LIB_A_SRCS_C = $(filter %.c,$(EXAMPLES_PACKAGE_LIB_A_FILES))
EXAMPLES_PACKAGE_LIB_A_CHECKS = $(EXAMPLES_PACKAGE_LIB_A).pkg

EXAMPLES_PACKAGE_LIB_A_FILES :=					\
	$(wildcard examples/package/lib/*)			\
	$(wildcard examples/package/lib/thunks/*)

EXAMPLES_PACKAGE_LIB_A_SRCS =					\
	$(EXAMPLES_PACKAGE_LIB_A_SRCS_S)			\
	$(EXAMPLES_PACKAGE_LIB_A_SRCS_C)

EXAMPLES_PACKAGE_LIB_A_OBJS =					\
	$(EXAMPLES_PACKAGE_LIB_A_SRCS_S:%.S=o/$(MODE)/%.o)	\
	$(EXAMPLES_PACKAGE_LIB_A_SRCS_C:%.c=o/$(MODE)/%.o)	\
	$(EXAMPLES_PACKAGE_LIB_A_SRCS:%=o/$(MODE)/%.zip.o)

EXAMPLES_PACKAGE_LIB_A_DIRECTDEPS =				\
	LIBC_STDIO						\
	LIBC_STUBS

EXAMPLES_PACKAGE_LIB_A_DEPS :=					\
	$(call uniq,$(foreach x,$(EXAMPLES_PACKAGE_LIB_A_DIRECTDEPS),$($(x))))

$(EXAMPLES_PACKAGE_LIB_A):					\
		examples/package/lib/				\
		$(EXAMPLES_PACKAGE_LIB_A).pkg			\
		$(EXAMPLES_PACKAGE_LIB_A_OBJS)

$(EXAMPLES_PACKAGE_LIB_A).pkg:					\
		$(EXAMPLES_PACKAGE_LIB_A_OBJS)			\
		$(foreach x,$(EXAMPLES_PACKAGE_LIB_A_DIRECTDEPS),$($(x)_A).pkg)

$(EXAMPLES_PACKAGE_LIB_A_OBJS): examples/package/lib/lib.mk

EXAMPLES_PACKAGE_LIB_LIBS = $(foreach x,$(EXAMPLES_PACKAGE_LIB_ARTIFACTS),$($(x)))
EXAMPLES_PACKAGE_LIB_SRCS = $(foreach x,$(EXAMPLES_PACKAGE_LIB_ARTIFACTS),$($(x)_SRCS))
EXAMPLES_PACKAGE_LIB_HDRS = $(foreach x,$(EXAMPLES_PACKAGE_LIB_ARTIFACTS),$($(x)_HDRS))
EXAMPLES_PACKAGE_LIB_BINS = $(foreach x,$(EXAMPLES_PACKAGE_LIB_ARTIFACTS),$($(x)_BINS))
EXAMPLES_PACKAGE_LIB_CHECKS = $(foreach x,$(EXAMPLES_PACKAGE_LIB_ARTIFACTS),$($(x)_CHECKS))
EXAMPLES_PACKAGE_LIB_OBJS = $(foreach x,$(EXAMPLES_PACKAGE_LIB_ARTIFACTS),$($(x)_OBJS))
EXAMPLES_PACKAGE_LIB_TESTS = $(foreach x,$(EXAMPLES_PACKAGE_LIB_ARTIFACTS),$($(x)_TESTS))

.PHONY: o/$(MODE)/examples/package/lib
o/$(MODE)/examples/package/lib: $(EXAMPLES_PACKAGE_LIB_CHECKS)
