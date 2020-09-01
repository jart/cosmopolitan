#!/bin/sh
#-*-mode:sh;indent-tabs-mode:nil;tab-width:2;coding:utf-8-*-┐
#───vi: set net ft=sh ts=2 sts=2 fenc=utf-8 :vi─────────────┘
#
# OVERVIEW
#
#   GNU Disassembly Veneer
#
# DESCRIPTION
#
#   This script wraps normal objdump commands that're transparently
#   passed-through.
#
# EXAMPLE
#
#   build/objdump -xd o/tiny/examples/life.com.dbg

if [ ! -d o/third_party/gcc ]; then
  third_party/gcc/unbundle.sh
fi

exec o/third_party/gcc/bin/x86_64-linux-musl-objdump "$@"
