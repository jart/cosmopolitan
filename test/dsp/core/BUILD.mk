#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#── vi: set noet ft=make ts=8 sw=8 fenc=utf-8 :vi ────────────────────┘

PKGS += TEST_DSP_CORE

TEST_DSP_CORE_SRCS := $(wildcard test/dsp/core/*.c)
TEST_DSP_CORE_SRCS_TEST = $(filter %_test.c,$(TEST_DSP_CORE_SRCS))
TEST_DSP_CORE_BINS = $(TEST_DSP_CORE_COMS) $(TEST_DSP_CORE_COMS:%=%.dbg)

TEST_DSP_CORE_OBJS =						\
	$(TEST_DSP_CORE_SRCS:%.c=o/$(MODE)/%.o)

TEST_DSP_CORE_COMS =						\
	$(TEST_DSP_CORE_SRCS:%.c=o/$(MODE)/%.com)

TEST_DSP_CORE_TESTS =						\
	$(TEST_DSP_CORE_SRCS_TEST:%.c=o/$(MODE)/%.com.ok)

TEST_DSP_CORE_CHECKS =						\
	$(TEST_DSP_CORE_SRCS_TEST:%.c=o/$(MODE)/%.com.runs)

TEST_DSP_CORE_DIRECTDEPS =					\
	DSP_CORE						\
	DSP_MPEG						\
	LIBC_TINYMATH						\
	LIBC_LOG						\
	LIBC_RUNTIME						\
	LIBC_TESTLIB						\
	TOOL_VIZ_LIB						\
	THIRD_PARTY_BLAS					\
	THIRD_PARTY_COMPILER_RT

TEST_DSP_CORE_DEPS :=						\
	$(call uniq,$(foreach x,$(TEST_DSP_CORE_DIRECTDEPS),$($(x))))

o/$(MODE)/test/dsp/core/core.pkg:				\
		$(TEST_DSP_CORE_OBJS)				\
		$(foreach x,$(TEST_DSP_CORE_DIRECTDEPS),$($(x)_A).pkg)

o/$(MODE)/test/dsp/core/%.com.dbg:				\
		$(TEST_DSP_CORE_DEPS)				\
		o/$(MODE)/test/dsp/core/%.o			\
		$(LIBC_TESTMAIN)				\
		$(CRT)						\
		$(APE_NO_MODIFY_SELF)
	@$(APELINK)

.PHONY: o/$(MODE)/test/dsp/core
o/$(MODE)/test/dsp/core:					\
		$(TEST_DSP_CORE_BINS)				\
		$(TEST_DSP_CORE_CHECKS)
