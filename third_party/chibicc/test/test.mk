#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#───vi: set et ft=make ts=8 tw=8 fenc=utf-8 :vi───────────────────────┘
#
# SYNOPSIS
#
#   C Compiler Unit Tests
#
# OVERVIEW
#
#   This makefile compiles and runs each test twice. The first with
#   GCC-built chibicc, and a second time with chibicc-built chibicc

PKGS += THIRD_PARTY_CHIBICC_TEST

THIRD_PARTY_CHIBICC_TEST_A = o/$(MODE)/third_party/chibicc/test/test.a
THIRD_PARTY_CHIBICC_TEST2_A = o/$(MODE)/third_party/chibicc/test/test2.a
THIRD_PARTY_CHIBICC_TEST_FILES := $(wildcard third_party/chibicc/test/*)
THIRD_PARTY_CHIBICC_TEST_SRCS = $(filter %.c,$(THIRD_PARTY_CHIBICC_TEST_FILES))
THIRD_PARTY_CHIBICC_TEST_SRCS_TEST = $(filter %_test.c,$(THIRD_PARTY_CHIBICC_TEST_SRCS))
THIRD_PARTY_CHIBICC_TEST_HDRS = $(filter %.h,$(THIRD_PARTY_CHIBICC_TEST_FILES))
THIRD_PARTY_CHIBICC_TEST_TESTS = $(THIRD_PARTY_CHIBICC_TEST_COMS:%=%.ok)

THIRD_PARTY_CHIBICC_TEST_COMS =							\
	$(THIRD_PARTY_CHIBICC_TEST_SRCS_TEST:%.c=o/$(MODE)/%.com)		\
	$(THIRD_PARTY_CHIBICC_TEST_SRCS_TEST:%.c=o/$(MODE)/%2.com)

THIRD_PARTY_CHIBICC_TEST_OBJS =							\
	$(THIRD_PARTY_CHIBICC_TEST_SRCS:%=o/$(MODE)/%.zip.o)			\
	$(THIRD_PARTY_CHIBICC_TEST_SRCS:%.c=o/$(MODE)/%.chibicc.o)

THIRD_PARTY_CHIBICC_TEST2_OBJS =						\
	$(THIRD_PARTY_CHIBICC_TEST_SRCS:%=o/$(MODE)/%.zip.o)			\
	$(THIRD_PARTY_CHIBICC_TEST_SRCS:%.c=o/$(MODE)/%.chibicc2.o)

THIRD_PARTY_CHIBICC_TEST_BINS =							\
	$(THIRD_PARTY_CHIBICC_TEST_COMS)					\
	$(THIRD_PARTY_CHIBICC_TEST_COMS:%=%.dbg)

THIRD_PARTY_CHIBICC_TEST_CHECKS =						\
	$(THIRD_PARTY_CHIBICC_TEST_COMS:%=%.runs)				\
	$(THIRD_PARTY_CHIBICC_TEST_HDRS:%=o/$(MODE)/%.ok)

THIRD_PARTY_CHIBICC_TEST_DIRECTDEPS =						\
	LIBC_RUNTIME								\
	LIBC_FMT								\
	LIBC_STR								\
	LIBC_STDIO								\
	LIBC_STUBS								\
	LIBC_NEXGEN32E								\
	LIBC_UNICODE								\
	LIBC_MEM								\
	LIBC_TINYMATH								\
	LIBC_X									\
	THIRD_PARTY_CHIBICC							\
	THIRD_PARTY_COMPILER_RT

THIRD_PARTY_CHIBICC_TEST_DEPS :=						\
	$(call uniq,$(foreach x,$(THIRD_PARTY_CHIBICC_TEST_DIRECTDEPS),$($(x))))

$(THIRD_PARTY_CHIBICC_TEST_A):							\
		third_party/chibicc/test/					\
		$(THIRD_PARTY_CHIBICC_TEST_A).pkg				\
		$(THIRD_PARTY_CHIBICC_TEST_OBJS)

$(THIRD_PARTY_CHIBICC_TEST2_A):							\
		third_party/chibicc/test/					\
		$(THIRD_PARTY_CHIBICC_TEST2_A).pkg				\
		$(THIRD_PARTY_CHIBICC_TEST2_OBJS)

$(THIRD_PARTY_CHIBICC_TEST_A).pkg:						\
		$(THIRD_PARTY_CHIBICC_TEST_OBJS)				\
		$(foreach x,$(THIRD_PARTY_CHIBICC_TEST_DIRECTDEPS),$($(x)_A).pkg)

$(THIRD_PARTY_CHIBICC_TEST2_A).pkg:						\
		$(THIRD_PARTY_CHIBICC_TEST2_OBJS)				\
		$(foreach x,$(THIRD_PARTY_CHIBICC_TEST_DIRECTDEPS),$($(x)_A).pkg)

o/$(MODE)/third_party/chibicc/test/%.com.dbg:					\
		$(THIRD_PARTY_CHIBICC_TEST_DEPS)				\
		$(THIRD_PARTY_CHIBICC_TEST_A)					\
		o/$(MODE)/third_party/chibicc/test/%.chibicc.o			\
		$(THIRD_PARTY_CHIBICC_TEST_A).pkg				\
		$(CRT)								\
		$(APE)
	@$(APELINK)

o/$(MODE)/third_party/chibicc/test/%2.com.dbg:					\
		$(THIRD_PARTY_CHIBICC_TEST_DEPS)				\
		$(THIRD_PARTY_CHIBICC_TEST2_A)					\
		o/$(MODE)/third_party/chibicc/test/%.chibicc2.o			\
		$(THIRD_PARTY_CHIBICC_TEST2_A).pkg				\
		$(CRT)								\
		$(APE)
	@$(APELINK)

.PHONY: o/$(MODE)/third_party/chibicc/test
o/$(MODE)/third_party/chibicc/test:						\
		$(THIRD_PARTY_CHIBICC_TEST_BINS)				\
		$(THIRD_PARTY_CHIBICC_TEST_CHECKS)
