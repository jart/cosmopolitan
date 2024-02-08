#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#── vi: set noet ft=make ts=8 sw=8 fenc=utf-8 :vi ────────────────────┘

PKGS += TEST_LIBC_TIME

TEST_LIBC_TIME_SRCS := $(wildcard test/libc/time/*.c)
TEST_LIBC_TIME_SRCS_TEST = $(filter %_test.c,$(TEST_LIBC_TIME_SRCS))
TEST_LIBC_TIME_BINS = $(TEST_LIBC_TIME_COMS) $(TEST_LIBC_TIME_COMS:%=%.dbg)

TEST_LIBC_TIME_OBJS =					\
	$(TEST_LIBC_TIME_SRCS:%.c=o/$(MODE)/%.o)

TEST_LIBC_TIME_COMS =					\
	$(TEST_LIBC_TIME_SRCS:%.c=o/$(MODE)/%.com)

TEST_LIBC_TIME_TESTS = $(TEST_LIBC_TIME_SRCS_TEST:%.c=o/$(MODE)/%.com.ok)
TEST_LIBC_TIME_CHECKS =					\
	$(TEST_LIBC_TIME_SRCS_TEST:%.c=o/$(MODE)/%.com.runs)

TEST_LIBC_TIME_DIRECTDEPS =				\
	LIBC_CALLS					\
	LIBC_INTRIN					\
	LIBC_LOG					\
	LIBC_MEM					\
	LIBC_NEXGEN32E					\
	LIBC_RUNTIME					\
	LIBC_SYSV					\
	LIBC_TESTLIB					\
	LIBC_TIME					\
	LIBC_X

TEST_LIBC_TIME_DEPS :=					\
	$(call uniq,$(foreach x,$(TEST_LIBC_TIME_DIRECTDEPS),$($(x))))

o/$(MODE)/test/libc/time/time.pkg:			\
		$(TEST_LIBC_TIME_OBJS)			\
		$(foreach x,$(TEST_LIBC_TIME_DIRECTDEPS),$($(x)_A).pkg)

o/$(MODE)/test/libc/time/%.com.dbg:			\
		$(TEST_LIBC_TIME_DEPS)			\
		o/$(MODE)/test/libc/time/%.o		\
		o/$(MODE)/test/libc/time/time.pkg	\
		$(LIBC_TESTMAIN)			\
		$(CRT)					\
		$(APE_NO_MODIFY_SELF)
	@$(APELINK)

.PHONY: o/$(MODE)/test/libc/time
o/$(MODE)/test/libc/time:				\
		$(TEST_LIBC_TIME_BINS)			\
		$(TEST_LIBC_TIME_CHECKS)
