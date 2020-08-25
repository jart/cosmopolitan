#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#───vi: set et ft=make ts=8 tw=8 fenc=utf-8 :vi───────────────────────┘

PKGS += THIRD_PARTY_MUSL

THIRD_PARTY_MUSL_SRCS = $(THIRD_PARTY_MUSL_A_SRCS)
THIRD_PARTY_MUSL_HDRS = $(THIRD_PARTY_MUSL_A_HDRS)

THIRD_PARTY_MUSL_ARTIFACTS += THIRD_PARTY_MUSL_A
THIRD_PARTY_MUSL = $(THIRD_PARTY_MUSL_A_DEPS) $(THIRD_PARTY_MUSL_A)
THIRD_PARTY_MUSL_A = o/$(MODE)/third_party/musl/musl.a
THIRD_PARTY_MUSL_A_FILES := $(wildcard third_party/musl/*)
THIRD_PARTY_MUSL_A_HDRS = $(filter %.h,$(THIRD_PARTY_MUSL_A_FILES))
THIRD_PARTY_MUSL_A_SRCS = $(filter %.c,$(THIRD_PARTY_MUSL_A_FILES))

THIRD_PARTY_MUSL_A_OBJS =				\
	$(THIRD_PARTY_MUSL_A_SRCS:%=o/$(MODE)/%.zip.o)	\
	$(THIRD_PARTY_MUSL_A_SRCS:%.c=o/$(MODE)/%.o)

THIRD_PARTY_MUSL_A_DIRECTDEPS =				\
	LIBC_ALG					\
	LIBC_CALLS					\
	LIBC_CALLS_HEFTY				\
	LIBC_MEM					\
	LIBC_NEXGEN32E					\
	LIBC_STDIO					\
	LIBC_STR					\
	LIBC_STUBS					\
	LIBC_SYSV

THIRD_PARTY_MUSL_A_DEPS :=				\
	$(call uniq,$(foreach x,$(THIRD_PARTY_MUSL_A_DIRECTDEPS),$($(x))))

THIRD_PARTY_MUSL_A_CHECKS =				\
	$(THIRD_PARTY_MUSL_A).pkg			\
	$(THIRD_PARTY_MUSL_A_HDRS:%=o/$(MODE)/%.ok)

$(THIRD_PARTY_MUSL_A):					\
		third_party/musl/			\
		$(THIRD_PARTY_MUSL_A).pkg		\
		$(THIRD_PARTY_MUSL_A_OBJS)

$(THIRD_PARTY_MUSL_A).pkg:				\
		$(THIRD_PARTY_MUSL_A_OBJS)		\
		$(foreach x,$(THIRD_PARTY_MUSL_A_DIRECTDEPS),$($(x)_A).pkg)

THIRD_PARTY_MUSL_LIBS = $(foreach x,$(THIRD_PARTY_MUSL_ARTIFACTS),$($(x)))
THIRD_PARTY_MUSL_SRCS = $(foreach x,$(THIRD_PARTY_MUSL_ARTIFACTS),$($(x)_SRCS))
THIRD_PARTY_MUSL_CHECKS = $(foreach x,$(THIRD_PARTY_MUSL_ARTIFACTS),$($(x)_CHECKS))
THIRD_PARTY_MUSL_OBJS = $(foreach x,$(THIRD_PARTY_MUSL_ARTIFACTS),$($(x)_OBJS))
$(THIRD_PARTY_MUSL_OBJS): third_party/musl/musl.mk

.PHONY: o/$(MODE)/third_party/musl
o/$(MODE)/third_party/musl: $(THIRD_PARTY_MUSL_CHECKS)
