#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#── vi: set noet ft=make ts=8 sw=8 fenc=utf-8 :vi ────────────────────┘

TEST_LIBC_XED_FILES := $(wildcard test/libc/xed/*)
TEST_LIBC_XED_HDRS = $(filter %.h,$(TEST_LIBC_XED_FILES))

#───────────────────────────────────────────────────────────────────────────────

PKGS += TEST_LIBC_XED_TESTLIB

TEST_LIBC_XED_TESTLIB_SRCS =					\
	$(TEST_LIBC_XED_TESTLIB_A_SRCS)

TEST_LIBC_XED_TESTLIB_OBJS =					\
	$(TEST_LIBC_XED_TESTLIB_A_OBJS)

TEST_LIBC_XED_TESTLIB =						\
	$(TEST_LIBC_XED_TESTLIB_A_DEPS)				\
	$(TEST_LIBC_XED_TESTLIB_A)

TEST_LIBC_XED_TESTLIB_A = o/$(MODE)/test/libc/xed/testlib.a
TEST_LIBC_XED_TESTLIB_A_SRCS = $(filter %_lib.c,$(TEST_LIBC_XED_FILES))

TEST_LIBC_XED_TESTLIB_A_OBJS =					\
	$(TEST_LIBC_XED_TESTLIB_A_SRCS:%.c=o/$(MODE)/%.o)

TEST_LIBC_XED_TESTLIB_A_DIRECTDEPS =				\
	LIBC_INTRIN						\
	LIBC_STR						\
	LIBC_MEM						\
	LIBC_NEXGEN32E						\
	LIBC_RUNTIME						\
	LIBC_TESTLIB						\
	LIBC_X							\
	THIRD_PARTY_XED

TEST_LIBC_XED_TESTLIB_A_DEPS :=					\
	$(call uniq,$(foreach x,$(TEST_LIBC_XED_TESTLIB_A_DIRECTDEPS),$($(x))))

$(TEST_LIBC_XED_TESTLIB_A).pkg:					\
		$(TEST_LIBC_XED_TESTLIB_A_OBJS)			\
		$(foreach x,$(TEST_LIBC_XED_TESTLIB_A_DIRECTDEPS),$($(x)_A).pkg)

$(TEST_LIBC_XED_TESTLIB_A):					\
		test/libc/xed/					\
		$(TEST_LIBC_XED_TESTLIB_A).pkg			\
		$(TEST_LIBC_XED_TESTLIB_A_OBJS)

#───────────────────────────────────────────────────────────────────────────────

PKGS += TEST_LIBC_XED

TEST_LIBC_XED_SRCS = $(filter %_test.c,$(TEST_LIBC_XED_FILES))

TEST_LIBC_XED_OBJS =						\
	$(TEST_LIBC_XED_SRCS:%.c=o/$(MODE)/%.o)

TEST_LIBC_XED_COMS =						\
	$(TEST_LIBC_XED_SRCS:%.c=o/$(MODE)/%.com)

TEST_LIBC_XED_BINS =						\
	$(TEST_LIBC_XED_COMS)					\
	$(TEST_LIBC_XED_COMS:%=%.dbg)

TEST_LIBC_XED_TESTS =						\
	$(TEST_LIBC_XED_SRCS:%.c=o/$(MODE)/%.com.ok)

TEST_LIBC_XED_CHECKS =						\
	$(TEST_LIBC_XED_SRCS:%.c=o/$(MODE)/%.com.runs)

TEST_LIBC_XED_DIRECTDEPS =					\
	LIBC_INTRIN						\
	LIBC_MEM						\
	LIBC_NEXGEN32E						\
	LIBC_RUNTIME						\
	LIBC_RUNTIME						\
	LIBC_TESTLIB						\
	TEST_LIBC_XED_TESTLIB					\
	THIRD_PARTY_XED

TEST_LIBC_XED_DEPS :=						\
	$(call uniq,$(foreach x,$(TEST_LIBC_XED_DIRECTDEPS),$($(x))))

o/$(MODE)/test/libc/xed/xed.pkg:				\
		$(TEST_LIBC_XED_OBJS)				\
		$(foreach x,$(TEST_LIBC_XED_DIRECTDEPS),$($(x)_A).pkg)

o/$(MODE)/test/libc/xed/%.com.dbg:				\
		$(TEST_LIBC_XED_DEPS)				\
		o/$(MODE)/test/libc/xed/%.o			\
		o/$(MODE)/test/libc/xed/xed.pkg			\
		$(LIBC_TESTMAIN)				\
		$(CRT)						\
		$(APE_NO_MODIFY_SELF)
	@$(APELINK)

#───────────────────────────────────────────────────────────────────────────────

.PHONY: o/$(MODE)/test/libc/xed
o/$(MODE)/test/libc/xed:					\
		$(TEST_LIBC_XED_BINS)				\
		$(TEST_LIBC_XED_CHECKS)
