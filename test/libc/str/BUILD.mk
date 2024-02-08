#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#── vi: set noet ft=make ts=8 sw=8 fenc=utf-8 :vi ────────────────────┘

PKGS += TEST_LIBC_STR

TEST_LIBC_STR_FILES := $(wildcard test/libc/str/*)
TEST_LIBC_STR_SRCS_C = $(filter %.c,$(TEST_LIBC_STR_FILES))
TEST_LIBC_STR_SRCS_CC = $(filter %.cc,$(TEST_LIBC_STR_FILES))
TEST_LIBC_STR_SRCS = $(TEST_LIBC_STR_SRCS_C) $(TEST_LIBC_STR_SRCS_CC)
TEST_LIBC_STR_SRCS_TEST_C = $(filter %_test.c,$(TEST_LIBC_STR_FILES))
TEST_LIBC_STR_SRCS_TEST_CC = $(filter %_test.cc,$(TEST_LIBC_STR_FILES))

TEST_LIBC_STR_OBJS =							\
	$(TEST_LIBC_STR_SRCS_C:%.c=o/$(MODE)/%.o)			\
	$(TEST_LIBC_STR_SRCS_CC:%.cc=o/$(MODE)/%.o)

TEST_LIBC_STR_COMS =							\
	$(TEST_LIBC_STR_SRCS_TEST_C:%.c=o/$(MODE)/%.com)		\
	$(TEST_LIBC_STR_SRCS_TEST_CC:%.cc=o/$(MODE)/%.com)

TEST_LIBC_STR_BINS =							\
	$(TEST_LIBC_STR_COMS)						\
	$(TEST_LIBC_STR_COMS:%=%.dbg)

TEST_LIBC_STR_TESTS =							\
	$(TEST_LIBC_STR_SRCS_TEST_C:%.c=o/$(MODE)/%.com.ok)		\
	$(TEST_LIBC_STR_SRCS_TEST_CC:%.cc=o/$(MODE)/%.com.ok)

TEST_LIBC_STR_CHECKS =							\
	$(TEST_LIBC_STR_SRCS_TEST_C:%.c=o/$(MODE)/%.com.runs)		\
	$(TEST_LIBC_STR_SRCS_TEST_CC:%.cc=o/$(MODE)/%.com.runs)

TEST_LIBC_STR_DIRECTDEPS =						\
	LIBC_CALLS							\
	LIBC_FMT							\
	LIBC_INTRIN							\
	LIBC_LOG							\
	LIBC_TINYMATH							\
	LIBC_MEM							\
	LIBC_NEXGEN32E							\
	LIBC_RUNTIME							\
	LIBC_STDIO							\
	LIBC_STR							\
	LIBC_SYSV							\
	LIBC_SYSV_CALLS							\
	LIBC_TESTLIB							\
	LIBC_X								\
	THIRD_PARTY_COMPILER_RT						\
	THIRD_PARTY_MBEDTLS						\
	THIRD_PARTY_REGEX						\
	THIRD_PARTY_ZLIB						\
	THIRD_PARTY_LIBCXX						\
	THIRD_PARTY_SMALLZ4						\
	THIRD_PARTY_VQSORT

TEST_LIBC_STR_DEPS :=							\
	$(call uniq,$(foreach x,$(TEST_LIBC_STR_DIRECTDEPS),$($(x))))

o/$(MODE)/test/libc/str/str.pkg:					\
		$(TEST_LIBC_STR_OBJS)					\
		$(foreach x,$(TEST_LIBC_STR_DIRECTDEPS),$($(x)_A).pkg)

o/$(MODE)/test/libc/str/tpenc_test.o: private				\
		CFLAGS +=						\
			$(TRADITIONAL)

o/$(MODE)/test/libc/str/%.com.dbg:					\
		$(TEST_LIBC_STR_DEPS)					\
		o/$(MODE)/test/libc/str/%.o				\
		o/$(MODE)/test/libc/str/str.pkg				\
		$(LIBC_TESTMAIN)					\
		$(CRT)							\
		$(APE_NO_MODIFY_SELF)
	@$(APELINK)

$(TEST_LIBC_STR_OBJS): private						\
		DEFAULT_CCFLAGS +=					\
			-fno-builtin

o/$(MODE)/test/libc/str/memmove_test.o: private				\
		CFLAGS +=						\
			-O2 -D_FORTIFY_SOURCE=2

.PHONY: o/$(MODE)/test/libc/str
o/$(MODE)/test/libc/str:						\
		$(TEST_LIBC_STR_BINS)					\
		$(TEST_LIBC_STR_CHECKS)
