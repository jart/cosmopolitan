#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#───vi: set et ft=make ts=8 tw=8 fenc=utf-8 :vi───────────────────────┘

PKGS += TEST_LIBC_UNICODE

TEST_LIBC_UNICODE_SRCS := $(wildcard test/libc/unicode/*.c)
TEST_LIBC_UNICODE_SRCS_TEST = $(filter %_test.c,$(TEST_LIBC_UNICODE_SRCS))

TEST_LIBC_UNICODE_OBJS =				\
	$(TEST_LIBC_UNICODE_SRCS:%=o/$(MODE)/%.zip.o)	\
	$(TEST_LIBC_UNICODE_SRCS:%.c=o/$(MODE)/%.o)

TEST_LIBC_UNICODE_COMS =				\
	$(TEST_LIBC_UNICODE_SRCS:%.c=o/$(MODE)/%.com)

TEST_LIBC_UNICODE_BINS =				\
	$(TEST_LIBC_UNICODE_COMS)			\
	$(TEST_LIBC_UNICODE_COMS:%=%.dbg)

TEST_LIBC_UNICODE_TESTS =				\
	$(TEST_LIBC_UNICODE_SRCS_TEST:%.c=o/$(MODE)/%.com.ok)

TEST_LIBC_UNICODE_CHECKS =				\
	$(TEST_LIBC_UNICODE_SRCS_TEST:%.c=o/$(MODE)/%.com.runs)

TEST_LIBC_UNICODE_DIRECTDEPS =				\
	LIBC_NEXGEN32E					\
	LIBC_STR					\
	LIBC_STUBS					\
	LIBC_TESTLIB					\
	LIBC_UNICODE

TEST_LIBC_UNICODE_DEPS :=				\
	$(call uniq,$(foreach x,$(TEST_LIBC_UNICODE_DIRECTDEPS),$($(x))))

o/$(MODE)/test/libc/unicode/unicode.pkg:		\
		$(TEST_LIBC_UNICODE_OBJS)		\
		$(foreach x,$(TEST_LIBC_UNICODE_DIRECTDEPS),$($(x)_A).pkg)

o/$(MODE)/test/libc/unicode/%.com.dbg:			\
		$(TEST_LIBC_UNICODE_DEPS)		\
		o/$(MODE)/test/libc/unicode/%.o		\
		o/$(MODE)/test/libc/unicode/unicode.pkg	\
		$(LIBC_TESTMAIN)			\
		$(CRT)					\
		$(APE)
	@$(APELINK)

.PHONY: o/$(MODE)/test/libc/unicode
o/$(MODE)/test/libc/unicode:				\
		$(TEST_LIBC_UNICODE_BINS)		\
		$(TEST_LIBC_UNICODE_CHECKS)
