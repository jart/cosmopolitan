#!/bin/sh
#-*-mode:sh;indent-tabs-mode:nil;tab-width:2;coding:utf-8-*-┐
#───vi: set net ft=sh ts=2 sts=2 fenc=utf-8 :vi─────────────┘
#
# OVERVIEW
#
#   SSH Command Wrapper
#
# DESCRIPTION
#
#   This script, like most of our wrappers, asks the tooling to whine
#   less often.

exec ${SSH:-ssh} -o LogLevel=QUIET "$@"
