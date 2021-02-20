#!/bin/sh
#
# SYNOPSIS
#
#   gcc -m32 -wrapper build/i386ify.sh -c -o foo.o foo.c
#
# OVERVIEW
#
#   Compiles 32-bit code inside 64-bit ELF objects.

if [ "${1##*/}" = as ]; then
  FIRST=0
  for x; do
    if [ $FIRST -eq 0 ]; then
      set --
      FIRST=1
    fi
    if [ "$x" = "--32" ]; then
      continue
    fi
    if [ "${x##*.}" = s ]; then
      {
        printf "\t.section .yoink\n"
        printf "\tnopl\t_start32\n"
        printf "\t.previous\n"
        printf "\t.code32\n"
        cat "$x"
      } >"$x".tmp
      mv -f "$x".tmp "$x"
    fi
    set -- "$@" "$x"
  done
fi

exec "$@"
