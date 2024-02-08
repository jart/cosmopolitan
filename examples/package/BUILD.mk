#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#── vi: set noet ft=make ts=8 sw=8 fenc=utf-8 :vi ────────────────────┘
#
# SYNOPSIS
#
#   Your package build config for executable programs
#
# DESCRIPTION
#
#   We assume each .c file in this directory has a main() function, so
#   that it becomes as easy as possible to write lots of tiny programs
#
# EXAMPLE
#
#   make o//examples/package
#   o/examples/package/program.com
#
# AUTHORS
#
#   %AUTHOR%

PKGS += EXAMPLES_PACKAGE

# Reads into memory the list of files in this directory.
EXAMPLES_PACKAGE_FILES := $(wildcard examples/package/*)

# Defines sets of files without needing further iops.
EXAMPLES_PACKAGE_SRCS = $(filter %.c,$(EXAMPLES_PACKAGE_FILES))
EXAMPLES_PACKAGE_HDRS = $(filter %.h,$(EXAMPLES_PACKAGE_FILES))
EXAMPLES_PACKAGE_COMS = $(EXAMPLES_PACKAGE_SRCS:%.c=o/$(MODE)/%.com)
EXAMPLES_PACKAGE_BINS =					\
	$(EXAMPLES_PACKAGE_COMS)			\
	$(EXAMPLES_PACKAGE_COMS:%=%.dbg)

# Remaps source file names to object names.
# Also asks a wildcard rule to automatically run tool/build/zipobj.c
EXAMPLES_PACKAGE_OBJS =					\
	$(EXAMPLES_PACKAGE_SRCS:%.c=o/$(MODE)/%.o)

# Lists packages whose symbols are or may be directly referenced here.
# Note that linking stubs is always a good idea due to synthetic code.
EXAMPLES_PACKAGE_DIRECTDEPS =				\
	EXAMPLES_PACKAGE_LIB				\
	LIBC_INTRIN					\
	LIBC_STDIO					\
	LIBC_TINYMATH

# Evaluates the set of transitive package dependencies.
EXAMPLES_PACKAGE_DEPS :=				\
	$(call uniq,$(foreach x,$(EXAMPLES_PACKAGE_DIRECTDEPS),$($(x))))

$(EXAMPLES_PACKAGE_A).pkg:				\
		$(EXAMPLES_PACKAGE_OBJS)		\
		$(foreach x,$(EXAMPLES_PACKAGE_DIRECTDEPS),$($(x)_A).pkg)

# Specifies how to build programs as ELF binaries with DWARF debug info.
# @see build/rules.mk for definition of rule that does .com.dbg -> .com
o/$(MODE)/examples/package/%.com.dbg:			\
		$(EXAMPLES_PACKAGE_DEPS)		\
		o/$(MODE)/examples/package/%.o		\
		$(CRT)					\
		$(APE_NO_MODIFY_SELF)
	@$(APELINK)

# Invalidates objects in package when makefile is edited.
$(EXAMPLES_PACKAGE_OBJS): examples/package/BUILD.mk

# Creates target building everything in package and subpackages.
.PHONY: o/$(MODE)/examples/package
o/$(MODE)/examples/package:				\
		o/$(MODE)/examples/package/lib		\
		$(EXAMPLES_PACKAGE_BINS)
