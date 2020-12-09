#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#───vi: set et ft=make ts=8 tw=8 fenc=utf-8 :vi───────────────────────┘

PKGS += TOOL_BUILD_LIB

TOOL_BUILD_LIB_ARTIFACTS += TOOL_BUILD_LIB_A
TOOL_BUILD_LIB = $(TOOL_BUILD_LIB_A_DEPS) $(TOOL_BUILD_LIB_A)
TOOL_BUILD_LIB_A = o/$(MODE)/tool/build/lib/buildlib.a
TOOL_BUILD_LIB_A_FILES := $(wildcard tool/build/lib/*)
TOOL_BUILD_LIB_A_HDRS = $(filter %.h,$(TOOL_BUILD_LIB_A_FILES))
TOOL_BUILD_LIB_A_SRCS_S = $(filter %.S,$(TOOL_BUILD_LIB_A_FILES))
TOOL_BUILD_LIB_A_SRCS_C = $(filter %.c,$(TOOL_BUILD_LIB_A_FILES))

TOOL_BUILD_LIB_A_CHECKS =				\
	$(TOOL_BUILD_LIB_A_HDRS:%=o/$(MODE)/%.ok)	\
	$(TOOL_BUILD_LIB_A).pkg

TOOL_BUILD_LIB_A_SRCS =					\
	$(TOOL_BUILD_LIB_A_SRCS_S)			\
	$(TOOL_BUILD_LIB_A_SRCS_C)

TOOL_BUILD_LIB_A_OBJS =					\
	$(TOOL_BUILD_LIB_A_SRCS:%=o/$(MODE)/%.zip.o)	\
	$(TOOL_BUILD_LIB_A_SRCS_S:%.S=o/$(MODE)/%.o)	\
	$(TOOL_BUILD_LIB_A_SRCS_C:%.c=o/$(MODE)/%.o)

TOOL_BUILD_LIB_A_DIRECTDEPS =				\
	LIBC_ALG					\
	LIBC_BITS					\
	LIBC_CALLS					\
	LIBC_CALLS_HEFTY				\
	LIBC_ELF					\
	LIBC_FMT					\
	LIBC_INTRIN					\
	LIBC_LOG					\
	LIBC_MEM					\
	LIBC_NEXGEN32E					\
	LIBC_RAND					\
	LIBC_RUNTIME					\
	LIBC_SOCK					\
	LIBC_STDIO					\
	LIBC_STR					\
	LIBC_STUBS					\
	LIBC_SYSV					\
	LIBC_SYSV_CALLS					\
	LIBC_TIME					\
	LIBC_TINYMATH					\
	LIBC_UNICODE					\
	LIBC_X						\
	THIRD_PARTY_COMPILER_RT				\
	THIRD_PARTY_XED

TOOL_BUILD_LIB_A_DEPS :=				\
	$(call uniq,$(foreach x,$(TOOL_BUILD_LIB_A_DIRECTDEPS),$($(x))))

$(TOOL_BUILD_LIB_A):					\
		tool/build/lib/				\
		$(TOOL_BUILD_LIB_A).pkg			\
		$(TOOL_BUILD_LIB_A_OBJS)

$(TOOL_BUILD_LIB_A).pkg:				\
		$(TOOL_BUILD_LIB_A_OBJS)		\
		$(foreach x,$(TOOL_BUILD_LIB_A_DIRECTDEPS),$($(x)_A).pkg)

# ifeq (,$(MODE))
# $(TOOL_BUILD_LIB_A_OBJS):				\
# 		OVERRIDE_CFLAGS +=			\
# 			-fsanitize=address
# endif

o/$(MODE)/tool/build/lib/ssefloat.o:			\
		TARGET_ARCH +=				\
			-msse3

TOOL_BUILD_LIB_LIBS = $(foreach x,$(TOOL_BUILD_LIB_ARTIFACTS),$($(x)))
TOOL_BUILD_LIB_SRCS = $(foreach x,$(TOOL_BUILD_LIB_ARTIFACTS),$($(x)_SRCS))
TOOL_BUILD_LIB_HDRS = $(foreach x,$(TOOL_BUILD_LIB_ARTIFACTS),$($(x)_HDRS))
TOOL_BUILD_LIB_BINS = $(foreach x,$(TOOL_BUILD_LIB_ARTIFACTS),$($(x)_BINS))
TOOL_BUILD_LIB_CHECKS = $(foreach x,$(TOOL_BUILD_LIB_ARTIFACTS),$($(x)_CHECKS))
TOOL_BUILD_LIB_OBJS = $(foreach x,$(TOOL_BUILD_LIB_ARTIFACTS),$($(x)_OBJS))
TOOL_BUILD_LIB_TESTS = $(foreach x,$(TOOL_BUILD_LIB_ARTIFACTS),$($(x)_TESTS))

.PHONY: o/$(MODE)/tool/build/lib
o/$(MODE)/tool/build/lib: $(TOOL_BUILD_LIB_CHECKS)
