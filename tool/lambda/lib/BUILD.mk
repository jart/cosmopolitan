#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#── vi: set noet ft=make ts=8 sw=8 fenc=utf-8 :vi ────────────────────┘

PKGS += TOOL_LAMBDA_LIB

TOOL_LAMBDA_LIB_ARTIFACTS += TOOL_LAMBDA_LIB_A
TOOL_LAMBDA_LIB = $(TOOL_LAMBDA_LIB_A_DEPS) $(TOOL_LAMBDA_LIB_A)
TOOL_LAMBDA_LIB_A = o/$(MODE)/tool/lambda/lib/lambdalib.a
TOOL_LAMBDA_LIB_A_FILES := $(filter-out %/.%,$(wildcard tool/lambda/lib/*))
TOOL_LAMBDA_LIB_A_HDRS = $(filter %.h,$(TOOL_LAMBDA_LIB_A_FILES))
TOOL_LAMBDA_LIB_A_SRCS_S = $(filter %.S,$(TOOL_LAMBDA_LIB_A_FILES))
TOOL_LAMBDA_LIB_A_SRCS_C = $(filter %.c,$(TOOL_LAMBDA_LIB_A_FILES))

TOOL_LAMBDA_LIB_A_CHECKS =				\
	$(TOOL_LAMBDA_LIB_A_HDRS:%=o/$(MODE)/%.ok)	\
	$(TOOL_LAMBDA_LIB_A).pkg

TOOL_LAMBDA_LIB_A_SRCS =				\
	$(TOOL_LAMBDA_LIB_A_SRCS_S)			\
	$(TOOL_LAMBDA_LIB_A_SRCS_C)

TOOL_LAMBDA_LIB_A_OBJS =				\
	$(TOOL_LAMBDA_LIB_A_SRCS_S:%.S=o/$(MODE)/%.o)	\
	$(TOOL_LAMBDA_LIB_A_SRCS_C:%.c=o/$(MODE)/%.o)

TOOL_LAMBDA_LIB_A_DIRECTDEPS =				\
	LIBC_CALLS					\
	LIBC_FMT					\
	LIBC_INTRIN					\
	LIBC_LOG					\
	LIBC_MEM					\
	LIBC_NEXGEN32E					\
	LIBC_RUNTIME					\
	LIBC_SOCK					\
	LIBC_STDIO					\
	LIBC_STR					\
	LIBC_SYSV					\
	THIRD_PARTY_COMPILER_RT				\
	THIRD_PARTY_GETOPT

TOOL_LAMBDA_LIB_A_DEPS :=				\
	$(call uniq,$(foreach x,$(TOOL_LAMBDA_LIB_A_DIRECTDEPS),$($(x))))

$(TOOL_LAMBDA_LIB_A):					\
		$(TOOL_LAMBDA_LIB_A).pkg		\
		$(TOOL_LAMBDA_LIB_A_OBJS)

$(TOOL_LAMBDA_LIB_A).pkg:				\
		$(TOOL_LAMBDA_LIB_A_OBJS)		\
		$(foreach x,$(TOOL_LAMBDA_LIB_A_DIRECTDEPS),$($(x)_A).pkg)

TOOL_LAMBDA_LIB_LIBS = $(foreach x,$(TOOL_LAMBDA_LIB_ARTIFACTS),$($(x)))
TOOL_LAMBDA_LIB_SRCS = $(foreach x,$(TOOL_LAMBDA_LIB_ARTIFACTS),$($(x)_SRCS))
TOOL_LAMBDA_LIB_HDRS = $(foreach x,$(TOOL_LAMBDA_LIB_ARTIFACTS),$($(x)_HDRS))
TOOL_LAMBDA_LIB_BINS = $(foreach x,$(TOOL_LAMBDA_LIB_ARTIFACTS),$($(x)_BINS))
TOOL_LAMBDA_LIB_CHECKS = $(foreach x,$(TOOL_LAMBDA_LIB_ARTIFACTS),$($(x)_CHECKS))
TOOL_LAMBDA_LIB_OBJS = $(foreach x,$(TOOL_LAMBDA_LIB_ARTIFACTS),$($(x)_OBJS))
TOOL_LAMBDA_LIB_TESTS = $(foreach x,$(TOOL_LAMBDA_LIB_ARTIFACTS),$($(x)_TESTS))

.PHONY: o/$(MODE)/tool/lambda/lib
o/$(MODE)/tool/lambda/lib: $(TOOL_LAMBDA_LIB_CHECKS)
