#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#── vi: set noet ft=make ts=8 sw=8 fenc=utf-8 :vi ────────────────────┘

ifeq ($(ARCH), x86_64)

PKGS += TOOL_PLINKO_LIB

TOOL_PLINKO_LIB_ARTIFACTS += TOOL_PLINKO_LIB_A
TOOL_PLINKO_LIB = $(TOOL_PLINKO_LIB_A_DEPS) $(TOOL_PLINKO_LIB_A)
TOOL_PLINKO_LIB_A = o/$(MODE)/tool/plinko/lib/plinkolib.a
TOOL_PLINKO_LIB_A_FILES := $(filter-out %/.%,$(wildcard tool/plinko/lib/*))
TOOL_PLINKO_LIB_A_HDRS = $(filter %.h,$(TOOL_PLINKO_LIB_A_FILES))
TOOL_PLINKO_LIB_A_SRCS_S = $(filter %.S,$(TOOL_PLINKO_LIB_A_FILES))
TOOL_PLINKO_LIB_A_SRCS_C = $(filter %.c,$(TOOL_PLINKO_LIB_A_FILES))

TOOL_PLINKO_LIB_A_CHECKS =				\
	$(TOOL_PLINKO_LIB_A_HDRS:%=o/$(MODE)/%.ok)	\
	$(TOOL_PLINKO_LIB_A).pkg

TOOL_PLINKO_LIB_A_SRCS =				\
	$(TOOL_PLINKO_LIB_A_SRCS_S)			\
	$(TOOL_PLINKO_LIB_A_SRCS_C)

TOOL_PLINKO_LIB_A_OBJS =				\
	$(TOOL_PLINKO_LIB_A_SRCS_S:%.S=o/$(MODE)/%.o)	\
	$(TOOL_PLINKO_LIB_A_SRCS_C:%.c=o/$(MODE)/%.o)

TOOL_PLINKO_LIB_A_DIRECTDEPS =				\
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
	LIBC_SYSV_CALLS					\
	THIRD_PARTY_COMPILER_RT				\
	THIRD_PARTY_GETOPT

TOOL_PLINKO_LIB_A_DEPS :=				\
	$(call uniq,$(foreach x,$(TOOL_PLINKO_LIB_A_DIRECTDEPS),$($(x))))

$(TOOL_PLINKO_LIB_A):					\
		$(TOOL_PLINKO_LIB_A).pkg		\
		$(TOOL_PLINKO_LIB_A_OBJS)

$(TOOL_PLINKO_LIB_A).pkg:				\
		$(TOOL_PLINKO_LIB_A_OBJS)		\
		$(foreach x,$(TOOL_PLINKO_LIB_A_DIRECTDEPS),$($(x)_A).pkg)

ifeq ($(MODE),)
$(TOOL_PLINKO_LIB_A_OBJS): private CFLAGS += -fno-inline
endif

ifeq ($(MODE),dbg)
$(TOOL_PLINKO_LIB_A_OBJS): private CFLAGS += -fno-inline
endif

$(TOOL_PLINKO_LIB_A_OBJS): private CFLAGS += -ffast-math -foptimize-sibling-calls -O2

TOOL_PLINKO_LIB_LIBS = $(foreach x,$(TOOL_PLINKO_LIB_ARTIFACTS),$($(x)))
TOOL_PLINKO_LIB_SRCS = $(foreach x,$(TOOL_PLINKO_LIB_ARTIFACTS),$($(x)_SRCS))
TOOL_PLINKO_LIB_HDRS = $(foreach x,$(TOOL_PLINKO_LIB_ARTIFACTS),$($(x)_HDRS))
TOOL_PLINKO_LIB_BINS = $(foreach x,$(TOOL_PLINKO_LIB_ARTIFACTS),$($(x)_BINS))
TOOL_PLINKO_LIB_CHECKS = $(foreach x,$(TOOL_PLINKO_LIB_ARTIFACTS),$($(x)_CHECKS))
TOOL_PLINKO_LIB_OBJS = $(foreach x,$(TOOL_PLINKO_LIB_ARTIFACTS),$($(x)_OBJS))
TOOL_PLINKO_LIB_TESTS = $(foreach x,$(TOOL_PLINKO_LIB_ARTIFACTS),$($(x)_TESTS))

endif

.PHONY: o/$(MODE)/tool/plinko/lib
o/$(MODE)/tool/plinko/lib: $(TOOL_PLINKO_LIB_CHECKS)
