#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#───vi: set et ft=make ts=8 tw=8 fenc=utf-8 :vi───────────────────────┘

PKGS += TEST_LIBC_BITS

TEST_LIBC_BITS_SRCS := $(wildcard test/libc/bits/*.c)
TEST_LIBC_BITS_SRCS_TEST = $(filter %_test.c,$(TEST_LIBC_BITS_SRCS))

TEST_LIBC_BITS_OBJS =					\
	$(TEST_LIBC_BITS_SRCS:%=o/$(MODE)/%.zip.o)	\
	$(TEST_LIBC_BITS_SRCS:%.c=o/$(MODE)/%.o)

TEST_LIBC_BITS_COMS =					\
	$(TEST_LIBC_BITS_SRCS:%.c=o/$(MODE)/%.com)

TEST_LIBC_BITS_BINS =					\
	$(TEST_LIBC_BITS_COMS)				\
	$(TEST_LIBC_BITS_COMS:%=%.dbg)

TEST_LIBC_BITS_TESTS =					\
	$(TEST_LIBC_BITS_SRCS_TEST:%.c=o/$(MODE)/%.com.ok)

TEST_LIBC_BITS_CHECKS =					\
	$(TEST_LIBC_BITS_SRCS_TEST:%.c=o/$(MODE)/%.com.runs)

TEST_LIBC_BITS_DIRECTDEPS =				\
	LIBC_X						\
	LIBC_BITS					\
	LIBC_NEXGEN32E					\
	LIBC_STUBS					\
	LIBC_TESTLIB					\
	THIRD_PARTY_COMPILER_RT

TEST_LIBC_BITS_DEPS :=					\
	$(call uniq,$(foreach x,$(TEST_LIBC_BITS_DIRECTDEPS),$($(x))))

o/$(MODE)/test/libc/bits/bits.pkg:			\
		$(TEST_LIBC_BITS_OBJS)			\
		$(foreach x,$(TEST_LIBC_BITS_DIRECTDEPS),$($(x)_A).pkg)

o/$(MODE)/test/libc/bits/%.com.dbg:			\
		$(TEST_LIBC_BITS_DEPS)			\
		o/$(MODE)/test/libc/bits/%.o		\
		o/$(MODE)/test/libc/bits/bits.pkg	\
		$(LIBC_TESTMAIN)			\
		$(CRT)					\
		$(APE)
	@$(APELINK)

$(TEST_LIBC_BITS_OBJS):					\
	DEFAULT_CCFLAGS +=				\
		-fno-builtin

.PHONY: o/$(MODE)/test/libc/bits
o/$(MODE)/test/libc/bits:				\
		$(TEST_LIBC_BITS_BINS)			\
		$(TEST_LIBC_BITS_CHECKS)
