#!/bin/sh
#-*-mode:sh;indent-tabs-mode:nil;tab-width:2;coding:utf-8-*-┐
#───vi: set net ft=sh ts=2 sts=2 fenc=utf-8 :vi─────────────┘

if CLANG=$(command -v clang); then
  $COMPILE $CLANG                                     \
      -o o/$MODE/test/libc/release/smokeclang.com.dbg \
      -Os                                             \
      -static                                         \
      -no-pie                                         \
      -fno-pie                                        \
      -nostdlib                                       \
      -nostdinc                                       \
      -mno-red-zone                                   \
      -Wl,--gc-sections                               \
      -Wl,-z,max-page-size=0x1000                     \
      -Wl,-T,o/$MODE/ape/ape.lds                      \
      -include o/cosmopolitan.h                       \
      test/libc/release/smoke.c                       \
      o/$MODE/libc/crt/crt.o                          \
      o/$MODE/ape/ape.o                               \
      o/$MODE/cosmopolitan.a || exit
  o/$MODE/test/libc/release/smokeclang.com.dbg || exit
fi
