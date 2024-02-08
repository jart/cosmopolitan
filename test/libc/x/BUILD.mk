#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#── vi: set noet ft=make ts=8 sw=8 fenc=utf-8 :vi ────────────────────┘

PKGS += TEST_LIBC_X

TEST_LIBC_X_SRCS := $(wildcard test/libc/x/*.c)
TEST_LIBC_X_SRCS_TEST = $(filter %_test.c,$(TEST_LIBC_X_SRCS))

TEST_LIBC_X_OBJS =				\
	$(TEST_LIBC_X_SRCS:%.c=o/$(MODE)/%.o)

TEST_LIBC_X_COMS =				\
	$(TEST_LIBC_X_SRCS:%.c=o/$(MODE)/%.com)

TEST_LIBC_X_BINS =				\
	$(TEST_LIBC_X_COMS)			\
	$(TEST_LIBC_X_COMS:%=%.dbg)

TEST_LIBC_X_TESTS =				\
	$(TEST_LIBC_X_SRCS_TEST:%.c=o/$(MODE)/%.com.ok)

TEST_LIBC_X_CHECKS =				\
	$(TEST_LIBC_X_SRCS_TEST:%.c=o/$(MODE)/%.com.runs)

TEST_LIBC_X_DIRECTDEPS =			\
	LIBC_CALLS				\
	LIBC_FMT				\
	LIBC_INTRIN				\
	LIBC_MEM				\
	LIBC_NEXGEN32E				\
	LIBC_RUNTIME				\
	LIBC_STDIO				\
	LIBC_STR				\
	LIBC_SOCK				\
	LIBC_SYSV				\
	LIBC_THREAD				\
	LIBC_TESTLIB				\
	LIBC_X					\
	THIRD_PARTY_GDTOA

TEST_LIBC_X_DEPS :=				\
	$(call uniq,$(foreach x,$(TEST_LIBC_X_DIRECTDEPS),$($(x))))

o/$(MODE)/test/libc/x/x.pkg:			\
		$(TEST_LIBC_X_OBJS)		\
		$(foreach x,$(TEST_LIBC_X_DIRECTDEPS),$($(x)_A).pkg)

o/$(MODE)/test/libc/x/%.com.dbg:		\
		$(TEST_LIBC_X_DEPS)		\
		o/$(MODE)/test/libc/x/%.o	\
		o/$(MODE)/test/libc/x/x.pkg	\
		$(LIBC_TESTMAIN)		\
		$(CRT)				\
		$(APE_NO_MODIFY_SELF)
	@$(APELINK)

.PHONY: o/$(MODE)/test/libc/x
o/$(MODE)/test/libc/x:				\
		$(TEST_LIBC_X_BINS)		\
		$(TEST_LIBC_X_CHECKS)
