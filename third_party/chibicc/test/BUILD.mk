#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#── vi: set noet ft=make ts=8 sw=8 fenc=utf-8 :vi ────────────────────┘
#
# SYNOPSIS
#
#   C Compiler Unit Tests
#
# OVERVIEW
#
#   This makefile compiles and runs each test twice. The first with
#   GCC-built chibicc, and a second time with chibicc-built chibicc

ifeq ($(ARCH), x86_64)

PKGS += THIRD_PARTY_CHIBICC_TEST

THIRD_PARTY_CHIBICC_TEST_A = o/$(MODE)/third_party/chibicc/test/test.a
THIRD_PARTY_CHIBICC_TEST_FILES := $(wildcard third_party/chibicc/test/*)
THIRD_PARTY_CHIBICC_TEST_SRCS = $(filter %.c,$(THIRD_PARTY_CHIBICC_TEST_FILES))
THIRD_PARTY_CHIBICC_TEST_SRCS_TEST = $(filter %_test.c,$(THIRD_PARTY_CHIBICC_TEST_SRCS))
THIRD_PARTY_CHIBICC_TEST_HDRS = $(filter %.h,$(THIRD_PARTY_CHIBICC_TEST_FILES))
THIRD_PARTY_CHIBICC_TEST_TESTS = $(THIRD_PARTY_CHIBICC_TEST_COMS:%=%.ok)

THIRD_PARTY_CHIBICC_TEST_COMS =							\
	$(THIRD_PARTY_CHIBICC_TEST_SRCS_TEST:%_test.c=o/$(MODE)/%_test.com)

THIRD_PARTY_CHIBICC_TEST_OBJS =							\
	$(THIRD_PARTY_CHIBICC_TEST_SRCS:%.c=o/$(MODE)/%.o)

THIRD_PARTY_CHIBICC_TEST_BINS =							\
	$(THIRD_PARTY_CHIBICC_TEST_COMS)					\
	$(THIRD_PARTY_CHIBICC_TEST_COMS:%=%.dbg)

THIRD_PARTY_CHIBICC_TEST_CHECKS =						\
	$(THIRD_PARTY_CHIBICC_TEST_COMS:%=%.runs)				\
	$(THIRD_PARTY_CHIBICC_TEST_HDRS:%=o/$(MODE)/%.ok)

THIRD_PARTY_CHIBICC_TEST_DIRECTDEPS =						\
	LIBC_CALLS								\
	LIBC_FMT								\
	LIBC_INTRIN								\
	LIBC_MEM								\
	LIBC_NEXGEN32E								\
	LIBC_RUNTIME								\
	LIBC_STDIO								\
	LIBC_STR								\
	LIBC_TINYMATH								\
	LIBC_X									\
	THIRD_PARTY_CHIBICC							\
	THIRD_PARTY_COMPILER_RT

THIRD_PARTY_CHIBICC_TEST_DEPS :=						\
	$(call uniq,$(foreach x,$(THIRD_PARTY_CHIBICC_TEST_DIRECTDEPS),$($(x))))

$(THIRD_PARTY_CHIBICC_TEST_A):							\
		$(THIRD_PARTY_CHIBICC_TEST_A).pkg				\
		o/$(MODE)/third_party/chibicc/test/common.o

$(THIRD_PARTY_CHIBICC_TEST_A).pkg:						\
		o/$(MODE)/third_party/chibicc/test/common.o			\
		$(foreach x,$(THIRD_PARTY_CHIBICC_TEST_DIRECTDEPS),$($(x)_A).pkg)

o/$(MODE)/third_party/chibicc/test/%.com.dbg:					\
		$(THIRD_PARTY_CHIBICC_TEST_DEPS)				\
		$(THIRD_PARTY_CHIBICC_TEST_A)					\
		o/$(MODE)/third_party/chibicc/test/%.o				\
		$(THIRD_PARTY_CHIBICC_TEST_A).pkg				\
		$(CRT)								\
		$(APE_NO_MODIFY_SELF)
	@$(APELINK)

o/$(MODE)/third_party/chibicc/test/%.o:						\
		third_party/chibicc/test/%.c					\
		$(CHIBICC)
	@$(COMPILE) -wAOBJECTIFY.c $(CHIBICC) $(CHIBICC_FLAGS) $(OUTPUT_OPTION) -c $<

endif

.PHONY: o/$(MODE)/third_party/chibicc/test
o/$(MODE)/third_party/chibicc/test:						\
		$(THIRD_PARTY_CHIBICC_TEST_BINS)				\
		$(THIRD_PARTY_CHIBICC_TEST_CHECKS)
