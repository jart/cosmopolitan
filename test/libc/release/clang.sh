#!/bin/sh
#-*-mode:sh;indent-tabs-mode:nil;tab-width:2;coding:utf-8-*-┐
#───vi: set net ft=sh ts=2 sts=2 fenc=utf-8 :vi─────────────┘

# TODO: someone who uses clang please mantain this

# if CLANG=$(command -v clang); then
#   mkdir -p o/$MODE/test/libc/release
#   $CLANG                                               \
#       -o o/$MODE/test/libc/release/smokeclang2.com.dbg \
#       -Os                                              \
#       -Wall                                            \
#       -Werror                                          \
#       -static                                          \
#       -fno-pie                                         \
#       -nostdlib                                        \
#       -nostdinc                                        \
#       -fuse-ld=lld                                     \
#       -mno-red-zone                                    \
#       -Wl,-T,o/$MODE/ape/ape.lds                       \
#       -include o/cosmopolitan.h                        \
#       test/libc/release/smoke.c                        \
#       o/$MODE/libc/crt/crt.o                           \
#       o/$MODE/ape/ape.o                                \
#       o/$MODE/cosmopolitan.a || exit
#   o/$MODE/test/libc/release/smokeclang2.com.dbg || exit
# fi
