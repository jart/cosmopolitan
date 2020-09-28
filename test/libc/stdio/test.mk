#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#───vi: set et ft=make ts=8 tw=8 fenc=utf-8 :vi───────────────────────┘

PKGS += TEST_LIBC_STDIO

TEST_LIBC_STDIO_SRCS := $(wildcard test/libc/stdio/*.c)
TEST_LIBC_STDIO_SRCS_TEST = $(filter %_test.c,$(TEST_LIBC_STDIO_SRCS))

TEST_LIBC_STDIO_OBJS =					\
	$(TEST_LIBC_STDIO_SRCS:%=o/$(MODE)/%.zip.o)	\
	$(TEST_LIBC_STDIO_SRCS:%.c=o/$(MODE)/%.o)

TEST_LIBC_STDIO_COMS =					\
	$(TEST_LIBC_STDIO_SRCS:%.c=o/$(MODE)/%.com)

TEST_LIBC_STDIO_BINS =					\
	$(TEST_LIBC_STDIO_COMS)				\
	$(TEST_LIBC_STDIO_COMS:%=%.dbg)

TEST_LIBC_STDIO_TESTS =					\
	$(TEST_LIBC_STDIO_SRCS_TEST:%.c=o/$(MODE)/%.com.ok)

TEST_LIBC_STDIO_CHECKS =				\
	$(TEST_LIBC_STDIO_SRCS_TEST:%.c=o/$(MODE)/%.com.runs)

TEST_LIBC_STDIO_DIRECTDEPS =				\
	LIBC_BITS					\
	LIBC_CALLS					\
	LIBC_CALLS_HEFTY				\
	LIBC_FMT					\
	LIBC_NEXGEN32E					\
	LIBC_RUNTIME					\
	LIBC_STDIO					\
	LIBC_STR					\
	LIBC_STUBS					\
	LIBC_SYSV					\
	LIBC_TESTLIB					\
	LIBC_X

TEST_LIBC_STDIO_DEPS :=					\
	$(call uniq,$(foreach x,$(TEST_LIBC_STDIO_DIRECTDEPS),$($(x))))

o/$(MODE)/test/libc/stdio/stdio.pkg:			\
		$(TEST_LIBC_STDIO_OBJS)			\
		$(foreach x,$(TEST_LIBC_STDIO_DIRECTDEPS),$($(x)_A).pkg)

o/$(MODE)/test/libc/stdio/%.com.dbg:			\
		$(TEST_LIBC_STDIO_DEPS)			\
		o/$(MODE)/test/libc/stdio/%.o		\
		o/$(MODE)/test/libc/stdio/stdio.pkg	\
		$(LIBC_TESTMAIN)			\
		$(CRT)					\
		$(APE)
	@$(APELINK)

$(TEST_LIBC_STDIO_OBJS):				\
	DEFAULT_CCFLAGS +=				\
		-fno-builtin

.PHONY: o/$(MODE)/test/libc/stdio
o/$(MODE)/test/libc/stdio:				\
		$(TEST_LIBC_STDIO_BINS)			\
		$(TEST_LIBC_STDIO_CHECKS)
