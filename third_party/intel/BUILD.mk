#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#── vi: set noet ft=make ts=8 sw=8 fenc=utf-8 :vi ────────────────────┘

PKGS += THIRD_PARTY_INTEL
THIRD_PARTY_INTEL_HDRS = $(filter %.h,$(THIRD_PARTY_INTEL_FILES))
THIRD_PARTY_INTEL_FILES := $(wildcard third_party/intel/*)
