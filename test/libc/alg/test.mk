#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#───vi: set et ft=make ts=8 tw=8 fenc=utf-8 :vi───────────────────────┘

PKGS += TEST_LIBC_ALG

TEST_LIBC_ALG_SRCS := $(wildcard test/libc/alg/*.c)
TEST_LIBC_ALG_SRCS_TEST = $(filter %_test.c,$(TEST_LIBC_ALG_SRCS))

TEST_LIBC_ALG_OBJS =					\
	$(TEST_LIBC_ALG_SRCS:%=o/$(MODE)/%.zip.o)	\
	$(TEST_LIBC_ALG_SRCS:%.c=o/$(MODE)/%.o)

TEST_LIBC_ALG_COMS =					\
	$(TEST_LIBC_ALG_SRCS:%.c=o/$(MODE)/%.com)

TEST_LIBC_ALG_BINS =					\
	$(TEST_LIBC_ALG_COMS)				\
	$(TEST_LIBC_ALG_COMS:%=%.dbg)

TEST_LIBC_ALG_TESTS = $(TEST_LIBC_ALG_SRCS_TEST:%.c=o/$(MODE)/%.com.ok)

TEST_LIBC_ALG_CHECKS =					\
	$(TEST_LIBC_ALG_SRCS_TEST:%.c=o/$(MODE)/%.com.runs)

TEST_LIBC_ALG_DIRECTDEPS =				\
	LIBC_ALG					\
	LIBC_RAND					\
	LIBC_LOG					\
	LIBC_STDIO					\
	LIBC_CALLS_HEFTY				\
	LIBC_FMT					\
	LIBC_MEM					\
	LIBC_NEXGEN32E					\
	LIBC_RUNTIME					\
	LIBC_STR					\
	LIBC_STUBS					\
	LIBC_SYSV					\
	LIBC_TESTLIB					\
	LIBC_X

TEST_LIBC_ALG_DEPS :=					\
	$(call uniq,$(foreach x,$(TEST_LIBC_ALG_DIRECTDEPS),$($(x))))

o/$(MODE)/test/libc/alg/alg.pkg:			\
		$(TEST_LIBC_ALG_OBJS)			\
		$(foreach x,$(TEST_LIBC_ALG_DIRECTDEPS),$($(x)_A).pkg)

o/$(MODE)/test/libc/alg/%.com.dbg:			\
		$(TEST_LIBC_ALG_DEPS)			\
		o/$(MODE)/test/libc/alg/%.o		\
		o/$(MODE)/test/libc/alg/alg.pkg		\
		$(LIBC_TESTMAIN)			\
		$(CRT)					\
		$(APE)
	@$(APELINK)

$(TEST_LIBC_ALG_OBJS): test/libc/alg/test.mk

$(TEST_LIBC_ALG_OBJS):					\
	DEFAULT_CCFLAGS +=				\
		-fno-builtin

.PHONY: o/$(MODE)/test/libc/alg
o/$(MODE)/test/libc/alg:				\
		$(TEST_LIBC_ALG_BINS)			\
		$(TEST_LIBC_ALG_CHECKS)
