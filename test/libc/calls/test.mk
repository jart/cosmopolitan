#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#───vi: set et ft=make ts=8 tw=8 fenc=utf-8 :vi───────────────────────┘

PKGS += TEST_LIBC_CALLS

TEST_LIBC_CALLS_SRCS :=					\
	$(wildcard test/libc/calls/*.c)			\
	$(wildcard test/libc/calls/hefty/*.c)
TEST_LIBC_CALLS_SRCS_TEST = $(filter %_test.c,$(TEST_LIBC_CALLS_SRCS))

TEST_LIBC_CALLS_OBJS =					\
	$(TEST_LIBC_CALLS_SRCS:%=o/$(MODE)/%.zip.o)	\
	$(TEST_LIBC_CALLS_SRCS:%.c=o/$(MODE)/%.o)

TEST_LIBC_CALLS_COMS =					\
	$(TEST_LIBC_CALLS_SRCS:%.c=o/$(MODE)/%.com)

TEST_LIBC_CALLS_BINS =					\
	$(TEST_LIBC_CALLS_COMS)				\
	$(TEST_LIBC_CALLS_COMS:%=%.dbg)

TEST_LIBC_CALLS_TESTS =					\
	$(TEST_LIBC_CALLS_SRCS_TEST:%.c=o/$(MODE)/%.com.ok)

TEST_LIBC_CALLS_CHECKS =				\
	$(TEST_LIBC_CALLS_SRCS_TEST:%.c=o/$(MODE)/%.com.runs)

TEST_LIBC_CALLS_DIRECTDEPS =				\
	LIBC_CALLS					\
	LIBC_CALLS_HEFTY				\
	LIBC_FMT					\
	LIBC_LOG					\
	LIBC_MEM					\
	LIBC_NEXGEN32E					\
	LIBC_RUNTIME					\
	LIBC_STR					\
	LIBC_RAND					\
	LIBC_STUBS					\
	LIBC_SYSV					\
	LIBC_TESTLIB					\
	LIBC_X

TEST_LIBC_CALLS_DEPS :=					\
	$(call uniq,$(foreach x,$(TEST_LIBC_CALLS_DIRECTDEPS),$($(x))))

o/$(MODE)/test/libc/calls/calls.pkg:			\
		$(TEST_LIBC_CALLS_OBJS)			\
		$(foreach x,$(TEST_LIBC_CALLS_DIRECTDEPS),$($(x)_A).pkg)

o/$(MODE)/test/libc/calls/%.com.dbg:			\
		$(TEST_LIBC_CALLS_DEPS)			\
		o/$(MODE)/test/libc/calls/%.o		\
		o/$(MODE)/test/libc/calls/calls.pkg	\
		$(LIBC_TESTMAIN)			\
		$(CRT)					\
		$(APE)
	@$(APELINK)

.PHONY: o/$(MODE)/test/libc/calls
o/$(MODE)/test/libc/calls:				\
		$(TEST_LIBC_CALLS_BINS)			\
		$(TEST_LIBC_CALLS_CHECKS)
