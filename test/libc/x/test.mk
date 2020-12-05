#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#───vi: set et ft=make ts=8 tw=8 fenc=utf-8 :vi───────────────────────┘

PKGS += TEST_LIBC_X

TEST_LIBC_X_SRCS := $(wildcard test/libc/x/*.c)
TEST_LIBC_X_SRCS_TEST = $(filter %_test.c,$(TEST_LIBC_X_SRCS))

TEST_LIBC_X_OBJS =				\
	$(TEST_LIBC_X_SRCS:%=o/$(MODE)/%.zip.o)	\
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
	LIBC_MEM				\
	LIBC_STDIO				\
	LIBC_STR				\
	LIBC_NEXGEN32E				\
	LIBC_RUNTIME				\
	LIBC_X					\
	LIBC_STUBS				\
	LIBC_TESTLIB				\
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
		$(APE)
	@$(APELINK)

.PHONY: o/$(MODE)/test/libc/x
o/$(MODE)/test/libc/x:				\
		$(TEST_LIBC_X_BINS)		\
		$(TEST_LIBC_X_CHECKS)
