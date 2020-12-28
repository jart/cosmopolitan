#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#───vi: set et ft=make ts=8 tw=8 fenc=utf-8 :vi───────────────────────┘

PKGS += DSP_TTY

DSP_TTY_ARTIFACTS += DSP_TTY_A
DSP_TTY = $(DSP_TTY_A_DEPS) $(DSP_TTY_A)
DSP_TTY_A = o/$(MODE)/dsp/tty/tty.a
DSP_TTY_A_FILES := $(wildcard dsp/tty/*)
DSP_TTY_A_HDRS = $(filter %.h,$(DSP_TTY_A_FILES))
DSP_TTY_A_SRCS_S = $(filter %.S,$(DSP_TTY_A_FILES))
DSP_TTY_A_SRCS_C = $(filter %.c,$(DSP_TTY_A_FILES))

DSP_TTY_A_SRCS =				\
	$(DSP_TTY_A_SRCS_S)			\
	$(DSP_TTY_A_SRCS_C)

DSP_TTY_A_OBJS =				\
	$(DSP_TTY_A_SRCS:%=o/$(MODE)/%.zip.o)	\
	$(DSP_TTY_A_SRCS_S:%.S=o/$(MODE)/%.o)	\
	$(DSP_TTY_A_SRCS_C:%.c=o/$(MODE)/%.o)

DSP_TTY_A_CHECKS =				\
	$(DSP_TTY_A).pkg			\
	$(DSP_TTY_A_HDRS:%=o/$(MODE)/%.ok)

DSP_TTY_A_DIRECTDEPS =				\
	DSP_CORE				\
	LIBC_ALG				\
	LIBC_CALLS				\
	LIBC_FMT				\
	LIBC_LOG				\
	LIBC_RUNTIME				\
	LIBC_MEM				\
	LIBC_NEXGEN32E				\
	LIBC_NT_KERNEL32			\
	LIBC_STR				\
	LIBC_STDIO				\
	LIBC_STUBS				\
	LIBC_SOCK				\
	LIBC_SYSV				\
	LIBC_TINYMATH				\
	LIBC_TIME				\
	LIBC_X					\
	LIBC_UNICODE

DSP_TTY_A_DEPS :=				\
	$(call uniq,$(foreach x,$(DSP_TTY_A_DIRECTDEPS),$($(x))))

$(DSP_TTY_A):	dsp/tty/			\
		$(DSP_TTY_A).pkg		\
		$(DSP_TTY_A_OBJS)

$(DSP_TTY_A).pkg:				\
		$(DSP_TTY_A_OBJS)		\
		$(foreach x,$(DSP_TTY_A_DIRECTDEPS),$($(x)_A).pkg)

o/$(MODE)/dsp/tty/ttyraster.o:			\
		OVERRIDE_CFLAGS +=		\
			$(MATHEMATICAL)

# ifeq (,$(MODE))
# $(DSP_TTY_OBJS):				\
# 		OVERRIDE_CFLAGS +=		\
# 			-fsanitize=address
# endif

DSP_TTY_LIBS = $(foreach x,$(DSP_TTY_ARTIFACTS),$($(x)))
DSP_TTY_SRCS = $(foreach x,$(DSP_TTY_ARTIFACTS),$($(x)_SRCS))
DSP_TTY_HDRS = $(foreach x,$(DSP_TTY_ARTIFACTS),$($(x)_HDRS))
DSP_TTY_CHECKS = $(foreach x,$(DSP_TTY_ARTIFACTS),$($(x)_CHECKS))
DSP_TTY_OBJS = $(foreach x,$(DSP_TTY_ARTIFACTS),$($(x)_OBJS))
$(DSP_TTY_OBJS): $(BUILD_FILES) dsp/tty/tty.mk

.PHONY: o/$(MODE)/dsp/tty
o/$(MODE)/dsp/tty: $(DSP_TTY_CHECKS)
