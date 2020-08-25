#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#───vi: set et ft=make ts=8 tw=8 fenc=utf-8 :vi───────────────────────┘

PKGS += TEST_LIBC_CONV

TEST_LIBC_CONV_SRCS := $(wildcard test/libc/conv/*.c)
TEST_LIBC_CONV_SRCS_TEST = $(filter %_test.c,$(TEST_LIBC_CONV_SRCS))

TEST_LIBC_CONV_OBJS =					\
	$(TEST_LIBC_CONV_SRCS:%=o/$(MODE)/%.zip.o)	\
	$(TEST_LIBC_CONV_SRCS:%.c=o/$(MODE)/%.o)

TEST_LIBC_CONV_COMS =					\
	$(TEST_LIBC_CONV_SRCS:%.c=o/$(MODE)/%.com)

TEST_LIBC_CONV_BINS =					\
	$(TEST_LIBC_CONV_COMS)				\
	$(TEST_LIBC_CONV_COMS:%=%.dbg)

TEST_LIBC_CONV_TESTS =					\
	$(TEST_LIBC_CONV_SRCS_TEST:%.c=o/$(MODE)/%.com.ok)

TEST_LIBC_CONV_CHECKS =					\
	$(TEST_LIBC_CONV_SRCS_TEST:%.c=o/$(MODE)/%.com.runs)

TEST_LIBC_CONV_DIRECTDEPS =				\
	LIBC_CONV					\
	LIBC_NEXGEN32E					\
	LIBC_STUBS					\
	LIBC_TESTLIB

TEST_LIBC_CONV_DEPS :=					\
	$(call uniq,$(foreach x,$(TEST_LIBC_CONV_DIRECTDEPS),$($(x))))

o/$(MODE)/test/libc/conv/conv.pkg:			\
		$(TEST_LIBC_CONV_OBJS)			\
		$(foreach x,$(TEST_LIBC_CONV_DIRECTDEPS),$($(x)_A).pkg)

o/$(MODE)/test/libc/conv/%.com.dbg:			\
		$(TEST_LIBC_CONV_DEPS)			\
		o/$(MODE)/test/libc/conv/%.o		\
		o/$(MODE)/test/libc/conv/conv.pkg	\
		$(LIBC_TESTMAIN)			\
		$(CRT)					\
		$(APE)
	@$(APELINK)

$(TEST_LIBC_CONV_OBJS): test/libc/conv/test.mk

$(TEST_LIBC_CONV_OBJS):					\
	DEFAULT_CCFLAGS +=				\
		-fno-builtin

.PHONY: o/$(MODE)/test/libc/conv
o/$(MODE)/test/libc/conv:				\
		$(TEST_LIBC_CONV_BINS)			\
		$(TEST_LIBC_CONV_CHECKS)
