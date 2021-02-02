#!/bin/sh
#-*-mode:sh;indent-tabs-mode:nil;tab-width:2;coding:utf-8-*-┐
#───vi: set net ft=sh ts=2 sts=2 fenc=utf-8 :vi─────────────┘

if [ -x "o/$MODE/tool/build/mkdeps.com" ]; then
  set -- "o/$MODE/tool/build/mkdeps.com" "$@"
else
  if [ ! -x o/build/bootstrap/mkdeps.com ]; then
    mkdir -p o/build/bootstrap &&
    cp -a build/bootstrap/mkdeps.com \
       o/build/bootstrap/mkdeps.com || exit
  fi
  set -- o/build/bootstrap/mkdeps.com "$@"
fi

if [ "$SILENT" = "0" ]; then
  printf "%s\n" "$*" >&2
else
  printf "$LOGFMT" "${ACTION:-MKDEPS}" "$3" >&2
fi

exec "$@"
