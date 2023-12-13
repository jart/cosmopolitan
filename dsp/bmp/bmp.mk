#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#── vi: set et ft=make ts=8 sw=8 fenc=utf-8 :vi ──────────────────────┘

PKGS += DSP_BMP

DSP_BMP_ARTIFACTS += DSP_BMP_A
DSP_BMP = $(DSP_BMP_A_DEPS) $(DSP_BMP_A)
DSP_BMP_A = o/$(MODE)/dsp/bmp/bmp.a
DSP_BMP_A_FILES := $(wildcard dsp/bmp/*)
DSP_BMP_A_HDRS = $(filter %.h,$(DSP_BMP_A_FILES))
DSP_BMP_A_SRCS_S = $(filter %.S,$(DSP_BMP_A_FILES))
DSP_BMP_A_SRCS_C = $(filter %.c,$(DSP_BMP_A_FILES))

DSP_BMP_A_SRCS =				\
	$(DSP_BMP_A_SRCS_S)			\
	$(DSP_BMP_A_SRCS_C)

DSP_BMP_A_OBJS =				\
	$(DSP_BMP_A_SRCS_S:%.S=o/$(MODE)/%.o)	\
	$(DSP_BMP_A_SRCS_C:%.c=o/$(MODE)/%.o)

DSP_BMP_A_CHECKS =				\
	$(DSP_BMP_A).pkg			\
	$(DSP_BMP_A_HDRS:%=o/$(MODE)/%.ok)

DSP_BMP_A_DIRECTDEPS =				\
	LIBC_NEXGEN32E				\
	LIBC_TINYMATH

DSP_BMP_A_DEPS :=				\
	$(call uniq,$(foreach x,$(DSP_BMP_A_DIRECTDEPS),$($(x))))

$(DSP_BMP_A):	dsp/bmp/			\
		$(DSP_BMP_A).pkg		\
		$(DSP_BMP_A_OBJS)

$(DSP_BMP_A).pkg:				\
		$(DSP_BMP_A_OBJS)		\
		$(foreach x,$(DSP_BMP_A_DIRECTDEPS),$($(x)_A).pkg)

o/$(MODE)/dsp/bmp/float2short.o			\
o/$(MODE)/dsp/bmp/scalevolume.o: private	\
		CFLAGS +=			\
			$(MATHEMATICAL)

o/tiny/dsp/bmp/scalevolume.o: private		\
		CFLAGS +=			\
			-Os

DSP_BMP_LIBS = $(foreach x,$(DSP_BMP_ARTIFACTS),$($(x)))
DSP_BMP_SRCS = $(foreach x,$(DSP_BMP_ARTIFACTS),$($(x)_SRCS))
DSP_BMP_HDRS = $(foreach x,$(DSP_BMP_ARTIFACTS),$($(x)_HDRS))
DSP_BMP_CHECKS = $(foreach x,$(DSP_BMP_ARTIFACTS),$($(x)_CHECKS))
DSP_BMP_OBJS = $(foreach x,$(DSP_BMP_ARTIFACTS),$($(x)_OBJS))
$(DSP_BMP_OBJS): $(BUILD_FILES) dsp/bmp/bmp.mk

.PHONY: o/$(MODE)/dsp/bmp
o/$(MODE)/dsp/bmp: $(DSP_BMP_CHECKS)
