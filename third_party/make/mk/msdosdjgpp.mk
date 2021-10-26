# GNU -*-Makefile-*- to build GNU make on MS-DOS with DJGPP
#
# MS-DOS overrides for use with Basic.mk.
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

OBJEXT = o
EXEEXT = .exe

CC = gcc

# Translate a POSIX path into a Windows path.  Don't bother with drives.
# Used only inside recipes, with DOS/CMD tools that require it.
P2W = $(subst /,\,$1)

prog_SOURCES += $(loadavg_SOURCES) $(glob_SOURCES)

BUILT_SOURCES += $(lib)alloca.h $(lib)fnmatch.h $(lib)glob.h

INCLUDEDIR = c:/djgpp/include
LIBDIR = c:/djgpp/lib
LOCALEDIR = c:/djgpp/share

MKDIR.cmd = command.com /c mkdir $(call P2W,$1)
RM.cmd = command.com /c del /F /Q $(call P2W,$1)
CP.cmd = command.com /c copy /Y $(call P2W,$1 $2)

$(OUTDIR)src/config.h: $(SRCDIR)/src/configh.dos
	$(call CP.cmd,$<,$@)
