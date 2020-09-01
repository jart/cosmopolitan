#!/bin/sh
#-*-mode:sh;indent-tabs-mode:nil;tab-width:2;coding:utf-8-*-┐
#───vi: set net ft=sh ts=2 sts=2 fenc=utf-8 :vi─────────────┘
#
# OVERVIEW
#
#   Compiler Version Discovery
#
# DESCRIPTION
#
#   Cosmopolitan itself may be built using either GCC and Clang, and our
#   build irons out many of the differences between the two. This script
#   is used by build/definitions.mk alongside build/getccname to support
#   the different versions folks use.
#
#   Our aim is to support GCC 4.2.1+ since that's the last GPLv2 version
#   with any sort of industry consensus. Please note, Cosmopolitan never
#   links GCC runtimes when using later versions, so some concerns might
#   not apply.

if [ ! -d o/third_party/gcc ]; then
  third_party/gcc/unbundle.sh
fi

set -e

MAJOR_VERSION=$(
  $1 --version |
    head -n1 |
    sed -n '
        s!^[^(]*([^)]*) \([[:digit:]][[:digit:]]*\).*!\1!p
        s!^.*clang.*version \([[:digit:]][[:digit:]]*\).*!\1!p
      ')

if [ -z "$MAJOR_VERSION" ]; then
  echo 6
else
  printf '%s\n' "$MAJOR_VERSION"
fi
