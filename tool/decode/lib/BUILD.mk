#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#── vi: set noet ft=make ts=8 sw=8 fenc=utf-8 :vi ────────────────────┘

PKGS += TOOL_DECODE_LIB

TOOL_DECODE_LIB_ARTIFACTS += TOOL_DECODE_LIB_A
TOOL_DECODE_LIB = $(TOOL_DECODE_LIB_A_DEPS) $(TOOL_DECODE_LIB_A)
TOOL_DECODE_LIB_A = o/$(MODE)/tool/decode/lib/decodelib.a
TOOL_DECODE_LIB_A_FILES := $(wildcard tool/decode/lib/*)
TOOL_DECODE_LIB_A_HDRS = $(filter %.h,$(TOOL_DECODE_LIB_A_FILES))
TOOL_DECODE_LIB_A_SRCS_S = $(filter %.S,$(TOOL_DECODE_LIB_A_FILES))
TOOL_DECODE_LIB_A_SRCS_C = $(filter %.c,$(TOOL_DECODE_LIB_A_FILES))
TOOL_DECODE_LIB_A_CHECKS = $(TOOL_DECODE_LIB_A).pkg

TOOL_DECODE_LIB_A_SRCS =				\
	$(TOOL_DECODE_LIB_A_SRCS_S)			\
	$(TOOL_DECODE_LIB_A_SRCS_C)

TOOL_DECODE_LIB_A_OBJS =				\
	$(TOOL_DECODE_LIB_A_SRCS_S:%.S=o/$(MODE)/%.o)	\
	$(TOOL_DECODE_LIB_A_SRCS_C:%.c=o/$(MODE)/%.o)

TOOL_DECODE_LIB_A_DIRECTDEPS =				\
	LIBC_FMT					\
	LIBC_INTRIN					\
	LIBC_MEM					\
	LIBC_NEXGEN32E					\
	LIBC_RUNTIME					\
	LIBC_STDIO					\
	LIBC_STR					\
	LIBC_SYSV

TOOL_DECODE_LIB_A_DEPS :=				\
	$(call uniq,$(foreach x,$(TOOL_DECODE_LIB_A_DIRECTDEPS),$($(x))))

$(TOOL_DECODE_LIB_A):					\
		tool/decode/lib/			\
		$(TOOL_DECODE_LIB_A).pkg		\
		$(TOOL_DECODE_LIB_A_OBJS)

$(TOOL_DECODE_LIB_A).pkg:				\
		$(TOOL_DECODE_LIB_A_OBJS)		\
		$(foreach x,$(TOOL_DECODE_LIB_A_DIRECTDEPS),$($(x)_A).pkg)

TOOL_DECODE_LIB_LIBS = $(foreach x,$(TOOL_DECODE_LIB_ARTIFACTS),$($(x)))
TOOL_DECODE_LIB_SRCS = $(foreach x,$(TOOL_DECODE_LIB_ARTIFACTS),$($(x)_SRCS))
TOOL_DECODE_LIB_HDRS = $(foreach x,$(TOOL_DECODE_LIB_ARTIFACTS),$($(x)_HDRS))
TOOL_DECODE_LIB_BINS = $(foreach x,$(TOOL_DECODE_LIB_ARTIFACTS),$($(x)_BINS))
TOOL_DECODE_LIB_CHECKS = $(foreach x,$(TOOL_DECODE_LIB_ARTIFACTS),$($(x)_CHECKS))
TOOL_DECODE_LIB_OBJS = $(foreach x,$(TOOL_DECODE_LIB_ARTIFACTS),$($(x)_OBJS))
TOOL_DECODE_LIB_TESTS = $(foreach x,$(TOOL_DECODE_LIB_ARTIFACTS),$($(x)_TESTS))

o/$(MODE)/tool/decode/lib/elfidnames.o			\
o/$(MODE)/tool/decode/lib/machoidnames.o		\
o/$(MODE)/tool/decode/lib/peidnames.o: private		\
	DEFAULT_CFLAGS +=				\
		-fdata-sections

.PHONY: o/$(MODE)/tool/decode/lib
o/$(MODE)/tool/decode/lib: $(TOOL_DECODE_LIB_CHECKS)
