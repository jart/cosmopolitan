#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#── vi: set et ft=make ts=8 sw=8 fenc=utf-8 :vi ──────────────────────┘
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
uniqr = $(if $1,$(call uniqr,$(filter-out $(firstword $1),$1)) $(firstword $1))

# polyfill uniq native (landlock make 1.4)
ifneq ($(call uniq,c b c a),c b a)
uniq = $(if $1,$(firstword $1) $(call uniq,$(filter-out $(firstword $1),$1)))
endif
