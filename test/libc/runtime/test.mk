#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#───vi: set et ft=make ts=8 tw=8 fenc=utf-8 :vi───────────────────────┘

PKGS += TEST_LIBC_RUNTIME

TEST_LIBC_RUNTIME_SRCS := $(wildcard test/libc/runtime/*.c)
TEST_LIBC_RUNTIME_SRCS_TEST = $(filter %_test.c,$(TEST_LIBC_RUNTIME_SRCS))

TEST_LIBC_RUNTIME_OBJS =					\
	$(TEST_LIBC_RUNTIME_SRCS:%.c=o/$(MODE)/%.o)

TEST_LIBC_RUNTIME_COMS =					\
	$(TEST_LIBC_RUNTIME_SRCS:%.c=o/$(MODE)/%.com)

TEST_LIBC_RUNTIME_BINS =					\
	$(TEST_LIBC_RUNTIME_COMS)				\
	$(TEST_LIBC_RUNTIME_COMS:%=%.dbg)

TEST_LIBC_RUNTIME_TESTS =					\
	$(TEST_LIBC_RUNTIME_SRCS_TEST:%.c=o/$(MODE)/%.com.ok)

TEST_LIBC_RUNTIME_CHECKS =					\
	$(TEST_LIBC_RUNTIME_SRCS_TEST:%.c=o/$(MODE)/%.com.runs)

TEST_LIBC_RUNTIME_DIRECTDEPS =					\
	LIBC_CALLS						\
	LIBC_FMT						\
	LIBC_INTRIN						\
	LIBC_LOG						\
	LIBC_MEM						\
	LIBC_NEXGEN32E						\
	LIBC_RUNTIME						\
	LIBC_STDIO						\
	LIBC_STR						\
	LIBC_STUBS						\
	LIBC_SYSV						\
	LIBC_THREAD						\
	LIBC_TESTLIB						\
	LIBC_TINYMATH						\
	LIBC_X							\
	LIBC_ZIPOS						\
	TOOL_BUILD_LIB						\
	THIRD_PARTY_XED

TEST_LIBC_RUNTIME_DEPS :=					\
	$(call uniq,$(foreach x,$(TEST_LIBC_RUNTIME_DIRECTDEPS),$($(x))))

o/$(MODE)/test/libc/runtime/runtime.pkg:			\
		$(TEST_LIBC_RUNTIME_OBJS)			\
		$(foreach x,$(TEST_LIBC_RUNTIME_DIRECTDEPS),$($(x)_A).pkg)

o/$(MODE)/test/libc/runtime/%.com.dbg:				\
		$(TEST_LIBC_RUNTIME_DEPS)			\
		o/$(MODE)/test/libc/mem/prog/life.elf.zip.o			\
		o/$(MODE)/test/libc/runtime/%.o			\
		o/$(MODE)/test/libc/runtime/runtime.pkg		\
		$(LIBC_TESTMAIN)				\
		$(CRT)						\
		$(APE_NO_MODIFY_SELF)
	@$(APELINK)

o/$(MODE)/test/libc/runtime/ape_test.com.dbg:			\
		$(TEST_LIBC_RUNTIME_DEPS)			\
		o/$(MODE)/test/libc/runtime/ape_test.o		\
		o/$(MODE)/test/libc/runtime/runtime.pkg		\
		$(LIBC_TESTMAIN)				\
		$(CRT)						\
		$(APE_NO_MODIFY_SELF)
	@$(APELINK)

$(TEST_LIBC_RUNTIME_OBJS): private				\
	DEFAULT_CCFLAGS +=					\
		-fno-builtin

o/$(MODE)/test/libc/runtime/getenv_test.com.runs:		\
		o/$(MODE)/test/libc/runtime/getenv_test.com
	@HELLO=THERE build/runit $@ $<

o/$(MODE)/test/libc/runtime/itsatrap_test.o: private		\
		OVERRIDE_CFLAGS +=				\
			-fno-sanitize=all			\
			-ftrapv

.PHONY: o/$(MODE)/test/libc/runtime
o/$(MODE)/test/libc/runtime:					\
		$(TEST_LIBC_RUNTIME_BINS)			\
		$(TEST_LIBC_RUNTIME_CHECKS)
