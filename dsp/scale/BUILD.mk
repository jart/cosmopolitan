#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#── vi: set noet ft=make ts=8 sw=8 fenc=utf-8 :vi ────────────────────┘

PKGS += DSP_SCALE

DSP_SCALE_ARTIFACTS += DSP_SCALE_A
DSP_SCALE = $(DSP_SCALE_A_DEPS) $(DSP_SCALE_A)
DSP_SCALE_A = o/$(MODE)/dsp/scale/scale.a
DSP_SCALE_A_FILES := $(wildcard dsp/scale/*)
DSP_SCALE_A_HDRS = $(filter %.h,$(DSP_SCALE_A_FILES))
DSP_SCALE_A_SRCS_S = $(filter %.S,$(DSP_SCALE_A_FILES))
DSP_SCALE_A_SRCS_C = $(filter %.c,$(DSP_SCALE_A_FILES))

DSP_SCALE_A_SRCS =				\
	$(DSP_SCALE_A_SRCS_S)			\
	$(DSP_SCALE_A_SRCS_C)

DSP_SCALE_A_OBJS =				\
	$(DSP_SCALE_A_SRCS_S:%.S=o/$(MODE)/%.o)	\
	$(DSP_SCALE_A_SRCS_C:%.c=o/$(MODE)/%.o)

DSP_SCALE_A_CHECKS =				\
	$(DSP_SCALE_A).pkg			\
	$(DSP_SCALE_A_HDRS:%=o/$(MODE)/%.ok)

DSP_SCALE_A_DIRECTDEPS =			\
	DSP_CORE				\
	LIBC_INTRIN				\
	LIBC_LOG				\
	LIBC_MEM				\
	LIBC_NEXGEN32E				\
	LIBC_RUNTIME				\
	LIBC_STR				\
	LIBC_TIME				\
	LIBC_TINYMATH				\
	LIBC_X

DSP_SCALE_A_DEPS :=				\
	$(call uniq,$(foreach x,$(DSP_SCALE_A_DIRECTDEPS),$($(x))))

$(DSP_SCALE_A):	dsp/scale/			\
		$(DSP_SCALE_A).pkg		\
		$(DSP_SCALE_A_OBJS)

$(DSP_SCALE_A).pkg:				\
		$(DSP_SCALE_A_OBJS)		\
		$(foreach x,$(DSP_SCALE_A_DIRECTDEPS),$($(x)_A).pkg)

o/$(MODE)/dsp/scale/cdecimate2xuint8x8.o	\
o/$(MODE)/dsp/scale/gyarados.o			\
o/$(MODE)/dsp/scale/magikarp.o			\
o/$(MODE)/dsp/scale/scale.o: private		\
		CFLAGS +=			\
			$(MATHEMATICAL)

DSP_SCALE_LIBS = $(foreach x,$(DSP_SCALE_ARTIFACTS),$($(x)))
DSP_SCALE_SRCS = $(foreach x,$(DSP_SCALE_ARTIFACTS),$($(x)_SRCS))
DSP_SCALE_HDRS = $(foreach x,$(DSP_SCALE_ARTIFACTS),$($(x)_HDRS))
DSP_SCALE_CHECKS = $(foreach x,$(DSP_SCALE_ARTIFACTS),$($(x)_CHECKS))
DSP_SCALE_OBJS = $(foreach x,$(DSP_SCALE_ARTIFACTS),$($(x)_OBJS))
$(DSP_SCALE_OBJS): $(BUILD_FILES) dsp/scale/BUILD.mk

.PHONY: o/$(MODE)/dsp/scale
o/$(MODE)/dsp/scale: $(DSP_SCALE_CHECKS)
