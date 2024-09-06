#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#── vi: set noet ft=make ts=8 sw=8 fenc=utf-8 :vi ────────────────────┘

PKGS += DSP_AUDIO

DSP_AUDIO_ARTIFACTS += DSP_AUDIO_A
DSP_AUDIO = $(DSP_AUDIO_A_DEPS) $(DSP_AUDIO_A)
DSP_AUDIO_A = o/$(MODE)/dsp/audio/audio.a
DSP_AUDIO_A_FILES := $(wildcard dsp/audio/*)
DSP_AUDIO_A_HDRS = $(filter %.h,$(DSP_AUDIO_A_FILES)) dsp/audio/cosmoaudio/cosmoaudio.h
DSP_AUDIO_A_SRCS = $(filter %.c,$(DSP_AUDIO_A_FILES))

DSP_AUDIO_A_DATA =				\
	dsp/audio/cosmoaudio/miniaudio.h	\
	dsp/audio/cosmoaudio/cosmoaudio.c	\
	dsp/audio/cosmoaudio/cosmoaudio.h	\
	dsp/audio/cosmoaudio/cosmoaudio.dll	\

DSP_AUDIO_A_OBJS =				\
	$(DSP_AUDIO_A_SRCS:%.c=o/$(MODE)/%.o)	\
	$(DSP_AUDIO_A_DATA:%=o/$(MODE)/%.zip.o)	\

DSP_AUDIO_A_CHECKS =				\
	$(DSP_AUDIO_A).pkg			\
	$(DSP_AUDIO_A_HDRS:%=o/$(MODE)/%.ok)

DSP_AUDIO_A_DIRECTDEPS =			\
	LIBC_CALLS				\
	LIBC_DLOPEN				\
	LIBC_INTRIN				\
	LIBC_NEXGEN32E				\
	LIBC_STR				\
	LIBC_SYSV				\
	LIBC_PROC				\
	LIBC_THREAD				\

DSP_AUDIO_A_DEPS :=				\
	$(call uniq,$(foreach x,$(DSP_AUDIO_A_DIRECTDEPS),$($(x))))

$(DSP_AUDIO_A):	dsp/audio/			\
		$(DSP_AUDIO_A).pkg		\
		$(DSP_AUDIO_A_OBJS)

$(DSP_AUDIO_A).pkg:				\
		$(DSP_AUDIO_A_OBJS)		\
		$(foreach x,$(DSP_AUDIO_A_DIRECTDEPS),$($(x)_A).pkg)

DSP_AUDIO_LIBS = $(foreach x,$(DSP_AUDIO_ARTIFACTS),$($(x)))
DSP_AUDIO_SRCS = $(foreach x,$(DSP_AUDIO_ARTIFACTS),$($(x)_SRCS))
DSP_AUDIO_HDRS = $(foreach x,$(DSP_AUDIO_ARTIFACTS),$($(x)_HDRS))
DSP_AUDIO_CHECKS = $(foreach x,$(DSP_AUDIO_ARTIFACTS),$($(x)_CHECKS))
DSP_AUDIO_OBJS = $(foreach x,$(DSP_AUDIO_ARTIFACTS),$($(x)_OBJS))
$(DSP_AUDIO_OBJS): $(BUILD_FILES) dsp/audio/BUILD.mk

.PHONY: o/$(MODE)/dsp/audio
o/$(MODE)/dsp/audio: $(DSP_AUDIO_CHECKS)
