#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#── vi: set noet ft=make ts=8 sw=8 fenc=utf-8 :vi ────────────────────┘

PKGS += THIRD_PARTY_PCRE

THIRD_PARTY_PCRE_ARTIFACTS += THIRD_PARTY_PCRE_A
THIRD_PARTY_PCRE = $(THIRD_PARTY_PCRE_A_DEPS) $(THIRD_PARTY_PCRE_A)
THIRD_PARTY_PCRE_A = o/$(MODE)/third_party/pcre/pcre.a
THIRD_PARTY_PCRE_A_FILES := $(wildcard third_party/pcre/*)
THIRD_PARTY_PCRE_A_HDRS = $(filter %.h,$(THIRD_PARTY_PCRE_A_FILES))
THIRD_PARTY_PCRE_A_INCS = $(filter %.inc,$(THIRD_PARTY_PCRE_A_FILES))
THIRD_PARTY_PCRE_A_SRCS = $(filter %.c,$(THIRD_PARTY_PCRE_A_FILES))
THIRD_PARTY_PCRE_A_OBJS = $(THIRD_PARTY_PCRE_A_SRCS:%.c=o/$(MODE)/%.o)

THIRD_PARTY_PCRE_A_CHECKS =				\
	$(THIRD_PARTY_PCRE_A).pkg			\
	o/$(MODE)/third_party/pcre/pcre2posix_test.com.runs

THIRD_PARTY_PCRE_A_DIRECTDEPS =				\
	LIBC_CALLS					\
	LIBC_FMT					\
	LIBC_INTRIN					\
	LIBC_MEM					\
	LIBC_NEXGEN32E					\
	LIBC_PROC					\
	LIBC_RUNTIME					\
	LIBC_STDIO					\
	LIBC_STR					\
	LIBC_SYSV

THIRD_PARTY_PCRE_A_DEPS :=				\
	$(call uniq,$(foreach x,$(THIRD_PARTY_PCRE_A_DIRECTDEPS),$($(x))))

$(THIRD_PARTY_PCRE_A):					\
		third_party/pcre/			\
		$(THIRD_PARTY_PCRE_A).pkg		\
		$(THIRD_PARTY_PCRE_A_OBJS)

$(THIRD_PARTY_PCRE_A).pkg:				\
		$(THIRD_PARTY_PCRE_A_OBJS)		\
		$(foreach x,$(THIRD_PARTY_PCRE_A_DIRECTDEPS),$($(x)_A).pkg)

$(THIRD_PARTY_PCRE_A_OBJS): private CPPFLAGS += -DHAVE_CONFIG_H -DNDEBUG

o/$(MODE)/third_party/pcre/%.com.dbg:			\
		$(THIRD_PARTY_PCRE)			\
		o/$(MODE)/third_party/pcre/%.o		\
		$(CRT)					\
		$(APE_NO_MODIFY_SELF)
	@$(APELINK)

THIRD_PARTY_PCRE_COMS =					\
	o/$(MODE)/third_party/pcre/pcre2grep.com	\
	o/$(MODE)/third_party/pcre/pcre2posix_test.com

THIRD_PARTY_PCRE_BINS = $(THIRD_PARTY_PCRE_COMS) $(THIRD_PARTY_PCRE_COMS:%=%.dbg)
THIRD_PARTY_PCRE_LIBS = $(foreach x,$(THIRD_PARTY_PCRE_ARTIFACTS),$($(x)))
THIRD_PARTY_PCRE_SRCS = $(foreach x,$(THIRD_PARTY_PCRE_ARTIFACTS),$($(x)_SRCS))
THIRD_PARTY_PCRE_HDRS = $(foreach x,$(THIRD_PARTY_PCRE_ARTIFACTS),$($(x)_HDRS))
THIRD_PARTY_PCRE_INCS = $(foreach x,$(THIRD_PARTY_PCRE_ARTIFACTS),$($(x)_INCS))
THIRD_PARTY_PCRE_CHECKS = $(foreach x,$(THIRD_PARTY_PCRE_ARTIFACTS),$($(x)_CHECKS))
THIRD_PARTY_PCRE_OBJS = $(foreach x,$(THIRD_PARTY_PCRE_ARTIFACTS),$($(x)_OBJS))
$(THIRD_PARTY_PCRE_OBJS): $(BUILD_FILES) third_party/pcre/BUILD.mk

.PHONY: o/$(MODE)/third_party/pcre
o/$(MODE)/third_party/pcre:				\
		$(THIRD_PARTY_PCRE_A)			\
		$(THIRD_PARTY_PCRE_BINS)		\
		$(THIRD_PARTY_PCRE_CHECKS)
