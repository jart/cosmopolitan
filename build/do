#!/bin/sh
#-*-mode:sh;indent-tabs-mode:nil;tab-width:2;coding:utf-8-*-┐
#───vi: set net ft=sh ts=2 sts=2 fenc=utf-8 :vi─────────────┘
#
# OVERVIEW
#
#   Generic Command Runner
#
# DESCRIPTION
#
#   This does auto mkdir and ephemeral logging.
#
# EXAMPLE
#
#   build/do PROG [ARGS...]

MKDIR=${MKDIR:-$(command -v mkdir) -p} || exit

# Ensure directory creation if -o PATH flag is passed.
OUT="$TARGET"
FIRST=1
OUTARG=0
for x; do
  if [ $FIRST -eq 1 ]; then
    set --
    FIRST=0
  elif [ $OUTARG -eq 1 ]; then
    OUTARG=0
    OUT="$x"
  fi
  case "$x" in
    -o)
      OUTARG=1
      ;;
    -o*)
      OUT=${x#-o}
      ;;
  esac
  set -- "$@" "$x"
done
if [ "$OUT" ]; then
  OUTDIR="${OUT%/*}"
  if [ "$OUTDIR" != "$OUT" ] && [ ! -d "$OUTDIR" ]; then
    $MKDIR "$OUTDIR" || exit 2
  fi
fi

# Log command.
if [ "$SILENT" = "0" ]; then
  printf "%s\n" "$*" >&2
else
  printf "$LOGFMT" "${ACTION:-BUILD}" "$TARGET" >&2
fi

exec "$@"
