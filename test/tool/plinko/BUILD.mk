#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#── vi: set noet ft=make ts=8 sw=8 fenc=utf-8 :vi ────────────────────┘

ifeq ($(ARCH), x86_64)

PKGS += TEST_TOOL_PLINKO

TEST_TOOL_PLINKO = $(TOOL_PLINKO_A_DEPS) $(TOOL_PLINKO_A)
TEST_TOOL_PLINKO_A = o/$(MODE)/test/tool/plinko/plinkolib.a
TEST_TOOL_PLINKO_FILES := $(wildcard test/tool/plinko/*)
TEST_TOOL_PLINKO_SRCS = $(filter %.c,$(TEST_TOOL_PLINKO_FILES))
TEST_TOOL_PLINKO_SRCS_TEST = $(filter %_test.c,$(TEST_TOOL_PLINKO_SRCS))
TEST_TOOL_PLINKO_HDRS = $(filter %.h,$(TEST_TOOL_PLINKO_FILES))
TEST_TOOL_PLINKO_COMS = $(TEST_TOOL_PLINKO_OBJS:%.o=%.com)

TEST_TOOL_PLINKO_OBJS =						\
	$(TEST_TOOL_PLINKO_SRCS:%.c=o/$(MODE)/%.o)		\
	o/$(MODE)/tool/plinko/plinko.com.zip.o			\
	o/$(MODE)/tool/plinko/lib/library.lisp.zip.o		\
	o/$(MODE)/tool/plinko/lib/binarytrees.lisp.zip.o	\
	o/$(MODE)/tool/plinko/lib/algebra.lisp.zip.o		\
	o/$(MODE)/tool/plinko/lib/infix.lisp.zip.o		\
	o/$(MODE)/tool/plinko/lib/ok.lisp.zip.o			\
	o/$(MODE)/test/tool/plinko/library_test.lisp.zip.o	\
	o/$(MODE)/test/tool/plinko/algebra_test.lisp.zip.o

TEST_TOOL_PLINKO_COMS =						\
	$(TEST_TOOL_PLINKO_SRCS:%.c=o/$(MODE)/%.com)

TEST_TOOL_PLINKO_BINS =						\
	$(TEST_TOOL_PLINKO_COMS)				\
	$(TEST_TOOL_PLINKO_COMS:%=%.dbg)

TEST_TOOL_PLINKO_TESTS =					\
	$(TEST_TOOL_PLINKO_SRCS_TEST:%.c=o/$(MODE)/%.com.ok)

TEST_TOOL_PLINKO_CHECKS =					\
	$(TEST_TOOL_PLINKO_HDRS:%=o/$(MODE)/%.ok)		\
	$(TEST_TOOL_PLINKO_SRCS_TEST:%.c=o/$(MODE)/%.com.runs)

TEST_TOOL_PLINKO_DIRECTDEPS =					\
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
	LIBC_TESTLIB						\
	LIBC_X							\
	THIRD_PARTY_COMPILER_RT					\
	THIRD_PARTY_XED

TEST_TOOL_PLINKO_DEPS :=					\
	$(call uniq,$(foreach x,$(TEST_TOOL_PLINKO_DIRECTDEPS),$($(x))))

$(TEST_TOOL_PLINKO_A):						\
		test/tool/plinko/				\
		$(TEST_TOOL_PLINKO_A).pkg			\
		$(TEST_TOOL_PLINKO_OBJS)

$(TEST_TOOL_PLINKO_A).pkg:					\
		$(TEST_TOOL_PLINKO_OBJS)			\
		$(foreach x,$(TEST_TOOL_PLINKO_DIRECTDEPS),$($(x)_A).pkg)

o/$(MODE)/test/tool/plinko/%.com.dbg:				\
		$(TEST_TOOL_PLINKO_DEPS)			\
		$(TEST_TOOL_PLINKO_A)				\
		o/$(MODE)/test/tool/plinko/%.o			\
		$(TEST_TOOL_PLINKO_A).pkg			\
		$(LIBC_TESTMAIN)				\
		$(CRT)						\
		$(APE_NO_MODIFY_SELF)
	@$(APELINK)

o/$(MODE)/test/tool/plinko/plinko_test.com.runs: private	\
		QUOTA =	-M100g

o/$(MODE)/test/tool/plinko/algebra_test.lisp.zip.o: private ZIPOBJ_FLAGS += -B
o/$(MODE)/test/tool/plinko/library_test.lisp.zip.o: private ZIPOBJ_FLAGS += -B

.PHONY: o/$(MODE)/test/tool/plinko
o/$(MODE)/test/tool/plinko:					\
		$(TEST_TOOL_PLINKO_BINS)			\
		$(TEST_TOOL_PLINKO_CHECKS)

else
.PHONY: o/$(MODE)/test/tool/plinko
o/$(MODE)/test/tool/plinko:
endif
