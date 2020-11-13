#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#───vi: set et ft=make ts=8 tw=8 fenc=utf-8 :vi───────────────────────┘

PKGS += TEST_APE_LIB

TEST_APE_LIB_SRCS := $(wildcard test/ape/lib/*.c)
TEST_APE_LIB_SRCS_TEST = $(filter %_test.c,$(TEST_APE_LIB_SRCS))

TEST_APE_LIB_OBJS =					\
	$(TEST_APE_LIB_SRCS:%=o/$(MODE)/%.zip.o)	\
	$(TEST_APE_LIB_SRCS:%.c=o/$(MODE)/%.o)

TEST_APE_LIB_COMS =					\
	$(TEST_APE_LIB_SRCS:%.c=o/$(MODE)/%.com)

TEST_APE_LIB_BINS =					\
	$(TEST_APE_LIB_COMS)				\
	$(TEST_APE_LIB_COMS:%=%.dbg)

TEST_APE_LIB_TESTS =					\
	$(TEST_APE_LIB_SRCS_TEST:%.c=o/$(MODE)/%.com.ok)

TEST_APE_LIB_CHECKS =					\
	$(TEST_APE_LIB_SRCS_TEST:%.c=o/$(MODE)/%.com.runs)

TEST_APE_LIB_DIRECTDEPS =				\
	APE_LIB						\
	LIBC_CALLS_HEFTY				\
	LIBC_NEXGEN32E					\
	LIBC_RUNTIME					\
	LIBC_STR					\
	LIBC_LOG					\
	LIBC_STUBS					\
	LIBC_TESTLIB					\
	LIBC_X

TEST_APE_LIB_DEPS :=					\
	$(call uniq,$(foreach x,$(TEST_APE_LIB_DIRECTDEPS),$($(x))))

o/$(MODE)/test/ape/lib.pkg:				\
		$(TEST_APE_LIB_OBJS)			\
		$(foreach x,$(TEST_APE_LIB_DIRECTDEPS),$($(x)_A).pkg)

o/$(MODE)/test/ape/lib/%.com.dbg:			\
		$(TEST_APE_LIB_DEPS)			\
		o/$(MODE)/test/ape/lib/%.o		\
		o/$(MODE)/test/ape/lib.pkg		\
		$(LIBC_TESTMAIN)			\
		$(CRT)					\
		$(APE)
	@$(APELINK)

# $(TEST_APE_LIB_OBJS):					\
# 		OVERRIDE_CFLAGS +=			\
# 			-fsanitize=address

.PHONY: o/$(MODE)/test/ape/lib
o/$(MODE)/test/ape/lib:	$(TEST_APE_LIB_BINS)		\
		$(TEST_APE_LIB_CHECKS)
