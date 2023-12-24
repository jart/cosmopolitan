#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#── vi: set noet ft=make ts=8 sw=8 fenc=utf-8 :vi ────────────────────┘

PKGS += TEST_DSP_TTY

TEST_DSP_TTY_SRCS := $(wildcard test/dsp/tty/*.c)
TEST_DSP_TTY_SRCS_TEST = $(filter %_test.c,$(TEST_DSP_TTY_SRCS))
TEST_DSP_TTY_BINS = $(TEST_DSP_TTY_COMS) $(TEST_DSP_TTY_COMS:%=%.dbg)

TEST_DSP_TTY_OBJS =					\
	$(TEST_DSP_TTY_SRCS:%.c=o/$(MODE)/%.o)

TEST_DSP_TTY_COMS =					\
	$(TEST_DSP_TTY_SRCS:%.c=o/$(MODE)/%.com)

TEST_DSP_TTY_TESTS =					\
	$(TEST_DSP_TTY_SRCS_TEST:%.c=o/$(MODE)/%.com.ok)

TEST_DSP_TTY_CHECKS =					\
	$(TEST_DSP_TTY_SRCS_TEST:%.c=o/$(MODE)/%.com.runs)

TEST_DSP_TTY_DIRECTDEPS =				\
	DSP_TTY						\
	LIBC_INTRIN					\
	LIBC_LOG					\
	LIBC_MEM					\
	LIBC_NEXGEN32E					\
	LIBC_RUNTIME					\
	LIBC_STDIO					\
	LIBC_STR					\
	LIBC_TINYMATH					\
	LIBC_TESTLIB

TEST_DSP_TTY_DEPS :=					\
	$(call uniq,$(foreach x,$(TEST_DSP_TTY_DIRECTDEPS),$($(x))))

o/$(MODE)/test/dsp/tty/tty.pkg:				\
		$(TEST_DSP_TTY_OBJS)			\
		$(foreach x,$(TEST_DSP_TTY_DIRECTDEPS),$($(x)_A).pkg)

o/$(MODE)/test/dsp/tty/%.com.dbg:			\
		$(TEST_DSP_TTY_DEPS)			\
		o/$(MODE)/test/dsp/tty/%.o		\
		o/$(MODE)/test/dsp/tty/tty.pkg		\
		$(LIBC_TESTMAIN)			\
		$(CRT)					\
		$(APE_NO_MODIFY_SELF)
	@$(APELINK)

.PHONY: o/$(MODE)/test/dsp/tty
o/$(MODE)/test/dsp/tty:					\
		$(TEST_DSP_TTY_BINS)			\
		$(TEST_DSP_TTY_CHECKS)
