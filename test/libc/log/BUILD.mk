#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#── vi: set noet ft=make ts=8 sw=8 fenc=utf-8 :vi ────────────────────┘

PKGS += TEST_LIBC_LOG

TEST_LIBC_LOG_SRCS := $(wildcard test/libc/log/*.c)
TEST_LIBC_LOG_SRCS_TEST = $(filter %_test.c,$(TEST_LIBC_LOG_SRCS))
TEST_LIBC_LOG_OBJS = $(TEST_LIBC_LOG_SRCS:%.c=o/$(MODE)/%.o)
TEST_LIBC_LOG_COMS = $(TEST_LIBC_LOG_SRCS:%.c=o/$(MODE)/%)

TEST_LIBC_LOG_BINS =						\
	$(TEST_LIBC_LOG_COMS)					\
	$(TEST_LIBC_LOG_COMS:%=%.dbg)

TEST_LIBC_LOG_TESTS =						\
	$(TEST_LIBC_LOG_SRCS_TEST:%.c=o/$(MODE)/%.ok)

TEST_LIBC_LOG_CHECKS =						\
	$(TEST_LIBC_LOG_SRCS_TEST:%.c=o/$(MODE)/%.runs)

TEST_LIBC_LOG_DIRECTDEPS =					\
	LIBC_CALLS						\
	LIBC_FMT						\
	LIBC_INTRIN						\
	LIBC_LOG						\
	LIBC_MEM						\
	LIBC_NEXGEN32E						\
	LIBC_PROC						\
	LIBC_RUNTIME						\
	LIBC_STDIO						\
	LIBC_STR						\
	LIBC_SYSV						\
	LIBC_TESTLIB						\

TEST_LIBC_LOG_DEPS :=						\
	$(call uniq,$(foreach x,$(TEST_LIBC_LOG_DIRECTDEPS),$($(x))))

o/$(MODE)/test/libc/log/log.pkg:				\
		$(TEST_LIBC_LOG_OBJS)				\
		$(foreach x,$(TEST_LIBC_LOG_DIRECTDEPS),$($(x)_A).pkg)

o/$(MODE)/test/libc/log/%.dbg:					\
		$(TEST_LIBC_LOG_DEPS)				\
		o/$(MODE)/test/libc/log/%.o			\
		o/$(MODE)/test/libc/log/log.pkg			\
		$(LIBC_TESTMAIN)				\
		$(CRT)						\
		$(APE_NO_MODIFY_SELF)
	@$(APELINK)

.PHONY: o/$(MODE)/test/libc/log
o/$(MODE)/test/libc/log:					\
		$(TEST_LIBC_LOG_BINS)				\
		$(TEST_LIBC_LOG_CHECKS)
