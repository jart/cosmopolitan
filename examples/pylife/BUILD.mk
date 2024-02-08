#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#── vi: set noet ft=make ts=8 sw=8 fenc=utf-8 :vi ────────────────────┘
#
# SYNOPSIS
#
#   Actually Portable Python Tutorial
#
# DESCRIPTION
#
#   This tutorial demonstrates how to compile Python apps as tiny
#   static multiplatform APE executables as small as 1.9m in size
#   using Cosmopolitan, which is a BSD-style multitenant codebase
#
# GETTING STARTED
#
#   # run these commands after cloning the cosmo repo on linux
#   $ make -j8 o//examples/pylife/pylife.com
#   $ o//examples/pylife/pylife.com
#   cosmopolitan is cool!
#
# HOW IT WORKS
#
#   $ pyobj.com -m -o pylife.o pylife.py
#   $ ld -static -nostdlib -T o//ape/ape.lds ape.o crt.o \
#       pylife.o \
#       cosmopolitan-python-stage2.a \
#       cosmopolitan-sqlite3.a \
#       cosmopolitan-linenoise.a \
#       cosmopolitan-bzip2.a \
#       cosmopolitan-python-stage1.a \
#       cosmopolitan.a
#   $ ./pylife.com
#   cosmopolitan is cool!
#
# NOTES
#
#   If you enjoy this tutorial, let us know jtunney@gmail.com. If
#   you're building something cool, then we can we can add you to
#   our .gitowners file which grants you commit access so you can
#   indepnedently maintain your package, as part of the mono-repo

PKGS      += PYLIFE
PYLIFE      = $(PYLIFE_DEPS) o/$(MODE)/examples/pylife/pylife.a
PYLIFE_COMS = o/$(MODE)/examples/pylife/pylife.com
PYLIFE_BINS = $(PYLIFE_COMS) $(PYLIFE_COMS:%=%.dbg)

# Specify our Cosmopolitan library dependencies
#
# - THIRD_PARTY_PYTHON_STAGE1 plus THIRD_PARTY_PYTHON_STAGE2 will
#   define the Python CAPI and supported standard library modules
#
PYLIFE_DIRECTDEPS = \
	THIRD_PARTY_PYTHON_STAGE2

# Compute the transitive closure of dependencies. There's dozens of
# other static libraries we need, in order to build a static binary
# such as fmt.a, runtime.a, str.a etc. This magic statement figures
# them all out and arranges them in the correct order.
PYLIFE_DEPS := $(call uniq,$(foreach x,$(PYLIFE_DIRECTDEPS),$($(x))))

# # Asks PYOBJ.COM to turn our Python source into an ELF object which
# # contains (a) embedded zip file artifacts of our .py file and .pyc
# # which it it compiled; and (b) statically analyzed listings of our
# # python namespaces and imports that GNU ld will use for tree shake
# # NOTE: This code can be commented out since it's in build/rules.mk
# o/$(MODE)/examples/pylife/pylife.o: examples/pylife/pylife.py o/$(MODE)/third_party/python/pyobj
# 	o/$(MODE)/third_party/python/pyobj $(PYFLAGS) -o $@ $<

# We need to define how the repository source code path gets mapped
# into an APE ZIP file path. By convention, we place Python modules
# in `.python/` (which is accessible via open() system calls, using
# the synthetic path `"/zip/.python/"`) which means that if we want
# to turn `pylife/pylife.py` into `.python/pylife.py` so it's imported
# using `import pylife` then we can simply append to PYOBJ.COM flags
# flags above asking it to strip one directory component and prefix
# Lastly be sure that whenever you use this variable override trick
# you only do it to .o files, since otherwise it'll ruin everything
# Passing -m to PYOBJ.COM causes a C main() function to get yoinked
# and it means our Python module can no longer be used as a library
o/$(MODE)/examples/pylife/pylife.o: PYFLAGS += -m -C2 -P.python

# Asks PACKAGE.COM to sanity check our DIRECTDEPS and symbol graph.
# This program functions as an incremental linker. It also provides
# enhancements to the object code that GCC generated similar to LTO
# so be certain that your .com.dbg rule depends on the .pkg output!
o/$(MODE)/examples/pylife/pylife.pkg: \
		o/$(MODE)/examples/pylife/pylife.o \
		$(foreach x,$(PYLIFE_DIRECTDEPS),$($(x)_A).pkg)

# Ask GNU LD to link our APE executable within an ELF binary shell.
# The CRT and APE dependencies are special dependencies that define
# your _start() / WinMain() entrpoints as well as APE linker script
o/$(MODE)/examples/pylife/pylife.com.dbg: \
		$(PYLIFE_DEPS) \
		o/$(MODE)/examples/pylife/pylife.pkg \
		o/$(MODE)/examples/pylife/pylife.o \
		$(CRT) \
		$(APE_NO_MODIFY_SELF)
	@$(COMPILE) -ALINK.ape $(LINK) $(LINKARGS) -o $@

# # Unwrap the APE .COM binary, that's embedded within the linked file
# # NOTE: This line can be commented out, since it's in build/rules.mk
# o/$(MODE)/examples/pylife/pylife.com: \
# 		o/$(MODE)/examples/pylife/pylife.com.dbg
# 	$(OBJCOPY) -S -O binary $< $@

# Ensure that build config changes will invalidate build artifacts.
o/$(MODE)/examples/pylife/pylife.o: \
		examples/pylife/BUILD.mk

# By convention we want to be able to say `make -j8 o//examples/pylife`
# and have it build all targets the package defines.
.PHONY: o/$(MODE)/examples/pylife
o/$(MODE)/examples/pylife: \
		o/$(MODE)/examples/pylife/pylife.com \
		o/$(MODE)/examples/pylife/pylife.com.dbg
