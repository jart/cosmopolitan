#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#── vi: set noet ft=make ts=8 sw=8 fenc=utf-8 :vi ────────────────────┘

PKGS += THIRD_PARTY_REGEX

THIRD_PARTY_REGEX_ARTIFACTS += THIRD_PARTY_REGEX_A
THIRD_PARTY_REGEX = $(THIRD_PARTY_REGEX_A_DEPS) $(THIRD_PARTY_REGEX_A)
THIRD_PARTY_REGEX_A = o/$(MODE)/third_party/regex/regex.a
THIRD_PARTY_REGEX_A_FILES := $(wildcard third_party/regex/*)
THIRD_PARTY_REGEX_A_HDRS = $(filter %.h,$(THIRD_PARTY_REGEX_A_FILES))
THIRD_PARTY_REGEX_A_INCS = $(filter %.inc,$(THIRD_PARTY_REGEX_A_FILES))
THIRD_PARTY_REGEX_A_SRCS = $(filter %.c,$(THIRD_PARTY_REGEX_A_FILES))

THIRD_PARTY_REGEX_A_OBJS =				\
	$(THIRD_PARTY_REGEX_A_SRCS:%.c=o/$(MODE)/%.o)

THIRD_PARTY_REGEX_A_DIRECTDEPS =			\
	LIBC_STDIO					\
	LIBC_INTRIN					\
	LIBC_MEM					\
	LIBC_NEXGEN32E					\
	LIBC_RUNTIME					\
	LIBC_STR

THIRD_PARTY_REGEX_A_DEPS :=				\
	$(call uniq,$(foreach x,$(THIRD_PARTY_REGEX_A_DIRECTDEPS),$($(x))))

THIRD_PARTY_REGEX_A_CHECKS =				\
	$(THIRD_PARTY_REGEX_A).pkg			\
	$(THIRD_PARTY_REGEX_A_HDRS:%=o/$(MODE)/%.ok)

$(THIRD_PARTY_REGEX_A):					\
		third_party/regex/			\
		$(THIRD_PARTY_REGEX_A).pkg		\
		$(THIRD_PARTY_REGEX_A_OBJS)

$(THIRD_PARTY_REGEX_A).pkg:				\
		$(THIRD_PARTY_REGEX_A_OBJS)		\
		$(foreach x,$(THIRD_PARTY_REGEX_A_DIRECTDEPS),$($(x)_A).pkg)

THIRD_PARTY_REGEX_LIBS = $(foreach x,$(THIRD_PARTY_REGEX_ARTIFACTS),$($(x)))
THIRD_PARTY_REGEX_HDRS = $(foreach x,$(THIRD_PARTY_REGEX_ARTIFACTS),$($(x)_HDRS))
THIRD_PARTY_REGEX_SRCS = $(foreach x,$(THIRD_PARTY_REGEX_ARTIFACTS),$($(x)_SRCS))
THIRD_PARTY_REGEX_INCS = $(foreach x,$(THIRD_PARTY_REGEX_ARTIFACTS),$($(x)_INCS))
THIRD_PARTY_REGEX_CHECKS = $(foreach x,$(THIRD_PARTY_REGEX_ARTIFACTS),$($(x)_CHECKS))
THIRD_PARTY_REGEX_OBJS = $(foreach x,$(THIRD_PARTY_REGEX_ARTIFACTS),$($(x)_OBJS))

$(THIRD_PARTY_REGEX_OBJS): third_party/regex/BUILD.mk

o/$(MODE)/third_party/regex/regcomp.o			\
o/$(MODE)/third_party/regex/regexec.o			\
o/$(MODE)/third_party/regex/tre-mem.o: private		\
		CFLAGS +=				\
			$(OLD_CODE)

.PHONY: o/$(MODE)/third_party/regex
o/$(MODE)/third_party/regex: $(THIRD_PARTY_REGEX_CHECKS)
