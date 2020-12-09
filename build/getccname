#!/bin/sh
#-*-mode:sh;indent-tabs-mode:nil;tab-width:2;coding:utf-8-*-┐
#───vi: set net ft=sh ts=2 sts=2 fenc=utf-8 :vi─────────────┘
#
# OVERVIEW
#
#   Compiler Name Discovery
#
# DESCRIPTION
#
#   Cosmopolitan itself may be built using either GCC and Clang, and our
#   build irons out many of the differences between the two. This script
#   determines which one's in play, which is nontrivial, since they tend
#   to call themselves so many different names.

if [ ! -d o/third_party/gcc ]; then
  third_party/gcc/unbundle.sh
fi

set -e

SPECIAL_TEXT=$(
  $1 --version |
    sed -n '
        /chibicc/ {
          i\
chibicc
          q
        }
        /Free Software/ {
          i\
gcc
          q
        }
        /clang/ {
          i\
clang
          q
        }
      ')

if [ -z "$SPECIAL_TEXT" ]; then
  echo gcc
else
  printf '%s\n' "$SPECIAL_TEXT"
fi
