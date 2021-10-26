# GNU -*-Makefile-*- to build GNU make on VMS
#
# VMS overrides for use with Basic.mk.
#
# Copyright (C) 2017-2020 Free Software Foundation, Inc.
# This file is part of GNU Make.
#
# GNU Make is free software; you can redistribute it and/or modify it under
# the terms of the GNU General Public License as published by the Free Software
# Foundation; either version 3 of the License, or (at your option) any later
# version.
#
# GNU Make is distributed in the hope that it will be useful, but WITHOUT ANY
# WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
# details.
#
# You should have received a copy of the GNU General Public License along with
# this program.  If not, see <http://www.gnu.org/licenses/>.

src = [.src]
lib = [.lib]
SRCDIR = []

OBJEXT = .obj
EXEEXT = .exe

e =
s = $e $e
c = ,

defs = HAVE_CONFIG_H

ifeq ($(CC),cc)
defs += VMS unlink=remove allocated_variable_expand_for_file=alloc_var_expand_for_file
else
defs += GCC_IS_NATIVE
ifeq ($(ARCH),VAX)
defs += VAX
endif
endif

extra_CPPFLAGS = /define=($(subst $s,$c,$(patsubst %,"%",$(defs))))

cinclude = /nested=none/include=($(src),$(lib))
ifeq ($(CC),cc)
cprefix = /prefix=(all,except=(glob,globfree))
cwarn = /standard=relaxed/warn=(disable=questcompare)
endif

extra_CFLAGS = $(cinclude)$(cprefix)$(cwarn)

#extra_LDFLAGS = /deb
extra_LDFLAGS =

# If your system needs extra libraries loaded in, define them here.
# System V probably need -lPW for alloca.
# if on vax, uncomment the following line
#LDLIBS = ,c.opt/opt
ifeq ($(CC),cc)
#LDLIBS =,sys$$library:vaxcrtl.olb/lib
else
LDLIBS =,gnu_cc_library:libgcc.olb/lib
endif

# If your system doesn't have alloca, or the one provided is bad,
# uncomment this
#ALLOCA = $(alloca_SOURCES)

# If your system doesn't have alloca.h, or the one provided is bad,
# uncomment this
#BUILT_SOURCES += $(lib)alloca.h

prog_SOURCES += $(ALLOCA) $(glob_SOURCES) $(vms_SOURCES)

BUILT_SOURCES += $(lib)fnmatch.h $(lib)glob.h

COMPILE.cmd = $(CC) $(extra_CFLAGS)$(CFLAGS)/obj=$@ $(extra_CPPFLAGS)$(CPPFLAGS) $1

LINK.cmd = $(LD)$(extra_LDFLAGS)$(LDFLAGS)/exe=$@ $(subst $s,$c,$1)$(LDLIBS)

# Don't know how to do this
CHECK.cmd =

MKDIR.cmd = create/dir $1
RM.cmd = delete $1
CP.cmd = copy $1 $2

define CLEANSPACE
	-purge [...]
	-delete $(PROG);
	-delete $(src)*.$(OBJEXT);
endef


$(OUTDIR)$(src)config.h: $(SRCDIR)$(src)config.h.W32
	$(call CP.cmd,$<,$@)
