#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#── vi: set noet ft=make ts=8 sw=8 fenc=utf-8 :vi ────────────────────┘

PKGS += THIRD_PARTY_MAXMIND

THIRD_PARTY_MAXMIND_ARTIFACTS += THIRD_PARTY_MAXMIND_A
THIRD_PARTY_MAXMIND = $(THIRD_PARTY_MAXMIND_A_DEPS) $(THIRD_PARTY_MAXMIND_A)
THIRD_PARTY_MAXMIND_A = o/$(MODE)/third_party/maxmind/maxmind.a
THIRD_PARTY_MAXMIND_A_FILES := $(wildcard third_party/maxmind/*)
THIRD_PARTY_MAXMIND_A_HDRS = $(filter %.h,$(THIRD_PARTY_MAXMIND_A_FILES))
THIRD_PARTY_MAXMIND_A_SRCS = $(filter %.c,$(THIRD_PARTY_MAXMIND_A_FILES))

THIRD_PARTY_MAXMIND_A_OBJS =						\
	$(THIRD_PARTY_MAXMIND_A_SRCS:%.c=o/$(MODE)/%.o)

THIRD_PARTY_MAXMIND_A_CHECKS =						\
	$(THIRD_PARTY_MAXMIND_A).pkg					\
	$(THIRD_PARTY_MAXMIND_A_HDRS:%=o/$(MODE)/%.ok)

THIRD_PARTY_MAXMIND_A_DIRECTDEPS =					\
	LIBC_CALLS							\
	LIBC_FMT							\
	LIBC_INTRIN							\
	LIBC_MEM							\
	LIBC_NEXGEN32E							\
	LIBC_RUNTIME							\
	LIBC_STDIO							\
	LIBC_STR							\
	LIBC_SYSV

THIRD_PARTY_MAXMIND_A_DEPS :=						\
	$(call uniq,$(foreach x,$(THIRD_PARTY_MAXMIND_A_DIRECTDEPS),$($(x))))

$(THIRD_PARTY_MAXMIND_A):						\
		third_party/maxmind/					\
		$(THIRD_PARTY_MAXMIND_A).pkg				\
		$(THIRD_PARTY_MAXMIND_A_OBJS)

$(THIRD_PARTY_MAXMIND_A).pkg:						\
		$(THIRD_PARTY_MAXMIND_A_OBJS)				\
		$(foreach x,$(THIRD_PARTY_MAXMIND_A_DIRECTDEPS),$($(x)_A).pkg)

$(THIRD_PARTY_MAXMIND_A_OBJS): private					\
		CFLAGS +=						\
			-fdata-sections					\
			-ffunction-sections

THIRD_PARTY_MAXMIND_LIBS = $(foreach x,$(THIRD_PARTY_MAXMIND_ARTIFACTS),$($(x)))
THIRD_PARTY_MAXMIND_SRCS = $(foreach x,$(THIRD_PARTY_MAXMIND_ARTIFACTS),$($(x)_SRCS))
THIRD_PARTY_MAXMIND_HDRS = $(foreach x,$(THIRD_PARTY_MAXMIND_ARTIFACTS),$($(x)_HDRS))
THIRD_PARTY_MAXMIND_CHECKS = $(foreach x,$(THIRD_PARTY_MAXMIND_ARTIFACTS),$($(x)_CHECKS))
THIRD_PARTY_MAXMIND_OBJS = $(foreach x,$(THIRD_PARTY_MAXMIND_ARTIFACTS),$($(x)_OBJS))

.PHONY: o/$(MODE)/third_party/maxmind
o/$(MODE)/third_party/maxmind:						\
		$(THIRD_PARTY_MAXMIND_CHECKS)
