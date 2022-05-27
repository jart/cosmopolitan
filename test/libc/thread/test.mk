#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#───vi: set et ft=make ts=8 tw=8 fenc=utf-8 :vi───────────────────────┘

PKGS += TEST_LIBC_THREAD

TEST_LIBC_THREAD_SRCS := $(wildcard test/libc/thread/*.c)
TEST_LIBC_THREAD_SRCS_TEST = $(filter %_test.c,$(TEST_LIBC_THREAD_SRCS))

TEST_LIBC_THREAD_OBJS =					\
	$(TEST_LIBC_THREAD_SRCS:%.c=o/$(MODE)/%.o)

TEST_LIBC_THREAD_COMS =					\
	$(TEST_LIBC_THREAD_SRCS:%.c=o/$(MODE)/%.com)

TEST_LIBC_THREAD_BINS =					\
	$(TEST_LIBC_THREAD_COMS)			\
	$(TEST_LIBC_THREAD_COMS:%=%.dbg)

TEST_LIBC_THREAD_TESTS =				\
	$(TEST_LIBC_THREAD_SRCS_TEST:%.c=o/$(MODE)/%.com.ok)

TEST_LIBC_THREAD_CHECKS =				\
	$(TEST_LIBC_THREAD_SRCS_TEST:%.c=o/$(MODE)/%.com.runs)

TEST_LIBC_THREAD_DIRECTDEPS =				\
	LIBC_CALLS					\
	LIBC_FMT					\
	LIBC_INTRIN					\
	LIBC_MEM					\
	LIBC_NEXGEN32E					\
	LIBC_RUNTIME					\
	LIBC_STR					\
	LIBC_STUBS					\
	LIBC_SYSV					\
	LIBC_THREAD					\
	LIBC_TESTLIB

TEST_LIBC_THREAD_DEPS :=				\
	$(call uniq,$(foreach x,$(TEST_LIBC_THREAD_DIRECTDEPS),$($(x))))

o/$(MODE)/test/libc/thread/thread.pkg:			\
		$(TEST_LIBC_THREAD_OBJS)		\
		$(foreach x,$(TEST_LIBC_THREAD_DIRECTDEPS),$($(x)_A).pkg)

o/$(MODE)/test/libc/thread/%.com.dbg:			\
		$(TEST_LIBC_THREAD_DEPS)		\
		o/$(MODE)/test/libc/thread/%.o		\
		o/$(MODE)/test/libc/thread/thread.pkg	\
		$(LIBC_TESTMAIN)			\
		$(CRT)					\
		$(APE_NO_MODIFY_SELF)
	@$(APELINK)

$(TEST_LIBC_THREAD_OBJS):				\
	DEFAULT_CCFLAGS +=				\
		-fno-builtin

o/$(MODE)/test/libc/thread/getenv_test.com.runs:	\
		o/$(MODE)/test/libc/thread/getenv_test.com
	@HELLO=THERE build/runit $@ $<

o/$(MODE)/test/libc/thread/fun_test.o			\
o/$(MODE)/test/libc/thread/itsatrap_test.o:		\
		OVERRIDE_CFLAGS +=			\
			-fno-sanitize=all		\
			-ftrapv

.PHONY: o/$(MODE)/test/libc/thread
o/$(MODE)/test/libc/thread:				\
		$(TEST_LIBC_THREAD_BINS)		\
		$(TEST_LIBC_THREAD_CHECKS)
