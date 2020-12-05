#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#───vi: set et ft=make ts=8 tw=8 fenc=utf-8 :vi───────────────────────┘

PKGS += TEST_LIBC_STR

TEST_LIBC_STR_SRCS := $(wildcard test/libc/str/*.c)
TEST_LIBC_STR_SRCS_TEST = $(filter %_test.c,$(TEST_LIBC_STR_SRCS))

TEST_LIBC_STR_OBJS =					\
	$(TEST_LIBC_STR_SRCS:%=o/$(MODE)/%.zip.o)	\
	$(TEST_LIBC_STR_SRCS:%.c=o/$(MODE)/%.o)

TEST_LIBC_STR_COMS =					\
	$(TEST_LIBC_STR_SRCS:%.c=o/$(MODE)/%.com)

TEST_LIBC_STR_BINS =					\
	$(TEST_LIBC_STR_COMS)				\
	$(TEST_LIBC_STR_COMS:%=%.dbg)

TEST_LIBC_STR_TESTS =					\
	$(TEST_LIBC_STR_SRCS_TEST:%.c=o/$(MODE)/%.com.ok)

TEST_LIBC_STR_CHECKS =					\
	$(TEST_LIBC_STR_SRCS_TEST:%.c=o/$(MODE)/%.com.runs)

TEST_LIBC_STR_DIRECTDEPS =				\
	LIBC_CALLS_HEFTY				\
	LIBC_ALG					\
	LIBC_FMT					\
	LIBC_NEXGEN32E					\
	LIBC_STDIO					\
	LIBC_MEM					\
	LIBC_RUNTIME					\
	LIBC_STR					\
	LIBC_RAND					\
	LIBC_UNICODE					\
	LIBC_CALLS					\
	LIBC_STUBS					\
	LIBC_SYSV					\
	LIBC_TESTLIB					\
	LIBC_LOG					\
	LIBC_X						\
	LIBC_ZIPOS					\
	THIRD_PARTY_REGEX				\
	THIRD_PARTY_ZLIB

TEST_LIBC_STR_DEPS :=					\
	$(call uniq,$(foreach x,$(TEST_LIBC_STR_DIRECTDEPS),$($(x))))

o/$(MODE)/test/libc/str/str.pkg:			\
		$(TEST_LIBC_STR_OBJS)			\
		$(foreach x,$(TEST_LIBC_STR_DIRECTDEPS),$($(x)_A).pkg)

o/$(MODE)/test/libc/str/tpenc_test.o:			\
		OVERRIDE_CFLAGS +=			\
			$(TRADITIONAL)

o/$(MODE)/test/libc/str/%.com.dbg:			\
		$(TEST_LIBC_STR_DEPS)			\
		o/$(MODE)/test/libc/str/%.o		\
		o/$(MODE)/test/libc/str/str.pkg		\
		$(LIBC_TESTMAIN)			\
		$(CRT)					\
		$(APE)
	@$(APELINK)

$(TEST_LIBC_STR_OBJS):					\
	DEFAULT_CCFLAGS +=				\
		-fno-builtin

.PHONY: o/$(MODE)/test/libc/str
o/$(MODE)/test/libc/str:				\
		$(TEST_LIBC_STR_BINS)			\
		$(TEST_LIBC_STR_CHECKS)
