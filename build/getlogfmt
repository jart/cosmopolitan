#!/usr/bin/env bash
#-*-mode:sh;indent-tabs-mode:nil;tab-width:2;coding:utf-8-*-┐
#───vi: set net ft=sh ts=2 sts=2 fenc=utf-8 :vi─────────────┘
#
# OVERVIEW
#
#   Printf Logger Initializer
#
# DESCRIPTION
#
#   This program is invoked once by build/definitions.mk to choose the
#   most appropriate format string when logging command invocations.

if [ "$SILENT" = "0" ]; then
  printf "''"
else
  W1=15
  if [ "$TERM" = "dumb" ]; then  # e.g. emacs' dismal tty
    if [ "$COLUMNS" = "" ]; then
      if TPUT=$(command -v tput); then
        COLUMNS=$("$TPUT" cols)
      else
        COLUMNS=80
      fi
    fi
    COLUMNS=$((COLUMNS - 1))
    W2=$((COLUMNS - W1))
    printf '%%-%ds%%-%ds\\r' "$W1" "$W2"
  else
    echo ♥cosmo >&2
    printf '\\033[F\\033[K%%-%ds%%s\\r\\n' "$W1"
  fi
fi
