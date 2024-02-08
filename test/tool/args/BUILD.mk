#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#── vi: set noet ft=make ts=8 sw=8 fenc=utf-8 :vi ────────────────────┘

PKGS += TEST_TOOL_ARGS

TEST_TOOL_ARGS = $(TOOL_ARGS_A_DEPS) $(TOOL_ARGS_A)
TEST_TOOL_ARGS_A = o/$(MODE)/test/tool/args/argstest.a
TEST_TOOL_ARGS_FILES := $(wildcard test/tool/args/*)
TEST_TOOL_ARGS_SRCS = $(filter %.c,$(TEST_TOOL_ARGS_FILES))
TEST_TOOL_ARGS_SRCS_TEST = $(filter %_test.c,$(TEST_TOOL_ARGS_SRCS))
TEST_TOOL_ARGS_HDRS = $(filter %.h,$(TEST_TOOL_ARGS_FILES))
TEST_TOOL_ARGS_COMS = $(TEST_TOOL_ARGS_OBJS:%.o=%.com)

TEST_TOOL_ARGS_OBJS =				\
	$(TEST_TOOL_ARGS_SRCS:%.c=o/$(MODE)/%.o)

TEST_TOOL_ARGS_COMS =				\
	$(TEST_TOOL_ARGS_SRCS:%.c=o/$(MODE)/%.com)

TEST_TOOL_ARGS_BINS =				\
	$(TEST_TOOL_ARGS_COMS)			\
	$(TEST_TOOL_ARGS_COMS:%=%.dbg)

TEST_TOOL_ARGS_TESTS =				\
	$(TEST_TOOL_ARGS_SRCS_TEST:%.c=o/$(MODE)/%.com.ok)

TEST_TOOL_ARGS_CHECKS =				\
	$(TEST_TOOL_ARGS_HDRS:%=o/$(MODE)/%.ok)	\
	$(TEST_TOOL_ARGS_SRCS_TEST:%.c=o/$(MODE)/%.com.runs)

TEST_TOOL_ARGS_DIRECTDEPS =			\
	LIBC_CALLS				\
	LIBC_FMT				\
	LIBC_INTRIN				\
	LIBC_LOG				\
	LIBC_MEM				\
	LIBC_NEXGEN32E				\
	LIBC_RUNTIME				\
	LIBC_STDIO				\
	LIBC_STR				\
	LIBC_SYSV				\
	LIBC_TESTLIB				\
	LIBC_X					\
	THIRD_PARTY_COMPILER_RT			\
	TOOL_ARGS

TEST_TOOL_ARGS_DEPS :=				\
	$(call uniq,$(foreach x,$(TEST_TOOL_ARGS_DIRECTDEPS),$($(x))))

$(TEST_TOOL_ARGS_A):				\
		test/tool/args/			\
		$(TEST_TOOL_ARGS_A).pkg		\
		$(TEST_TOOL_ARGS_OBJS)

$(TEST_TOOL_ARGS_A).pkg:			\
		$(TEST_TOOL_ARGS_OBJS)		\
		$(foreach x,$(TEST_TOOL_ARGS_DIRECTDEPS),$($(x)_A).pkg)

o/$(MODE)/test/tool/args/%.com.dbg:		\
		$(TEST_TOOL_ARGS_DEPS)		\
		$(TEST_TOOL_ARGS_A)		\
		o/$(MODE)/test/tool/args/%.o	\
		$(TEST_TOOL_ARGS_A).pkg		\
		$(LIBC_TESTMAIN)		\
		$(CRT)				\
		$(APE_NO_MODIFY_SELF)
	@$(APELINK)

.PHONY: o/$(MODE)/test/tool/args
o/$(MODE)/test/tool/args:			\
		$(TEST_TOOL_ARGS_BINS)		\
		$(TEST_TOOL_ARGS_CHECKS)
