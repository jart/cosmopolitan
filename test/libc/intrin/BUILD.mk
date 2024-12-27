#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#── vi: set noet ft=make ts=8 sw=8 fenc=utf-8 :vi ────────────────────┘

PKGS += TEST_LIBC_INTRIN

TEST_LIBC_INTRIN_FILES := $(wildcard test/libc/intrin/*)
TEST_LIBC_INTRIN_SRCS = $(filter %.c,$(TEST_LIBC_INTRIN_FILES))
TEST_LIBC_INTRIN_INCS = $(filter %.inc,$(TEST_LIBC_INTRIN_FILES))
TEST_LIBC_INTRIN_SRCS_TEST = $(filter %_test.c,$(TEST_LIBC_INTRIN_SRCS))

TEST_LIBC_INTRIN_OBJS =						\
	$(TEST_LIBC_INTRIN_SRCS:%.c=o/$(MODE)/%.o)

TEST_LIBC_INTRIN_COMS =						\
	$(TEST_LIBC_INTRIN_SRCS:%.c=o/$(MODE)/%)

TEST_LIBC_INTRIN_BINS =						\
	$(TEST_LIBC_INTRIN_COMS)				\
	$(TEST_LIBC_INTRIN_COMS:%=%.dbg)

TEST_LIBC_INTRIN_TESTS =					\
	$(TEST_LIBC_INTRIN_SRCS_TEST:%.c=o/$(MODE)/%.ok)

TEST_LIBC_INTRIN_CHECKS =					\
	$(TEST_LIBC_INTRIN_SRCS_TEST:%.c=o/$(MODE)/%.runs)

TEST_LIBC_INTRIN_DIRECTDEPS =					\
	LIBC_CALLS						\
	LIBC_FMT						\
	LIBC_INTRIN						\
	LIBC_LOG						\
	LIBC_MEM						\
	LIBC_NEXGEN32E						\
	LIBC_PROC						\
	LIBC_RUNTIME						\
	LIBC_STDIO						\
	LIBC_STR						\
	LIBC_SYSV						\
	LIBC_SYSV_CALLS						\
	LIBC_TESTLIB						\
	LIBC_THREAD						\
	LIBC_TINYMATH						\
	LIBC_X							\
	THIRD_PARTY_COMPILER_RT					\
	THIRD_PARTY_MUSL					\
	THIRD_PARTY_NSYNC					\
	THIRD_PARTY_OPENMP					\
	THIRD_PARTY_XED						\
	TOOL_VIZ_LIB						\

TEST_LIBC_INTRIN_DEPS :=					\
	$(call uniq,$(foreach x,$(TEST_LIBC_INTRIN_DIRECTDEPS),$($(x))))

o/$(MODE)/test/libc/intrin/intrin.pkg:				\
		$(TEST_LIBC_INTRIN_OBJS)			\
		$(foreach x,$(TEST_LIBC_INTRIN_DIRECTDEPS),$($(x)_A).pkg)

o/$(MODE)/test/libc/intrin/%.dbg:				\
		$(TEST_LIBC_INTRIN_DEPS)			\
		o/$(MODE)/test/libc/intrin/%.o			\
		o/$(MODE)/test/libc/intrin/intrin.pkg		\
		$(LIBC_TESTMAIN)				\
		$(CRT)						\
		$(APE_NO_MODIFY_SELF)
	@$(APELINK)

o/$(MODE)/test/libc/intrin/mmap_test.dbg:			\
		$(TEST_LIBC_INTRIN_DEPS)			\
		o/$(MODE)/test/libc/intrin/mmap_test.o		\
		o/$(MODE)/test/libc/intrin/intrin.pkg		\
		o/$(MODE)/test/libc/mem/prog/life.elf.zip.o	\
		$(LIBC_TESTMAIN)				\
		$(CRT)						\
		$(APE_NO_MODIFY_SELF)
	@$(APELINK)

# Test what happens when *NSYNC isn't linked.
o/$(MODE)/test/libc/intrin/lock_test.dbg:			\
		$(TEST_LIBC_INTRIN_DEPS)			\
		o/$(MODE)/test/libc/intrin/lock_test.o		\
		o/$(MODE)/test/libc/intrin/intrin.pkg		\
		$(CRT)						\
		$(APE_NO_MODIFY_SELF)
	@$(APELINK)

$(TEST_LIBC_INTRIN_OBJS): private				\
		CFLAGS +=					\
			-fno-builtin

$(TEST_LIBC_INTRIN_OBJS): private				\
		CXXFLAGS +=					\
			-fopenmp

.PHONY: o/$(MODE)/test/libc/intrin
o/$(MODE)/test/libc/intrin:					\
		$(TEST_LIBC_INTRIN_BINS)			\
		$(TEST_LIBC_INTRIN_CHECKS)
