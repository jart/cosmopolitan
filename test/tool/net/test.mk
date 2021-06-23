#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#───vi: set et ft=make ts=8 tw=8 fenc=utf-8 :vi───────────────────────┘

PKGS += TEST_TOOL_NET

TEST_TOOL_NET = $(TOOL_NET_A_DEPS) $(TOOL_NET_A)
TEST_TOOL_NET_A = o/$(MODE)/test/tool/net/net.a
TEST_TOOL_NET_FILES := $(wildcard test/tool/net/*)
TEST_TOOL_NET_SRCS = $(filter %.c,$(TEST_TOOL_NET_FILES))
TEST_TOOL_NET_SRCS_TEST = $(filter %_test.c,$(TEST_TOOL_NET_SRCS))
TEST_TOOL_NET_HDRS = $(filter %.h,$(TEST_TOOL_NET_FILES))
TEST_TOOL_NET_COMS = $(TEST_TOOL_NET_OBJS:%.o=%.com)
TEST_TOOL_NET_COMS = $(TEST_TOOL_NET_SRCS:%.c=o/$(MODE)/%.com)

TEST_TOOL_NET_OBJS =					\
	$(TEST_TOOL_NET_SRCS:%.c=o/$(MODE)/%.o)		\
	o/$(MODE)/tool/net/redbean.com.zip.o

TEST_TOOL_NET_BINS =					\
	$(TEST_TOOL_NET_COMS)				\
	$(TEST_TOOL_NET_COMS:%=%.dbg)

TEST_TOOL_NET_TESTS =					\
	$(TEST_TOOL_NET_SRCS_TEST:%.c=o/$(MODE)/%.com.ok)

TEST_TOOL_NET_CHECKS =					\
	$(TEST_TOOL_NET_HDRS:%=o/$(MODE)/%.ok)		\
	$(TEST_TOOL_NET_SRCS_TEST:%.c=o/$(MODE)/%.com.runs)

TEST_TOOL_NET_DIRECTDEPS =				\
	LIBC_CALLS					\
	LIBC_FMT					\
	LIBC_INTRIN					\
	LIBC_LOG					\
	LIBC_MEM					\
	LIBC_NEXGEN32E					\
	LIBC_RUNTIME					\
	LIBC_SOCK					\
	LIBC_STDIO					\
	LIBC_STR					\
	LIBC_STUBS					\
	LIBC_SYSV					\
	LIBC_TESTLIB					\
	LIBC_X						\
	LIBC_ZIPOS					\
	THIRD_PARTY_REGEX				\
	THIRD_PARTY_SQLITE3

TEST_TOOL_NET_DEPS :=					\
	$(call uniq,$(foreach x,$(TEST_TOOL_NET_DIRECTDEPS),$($(x))))

$(TEST_TOOL_NET_A):					\
		test/tool/net/				\
		$(TEST_TOOL_NET_A).pkg			\
		$(TEST_TOOL_NET_OBJS)

$(TEST_TOOL_NET_A).pkg:					\
		$(TEST_TOOL_NET_OBJS)			\
		$(foreach x,$(TEST_TOOL_NET_DIRECTDEPS),$($(x)_A).pkg)

o/$(MODE)/test/tool/net/%.com.dbg:			\
		$(TEST_TOOL_NET_DEPS)			\
		$(TEST_TOOL_NET_A)			\
		o/$(MODE)/test/tool/net/%.o		\
		$(TEST_TOOL_NET_A).pkg			\
		$(LIBC_TESTMAIN)			\
		$(CRT)					\
		$(APE)
	@$(APELINK)

.PHONY: o/$(MODE)/test/tool/net
o/$(MODE)/test/tool/net:				\
		$(TEST_TOOL_NET_BINS)			\
		$(TEST_TOOL_NET_CHECKS)
