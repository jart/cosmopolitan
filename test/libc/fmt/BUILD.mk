#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#── vi: set noet ft=make ts=8 sw=8 fenc=utf-8 :vi ────────────────────┘

PKGS += TEST_LIBC_FMT

TEST_LIBC_FMT_SRCS := $(wildcard test/libc/fmt/*.c)
TEST_LIBC_FMT_SRCS_TEST = $(filter %_test.c,$(TEST_LIBC_FMT_SRCS))
TEST_LIBC_FMT_BINS = $(TEST_LIBC_FMT_COMS) $(TEST_LIBC_FMT_COMS:%=%.dbg)
TEST_LIBC_FMT_TESTS = $(TEST_LIBC_FMT_SRCS_TEST:%.c=o/$(MODE)/%.com.ok)

TEST_LIBC_FMT_OBJS =					\
	$(TEST_LIBC_FMT_SRCS:%.c=o/$(MODE)/%.o)

TEST_LIBC_FMT_COMS =					\
	$(TEST_LIBC_FMT_SRCS:%.c=o/$(MODE)/%.com)

TEST_LIBC_FMT_CHECKS =					\
	$(TEST_LIBC_FMT_SRCS_TEST:%.c=o/$(MODE)/%.com.runs)

TEST_LIBC_FMT_DIRECTDEPS =				\
	LIBC_FMT					\
	LIBC_INTRIN					\
	LIBC_LOG					\
	LIBC_MEM					\
	LIBC_NEXGEN32E					\
	LIBC_RUNTIME					\
	LIBC_STDIO					\
	LIBC_STR					\
	LIBC_SYSV					\
	LIBC_TESTLIB					\
	LIBC_TINYMATH					\
	LIBC_X						\
	THIRD_PARTY_GDTOA

TEST_LIBC_FMT_DEPS :=					\
	$(call uniq,$(foreach x,$(TEST_LIBC_FMT_DIRECTDEPS),$($(x))))

o/$(MODE)/test/libc/fmt/fmt.pkg:			\
		$(TEST_LIBC_FMT_OBJS)			\
		$(foreach x,$(TEST_LIBC_FMT_DIRECTDEPS),$($(x)_A).pkg)

o/$(MODE)/test/libc/fmt/%.com.dbg:			\
		$(TEST_LIBC_FMT_DEPS)			\
		o/$(MODE)/test/libc/fmt/%.o		\
		o/$(MODE)/test/libc/fmt/fmt.pkg		\
		$(LIBC_TESTMAIN)			\
		$(CRT)					\
		$(APE_NO_MODIFY_SELF)
	@$(APELINK)

$(TEST_LIBC_FMT_OBJS): test/libc/fmt/BUILD.mk

$(TEST_LIBC_FMT_OBJS): private				\
	DEFAULT_CCFLAGS +=				\
		-fno-builtin

o/$(MODE)/test/libc/fmt/fprintf_test.com.runs:		\
	private .UNVEIL += w:/dev/full

.PHONY: o/$(MODE)/test/libc/fmt
o/$(MODE)/test/libc/fmt:				\
		$(TEST_LIBC_FMT_BINS)			\
		$(TEST_LIBC_FMT_CHECKS)
