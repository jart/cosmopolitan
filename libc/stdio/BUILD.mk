#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#── vi: set noet ft=make ts=8 sw=8 fenc=utf-8 :vi ────────────────────┘

PKGS += LIBC_STDIO

LIBC_STDIO_ARTIFACTS += LIBC_STDIO_A
LIBC_STDIO = $(LIBC_STDIO_A_DEPS) $(LIBC_STDIO_A)
LIBC_STDIO_A = o/$(MODE)/libc/stdio/stdio.a
LIBC_STDIO_A_FILES := $(wildcard libc/stdio/*)
LIBC_STDIO_A_HDRS = $(filter %.h,$(LIBC_STDIO_A_FILES))
LIBC_STDIO_A_INCS = $(filter %.inc,$(LIBC_STDIO_A_FILES))
LIBC_STDIO_A_SRCS_S = $(filter %.S,$(LIBC_STDIO_A_FILES))
LIBC_STDIO_A_SRCS_C = $(filter %.c,$(LIBC_STDIO_A_FILES))

LIBC_STDIO_A_SRCS =					\
	$(LIBC_STDIO_A_SRCS_S)				\
	$(LIBC_STDIO_A_SRCS_C)

LIBC_STDIO_A_OBJS =					\
	$(LIBC_STDIO_A_SRCS_S:%.S=o/$(MODE)/%.o)	\
	$(LIBC_STDIO_A_SRCS_C:%.c=o/$(MODE)/%.o)

LIBC_STDIO_A_CHECKS =					\
	$(LIBC_STDIO_A).pkg				\
	$(LIBC_STDIO_A_HDRS:%=o/$(MODE)/%.ok)

LIBC_STDIO_A_DIRECTDEPS =				\
	LIBC_CALLS					\
	LIBC_FMT					\
	LIBC_INTRIN					\
	LIBC_MEM					\
	LIBC_NEXGEN32E					\
	LIBC_NT_ADVAPI32				\
	LIBC_NT_KERNEL32				\
	LIBC_RUNTIME					\
	LIBC_PROC					\
	LIBC_STR					\
	LIBC_SYSV					\
	LIBC_SYSV_CALLS					\
	THIRD_PARTY_GDTOA

LIBC_STDIO_A_DEPS :=					\
	$(call uniq,$(foreach x,$(LIBC_STDIO_A_DIRECTDEPS),$($(x))))

$(LIBC_STDIO_A):libc/stdio/				\
		$(LIBC_STDIO_A).pkg			\
		$(LIBC_STDIO_A_OBJS)

$(LIBC_STDIO_A).pkg:					\
		$(LIBC_STDIO_A_OBJS)			\
		$(foreach x,$(LIBC_STDIO_A_DIRECTDEPS),$($(x)_A).pkg)

# offer assurances about the stack safety of cosmo libc
$(LIBC_STDIO_A_OBJS): private COPTS += -Wframe-larger-than=4096 -Walloca-larger-than=4096

o/$(MODE)/libc/stdio/fputc.o: private			\
		CFLAGS +=				\
			-O3

o//libc/stdio/appendw.o: private			\
		CFLAGS +=				\
			-Os

o/$(MODE)/libc/stdio/dirstream.o			\
o/$(MODE)/libc/stdio/mt19937.o: private			\
		CFLAGS +=				\
			-ffunction-sections

LIBC_STDIO_LIBS = $(foreach x,$(LIBC_STDIO_ARTIFACTS),$($(x)))
LIBC_STDIO_SRCS = $(foreach x,$(LIBC_STDIO_ARTIFACTS),$($(x)_SRCS))
LIBC_STDIO_HDRS = $(foreach x,$(LIBC_STDIO_ARTIFACTS),$($(x)_HDRS))
LIBC_STDIO_INCS = $(foreach x,$(LIBC_STDIO_ARTIFACTS),$($(x)_INCS))
LIBC_STDIO_CHECKS = $(foreach x,$(LIBC_STDIO_ARTIFACTS),$($(x)_CHECKS))
LIBC_STDIO_OBJS = $(foreach x,$(LIBC_STDIO_ARTIFACTS),$($(x)_OBJS))
$(LIBC_STDIO_OBJS): $(BUILD_FILES) libc/stdio/BUILD.mk

.PHONY: o/$(MODE)/libc/stdio
o/$(MODE)/libc/stdio: $(LIBC_STDIO_CHECKS)
