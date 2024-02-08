#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#── vi: set noet ft=make ts=8 sw=8 fenc=utf-8 :vi ────────────────────┘
#
# SYNOPSIS
#
#   Cosmopolitan x86 Instruction Decoding
#
# DESCRIPTION
#
#   See test/libc/xed/x86ild_test.c for more information.

PKGS += THIRD_PARTY_XED

THIRD_PARTY_XED_ARTIFACTS += THIRD_PARTY_XED_A
THIRD_PARTY_XED = $(THIRD_PARTY_XED_A_DEPS) $(THIRD_PARTY_XED_A)
THIRD_PARTY_XED_A = o/$(MODE)/third_party/xed/xed.a
THIRD_PARTY_XED_A_FILES := $(wildcard third_party/xed/*)
THIRD_PARTY_XED_A_HDRS = $(filter %.h,$(THIRD_PARTY_XED_A_FILES))
THIRD_PARTY_XED_A_SRCS = $(filter %.c,$(THIRD_PARTY_XED_A_FILES))
THIRD_PARTY_XED_A_OBJS = $(THIRD_PARTY_XED_A_SRCS:%.c=o/$(MODE)/%.o)

THIRD_PARTY_XED_A_CHECKS =				\
	$(THIRD_PARTY_XED_A).pkg			\
	$(THIRD_PARTY_XED_A_HDRS:%=o/$(MODE)/%.ok)

THIRD_PARTY_XED_A_DIRECTDEPS =				\
	LIBC_INTRIN					\
	LIBC_NEXGEN32E					\
	LIBC_STR

THIRD_PARTY_XED_A_DEPS :=				\
	$(call uniq,$(foreach x,$(THIRD_PARTY_XED_A_DIRECTDEPS),$($(x))))

ifneq ($(ARCH), aarch64)
o/$(MODE)/third_party/xed/x86ild.greg.o: private	\
		CFLAGS +=				\
			-mstringop-strategy=unrolled_loop
endif

$(THIRD_PARTY_XED_A):					\
		third_party/xed/			\
		$(THIRD_PARTY_XED_A).pkg		\
		$(THIRD_PARTY_XED_A_OBJS)

$(THIRD_PARTY_XED_A).pkg:				\
		$(THIRD_PARTY_XED_A_OBJS)		\
		$(foreach x,$(THIRD_PARTY_XED_A_DIRECTDEPS),$($(x)_A).pkg)

o/$(MODE)/third_party/xed/x86ild.greg.o: private	\
		CFLAGS +=				\
			-O3

$(THIRD_PARTY_XED_A_OBJS): private			\
		COPTS +=				\
			-ffreestanding			\
			-fno-sanitize=address		\
			-Wframe-larger-than=4096	\
			-Walloca-larger-than=4096

HIRD_PARTY_XED_LIBS = $(foreach x,$(THIRD_PARTY_XED_ARTIFACTS),$($(x)))
THIRD_PARTY_XED_SRCS = $(foreach x,$(THIRD_PARTY_XED_ARTIFACTS),$($(x)_SRCS))
THIRD_PARTY_XED_HDRS = $(foreach x,$(THIRD_PARTY_XED_ARTIFACTS),$($(x)_HDRS))
THIRD_PARTY_XED_CHECKS = $(foreach x,$(THIRD_PARTY_XED_ARTIFACTS),$($(x)_CHECKS))
THIRD_PARTY_XED_OBJS = $(foreach x,$(THIRD_PARTY_XED_ARTIFACTS),$($(x)_OBJS))
$(THIRD_PARTY_XED_OBJS): $(BUILD_FILES) third_party/xed/BUILD.mk

.PHONY: o/$(MODE)/third_party/xed
o/$(MODE)/third_party/xed: $(THIRD_PARTY_XED_CHECKS)
