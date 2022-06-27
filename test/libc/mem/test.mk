#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#───vi: set et ft=make ts=8 tw=8 fenc=utf-8 :vi───────────────────────┘

PKGS += TEST_LIBC_MEM

TEST_LIBC_MEM_FILES := $(wildcard test/libc/mem/*)
TEST_LIBC_MEM_SRCS = $(TEST_LIBC_MEM_SRCS_C) $(TEST_LIBC_MEM_SRCS_CC)
TEST_LIBC_MEM_SRCS_C = $(filter %_test.c,$(TEST_LIBC_MEM_FILES))
TEST_LIBC_MEM_SRCS_CC = $(filter %_test.cc,$(TEST_LIBC_MEM_FILES))

TEST_LIBC_MEM_OBJS =						\
	$(TEST_LIBC_MEM_SRCS_C:%.c=o/$(MODE)/%.o)		\
	$(TEST_LIBC_MEM_SRCS_CC:%.cc=o/$(MODE)/%.o)

TEST_LIBC_MEM_COMS =						\
	$(TEST_LIBC_MEM_SRCS_C:%.c=o/$(MODE)/%.com)		\
	$(TEST_LIBC_MEM_SRCS_CC:%.cc=o/$(MODE)/%.com)

TEST_LIBC_MEM_BINS =						\
	$(TEST_LIBC_MEM_COMS)					\
	$(TEST_LIBC_MEM_COMS:%=%.dbg)

TEST_LIBC_MEM_TESTS =						\
	$(TEST_LIBC_MEM_SRCS_C:%.c=o/$(MODE)/%.com.ok)		\
	$(TEST_LIBC_MEM_SRCS_CC:%.cc=o/$(MODE)/%.com.ok)

TEST_LIBC_MEM_CHECKS =						\
	$(TEST_LIBC_MEM_SRCS_C:%.c=o/$(MODE)/%.com.runs)	\
	$(TEST_LIBC_MEM_SRCS_CC:%.cc=o/$(MODE)/%.com.runs)

TEST_LIBC_MEM_DIRECTDEPS =					\
	LIBC_CALLS						\
	LIBC_FMT						\
	LIBC_INTRIN						\
	LIBC_LOG						\
	LIBC_MEM						\
	LIBC_NEXGEN32E						\
	LIBC_RAND						\
	LIBC_RUNTIME						\
	LIBC_SOCK						\
	LIBC_STDIO						\
	LIBC_STR						\
	LIBC_STUBS						\
	LIBC_SYSV						\
	LIBC_TESTLIB						\
	THIRD_PARTY_DLMALLOC					\
	THIRD_PARTY_LIBCXX

TEST_LIBC_MEM_DEPS :=						\
	$(call uniq,$(foreach x,$(TEST_LIBC_MEM_DIRECTDEPS),$($(x))))

o/$(MODE)/test/libc/mem/mem.pkg:				\
		$(TEST_LIBC_MEM_OBJS)				\
		$(foreach x,$(TEST_LIBC_MEM_DIRECTDEPS),$($(x)_A).pkg)

o/$(MODE)/test/libc/mem/%.com.dbg:				\
		$(TEST_LIBC_MEM_DEPS)				\
		o/$(MODE)/test/libc/mem/%.o			\
		o/$(MODE)/test/libc/mem/mem.pkg			\
		$(LIBC_TESTMAIN)				\
		$(CRT)						\
		$(APE_NO_MODIFY_SELF)
	@$(APELINK)

$(TEST_LIBC_MEM_OBJS):						\
		DEFAULT_CCFLAGS +=				\
			-fno-builtin

.PHONY: o/$(MODE)/test/libc/mem
o/$(MODE)/test/libc/mem:					\
		$(TEST_LIBC_MEM_BINS)				\
		$(TEST_LIBC_MEM_CHECKS)
