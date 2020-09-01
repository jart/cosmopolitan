#!/bin/sh
#-*-mode:sh;indent-tabs-mode:nil;tab-width:2;coding:utf-8-*-┐
#───vi: set net ft=sh ts=2 sts=2 fenc=utf-8 :vi─────────────┘

MKDIR=${MKDIR:-$(command -v mkdir) -p} || exit
CP=${CP:-$(command -v cp) -f} || exit
SED=${SED:-$(command -v sed)} || exit

MODE=

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
OUTDIR="${OUT%/*}"
if [ "$OUTDIR" != "$OUT" ] && [ ! -d "$OUTDIR" ]; then
  $MKDIR "$OUTDIR" || exit 2
fi

if [ -x "o/$MODE/tool/build/zipobj.com.dbg" ]; then
  set -- "o/$MODE/tool/build/zipobj.com.dbg" "$@"
else
  if [ ! -x o/build/bootstrap/zipobj.com ]; then
    $MKDIR o/build/bootstrap &&
    $CP -a build/bootstrap/zipobj.com \
       o/build/bootstrap/zipobj.com || exit
  fi
  set -- o/build/bootstrap/zipobj.com "$@"
fi

if [ "$SILENT" = "0" ]; then
  printf "%s\n" "$*" >&2
else
  printf "$LOGFMT" "${ACTION:-ZIPOBJ}" "$3" >&2
fi

exec "$@"
