#!/bin/sh
#-*-mode:sh;indent-tabs-mode:nil;tab-width:2;coding:utf-8-*-┐
#───vi: set net ft=sh ts=2 sts=2 fenc=utf-8 :vi─────────────┘
#
# OVERVIEW
#
#   GNU Linker Veneer
#
# DESCRIPTION
#
#   This script wraps normal linker commands that're transparently
#   passed-through. It adds ephemeral logging and directory creation.
#
# EXAMPLE
#
#   build/linker ld -o program program.o

if [ ! -d o/third_party/gcc ]; then
  third_party/gcc/unbundle.sh
fi

export LC_ALL=C  # very important for ld
MKDIR=${MKDIR:-$(command -v mkdir) -p} || exit

OUT=
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
  esac
  set -- "$@" "$x"
done

if [ "$SILENT" = "0" ]; then
  printf "%s\n" "$*" >&2
else
  printf "$LOGFMT" "${ACTION:-LINK.elf}" "$OUT" >&2
fi

OUTDIR="${OUT%/*}"
if [ "$OUTDIR" != "$OUT" ] && [ ! -d "$OUTDIR" ]; then
  $MKDIR "$OUTDIR" || exit 2
fi

REASON=failed
trap REASON=interrupted INT

"$@" && exit

if [ "$TERM" = "dumb" ]; then
  f='%s %s\r\n\r\n'
else
  f='\033[91m%s\033[39m \033[94m%s\033[39m\r\n\r\n'
fi
printf "$f" "link $REASON:" "$*" >&2
exit 1
