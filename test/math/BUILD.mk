#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#── vi: set noet ft=make ts=8 sw=8 fenc=utf-8 :vi ────────────────────┘

PKGS += TEST_MATH

TEST_MATH_SRCS := $(wildcard test/math/*.c)
TEST_MATH_SRCS_TEST = $(filter %_test.c,$(TEST_MATH_SRCS))
TEST_MATH_OBJS = $(TEST_MATH_SRCS:%.c=o/$(MODE)/%.o)
TEST_MATH_COMS = $(TEST_MATH_SRCS_TEST:%.c=o/$(MODE)/%)
TEST_MATH_BINS = $(TEST_MATH_COMS) $(TEST_MATH_COMS:%=%.dbg)
TEST_MATH_TESTS = $(TEST_MATH_SRCS_TEST:%.c=o/$(MODE)/%.ok)
TEST_MATH_CHECKS = $(TEST_MATH_SRCS_TEST:%.c=o/$(MODE)/%.runs)

TEST_MATH_DIRECTDEPS =				\
	LIBC_INTRIN				\
	LIBC_MEM				\
	LIBC_RUNTIME				\
	LIBC_SYSV				\
	LIBC_TINYMATH				\
	THIRD_PARTY_COMPILER_RT			\
	THIRD_PARTY_OPENMP

TEST_MATH_DEPS :=				\
	$(call uniq,$(foreach x,$(TEST_MATH_DIRECTDEPS),$($(x))))

o/$(MODE)/test/math/math.pkg:			\
		$(TEST_MATH_OBJS)		\
		$(foreach x,$(TEST_MATH_DIRECTDEPS),$($(x)_A).pkg)

o/$(MODE)/test/math/%.dbg:			\
		$(TEST_MATH_DEPS)		\
		o/$(MODE)/test/math/%.o		\
		o/$(MODE)/test/math/math.pkg	\
		$(CRT)				\
		$(APE_NO_MODIFY_SELF)
	@$(APELINK)

$(TEST_MATH_OBJS): private CFLAGS += -fno-builtin -fopenmp

.PHONY: o/$(MODE)/test/math
o/$(MODE)/test/math:				\
		$(TEST_MATH_BINS)		\
		$(TEST_MATH_CHECKS)
