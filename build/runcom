#!/bin/sh
#-*-mode:sh;indent-tabs-mode:nil;tab-width:2;coding:utf-8-*-┐
#───vi: set net ft=sh ts=2 sts=2 fenc=utf-8 :vi─────────────┘

DD=${DD:-$(command -v dd)} || exit
$DD if="$1" of="$1.bak" bs=4096 count=1 conv=notrunc 2>/dev/null
"$@"
rc=$?
$DD if="$1.bak" of="$1" bs=4096 count=1 conv=notrunc 2>/dev/null
exit $rc
