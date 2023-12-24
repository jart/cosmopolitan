#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#── vi: set noet ft=make ts=8 sw=8 fenc=utf-8 :vi ────────────────────┘

PKGS += TEST_TOOL_BUILD_LIB

TEST_TOOL_BUILD_LIB = $(TOOL_BUILD_LIB_A_DEPS) $(TOOL_BUILD_LIB_A)
TEST_TOOL_BUILD_LIB_A = o/$(MODE)/test/tool/build/lib/buildlib.a
TEST_TOOL_BUILD_LIB_FILES := $(wildcard test/tool/build/lib/*)
TEST_TOOL_BUILD_LIB_SRCS = $(filter %.c,$(TEST_TOOL_BUILD_LIB_FILES))
TEST_TOOL_BUILD_LIB_SRCS_TEST = $(filter %_test.c,$(TEST_TOOL_BUILD_LIB_SRCS))
TEST_TOOL_BUILD_LIB_HDRS = $(filter %.h,$(TEST_TOOL_BUILD_LIB_FILES))
TEST_TOOL_BUILD_LIB_COMS = $(TEST_TOOL_BUILD_LIB_OBJS:%.o=%.com)

TEST_TOOL_BUILD_LIB_OBJS =					\
	$(TEST_TOOL_BUILD_LIB_SRCS:%.c=o/$(MODE)/%.o)

TEST_TOOL_BUILD_LIB_COMS =					\
	$(TEST_TOOL_BUILD_LIB_SRCS:%.c=o/$(MODE)/%.com)

TEST_TOOL_BUILD_LIB_BINS =					\
	$(TEST_TOOL_BUILD_LIB_COMS)				\
	$(TEST_TOOL_BUILD_LIB_COMS:%=%.dbg)

TEST_TOOL_BUILD_LIB_TESTS =					\
	$(TEST_TOOL_BUILD_LIB_SRCS_TEST:%.c=o/$(MODE)/%.com.ok)

TEST_TOOL_BUILD_LIB_CHECKS =					\
	$(TEST_TOOL_BUILD_LIB_HDRS:%=o/$(MODE)/%.ok)		\
	$(TEST_TOOL_BUILD_LIB_SRCS_TEST:%.c=o/$(MODE)/%.com.runs)

TEST_TOOL_BUILD_LIB_DIRECTDEPS =				\
	LIBC_CALLS						\
	LIBC_FMT						\
	LIBC_INTRIN						\
	LIBC_LOG						\
	LIBC_MEM						\
	LIBC_NEXGEN32E						\
	LIBC_RUNTIME						\
	LIBC_STDIO						\
	LIBC_STR						\
	LIBC_SYSV						\
	LIBC_TESTLIB						\
	LIBC_X							\
	THIRD_PARTY_COMPILER_RT					\
	TOOL_BUILD_LIB						\
	THIRD_PARTY_XED

TEST_TOOL_BUILD_LIB_DEPS :=					\
	$(call uniq,$(foreach x,$(TEST_TOOL_BUILD_LIB_DIRECTDEPS),$($(x))))

$(TEST_TOOL_BUILD_LIB_A):					\
		test/tool/build/lib/				\
		$(TEST_TOOL_BUILD_LIB_A).pkg			\
		$(TEST_TOOL_BUILD_LIB_OBJS)

$(TEST_TOOL_BUILD_LIB_A).pkg:					\
		$(TEST_TOOL_BUILD_LIB_OBJS)			\
		$(foreach x,$(TEST_TOOL_BUILD_LIB_DIRECTDEPS),$($(x)_A).pkg)

o/$(MODE)/test/tool/build/lib/%.com.dbg:			\
		$(TEST_TOOL_BUILD_LIB_DEPS)			\
		$(TEST_TOOL_BUILD_LIB_A)			\
		o/$(MODE)/test/tool/build/lib/%.o		\
		$(TEST_TOOL_BUILD_LIB_A).pkg			\
		$(LIBC_TESTMAIN)				\
		$(CRT)						\
		$(APE_NO_MODIFY_SELF)
	@$(APELINK)

.PHONY: o/$(MODE)/test/tool/build/lib
o/$(MODE)/test/tool/build/lib:					\
		$(TEST_TOOL_BUILD_LIB_BINS)			\
		$(TEST_TOOL_BUILD_LIB_CHECKS)
