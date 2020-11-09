#!/bin/sh
#-*-mode:sh;indent-tabs-mode:nil;tab-width:2;coding:utf-8-*-┐
#───vi: set net ft=sh ts=2 sts=2 fenc=utf-8 :vi─────────────┘
#
# OVERVIEW
#
#   Cosmopolitan Archiver
#
# DESCRIPTION
#
#   This goes 100x faster than ar and ranlib.
#
# EXAMPLE
#
#   build/archive rcsD library.a foo.o ...

MKDIR=${MKDIR:-$(command -v mkdir) -p} || exit
# if [ -x "o/$MODE/tool/build/ar.com" ]; then
#   set -- "o/$MODE/tool/build/ar.com" "$@"
# else
if [ ! -x o/build/bootstrap/ar.com ]; then
  mkdir -p o/build/bootstrap &&
  cp -f build/bootstrap/ar.com o/build/bootstrap/ar.com.$$ &&
  mv -f o/build/bootstrap/ar.com.$$ o/build/bootstrap/ar.com || exit
fi
set -- o/build/bootstrap/ar.com "$@"
# fi
OUT=$3

printf "$LOGFMT" "${ACTION:-ARCHIVE.a}" "$OUT" >&2
# if [ "$SILENT" = "0" ]; then
#   # some of these windows nt archives are quite huge
#   COLUMNS=${COLUMNS:-80}
#   COLUMNS=$((COLUMNS - 4))
#   printf "%s\n" "$*" |
#     /usr/bin/fold -s -w $COLUMNS |
#     sed -e '1bb' -e 's/^/  /' -e ':b' -e '$b' -e 's/$/ \\/' >&2
# else
#   printf "$LOGFMT" "${ACTION:-ARCHIVE.a}" "$OUT" >&2
# fi

REASON=failed
trap REASON=interrupted INT

"$@" >/dev/null && exit

if [ "$TERM" = "dumb" ]; then
  f='%s %s\r\n\r\n'
else
  f='\033[91m%s\033[39m \033[94m%s\033[39m\r\n\r\n'
fi
printf "$f" "archive $REASON:" "$*" >&2
exit 1
