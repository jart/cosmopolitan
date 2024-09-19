#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#── vi: set noet ft=make ts=8 sw=8 fenc=utf-8 :vi ────────────────────┘

PKGS += DSP_PROG

DSP_PROG_FILES := $(wildcard dsp/prog/*)
DSP_PROG_HDRS = $(filter %.h,$(DSP_PROG_FILES))
DSP_PROG_SRCS = $(filter %.c,$(DSP_PROG_FILES))
DSP_PROG_OBJS = $(DSP_PROG_SRCS:%.c=o/$(MODE)/%.o)
DSP_PROG_COMS = $(DSP_PROG_SRCS:%.c=o/$(MODE)/%)
DSP_PROG_BINS = $(DSP_PROG_COMS) $(DSP_PROG_COMS:%=%.dbg)

DSP_PROG_DIRECTDEPS =				\
	DSP_AUDIO				\
	LIBC_CALLS				\
	LIBC_INTRIN				\
	LIBC_NEXGEN32E				\
	LIBC_RUNTIME				\
	LIBC_SOCK				\
	LIBC_STDIO				\
	LIBC_SYSV				\
	LIBC_TINYMATH				\
	THIRD_PARTY_MUSL			\

DSP_PROG_DEPS :=				\
	$(call uniq,$(foreach x,$(DSP_PROG_DIRECTDEPS),$($(x))))

o/$(MODE)/dsp/prog/prog.pkg:			\
		$(DSP_PROG_OBJS)		\
		$(foreach x,$(DSP_PROG_DIRECTDEPS),$($(x)_A).pkg)

o/$(MODE)/dsp/prog/%.dbg:			\
		$(DSP_PROG_DEPS)		\
		o/$(MODE)/dsp/prog/prog.pkg	\
		o/$(MODE)/dsp/prog/%.o		\
		$(CRT)				\
		$(APE_NO_MODIFY_SELF)
	@$(APELINK)

$(DSP_PROG_OBJS): dsp/prog/BUILD.mk

.PHONY: o/$(MODE)/dsp/prog
o/$(MODE)/dsp/prog: $(DSP_PROG_BINS)
