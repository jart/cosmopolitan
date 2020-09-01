#!/bin/sh
#-*-mode:sh;indent-tabs-mode:nil;tab-width:2;coding:utf-8-*-┐
#───vi: set net ft=sh ts=2 sts=2 fenc=utf-8 :vi─────────────┘
#
# OVERVIEW
#
#   Source File Concatenation Tool
#
# DESCRIPTION
#
#   This program is the same as cat, but inserts preprocessor directives
#   that allow compiler errors to point back to the original lines.

if [ $# -eq 0 ]; then
  cat
else
  for x; do
    printf '# 1 "%s"\n' "$x"
    cat "$x"
  done
fi
