#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#── vi: set noet ft=make ts=8 sw=8 fenc=utf-8 :vi ────────────────────┘

PKGS += TEST_LIBC_INTRIN

TEST_LIBC_INTRIN_SRCS := $(wildcard test/libc/intrin/*.c)
TEST_LIBC_INTRIN_SRCS_TEST = $(filter %_test.c,$(TEST_LIBC_INTRIN_SRCS))

TEST_LIBC_INTRIN_OBJS =					\
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
	LIBC_CALLS					\
	LIBC_FMT					\
	LIBC_INTRIN					\
	LIBC_LOG					\
	LIBC_MEM					\
	LIBC_NEXGEN32E					\
	LIBC_PROC					\
	LIBC_RUNTIME					\
	LIBC_STDIO					\
	LIBC_STR					\
	LIBC_SYSV					\
	LIBC_SYSV_CALLS					\
	LIBC_THREAD					\
	LIBC_TESTLIB					\
	LIBC_TINYMATH					\
	LIBC_X						\
	TOOL_VIZ_LIB					\
	THIRD_PARTY_COMPILER_RT				\
	THIRD_PARTY_NSYNC

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
		$(APE_NO_MODIFY_SELF)
	@$(APELINK)

# Test what happens when *NSYNC isn't linked.
o/$(MODE)/test/libc/intrin/lock_test.com.dbg:		\
		$(TEST_LIBC_INTRIN_DEPS)		\
		o/$(MODE)/test/libc/intrin/lock_test.o	\
		o/$(MODE)/test/libc/intrin/intrin.pkg	\
		$(CRT)					\
		$(APE_NO_MODIFY_SELF)
	@$(APELINK)

$(TEST_LIBC_INTRIN_OBJS): private			\
		CFLAGS +=				\
			-fno-builtin

.PHONY: o/$(MODE)/test/libc/intrin
o/$(MODE)/test/libc/intrin:				\
		$(TEST_LIBC_INTRIN_BINS)		\
		$(TEST_LIBC_INTRIN_CHECKS)
