# Basic GNU -*-Makefile-*- to build GNU make
#
# NOTE:
# If you have no 'make' program at all to process this makefile:
#   * On Windows, run ".\buildw32.bat" to bootstrap one.
#   * On MS-DOS, run ".\builddos.bat" to bootstrap one.
#
# Once you have a GNU make program created, you can use it with this makefile
# to keep it up to date if you make changes, as:
#
#   make.exe -f Basic.mk
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

all:

src = src/
lib = lib/

make_SOURCES = $(src)ar.c $(src)arscan.c $(src)commands.c $(src)default.c $(src)dir.c $(src)expand.c $(src)file.c $(src)function.c $(src)getopt.c $(src)getopt1.c $(src)guile.c $(src)hash.c $(src)implicit.c $(src)job.c $(src)load.c $(src)loadapi.c $(src)main.c $(src)misc.c $(src)output.c $(src)read.c $(src)remake.c $(src)rule.c $(src)signame.c $(src)strcache.c $(src)variable.c $(src)version.c $(src)vpath.c
glob_SOURCES = $(lib)fnmatch.c $(lib)glob.c
loadavg_SOURCES = $(lib)getloadavg.c
alloca_SOURCES = $(lib)alloca.c
w32_SOURCES = $(src)w32/pathstuff.c $(src)w32/w32os.c $(src)w32/compat/dirent.c $(src)w32/compat/posixfcn.c $(src)w32/subproc/misc.c $(src)w32/subproc/sub_proc.c $(src)w32/subproc/w32err.c
vms_SOURCES = $(src)vms_exit.c $(src)vms_export_symbol.c $(src)vms_progname.c $(src)vmsfunctions.c $(src)vmsify.c
amiga_SOURCES = $(src)amiga.c

posix_SOURCES = $(src)posixos.c
remote_SOURCES = $(src)remote-stub.c

OUTDIR =
SRCDIR = .

OBJEXT = o
EXEEXT =

PREFIX = /usr/local
INCLUDEDIR = $(PREFIX)/include
LIBDIR = $(PREFIX)/lib
LOCALEDIR = $(PREFIX)/share

PROG = $(OUTDIR)make$(EXEEXT)

prog_SOURCES = $(make_SOURCES) $(remote_SOURCES)

BUILT_SOURCES =

OBJECTS = $(patsubst %.c,$(OUTDIR)%.$(OBJEXT),$(prog_SOURCES))

OBJDIRS = $(addsuffix .,$(sort $(dir $(OBJECTS))))

# Use the default value of CC
LD = $(CC)

# Reserved for command-line override
CPPFLAGS =
CFLAGS = -g -O2
LDFLAGS =

extra_CPPFLAGS = -DHAVE_CONFIG_H -I$(OUTDIR)src -I$(SRCDIR)/src -I$(OUTDIR)lib -I$(SRCDIR)/lib \
	-DLIBDIR=\"$(LIBDIR)\" -DINCLUDEDIR=\"$(INCLUDEDIR)\" -DLOCALEDIR=\"$(LOCALDIR)\"
extra_CFLAGS =
extra_LDFLAGS = $(extra_CFLAGS) $(CFLAGS)

C_SOURCE = -c
OUTPUT_OPTION = -o $@
LINK_OUTPUT = -o $@

# Command lines

# $(call COMPILE.cmd,<src>,<tgt>)
COMPILE.cmd = $(CC) $(extra_CFLAGS) $(CFLAGS) $(extra_CPPFLAGS) $(CPPFLAGS) $(TARGET_ARCH) $(OUTPUT_OPTION) $(C_SOURCE) $1

# $(call LINK.cmd,<objectlist>)
LINK.cmd = $(LD) $(extra_LDFLAGS) $(LDFLAGS) $(TARGET_ARCH) $1 $(LDLIBS) $(LINK_OUTPUT)

# $(CHECK.cmd)
CHECK.cmd = cd $(SRCDIR)/tests && ./run_make_tests -make $(shell cd $(<D) && pwd)/$(<F)

# $(call MKDIR.cmd,<dirlist>)
MKDIR.cmd = mkdir -p $1

# $(call RM.cmd,<filelist>)
RM.cmd = rm -f $1

# $(call CP.cmd,<from>,<to>)
CP.cmd = cp $1 $2

CLEANSPACE = $(call RM.cmd,$(OBJECTS) $(PROG) $(BUILT_SOURCES))

# Load overrides for the above variables.
include $(firstword $(wildcard $(SRCDIR)/mk/$(lastword $(subst -, ,$(MAKE_HOST)).mk) $(OUTDIR)mk/Posix.mk $(SRCDIR)/mk/Posix.mk))

VERSION = 4.3

VPATH = $(SRCDIR)

all: $(PROG)

$(PROG): $(OBJECTS)
	$(call LINK.cmd,$^)

$(OBJECTS): $(OUTDIR)%.$(OBJEXT): %.c
	$(call COMPILE.cmd,$<)

$(OBJECTS): | $(OBJDIRS) $(BUILT_SOURCES)

$(OBJDIRS):
	$(call MKDIR.cmd,$@)

check:
	$(CHECK.cmd)

clean:
	$(CLEANSPACE)

$(filter %.h,$(BUILT_SOURCES)): %.h : %.in.h
	$(call RM.cmd,$@)
	$(call CP.cmd,$<,$@)

.PHONY: all check clean

# --------------- DEPENDENCIES
#

# src/.deps/amiga.Po
# dummy

# src/.deps/ar.Po
# dummy

# src/.deps/arscan.Po
# dummy

# src/.deps/commands.Po
# dummy

# src/.deps/default.Po
# dummy

# src/.deps/dir.Po
# dummy

# src/.deps/expand.Po
# dummy

# src/.deps/file.Po
# dummy

# src/.deps/function.Po
# dummy

# src/.deps/getopt.Po
# dummy

# src/.deps/getopt1.Po
# dummy

# src/.deps/guile.Po
# dummy

# src/.deps/hash.Po
# dummy

# src/.deps/implicit.Po
# dummy

# src/.deps/job.Po
# dummy

# src/.deps/load.Po
# dummy

# src/.deps/loadapi.Po
# dummy

# src/.deps/main.Po
# dummy

# src/.deps/misc.Po
# dummy

# src/.deps/output.Po
# dummy

# src/.deps/posixos.Po
# dummy

# src/.deps/read.Po
# dummy

# src/.deps/remake.Po
# dummy

# src/.deps/remote-cstms.Po
# dummy

# src/.deps/remote-stub.Po
# dummy

# src/.deps/rule.Po
# dummy

# src/.deps/signame.Po
# dummy

# src/.deps/strcache.Po
# dummy

# src/.deps/variable.Po
# dummy

# src/.deps/version.Po
# dummy

# src/.deps/vms_exit.Po
# dummy

# src/.deps/vms_export_symbol.Po
# dummy

# src/.deps/vms_progname.Po
# dummy

# src/.deps/vmsfunctions.Po
# dummy

# src/.deps/vmsify.Po
# dummy

# src/.deps/vpath.Po
# dummy
