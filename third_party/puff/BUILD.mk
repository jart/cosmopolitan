#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#── vi: set noet ft=make ts=8 sw=8 fenc=utf-8 :vi ────────────────────┘

PKGS += THIRD_PARTY_PUFF

THIRD_PARTY_PUFF_ARTIFACTS += THIRD_PARTY_PUFF_A
THIRD_PARTY_PUFF = $(THIRD_PARTY_PUFF_A_DEPS) $(THIRD_PARTY_PUFF_A)
THIRD_PARTY_PUFF_A = o/$(MODE)/third_party/puff/puff.a
THIRD_PARTY_PUFF_A_FILES := $(wildcard third_party/puff/*)
THIRD_PARTY_PUFF_A_HDRS = $(filter %.h,$(THIRD_PARTY_PUFF_A_FILES))
THIRD_PARTY_PUFF_A_SRCS = $(filter %.c,$(THIRD_PARTY_PUFF_A_FILES))

THIRD_PARTY_PUFF_A_OBJS =			\
	$(THIRD_PARTY_PUFF_A_SRCS:%.c=o/$(MODE)/%.o)

THIRD_PARTY_PUFF_A_CHECKS =			\
	$(THIRD_PARTY_PUFF_A).pkg		\
	$(THIRD_PARTY_PUFF_A_HDRS:%=o/$(MODE)/%.ok)

THIRD_PARTY_PUFF_A_DIRECTDEPS =			\
	LIBC_INTRIN				\
	LIBC_NEXGEN32E

THIRD_PARTY_PUFF_A_DEPS :=			\
	$(call uniq,$(foreach x,$(THIRD_PARTY_PUFF_A_DIRECTDEPS),$($(x))))

$(THIRD_PARTY_PUFF_A):				\
		third_party/puff/		\
		$(THIRD_PARTY_PUFF_A).pkg	\
		$(THIRD_PARTY_PUFF_A_OBJS)

$(THIRD_PARTY_PUFF_A).pkg:			\
		$(THIRD_PARTY_PUFF_A_OBJS)	\
		$(foreach x,$(THIRD_PARTY_PUFF_A_DIRECTDEPS),$($(x)_A).pkg)

$(THIRD_PARTY_PUFF_A_OBJS): private			\
		COPTS +=				\
			-ffreestanding			\
			-fno-sanitize=address		\
			-Wframe-larger-than=4096	\
			-Walloca-larger-than=4096

THIRD_PARTY_PUFF_LIBS = $(foreach x,$(THIRD_PARTY_PUFF_ARTIFACTS),$($(x)))
THIRD_PARTY_PUFF_SRCS = $(foreach x,$(THIRD_PARTY_PUFF_ARTIFACTS),$($(x)_SRCS))
THIRD_PARTY_PUFF_HDRS = $(foreach x,$(THIRD_PARTY_PUFF_ARTIFACTS),$($(x)_HDRS))
THIRD_PARTY_PUFF_CHECKS = $(foreach x,$(THIRD_PARTY_PUFF_ARTIFACTS),$($(x)_CHECKS))
THIRD_PARTY_PUFF_OBJS = $(foreach x,$(THIRD_PARTY_PUFF_ARTIFACTS),$($(x)_OBJS))

.PHONY: o/$(MODE)/third_party/puff
o/$(MODE)/third_party/puff:			\
		$(THIRD_PARTY_PUFF_CHECKS)
