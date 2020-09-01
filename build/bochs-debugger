#!/bin/sh
# -*- mode:sh;indent-tabs-mode:nil;tab-width:2;coding:utf-8 -*-
# vi: set net ft=sh ts=2 sts=2 sw=2 fenc=utf-8              :vi

echo c |
  bochs \
    -q \
    -f ape/etc/bochsrc.dbg \
    floppya:1_44=$1,status=inserted
