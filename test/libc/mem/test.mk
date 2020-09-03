#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#───vi: set et ft=make ts=8 tw=8 fenc=utf-8 :vi───────────────────────┘

PKGS += TEST_LIBC_MEM

TEST_LIBC_MEM_SRCS := $(wildcard test/libc/mem/*.c)
TEST_LIBC_MEM_SRCS_TEST = $(filter %_test.c,$(TEST_LIBC_MEM_SRCS))

TEST_LIBC_MEM_OBJS =					\
	$(TEST_LIBC_MEM_SRCS:%=o/$(MODE)/%.zip.o)	\
	$(TEST_LIBC_MEM_SRCS:%.c=o/$(MODE)/%.o)

TEST_LIBC_MEM_COMS =					\
	$(TEST_LIBC_MEM_SRCS:%.c=o/$(MODE)/%.com)

TEST_LIBC_MEM_BINS =					\
	$(TEST_LIBC_MEM_COMS)				\
	$(TEST_LIBC_MEM_COMS:%=%.dbg)

TEST_LIBC_MEM_TESTS = $(TEST_LIBC_MEM_SRCS_TEST:%.c=o/$(MODE)/%.com.ok)

TEST_LIBC_MEM_CHECKS =					\
	$(TEST_LIBC_MEM_SRCS_TEST:%.c=o/$(MODE)/%.com.runs)

TEST_LIBC_MEM_DIRECTDEPS =				\
	LIBC_MEM					\
	LIBC_CALLS					\
	LIBC_STUBS					\
	LIBC_NEXGEN32E					\
	LIBC_SYSV					\
	LIBC_FMT					\
	LIBC_RUNTIME					\
	LIBC_STDIO					\
	LIBC_RAND					\
	LIBC_TESTLIB

TEST_LIBC_MEM_DEPS :=					\
	$(call uniq,$(foreach x,$(TEST_LIBC_MEM_DIRECTDEPS),$($(x))))

o/$(MODE)/test/libc/mem/mem.pkg:			\
		$(TEST_LIBC_MEM_OBJS)			\
		$(foreach x,$(TEST_LIBC_MEM_DIRECTDEPS),$($(x)_A).pkg)

o/$(MODE)/test/libc/mem/%.com.dbg:			\
		$(TEST_LIBC_MEM_DEPS)			\
		o/$(MODE)/test/libc/mem/%.o		\
		o/$(MODE)/test/libc/mem/mem.pkg		\
		$(LIBC_TESTMAIN)			\
		$(CRT)					\
		$(APE)
	@$(APELINK)

$(TEST_LIBC_MEM_OBJS):					\
		DEFAULT_CCFLAGS +=			\
			-fno-builtin

.PHONY: o/$(MODE)/test/libc/mem
o/$(MODE)/test/libc/mem:				\
		$(TEST_LIBC_MEM_BINS)			\
		$(TEST_LIBC_MEM_CHECKS)
