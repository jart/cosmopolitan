#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#───vi: set et ft=make ts=8 tw=8 fenc=utf-8 :vi───────────────────────┘

PKGS += TEST_TOOL_VIZ_LIB

TEST_TOOL_VIZ_LIB_SRCS := $(wildcard test/tool/viz/lib/*.c)
TEST_TOOL_VIZ_LIB_SRCS_TEST = $(filter %_test.c,$(TEST_TOOL_VIZ_LIB_SRCS))

TEST_TOOL_VIZ_LIB_OBJS =				\
	$(TEST_TOOL_VIZ_LIB_SRCS:%=o/$(MODE)/%.zip.o)	\
	$(TEST_TOOL_VIZ_LIB_SRCS:%.c=o/$(MODE)/%.o)

TEST_TOOL_VIZ_LIB_COMS =				\
	$(TEST_TOOL_VIZ_LIB_SRCS:%.c=o/$(MODE)/%.com)

TEST_TOOL_VIZ_LIB_BINS =				\
	$(TEST_TOOL_VIZ_LIB_COMS)			\
	$(TEST_TOOL_VIZ_LIB_COMS:%=%.dbg)

TEST_TOOL_VIZ_LIB_TESTS =				\
	$(TEST_TOOL_VIZ_LIB_SRCS_TEST:%.c=o/$(MODE)/%.com.ok)

TEST_TOOL_VIZ_LIB_CHECKS =				\
	$(TEST_TOOL_VIZ_LIB_SRCS_TEST:%.c=o/$(MODE)/%.com.runs)

TEST_TOOL_VIZ_LIB_DIRECTDEPS =				\
	DSP_MPEG					\
	LIBC_FMT					\
	LIBC_LOG					\
	LIBC_UNICODE					\
	LIBC_TIME					\
	LIBC_MEM					\
	LIBC_NEXGEN32E					\
	LIBC_RUNTIME					\
	LIBC_ALG					\
	LIBC_RAND					\
	LIBC_STDIO					\
	LIBC_STUBS					\
	LIBC_TINYMATH					\
	LIBC_TESTLIB					\
	LIBC_X						\
	TOOL_VIZ_LIB

TEST_TOOL_VIZ_LIB_DEPS :=				\
	$(call uniq,$(foreach x,$(TEST_TOOL_VIZ_LIB_DIRECTDEPS),$($(x))))

o/$(MODE)/test/tool/viz/lib/vizlib.pkg:			\
		$(TEST_TOOL_VIZ_LIB_OBJS)		\
		$(foreach x,$(TEST_TOOL_VIZ_LIB_DIRECTDEPS),$($(x)_A).pkg)

o/$(MODE)/test/tool/viz/lib/%.com.dbg:			\
		$(TEST_TOOL_VIZ_LIB_DEPS)		\
		o/$(MODE)/test/tool/viz/lib/%.o		\
		o/$(MODE)/test/tool/viz/lib/vizlib.pkg	\
		$(LIBC_TESTMAIN)			\
		$(CRT)					\
		$(APE)
	@$(APELINK)

.PHONY:		o/$(MODE)/test/tool/viz/lib
o/$(MODE)/test/tool/viz/lib:				\
		$(TEST_TOOL_VIZ_LIB_BINS)		\
		$(TEST_TOOL_VIZ_LIB_CHECKS)
