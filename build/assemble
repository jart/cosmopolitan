#!/bin/sh
#-*-mode:sh;indent-tabs-mode:nil;tab-width:2;coding:utf-8-*-┐
#───vi: set net ft=sh ts=2 sts=2 fenc=utf-8 :vi─────────────┘
#
# OVERVIEW
#
#   GNU Assembler Veneer
#
# DESCRIPTION
#
#   This script wraps normal assembler commands that're transparently
#   passed-through. It adds ephemeral logging and directory creation.
#
# EXAMPLE
#
#   TARGET=program build/assemble as -o program program.o
#
# SEE ALSO
#
#   https://justine.storage.googleapis.com/perm/as.pdf

if [ ! -d o/third_party/gcc ]; then
  third_party/gcc/unbundle.sh
fi

export LC_ALL=C
MKDIR=${MKDIR:-$(command -v mkdir) -p} || exit

if [ "$SILENT" = "0" ]; then
  printf "%s\n" "$*" >&2
else
  printf "$LOGFMT" "${ACTION:-OBJECTIFY.s}" "$TARGET" >&2
fi

if [ "$TARGET" ]; then
  TARGETDIR="${TARGET%/*}"
  if [ "$TARGETDIR" != "$TARGET" ] && [ ! -d "$TARGETDIR" ]; then
    $MKDIR "$TARGETDIR" || exit 2
  fi
fi

"$@" && exit

if [ "$TERM" = "dumb" ]; then
  f='%s %s\r\n\r\n'
else
  f='\033[91m%s\033[39m \033[94m%s\033[39m\r\n\r\n'
fi
printf "$f" "assemble failed:" "$*" >&2
exit 1
