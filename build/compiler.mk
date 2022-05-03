#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#───vi: set et ft=make ts=8 tw=8 fenc=utf-8 :vi───────────────────────┘
#
# SYNOPSIS
#
#   Cosmopolitan Compiler and Toold Definitions
#

# see build/compile, etc. which run third_party/gcc/unbundle.sh
AR = build/bootstrap/ar.com
PKG = build/bootstrap/package.com
MKDEPS = build/bootstrap/mkdeps.com
ZIPOBJ = build/bootstrap/zipobj.com

ifeq ($(shell uname), Darwin)

TOOLPREFIX=/usr/local/bin/x86_64-linux-musl-
AS = $(TOOLPREFIX)as
CC = $(TOOLPREFIX)gcc
CXX = $(TOOLPREFIX)g++
CXXFILT = $(TOOLPREFIX)c++filt
LD = $(TOOLPREFIX)ld.bfd
NM = $(TOOLPREFIX)nm
GCC = $(TOOLPREFIX)gcc
STRIP = $(TOOLPREFIX)strip
OBJCOPY = $(TOOLPREFIX)objcopy
OBJDUMP = $(TOOLPREFIX)ojbdump
ADDRLINE = $(TOOLPREFIX)addr2line

else

AS = o/third_party/gcc/bin/x86_64-linux-musl-as
CC = o/third_party/gcc/bin/x86_64-linux-musl-gcc
CXX = o/third_party/gcc/bin/x86_64-linux-musl-g++
CXXFILT = o/third_party/gcc/bin/x86_64-linux-musl-c++filt
LD = o/third_party/gcc/bin/x86_64-linux-musl-ld.bfd
NM = o/third_party/gcc/bin/x86_64-linux-musl-nm
GCC = o/third_party/gcc/bin/x86_64-linux-musl-gcc
STRIP = o/third_party/gcc/bin/x86_64-linux-musl-strip
OBJCOPY = o/third_party/gcc/bin/x86_64-linux-musl-objcopy
OBJDUMP = o/third_party/gcc/bin/x86_64-linux-musl-objdump
ADDR2LINE = $(shell pwd)/o/third_party/gcc/bin/x86_64-linux-musl-addr2line

endif
