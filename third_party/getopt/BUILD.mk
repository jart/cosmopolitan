#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#── vi: set noet ft=make ts=8 sw=8 fenc=utf-8 :vi ────────────────────┘

PKGS += THIRD_PARTY_GETOPT

THIRD_PARTY_GETOPT_ARTIFACTS += THIRD_PARTY_GETOPT_A
THIRD_PARTY_GETOPT = $(THIRD_PARTY_GETOPT_A_DEPS) $(THIRD_PARTY_GETOPT_A)
THIRD_PARTY_GETOPT_A = o/$(MODE)/third_party/getopt/getopt.a
THIRD_PARTY_GETOPT_A_FILES := $(wildcard third_party/getopt/*)
THIRD_PARTY_GETOPT_A_HDRS = $(filter %.h,$(THIRD_PARTY_GETOPT_A_FILES))
THIRD_PARTY_GETOPT_A_SRCS = $(filter %.c,$(THIRD_PARTY_GETOPT_A_FILES))
THIRD_PARTY_GETOPT_A_OBJS = $(THIRD_PARTY_GETOPT_A_SRCS:%.c=o/$(MODE)/%.o)

THIRD_PARTY_GETOPT_A_CHECKS =					\
	$(THIRD_PARTY_GETOPT_A).pkg				\
	$(THIRD_PARTY_GETOPT_A_HDRS:%=o/$(MODE)/%.ok)

THIRD_PARTY_GETOPT_A_DIRECTDEPS =				\
	LIBC_CALLS						\
	LIBC_INTRIN						\
	LIBC_LOG						\
	LIBC_NEXGEN32E						\
	LIBC_STR

THIRD_PARTY_GETOPT_A_DEPS :=					\
	$(call uniq,$(foreach x,$(THIRD_PARTY_GETOPT_A_DIRECTDEPS),$($(x))))

$(THIRD_PARTY_GETOPT_A):					\
		third_party/getopt/				\
		$(THIRD_PARTY_GETOPT_A).pkg			\
		$(THIRD_PARTY_GETOPT_A_OBJS)

$(THIRD_PARTY_GETOPT_A).pkg:					\
		$(THIRD_PARTY_GETOPT_A_OBJS)			\
		$(foreach x,$(THIRD_PARTY_GETOPT_A_DIRECTDEPS),$($(x)_A).pkg)

THIRD_PARTY_GETOPT_LIBS = $(foreach x,$(THIRD_PARTY_GETOPT_ARTIFACTS),$($(x)))
THIRD_PARTY_GETOPT_SRCS = $(foreach x,$(THIRD_PARTY_GETOPT_ARTIFACTS),$($(x)_SRCS))
THIRD_PARTY_GETOPT_HDRS = $(foreach x,$(THIRD_PARTY_GETOPT_ARTIFACTS),$($(x)_HDRS))
THIRD_PARTY_GETOPT_CHECKS = $(foreach x,$(THIRD_PARTY_GETOPT_ARTIFACTS),$($(x)_CHECKS))
THIRD_PARTY_GETOPT_OBJS = $(foreach x,$(THIRD_PARTY_GETOPT_ARTIFACTS),$($(x)_OBJS))
$(THIRD_PARTY_GETOPT_OBJS): third_party/getopt/BUILD.mk

.PHONY: o/$(MODE)/third_party/getopt
o/$(MODE)/third_party/getopt: $(THIRD_PARTY_GETOPT_CHECKS)
