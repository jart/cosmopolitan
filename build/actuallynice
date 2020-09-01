#!/bin/sh
#-*-mode:sh;indent-tabs-mode:nil;tab-width:2;coding:utf-8-*-┐
#───vi: set net ft=sh ts=2 sts=2 fenc=utf-8 :vi─────────────┘
#
# SYNOPSIS
#
#   Program Deprioritizer
#
# OVERVIEW
#
#   This is a drop-in replacement for the traditional Unix `nice`
#   command that also invokes `ionice`, which is important, since
#   network and traffic is usually what clobber the system.

if [ -z "$IONICE" ]; then
  if IONICE=$(command -v ionice 2>/dev/null); then
    IONICE="$IONICE -c3"
  fi
fi

if [ -z "$NICE" ]; then
  NICE=$(command -v nice 2>/dev/null)
fi

if [ -z "$IONICE$NICE" ]; then
  echo "error: can't be nice" >&2
fi

exec $IONICE $NICE "$@"
