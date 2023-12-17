#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#── vi: set noet ft=make ts=8 sw=8 fenc=utf-8 :vi ────────────────────┘

PKGS += TEST_DSP_SCALE

TEST_DSP_SCALE_SRCS := $(wildcard test/dsp/scale/*.c)
TEST_DSP_SCALE_SRCS_TEST = $(filter %_test.c,$(TEST_DSP_SCALE_SRCS))
TEST_DSP_SCALE_BINS = $(TEST_DSP_SCALE_COMS) $(TEST_DSP_SCALE_COMS:%=%.dbg)

TEST_DSP_SCALE_OBJS =					\
	$(TEST_DSP_SCALE_SRCS:%.c=o/$(MODE)/%.o)

TEST_DSP_SCALE_COMS =					\
	$(TEST_DSP_SCALE_SRCS:%.c=o/$(MODE)/%.com)

TEST_DSP_SCALE_TESTS =					\
	$(TEST_DSP_SCALE_SRCS_TEST:%.c=o/$(MODE)/%.com.ok)

TEST_DSP_SCALE_CHECKS =					\
	$(TEST_DSP_SCALE_SRCS_TEST:%.c=o/$(MODE)/%.com.runs)

TEST_DSP_SCALE_DIRECTDEPS =				\
	DSP_CORE					\
	DSP_SCALE					\
	LIBC_FMT					\
	LIBC_INTRIN					\
	LIBC_LOG					\
	LIBC_MEM					\
	LIBC_NEXGEN32E					\
	LIBC_RUNTIME					\
	LIBC_STDIO					\
	LIBC_STR					\
	LIBC_TINYMATH					\
	LIBC_X						\
	TOOL_VIZ_LIB					\
	LIBC_TESTLIB

TEST_DSP_SCALE_DEPS :=					\
	$(call uniq,$(foreach x,$(TEST_DSP_SCALE_DIRECTDEPS),$($(x))))

o/$(MODE)/test/dsp/scale/scale.pkg:			\
		$(TEST_DSP_SCALE_OBJS)			\
		$(foreach x,$(TEST_DSP_SCALE_DIRECTDEPS),$($(x)_A).pkg)

o/$(MODE)/test/dsp/scale/%.com.dbg:			\
		$(TEST_DSP_SCALE_DEPS)			\
		o/$(MODE)/test/dsp/scale/%.o		\
		o/$(MODE)/test/dsp/scale/scale.pkg	\
		$(LIBC_TESTMAIN)			\
		$(CRT)					\
		$(APE_NO_MODIFY_SELF)
	@$(APELINK)

.PHONY: o/$(MODE)/test/dsp/scale
o/$(MODE)/test/dsp/scale:				\
		$(TEST_DSP_SCALE_BINS)			\
		$(TEST_DSP_SCALE_CHECKS)
