#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#── vi: set noet ft=make ts=8 sw=8 fenc=utf-8 :vi ────────────────────┘
#
# SYNOPSIS
#
#   Your package static library build config
#
# DESCRIPTION
#
#   Your library doesn't have a main() function and can be compromised
#   of sources written in multiple languages.
#
# WARNING
#
#   This library (examples/package/lib/) comes earlier in the
#   topological order of things than its parent (examples/package/)
#   because the parent package depends on the subpackage. Therefore,
#   this package needs to be written earlier in the Makefile includes.
#
# EXAMPLE
#
#   make o//examples/package/lib  # build library w/ sanity checks
#   ar t o//examples/package/lib/lib.a
#
# AUTHORS
#
#   %AUTHOR%

PKGS += EXAMPLES_PACKAGE_LIB

# Declares package i.e. library w/ transitive dependencies.
# We define packages as a thing that lumps similar sources.
# It's needed, so linkage can have a higher level overview.
# Mostly due to there being over 9,000 objects in the repo.
EXAMPLES_PACKAGE_LIB =						\
	$(EXAMPLES_PACKAGE_LIB_A_DEPS)				\
	$(EXAMPLES_PACKAGE_LIB_A)

# Declares library w/o transitive dependencies.
EXAMPLES_PACKAGE_LIB_A = o/$(MODE)/examples/package/lib/lib.a
EXAMPLES_PACKAGE_LIB_ARTIFACTS += EXAMPLES_PACKAGE_LIB_A

# Build configs might seem somewhat complicated. Rest assured they're
# mostly maintenance free. That's largely thanks to how we wildcard.
EXAMPLES_PACKAGE_LIB_A_FILES := $(wildcard examples/package/lib/*)
EXAMPLES_PACKAGE_LIB_A_HDRS = $(filter %.h,$(EXAMPLES_PACKAGE_LIB_A_FILES))

# Define sets of source files without needing further iops.
EXAMPLES_PACKAGE_LIB_A_SRCS_S =					\
	$(filter %.S,$(EXAMPLES_PACKAGE_LIB_A_FILES))
EXAMPLES_PACKAGE_LIB_A_SRCS_C =					\
	$(filter %.c,$(EXAMPLES_PACKAGE_LIB_A_FILES))
EXAMPLES_PACKAGE_LIB_A_SRCS =					\
	$(EXAMPLES_PACKAGE_LIB_A_SRCS_S)			\
	$(EXAMPLES_PACKAGE_LIB_A_SRCS_C)

# Change suffixes of different languages extensions into object names.
EXAMPLES_PACKAGE_LIB_A_OBJS =					\
	$(EXAMPLES_PACKAGE_LIB_A_SRCS_S:%.S=o/$(MODE)/%.o)	\
	$(EXAMPLES_PACKAGE_LIB_A_SRCS_C:%.c=o/$(MODE)/%.o)

# Does the two most important things for C/C++ code sustainability.
# 1. Guarantees each header builds, i.e. includes symbols it needs.
# 2. Guarantees transitive closure of packages is directed acyclic.
EXAMPLES_PACKAGE_LIB_A_CHECKS =					\
	$(EXAMPLES_PACKAGE_LIB_A).pkg				\
	$(EXAMPLES_PACKAGE_LIB_A_HDRS:%=o/$(MODE)/%.ok)

# Lists packages whose symbols are or may be directly referenced here.
# Note that linking stubs is always a good idea due to synthetic code.
EXAMPLES_PACKAGE_LIB_A_DIRECTDEPS =				\
	LIBC_INTRIN						\
	LIBC_NEXGEN32E						\
	LIBC_STDIO

# Evaluates variable as set of transitive package dependencies.
EXAMPLES_PACKAGE_LIB_A_DEPS :=					\
	$(call uniq,$(foreach x,$(EXAMPLES_PACKAGE_LIB_A_DIRECTDEPS),$($(x))))

# Concatenates object files into single file with symbol index.
# Please don't use fancy make features for mutating archives it's slow.
$(EXAMPLES_PACKAGE_LIB_A):					\
		examples/package/lib/				\
		$(EXAMPLES_PACKAGE_LIB_A).pkg			\
		$(EXAMPLES_PACKAGE_LIB_A_OBJS)

# Asks packager to index symbols and validate their relationships.
# It's the real secret sauce for having a huge Makefile w/o chaos.
# @see tool/build/package.c
# @see build/rules.mk
$(EXAMPLES_PACKAGE_LIB_A).pkg:					\
		$(EXAMPLES_PACKAGE_LIB_A_OBJS)			\
		$(foreach x,$(EXAMPLES_PACKAGE_LIB_A_DIRECTDEPS),$($(x)_A).pkg)

# Invalidates objects in package when makefile is edited.
$(EXAMPLES_PACKAGE_LIB_A_OBJS): examples/package/lib/BUILD.mk

# let these assembly objects build on aarch64
o/$(MODE)/examples/package/lib/myasm.o: examples/package/lib/myasm.S
	@$(COMPILE) -AOBJECTIFY.S $(OBJECTIFY.S) $(OUTPUT_OPTION) -c $<

EXAMPLES_PACKAGE_LIB_LIBS = $(foreach x,$(EXAMPLES_PACKAGE_LIB_ARTIFACTS),$($(x)))
EXAMPLES_PACKAGE_LIB_SRCS = $(foreach x,$(EXAMPLES_PACKAGE_LIB_ARTIFACTS),$($(x)_SRCS))
EXAMPLES_PACKAGE_LIB_HDRS = $(foreach x,$(EXAMPLES_PACKAGE_LIB_ARTIFACTS),$($(x)_HDRS))
EXAMPLES_PACKAGE_LIB_BINS = $(foreach x,$(EXAMPLES_PACKAGE_LIB_ARTIFACTS),$($(x)_BINS))
EXAMPLES_PACKAGE_LIB_CHECKS = $(foreach x,$(EXAMPLES_PACKAGE_LIB_ARTIFACTS),$($(x)_CHECKS))
EXAMPLES_PACKAGE_LIB_OBJS = $(foreach x,$(EXAMPLES_PACKAGE_LIB_ARTIFACTS),$($(x)_OBJS))
EXAMPLES_PACKAGE_LIB_TESTS = $(foreach x,$(EXAMPLES_PACKAGE_LIB_ARTIFACTS),$($(x)_TESTS))

.PHONY: o/$(MODE)/examples/package/lib
o/$(MODE)/examples/package/lib: $(EXAMPLES_PACKAGE_LIB_CHECKS)
