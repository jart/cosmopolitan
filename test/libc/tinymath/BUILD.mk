#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#── vi: set noet ft=make ts=8 sw=8 fenc=utf-8 :vi ────────────────────┘

PKGS += TEST_LIBC_TINYMATH

TEST_LIBC_TINYMATH_SRCS_C := $(wildcard test/libc/tinymath/*.c)
TEST_LIBC_TINYMATH_SRCS_CC := $(wildcard test/libc/tinymath/*.cc)
TEST_LIBC_TINYMATH_SRCS_TEST = $(filter %_test.c,$(TEST_LIBC_TINYMATH_SRCS))

TEST_LIBC_TINYMATH_SRCS =					\
	$(TEST_LIBC_TINYMATH_SRCS_C)				\
	$(TEST_LIBC_TINYMATH_SRCS_CC)

TEST_LIBC_TINYMATH_OBJS =					\
	$(TEST_LIBC_TINYMATH_SRCS_C:%.c=o/$(MODE)/%.o)		\
	$(TEST_LIBC_TINYMATH_SRCS_CC:%.cc=o/$(MODE)/%.o)

TEST_LIBC_TINYMATH_COMS =					\
	$(TEST_LIBC_TINYMATH_SRCS_C:%.c=o/$(MODE)/%)		\
	$(TEST_LIBC_TINYMATH_SRCS_CC:%.cc=o/$(MODE)/%)

TEST_LIBC_TINYMATH_BINS =					\
	$(TEST_LIBC_TINYMATH_COMS)				\
	$(TEST_LIBC_TINYMATH_COMS:%=%.dbg)

TEST_LIBC_TINYMATH_TESTS =					\
	$(TEST_LIBC_TINYMATH_SRCS_TEST:%.c=o/$(MODE)/%.ok)

TEST_LIBC_TINYMATH_CHECKS =					\
	$(TEST_LIBC_TINYMATH_SRCS_TEST:%.c=o/$(MODE)/%.runs)

TEST_LIBC_TINYMATH_DIRECTDEPS =					\
	LIBC_CALLS						\
	LIBC_FMT						\
	LIBC_INTRIN						\
	LIBC_LOG						\
	LIBC_MEM						\
	LIBC_NEXGEN32E						\
	LIBC_RUNTIME						\
	LIBC_STDIO						\
	LIBC_STR						\
	LIBC_SYSV						\
	LIBC_TESTLIB						\
	LIBC_TINYMATH						\
	LIBC_X							\
	THIRD_PARTY_COMPILER_RT					\
	THIRD_PARTY_COMPILER_RT					\
	THIRD_PARTY_DOUBLECONVERSION				\
	THIRD_PARTY_GDTOA					\
	THIRD_PARTY_LIBCXX					\
	THIRD_PARTY_LIBCXXABI					\
	THIRD_PARTY_LIBUNWIND					\

TEST_LIBC_TINYMATH_DEPS :=					\
	$(call uniq,$(foreach x,$(TEST_LIBC_TINYMATH_DIRECTDEPS),$($(x))))

o/$(MODE)/test/libc/tinymath/tinymath.pkg:			\
		$(TEST_LIBC_TINYMATH_OBJS)			\
		$(foreach x,$(TEST_LIBC_TINYMATH_DIRECTDEPS),$($(x)_A).pkg)

o/$(MODE)/test/libc/tinymath/%.dbg:				\
		$(TEST_LIBC_TINYMATH_DEPS)			\
		o/$(MODE)/test/libc/tinymath/%.o		\
		o/$(MODE)/test/libc/tinymath/tinymath.pkg	\
		$(LIBC_TESTMAIN)				\
		$(CRT)						\
		$(APE_NO_MODIFY_SELF)
	@$(APELINK)

$(TEST_LIBC_TINYMATH_OBJS): private				\
		CFLAGS +=					\
			-fno-builtin

.PHONY: o/$(MODE)/test/libc/tinymath
o/$(MODE)/test/libc/tinymath:					\
		$(TEST_LIBC_TINYMATH_BINS)			\
		$(TEST_LIBC_TINYMATH_CHECKS)
