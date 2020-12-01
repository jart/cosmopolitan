#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#───vi: set et ft=make ts=8 tw=8 fenc=utf-8 :vi───────────────────────┘

PKGS += TEST_LIBC_INTRIN

TEST_LIBC_INTRIN_SRCS := $(wildcard test/libc/intrin/*.c)
TEST_LIBC_INTRIN_SRCS_TEST = $(filter %_test.c,$(TEST_LIBC_INTRIN_SRCS))

TEST_LIBC_INTRIN_OBJS =					\
	$(TEST_LIBC_INTRIN_SRCS:%=o/$(MODE)/%.zip.o)	\
	$(TEST_LIBC_INTRIN_SRCS:%.c=o/$(MODE)/%.o)

TEST_LIBC_INTRIN_COMS =					\
	$(TEST_LIBC_INTRIN_SRCS:%.c=o/$(MODE)/%.com)

TEST_LIBC_INTRIN_BINS =					\
	$(TEST_LIBC_INTRIN_COMS)			\
	$(TEST_LIBC_INTRIN_COMS:%=%.dbg)

TEST_LIBC_INTRIN_TESTS =				\
	$(TEST_LIBC_INTRIN_SRCS_TEST:%.c=o/$(MODE)/%.com.ok)

TEST_LIBC_INTRIN_CHECKS =				\
	$(TEST_LIBC_INTRIN_SRCS_TEST:%.c=o/$(MODE)/%.com.runs)

TEST_LIBC_INTRIN_DIRECTDEPS =				\
	LIBC_FMT					\
	LIBC_INTRIN					\
	LIBC_NEXGEN32E					\
	LIBC_RAND					\
	LIBC_LOG					\
	LIBC_STUBS					\
	LIBC_STR					\
	LIBC_TESTLIB					\
	LIBC_TINYMATH					\
	LIBC_RUNTIME					\
	LIBC_X						\
	TOOL_VIZ_LIB

TEST_LIBC_INTRIN_DEPS :=				\
	$(call uniq,$(foreach x,$(TEST_LIBC_INTRIN_DIRECTDEPS),$($(x))))

o/$(MODE)/test/libc/intrin/intrin.pkg:			\
		$(TEST_LIBC_INTRIN_OBJS)		\
		$(foreach x,$(TEST_LIBC_INTRIN_DIRECTDEPS),$($(x)_A).pkg)

o/$(MODE)/test/libc/intrin/%.com.dbg:			\
		$(TEST_LIBC_INTRIN_DEPS)		\
		o/$(MODE)/test/libc/intrin/%.o		\
		o/$(MODE)/test/libc/intrin/intrin.pkg	\
		$(LIBC_TESTMAIN)			\
		$(CRT)					\
		$(APE)
	@$(APELINK)

# $(TEST_LIBC_INTRIN_OBJS):				\
# 		OVERRIDE_CFLAGS +=			\
# 			-fsanitize=address

.PHONY: o/$(MODE)/test/libc/intrin
o/$(MODE)/test/libc/intrin:				\
		$(TEST_LIBC_INTRIN_BINS)		\
		$(TEST_LIBC_INTRIN_CHECKS)
