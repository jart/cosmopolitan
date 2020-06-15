#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#───vi: set et ft=make ts=8 tw=8 fenc=utf-8 :vi───────────────────────┘

PKGS += DSP_MPEG

DSP_MPEG_ARTIFACTS += DSP_MPEG_A
DSP_MPEG = $(DSP_MPEG_A_DEPS) $(DSP_MPEG_A)
DSP_MPEG_A = o/$(MODE)/dsp/mpeg/mpeg.a
DSP_MPEG_A_FILES := $(wildcard dsp/mpeg/*)
DSP_MPEG_A_HDRS = $(filter %.h,$(DSP_MPEG_A_FILES))
DSP_MPEG_A_SRCS_S = $(filter %.S,$(DSP_MPEG_A_FILES))
DSP_MPEG_A_SRCS_C = $(filter %.c,$(DSP_MPEG_A_FILES))

DSP_MPEG_A_SRCS =				\
	$(DSP_MPEG_A_SRCS_S)			\
	$(DSP_MPEG_A_SRCS_C)

DSP_MPEG_A_OBJS =				\
	$(DSP_MPEG_A_SRCS:%=o/$(MODE)/%.zip.o)	\
	$(DSP_MPEG_A_SRCS_S:%.S=o/$(MODE)/%.o)	\
	$(DSP_MPEG_A_SRCS_C:%.c=o/$(MODE)/%.o)

DSP_MPEG_A_CHECKS =				\
	$(DSP_MPEG_A).pkg			\
	$(DSP_MPEG_A_HDRS:%=o/$(MODE)/%.ok)

DSP_MPEG_A_DIRECTDEPS =				\
	LIBC_CALLS				\
	LIBC_LOG				\
	LIBC_RUNTIME				\
	LIBC_TINYMATH				\
	LIBC_TIME				\
	LIBC_STUBS				\
	LIBC_STR				\
	LIBC_NEXGEN32E				\
	LIBC_STDIO				\
	LIBC_SYSV				\
	LIBC_MEM				\
	LIBC_LOG				\
	LIBC_FMT				\
	LIBC_UNICODE				\

DSP_MPEG_A_DEPS :=				\
	$(call uniq,$(foreach x,$(DSP_MPEG_A_DIRECTDEPS),$($(x))))

$(DSP_MPEG_A):	dsp/mpeg/			\
		$(DSP_MPEG_A).pkg		\
		$(DSP_MPEG_A_OBJS)

$(DSP_MPEG_A).pkg:				\
		$(DSP_MPEG_A_OBJS)		\
		$(foreach x,$(DSP_MPEG_A_DIRECTDEPS),$($(x)_A).pkg)

o/$(MODE)/dsp/mpeg/clamp4int256-k8.o:		\
		OVERRIDE_CFLAGS +=		\
			-Os

#o/$(MODE)/dsp/mpeg/macroblock.o:		\
		CC = $(CLANG)

DSP_MPEG_LIBS = $(foreach x,$(DSP_MPEG_ARTIFACTS),$($(x)))
DSP_MPEG_SRCS = $(foreach x,$(DSP_MPEG_ARTIFACTS),$($(x)_SRCS))
DSP_MPEG_HDRS = $(foreach x,$(DSP_MPEG_ARTIFACTS),$($(x)_HDRS))
DSP_MPEG_CHECKS = $(foreach x,$(DSP_MPEG_ARTIFACTS),$($(x)_CHECKS))
DSP_MPEG_OBJS = $(foreach x,$(DSP_MPEG_ARTIFACTS),$($(x)_OBJS))
$(DSP_MPEG_OBJS): $(BUILD_FILES) dsp/mpeg/mpeg.mk

.PHONY: o/$(MODE)/dsp/mpeg
o/$(MODE)/dsp/mpeg: $(DSP_MPEG_CHECKS)
