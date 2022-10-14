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
	LIBC_STDIO					\
	LIBC_STR					\
	LIBC_STUBS					\
	LIBC_SYSV					\
	LIBC_TESTLIB					\
	LIBC_THREAD					\
	LIBC_TIME					\
	THIRD_PARTY_NSYNC				\
	THIRD_PARTY_NSYNC_MEM

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

o/$(MODE)/test/libc/thread/pthread_create_test.o:	\
		private OVERRIDE_CPPFLAGS +=		\
			-DSTACK_FRAME_UNLIMITED

.PHONY: o/$(MODE)/test/libc/thread
o/$(MODE)/test/libc/thread:				\
		$(TEST_LIBC_THREAD_BINS)		\
		$(TEST_LIBC_THREAD_CHECKS)
