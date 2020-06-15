#!/bin/sh
# -*- mode:sh;indent-tabs-mode:nil;tab-width:2;coding:utf-8 -*-
# vi: set net ft=sh ts=2 sts=2 sw=2 fenc=utf-8              :vi
#
# bochs-scriptable executes a disk with serial uart stdio.
#
# USAGE
#
#   build/bochs-scriptable IMAGE...
#
# DESCRIPTION
#
#   This script is useful for end-to-end testing metal apps in <100ms.
#
# SEE ALSO
#
#   build/boot(1)

while getopts h X; do
  case $X in
    h)  exec less "$0" ;;
    \?) echo "$0: bad arg" >&2; exit 1 ;;
  esac
done
shift $((OPTIND - 1))

trap '' INT

IMG=$1
OUT=/tmp/$USER.$$.bochs.stdout
ERR=/tmp/$USER.$$.bochs.stderr

mkfifo $OUT || exit

cat <$OUT &
CAT=$!
exec 4>$OUT
rm -f $OUT

echo c |
  bochs \
    -q \
    -f ape/etc/bochsrc.ffs \
    display_library:nogui \
    floppya:1_44=$1,status=inserted \
    >>$ERR 2>>$ERR
RC=$?

kill $CAT
exec 4>&-
rm -f $ERR
