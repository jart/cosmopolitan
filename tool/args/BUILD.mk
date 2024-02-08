#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#── vi: set noet ft=make ts=8 sw=8 fenc=utf-8 :vi ────────────────────┘

PKGS += TOOL_ARGS

TOOL_ARGS_ARTIFACTS += TOOL_ARGS_A
TOOL_ARGS = $(TOOL_ARGS_A_DEPS) $(TOOL_ARGS_A)
TOOL_ARGS_A = o/$(MODE)/tool/args/args.a
TOOL_ARGS_A_FILES := $(wildcard tool/args/*)
TOOL_ARGS_A_HDRS = $(filter %.h,$(TOOL_ARGS_A_FILES))
TOOL_ARGS_A_SRCS = $(filter %.c,$(TOOL_ARGS_A_FILES))
TOOL_ARGS_A_OBJS = $(TOOL_ARGS_A_SRCS:%.c=o/$(MODE)/%.o)

TOOL_ARGS_A_CHECKS =				\
	$(TOOL_ARGS_A_HDRS:%=o/$(MODE)/%.ok)	\
	$(TOOL_ARGS_A).pkg

TOOL_ARGS_A_DIRECTDEPS =			\
	LIBC_INTRIN				\
	LIBC_MEM				\
	LIBC_NEXGEN32E				\
	LIBC_RUNTIME				\
	LIBC_STR				\
	LIBC_SYSV				\
	LIBC_X

TOOL_ARGS_A_DEPS :=				\
	$(call uniq,$(foreach x,$(TOOL_ARGS_A_DIRECTDEPS),$($(x))))

$(TOOL_ARGS_A):	tool/args/			\
		$(TOOL_ARGS_A).pkg		\
		$(TOOL_ARGS_A_OBJS)

$(TOOL_ARGS_A).pkg:				\
		$(TOOL_ARGS_A_OBJS)		\
		$(foreach x,$(TOOL_ARGS_A_DIRECTDEPS),$($(x)_A).pkg)

TOOL_ARGS_LIBS = $(foreach x,$(TOOL_ARGS_ARTIFACTS),$($(x)))
TOOL_ARGS_SRCS = $(foreach x,$(TOOL_ARGS_ARTIFACTS),$($(x)_SRCS))
TOOL_ARGS_HDRS = $(foreach x,$(TOOL_ARGS_ARTIFACTS),$($(x)_HDRS))
TOOL_ARGS_BINS = $(foreach x,$(TOOL_ARGS_ARTIFACTS),$($(x)_BINS))
TOOL_ARGS_CHECKS = $(foreach x,$(TOOL_ARGS_ARTIFACTS),$($(x)_CHECKS))
TOOL_ARGS_OBJS = $(foreach x,$(TOOL_ARGS_ARTIFACTS),$($(x)_OBJS))
TOOL_ARGS_TESTS = $(foreach x,$(TOOL_ARGS_ARTIFACTS),$($(x)_TESTS))

.PHONY: o/$(MODE)/tool/args
o/$(MODE)/tool/args: $(TOOL_ARGS_CHECKS)
