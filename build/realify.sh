#!/bin/sh
#
# SYNOPSIS
#
#   gcc -g0 -Os -wrapper build/realify.sh -ffixed-r{8,9,1{0,1,2,4,5}}
#
# OVERVIEW
#
#   Reconfigures x86_64 compiler to emit 16-bit PC boot code.

if [ "${1##*/}" = as ]; then
  for x; do
    if [ "${x##*.}" = s ]; then
      {
        printf "\t.code16gcc"
        sed -f build/realify.sed "$x"
      } >"$x".tmp
      mv -f "$x".tmp "$x"
    fi
  done
fi

exec "$@"
