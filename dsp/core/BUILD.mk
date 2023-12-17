#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#── vi: set noet ft=make ts=8 sw=8 fenc=utf-8 :vi ────────────────────┘

PKGS += DSP_CORE

DSP_CORE_ARTIFACTS += DSP_CORE_A
DSP_CORE = $(DSP_CORE_A_DEPS) $(DSP_CORE_A)
DSP_CORE_A = o/$(MODE)/dsp/core/core.a
DSP_CORE_A_FILES := $(wildcard dsp/core/*)
DSP_CORE_A_HDRS = $(filter %.h,$(DSP_CORE_A_FILES))
DSP_CORE_A_SRCS = $(filter %.c,$(DSP_CORE_A_FILES))
DSP_CORE_A_OBJS = $(DSP_CORE_A_SRCS:%.c=o/$(MODE)/%.o)

DSP_CORE_A_CHECKS =				\
	$(DSP_CORE_A).pkg			\
	$(DSP_CORE_A_HDRS:%=o/$(MODE)/%.ok)

DSP_CORE_A_DIRECTDEPS =				\
	LIBC_INTRIN				\
	LIBC_MEM				\
	LIBC_NEXGEN32E				\
	LIBC_STR				\
	LIBC_TINYMATH

DSP_CORE_A_DEPS :=				\
	$(call uniq,$(foreach x,$(DSP_CORE_A_DIRECTDEPS),$($(x))))

$(DSP_CORE_A):	dsp/core/			\
		$(DSP_CORE_A).pkg		\
		$(DSP_CORE_A_OBJS)

$(DSP_CORE_A).pkg:				\
		$(DSP_CORE_A_OBJS)		\
		$(foreach x,$(DSP_CORE_A_DIRECTDEPS),$($(x)_A).pkg)

o/$(MODE)/dsp/core/dct.o			\
o/$(MODE)/dsp/core/c1331.o			\
o/$(MODE)/dsp/core/magikarp.o			\
o/$(MODE)/dsp/core/c93654369.o			\
o/$(MODE)/dsp/core/float2short.o		\
o/$(MODE)/dsp/core/scalevolume.o: private	\
		CFLAGS +=			\
			$(MATHEMATICAL)

o/tiny/dsp/core/scalevolume.o: private		\
		CFLAGS +=			\
			-Os

o/$(MODE)/dsp/core/det3.o: private		\
		CFLAGS +=			\
			-ffast-math

DSP_CORE_LIBS = $(foreach x,$(DSP_CORE_ARTIFACTS),$($(x)))
DSP_CORE_SRCS = $(foreach x,$(DSP_CORE_ARTIFACTS),$($(x)_SRCS))
DSP_CORE_HDRS = $(foreach x,$(DSP_CORE_ARTIFACTS),$($(x)_HDRS))
DSP_CORE_CHECKS = $(foreach x,$(DSP_CORE_ARTIFACTS),$($(x)_CHECKS))
DSP_CORE_OBJS = $(foreach x,$(DSP_CORE_ARTIFACTS),$($(x)_OBJS))
$(DSP_CORE_OBJS): $(BUILD_FILES) dsp/core/BUILD.mk

.PHONY: o/$(MODE)/dsp/core
o/$(MODE)/dsp/core: $(DSP_CORE_CHECKS)
