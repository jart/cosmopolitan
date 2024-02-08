#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#── vi: set noet ft=make ts=8 sw=8 fenc=utf-8 :vi ────────────────────┘

PKGS += TEST_LIBC_THREAD

TEST_LIBC_THREAD_FILES := $(wildcard test/libc/thread/*)
TEST_LIBC_THREAD_SRCS_C = $(filter %_test.c,$(TEST_LIBC_THREAD_FILES))
TEST_LIBC_THREAD_SRCS_CC = $(filter %_test.cc,$(TEST_LIBC_THREAD_FILES))

TEST_LIBC_THREAD_SRCS =					\
	$(TEST_LIBC_THREAD_SRCS_C)			\
	$(TEST_LIBC_THREAD_SRCS_CC)

TEST_LIBC_THREAD_OBJS =					\
	$(TEST_LIBC_THREAD_SRCS_C:%.c=o/$(MODE)/%.o)	\
	$(TEST_LIBC_THREAD_SRCS_CC:%.cc=o/$(MODE)/%.o)

TEST_LIBC_THREAD_COMS =					\
	$(TEST_LIBC_THREAD_OBJS:%.o=%.com)

TEST_LIBC_THREAD_BINS =					\
	$(TEST_LIBC_THREAD_COMS)			\
	$(TEST_LIBC_THREAD_COMS:%=%.dbg)

TEST_LIBC_THREAD_TESTS =				\
	$(TEST_LIBC_THREAD_OBJS:%.o=%.com.ok)

TEST_LIBC_THREAD_CHECKS =				\
	$(TEST_LIBC_THREAD_OBJS:%.o=%.com.runs)

TEST_LIBC_THREAD_DIRECTDEPS =				\
	LIBC_CALLS					\
	LIBC_FMT					\
	LIBC_INTRIN					\
	LIBC_LOG					\
	LIBC_MEM					\
	LIBC_NEXGEN32E					\
	LIBC_NT_KERNEL32				\
	LIBC_PROC					\
	LIBC_RUNTIME					\
	LIBC_SOCK					\
	LIBC_STDIO					\
	LIBC_STR					\
	LIBC_SYSV					\
	LIBC_SYSV_CALLS					\
	LIBC_TESTLIB					\
	LIBC_THREAD					\
	LIBC_TIME					\
	LIBC_X						\
	THIRD_PARTY_LIBCXXABI				\
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

o/$(MODE)/test/libc/thread/pthread_kill_test.com.runs:	\
		private .PLEDGE = stdio rpath wpath cpath fattr proc inet

.PHONY: o/$(MODE)/test/libc/thread
o/$(MODE)/test/libc/thread:				\
		$(TEST_LIBC_THREAD_BINS)		\
		$(TEST_LIBC_THREAD_CHECKS)
