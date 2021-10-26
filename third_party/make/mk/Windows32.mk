# GNU -*-Makefile-*- to build GNU make on Windows
#
# Windows overrides for use with Basic.mk.
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

# TARGET_TYPE can be either "release" or "debug"
TARGET_TYPE = release

# TOOLCHAIN can be either "msvc" or "gcc"
TOOLCHAIN = msvc

# Translate a POSIX path into a Windows path.  Don't bother with drives.
# Used only inside recipes, with DOS/CMD tools that require it.
P2W = $(subst /,\,$1)

prog_SOURCES += $(loadavg_SOURCES) $(glob_SOURCES) $(w32_SOURCES)

BUILT_SOURCES += $(lib)alloca.h $(lib)fnmatch.h $(lib)glob.h

w32_LIBS = kernel32 user32 gdi32 winspool comdlg32 advapi32 shell32 ole32 \
	   oleaut32 uuid odbc32 odbccp32

CPPFLAGS =
CFLAGS =
LDFLAGS =

# --- Visual Studio
msvc_CC = cl.exe
msvc_LD = link.exe

msvc_CPPFLAGS = /DHAVE_CONFIG_H /DWINDOWS32 /DWIN32 /D_CONSOLE
msvc_CPPFLAGS += /I$(OUTDIR)src /I$(SRCDIR)/src /I$(SRCDIR)/src/w32/include /I$(OUTDIR)lib /I$(SRCDIR)/lib

msvc_CFLAGS = /nologo /MT /W4 /EHsc
msvc_CFLAGS += /FR$(OUTDIR) /Fp$(BASE_PROG).pch /Fd$(BASE_PROG).pdb

msvc_LDFLAGS = /nologo /SUBSYSTEM:console /PDB:$(BASE_PROG).pdb

msvc_LDLIBS = $(addsuffix .lib,$(w32_LIBS))

msvc_C_SOURCE = /c
msvc_OUTPUT_OPTION = /Fo$@
msvc_LINK_OUTPUT = /OUT:$@

release_msvc_OUTDIR = ./WinRel/
release_msvc_CPPFLAGS = /D NDEBUG
release_msvc_CFLAGS = /O2

debug_msvc_OUTDIR = ./WinDebug/
debug_msvc_CPPFLAGS = /D _DEBUG
debug_msvc_CFLAGS = /Zi /Od
debug_msvc_LDFLAGS = /DEBUG

# --- GCC
gcc_CC = gcc
gcc_LD = $(gcc_CC)

release_gcc_OUTDIR = ./GccRel/
debug_gcc_OUTDIR = ./GccDebug/

gcc_CPPFLAGS = -DHAVE_CONFIG_H -I$(OUTDIR)src -I$(SRCDIR)/src -I$(SRCDIR)/src/w32/include -I$(OUTDIR)lib -I$(SRCDIR)/lib
gcc_CFLAGS = -mthreads -Wall -std=gnu99 -gdwarf-2 -g3
gcc_LDFLAGS = -mthreads -gdwarf-2 -g3
gcc_LDLIBS = $(addprefix -l,$(w32_libs))

gcc_C_SOURCE = -c
gcc_OUTPUT_OPTION = -o $@
gcc_LINK_OUTPUT = -o $@

debug_gcc_CFLAGS = -O0
release_gcc_CFLAGS = -O2

# ---

LINK.cmd = $(LD) $(extra_LDFLAGS) $(LDFLAGS) $(TARGET_ARCH) $1 $(LDLIBS) $(LINK_OUTPUT)

CHECK.cmd = cmd /c cd tests \& .\run_make_tests.bat -make ../$(PROG)

MKDIR.cmd = cmd /c mkdir $(call P2W,$1)
RM.cmd = cmd /c del /F /Q $(call P2W,$1)
CP.cmd = cmd /c copy /Y $(call P2W,$1 $2)

CC = $($(TOOLCHAIN)_CC)
LD = $($(TOOLCHAIN)_LD)

C_SOURCE = $($(TOOLCHAIN)_C_SOURCE)
OUTPUT_OPTION = $($(TOOLCHAIN)_OUTPUT_OPTION)
LINK_OUTPUT = $($(TOOLCHAIN)_LINK_OUTPUT)

OUTDIR = $($(TARGET_TYPE)_$(TOOLCHAIN)_OUTDIR)

OBJEXT	= obj
EXEEXT	= .exe

_CUSTOM = $($(TOOLCHAIN)_$1) $($(TARGET_TYPE)_$1) $($(TARGET_TYPE)_$(TOOLCHAIN)_$1)

# I'm not sure why this builds gnumake rather than make...?
PROG = $(OUTDIR)gnumake$(EXEEXT)
BASE_PROG = $(basename $(PROG))

extra_CPPFLAGS = $(call _CUSTOM,CPPFLAGS)
extra_CFLAGS = $(call _CUSTOM,CFLAGS)
extra_LDFLAGS = $(call _CUSTOM,LDFLAGS)
LDLIBS = $(call _CUSTOM,LDLIBS)

$(OUTDIR)src/config.h: $(SRCDIR)/src/config.h.W32
	$(call CP.cmd,$<,$@)
