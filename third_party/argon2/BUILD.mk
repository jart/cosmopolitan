#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#── vi: set noet ft=make ts=8 sw=8 fenc=utf-8 :vi ────────────────────┘

PKGS += THIRD_PARTY_ARGON2

THIRD_PARTY_ARGON2_ARTIFACTS += THIRD_PARTY_ARGON2_A
THIRD_PARTY_ARGON2 = $(THIRD_PARTY_ARGON2_A_DEPS) $(THIRD_PARTY_ARGON2_A)
THIRD_PARTY_ARGON2_A = o/$(MODE)/third_party/argon2/argon2.a
THIRD_PARTY_ARGON2_A_FILES := $(wildcard third_party/argon2/*)
THIRD_PARTY_ARGON2_A_HDRS = $(filter %.h,$(THIRD_PARTY_ARGON2_A_FILES))
THIRD_PARTY_ARGON2_A_SRCS = $(filter %.c,$(THIRD_PARTY_ARGON2_A_FILES))
THIRD_PARTY_ARGON2_A_OBJS = $(THIRD_PARTY_ARGON2_A_SRCS:%.c=o/$(MODE)/%.o)

THIRD_PARTY_ARGON2_A_CHECKS =					\
	$(THIRD_PARTY_ARGON2_A).pkg				\
	$(THIRD_PARTY_ARGON2_A_HDRS:%=o/$(MODE)/%.ok)

THIRD_PARTY_ARGON2_A_DIRECTDEPS =				\
	LIBC_CALLS						\
	LIBC_FMT						\
	LIBC_INTRIN						\
	LIBC_NEXGEN32E						\
	LIBC_MEM						\
	LIBC_SYSV						\
	LIBC_STDIO						\
	LIBC_RUNTIME						\
	LIBC_SYSV_CALLS						\
	LIBC_STR

THIRD_PARTY_ARGON2_A_DEPS :=					\
	$(call uniq,$(foreach x,$(THIRD_PARTY_ARGON2_A_DIRECTDEPS),$($(x))))

$(THIRD_PARTY_ARGON2_A):					\
		third_party/argon2/				\
		$(THIRD_PARTY_ARGON2_A).pkg			\
		$(THIRD_PARTY_ARGON2_A_OBJS)

$(THIRD_PARTY_ARGON2_A).pkg:					\
		$(THIRD_PARTY_ARGON2_A_OBJS)			\
		$(foreach x,$(THIRD_PARTY_ARGON2_A_DIRECTDEPS),$($(x)_A).pkg)

# we can't use ubsan because:
#   it's just too slow to be practical (like 6s vs. 13s)
$(THIRD_PARTY_ARGON2_A_OBJS): private				\
		CFLAGS +=					\
			-ffunction-sections			\
			-fdata-sections				\
			-fno-sanitize=undefined

THIRD_PARTY_ARGON2_LIBS = $(foreach x,$(THIRD_PARTY_ARGON2_ARTIFACTS),$($(x)))
THIRD_PARTY_ARGON2_SRCS = $(foreach x,$(THIRD_PARTY_ARGON2_ARTIFACTS),$($(x)_SRCS))
THIRD_PARTY_ARGON2_HDRS = $(foreach x,$(THIRD_PARTY_ARGON2_ARTIFACTS),$($(x)_HDRS))
THIRD_PARTY_ARGON2_CHECKS = $(foreach x,$(THIRD_PARTY_ARGON2_ARTIFACTS),$($(x)_CHECKS))
THIRD_PARTY_ARGON2_OBJS = $(foreach x,$(THIRD_PARTY_ARGON2_ARTIFACTS),$($(x)_OBJS))
$(THIRD_PARTY_ARGON2_OBJS): third_party/argon2/BUILD.mk

.PHONY: o/$(MODE)/third_party/argon2
o/$(MODE)/third_party/argon2: $(THIRD_PARTY_ARGON2_CHECKS)
