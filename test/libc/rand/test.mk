#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#───vi: set et ft=make ts=8 tw=8 fenc=utf-8 :vi───────────────────────┘

PKGS += TEST_LIBC_RAND

TEST_LIBC_RAND_SRCS := $(wildcard test/libc/rand/*.c)
TEST_LIBC_RAND_SRCS_TEST = $(filter %_test.c,$(TEST_LIBC_RAND_SRCS))
TEST_LIBC_RAND_BINS = $(TEST_LIBC_RAND_COMS) $(TEST_LIBC_RAND_COMS:%=%.dbg)

TEST_LIBC_RAND_OBJS =					\
	$(TEST_LIBC_RAND_SRCS:%=o/$(MODE)/%.zip.o)	\
	$(TEST_LIBC_RAND_SRCS:%.c=o/$(MODE)/%.o)

TEST_LIBC_RAND_COMS =					\
	$(TEST_LIBC_RAND_SRCS:%.c=o/$(MODE)/%.com)

TEST_LIBC_RAND_TESTS = $(TEST_LIBC_RAND_SRCS_TEST:%.c=o/$(MODE)/%.com.ok)

TEST_LIBC_RAND_CHECKS =					\
	$(TEST_LIBC_RAND_SRCS_TEST:%.c=o/$(MODE)/%.com.runs)

TEST_LIBC_RAND_DIRECTDEPS =				\
	LIBC_CALLS_HEFTY				\
	LIBC_FMT					\
	LIBC_NEXGEN32E					\
	LIBC_RAND					\
	LIBC_RUNTIME					\
	LIBC_STR					\
	LIBC_STUBS					\
	LIBC_SYSV					\
	LIBC_TESTLIB					\
	LIBC_X

TEST_LIBC_RAND_DEPS :=					\
	$(call uniq,$(foreach x,$(TEST_LIBC_RAND_DIRECTDEPS),$($(x))))

o/$(MODE)/test/libc/rand/rand.pkg:			\
		$(TEST_LIBC_RAND_OBJS)			\
		$(foreach x,$(TEST_LIBC_RAND_DIRECTDEPS),$($(x)_A).pkg)

o/$(MODE)/test/libc/rand/%.com.dbg:			\
		$(TEST_LIBC_RAND_DEPS)			\
		o/$(MODE)/test/libc/rand/%.o		\
		o/$(MODE)/test/libc/rand/rand.pkg	\
		$(LIBC_TESTMAIN)			\
		$(CRT)					\
		$(APE)
	@$(APELINK)

$(TEST_LIBC_RAND_OBJS): test/libc/rand/test.mk

$(TEST_LIBC_RAND_OBJS):					\
	DEFAULT_CCFLAGS +=				\
		-fno-builtin

.PHONY: o/$(MODE)/test/libc/rand
o/$(MODE)/test/libc/rand:				\
		$(TEST_LIBC_RAND_BINS)			\
		$(TEST_LIBC_RAND_CHECKS)
