#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#───vi: set et ft=make ts=8 tw=8 fenc=utf-8 :vi───────────────────────┘

PKGS += TEST_LIBC_MATH

TEST_LIBC_MATH_SRCS := $(wildcard test/libc/math/*.c)
TEST_LIBC_MATH_SRCS_TEST = $(filter %_test.c,$(TEST_LIBC_MATH_SRCS))

TEST_LIBC_MATH_OBJS =					\
	$(TEST_LIBC_MATH_SRCS:%=o/$(MODE)/%.zip.o)	\
	$(TEST_LIBC_MATH_SRCS:%.c=o/$(MODE)/%.o)

TEST_LIBC_MATH_COMS =					\
	$(TEST_LIBC_MATH_SRCS:%.c=o/$(MODE)/%.com)

TEST_LIBC_MATH_BINS =					\
	$(TEST_LIBC_MATH_COMS)				\
	$(TEST_LIBC_MATH_COMS:%=%.dbg)

TEST_LIBC_MATH_TESTS = $(TEST_LIBC_MATH_SRCS_TEST:%.c=o/$(MODE)/%.com.ok)

TEST_LIBC_MATH_CHECKS =					\
	$(TEST_LIBC_MATH_SRCS_TEST:%.c=o/$(MODE)/%.com.runs)

TEST_LIBC_MATH_DIRECTDEPS =				\
	LIBC_CALLS_HEFTY				\
	LIBC_FMT					\
	LIBC_MATH					\
	LIBC_MEM					\
	LIBC_RUNTIME					\
	LIBC_STUBS					\
	LIBC_TESTLIB					\
	LIBC_X

TEST_LIBC_MATH_DEPS :=					\
	$(call uniq,$(foreach x,$(TEST_LIBC_MATH_DIRECTDEPS),$($(x))))

o/$(MODE)/test/libc/math/math.pkg:			\
		$(TEST_LIBC_MATH_OBJS)			\
		$(foreach x,$(TEST_LIBC_MATH_DIRECTDEPS),$($(x)_A).pkg)

o/$(MODE)/test/libc/math/%.com.dbg:			\
		$(TEST_LIBC_MATH_DEPS)			\
		o/$(MODE)/test/libc/math/%.o		\
		o/$(MODE)/test/libc/math/math.pkg	\
		$(LIBC_TESTMAIN)			\
		$(CRT)					\
		$(APE)
	@$(APELINK)

$(TEST_LIBC_MATH_OBJS):					\
	DEFAULT_CCFLAGS +=				\
		-fno-builtin

.PHONY: o/$(MODE)/test/libc/math
o/$(MODE)/test/libc/math:				\
		$(TEST_LIBC_MATH_BINS)			\
		$(TEST_LIBC_MATH_CHECKS)
