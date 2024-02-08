#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#── vi: set noet ft=make ts=8 sw=8 fenc=utf-8 :vi ────────────────────┘

PKGS += THIRD_PARTY_LINENOISE

THIRD_PARTY_LINENOISE_ARTIFACTS += THIRD_PARTY_LINENOISE_A
THIRD_PARTY_LINENOISE = $(THIRD_PARTY_LINENOISE_A_DEPS) $(THIRD_PARTY_LINENOISE_A)
THIRD_PARTY_LINENOISE_A = o/$(MODE)/third_party/linenoise/linenoise.a
THIRD_PARTY_LINENOISE_A_FILES := $(wildcard third_party/linenoise/*)
THIRD_PARTY_LINENOISE_A_HDRS = $(filter %.h,$(THIRD_PARTY_LINENOISE_A_FILES))
THIRD_PARTY_LINENOISE_A_SRCS = $(filter %.c,$(THIRD_PARTY_LINENOISE_A_FILES))
THIRD_PARTY_LINENOISE_A_OBJS = $(THIRD_PARTY_LINENOISE_A_SRCS:%.c=o/$(MODE)/%.o)

THIRD_PARTY_LINENOISE_A_CHECKS =				\
	$(THIRD_PARTY_LINENOISE_A).pkg				\
	$(THIRD_PARTY_LINENOISE_A_HDRS:%=o/$(MODE)/%.ok)

THIRD_PARTY_LINENOISE_A_DIRECTDEPS =				\
	LIBC_CALLS						\
	LIBC_FMT						\
	LIBC_INTRIN						\
	LIBC_NEXGEN32E						\
	LIBC_MEM						\
	LIBC_SYSV						\
	LIBC_SOCK						\
	LIBC_STDIO						\
	LIBC_RUNTIME						\
	LIBC_LOG						\
	LIBC_SYSV_CALLS						\
	LIBC_STR						\
	NET_HTTP

THIRD_PARTY_LINENOISE_A_DEPS :=					\
	$(call uniq,$(foreach x,$(THIRD_PARTY_LINENOISE_A_DIRECTDEPS),$($(x))))

$(THIRD_PARTY_LINENOISE_A):					\
		third_party/linenoise/				\
		$(THIRD_PARTY_LINENOISE_A).pkg			\
		$(THIRD_PARTY_LINENOISE_A_OBJS)

$(THIRD_PARTY_LINENOISE_A).pkg:					\
		$(THIRD_PARTY_LINENOISE_A_OBJS)			\
		$(foreach x,$(THIRD_PARTY_LINENOISE_A_DIRECTDEPS),$($(x)_A).pkg)

$(THIRD_PARTY_LINENOISE_A_OBJS): private			\
		CFLAGS +=					\
			-fno-jump-tables			\
			-ffunction-sections			\
			-fdata-sections

THIRD_PARTY_LINENOISE_LIBS = $(foreach x,$(THIRD_PARTY_LINENOISE_ARTIFACTS),$($(x)))
THIRD_PARTY_LINENOISE_SRCS = $(foreach x,$(THIRD_PARTY_LINENOISE_ARTIFACTS),$($(x)_SRCS))
THIRD_PARTY_LINENOISE_HDRS = $(foreach x,$(THIRD_PARTY_LINENOISE_ARTIFACTS),$($(x)_HDRS))
THIRD_PARTY_LINENOISE_CHECKS = $(foreach x,$(THIRD_PARTY_LINENOISE_ARTIFACTS),$($(x)_CHECKS))
THIRD_PARTY_LINENOISE_OBJS = $(foreach x,$(THIRD_PARTY_LINENOISE_ARTIFACTS),$($(x)_OBJS))
$(THIRD_PARTY_LINENOISE_OBJS): third_party/linenoise/BUILD.mk

.PHONY: o/$(MODE)/third_party/linenoise
o/$(MODE)/third_party/linenoise: $(THIRD_PARTY_LINENOISE_CHECKS)

