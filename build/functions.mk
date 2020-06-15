#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#───vi: set et ft=make ts=8 tw=8 fenc=utf-8 :vi───────────────────────┘
#
# SYNOPSIS
#
#   Cosmopolitan Functional Programming Build System
#
# OVERVIEW
#
#   It's basically Blaze except a few lines of code, made possible due
#   to the power and the glory of variables that're secretly lambdas.

tail = $(wordlist 2,$(words $1),$1)
reverse = $(if $1,$(call reverse,$(call tail,$1)) $(firstword $1))
uniq = $(if $1,$(firstword $1) $(call uniq,$(filter-out $(firstword $1),$1)))
uniqr = $(if $1,$(call uniqr,$(filter-out $(firstword $1),$1)) $(firstword $1))
