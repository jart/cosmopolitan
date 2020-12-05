#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#───vi: set et ft=make ts=8 tw=8 fenc=utf-8 :vi───────────────────────┘

PKGS += TEST_LIBC_TINYMATH

TEST_LIBC_TINYMATH_SRCS := $(wildcard test/libc/tinymath/*.c)
TEST_LIBC_TINYMATH_SRCS_TEST = $(filter %_test.c,$(TEST_LIBC_TINYMATH_SRCS))

TEST_LIBC_TINYMATH_OBJS =					\
	$(TEST_LIBC_TINYMATH_SRCS:%=o/$(MODE)/%.zip.o)		\
	$(TEST_LIBC_TINYMATH_SRCS:%.c=o/$(MODE)/%.o)

TEST_LIBC_TINYMATH_COMS =					\
	$(TEST_LIBC_TINYMATH_SRCS:%.c=o/$(MODE)/%.com)

TEST_LIBC_TINYMATH_BINS =					\
	$(TEST_LIBC_TINYMATH_COMS)				\
	$(TEST_LIBC_TINYMATH_COMS:%=%.dbg)

TEST_LIBC_TINYMATH_TESTS =					\
	$(TEST_LIBC_TINYMATH_SRCS_TEST:%.c=o/$(MODE)/%.com.ok)

TEST_LIBC_TINYMATH_CHECKS =					\
	$(TEST_LIBC_TINYMATH_SRCS_TEST:%.c=o/$(MODE)/%.com.runs)

TEST_LIBC_TINYMATH_DIRECTDEPS =					\
	LIBC_CALLS_HEFTY					\
	LIBC_FMT						\
	LIBC_TINYMATH						\
	LIBC_MEM						\
	LIBC_RUNTIME						\
	LIBC_NEXGEN32E						\
	LIBC_STUBS						\
	LIBC_TESTLIB						\
	LIBC_X

TEST_LIBC_TINYMATH_DEPS :=					\
	$(call uniq,$(foreach x,$(TEST_LIBC_TINYMATH_DIRECTDEPS),$($(x))))

o/$(MODE)/test/libc/tinymath/tinymath.pkg:			\
		$(TEST_LIBC_TINYMATH_OBJS)			\
		$(foreach x,$(TEST_LIBC_TINYMATH_DIRECTDEPS),$($(x)_A).pkg)

o/$(MODE)/test/libc/tinymath/%.com.dbg:				\
		$(TEST_LIBC_TINYMATH_DEPS)			\
		o/$(MODE)/test/libc/tinymath/%.o		\
		o/$(MODE)/test/libc/tinymath/tinymath.pkg	\
		$(LIBC_TESTMAIN)				\
		$(CRT)						\
		$(APE)
	@$(APELINK)

$(TEST_LIBC_TINYMATH_OBJS):					\
	DEFAULT_CCFLAGS +=					\
		-fno-builtin

.PHONY: o/$(MODE)/test/libc/tinymath
o/$(MODE)/test/libc/tinymath:					\
		$(TEST_LIBC_TINYMATH_BINS)			\
		$(TEST_LIBC_TINYMATH_CHECKS)
