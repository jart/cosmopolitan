#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#── vi: set noet ft=make ts=8 sw=8 fenc=utf-8 :vi ────────────────────┘

PKGS += TOOL_BUILD_LIB

TOOL_BUILD_LIB_ARTIFACTS += TOOL_BUILD_LIB_A
TOOL_BUILD_LIB = $(TOOL_BUILD_LIB_A_DEPS) $(TOOL_BUILD_LIB_A)
TOOL_BUILD_LIB_A = o/$(MODE)/tool/build/lib/buildlib.a
TOOL_BUILD_LIB_A_FILES := $(wildcard tool/build/lib/*)
TOOL_BUILD_LIB_A_HDRS = $(filter %.h,$(TOOL_BUILD_LIB_A_FILES))
TOOL_BUILD_LIB_A_SRCS_S = $(filter %.S,$(TOOL_BUILD_LIB_A_FILES))

TOOL_BUILD_LIB_A_SRCS_C =				\
	$(filter-out tool/build/lib/apetest.c,$(filter %.c,$(TOOL_BUILD_LIB_A_FILES)))

TOOL_BUILD_LIB_A_CHECKS =				\
	$(TOOL_BUILD_LIB_A_HDRS:%=o/$(MODE)/%.ok)	\
	$(TOOL_BUILD_LIB_A).pkg

TOOL_BUILD_LIB_A_SRCS =					\
	$(TOOL_BUILD_LIB_A_SRCS_S)			\
	$(TOOL_BUILD_LIB_A_SRCS_C)

TOOL_BUILD_LIB_COMS =					\
	o/$(MODE)/tool/build/lib/apetest.com		\
	o/$(MODE)/tool/build/lib/apetest2.com

TOOL_BUILD_LIB_A_OBJS =					\
	$(TOOL_BUILD_LIB_A_SRCS_S:%.S=o/$(MODE)/%.o)	\
	$(TOOL_BUILD_LIB_A_SRCS_C:%.c=o/$(MODE)/%.o)	\
	o/$(MODE)/tool/build/lib/apetest.com.zip.o	\
	o/$(MODE)/tool/build/lib/apetest2.com.zip.o

TOOL_BUILD_LIB_A_DIRECTDEPS =				\
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
	LIBC_PROC					\
	LIBC_THREAD					\
	LIBC_TIME					\
	LIBC_TINYMATH					\
	LIBC_X						\
	NET_HTTP					\
	NET_HTTPS					\
	THIRD_PARTY_COMPILER_RT				\
	THIRD_PARTY_MBEDTLS				\
	THIRD_PARTY_XED					\
	THIRD_PARTY_ZLIB

TOOL_BUILD_LIB_A_DEPS :=				\
	$(call uniq,$(foreach x,$(TOOL_BUILD_LIB_A_DIRECTDEPS),$($(x))))

$(TOOL_BUILD_LIB_A):					\
		tool/build/lib/				\
		$(TOOL_BUILD_LIB_A).pkg			\
		$(TOOL_BUILD_LIB_A_OBJS)

$(TOOL_BUILD_LIB_A).pkg:				\
		$(TOOL_BUILD_LIB_A_OBJS)		\
		$(foreach x,$(TOOL_BUILD_LIB_A_DIRECTDEPS),$($(x)_A).pkg)

ifeq ($(ARCH), x86_64)
o/$(MODE)/tool/build/lib/ssefloat.o: private		\
		TARGET_ARCH +=				\
			-msse3
endif

o/$(MODE)/tool/build/lib/apetest.com.dbg:		\
		$(TOOL_BUILD_LIB_A_DEPS)		\
		o/$(MODE)/tool/build/lib/apetest.o	\
		$(CRT)					\
		$(APE_NO_MODIFY_SELF)
	@$(APELINK)

o/$(MODE)/tool/build/lib/apetest2.com.dbg:		\
		$(TOOL_BUILD_LIB_A_DEPS)		\
		o/$(MODE)/tool/build/lib/apetest.o	\
		$(CRT)					\
		$(APE_COPY_SELF)
	@$(APELINK)

o/$(MODE)/tool/build/lib/apetest.com.zip.o		\
o/$(MODE)/tool/build/lib/apetest2.com.zip.o: private	\
		ZIPOBJ_FLAGS +=				\
			-B

o/$(MODE)/tool/build/lib/apetest.o:			\
		tool/build/lib/apetest.c		\
		libc/calls/calls.h

# these assembly files are safe to build on aarch64
o/$(MODE)/tool/build/lib/errnos.o: tool/build/lib/errnos.S
	@$(COMPILE) -AOBJECTIFY.S $(OBJECTIFY.S) $(OUTPUT_OPTION) -c $<

TOOL_BUILD_LIB_LIBS = $(foreach x,$(TOOL_BUILD_LIB_ARTIFACTS),$($(x)))
TOOL_BUILD_LIB_SRCS = $(foreach x,$(TOOL_BUILD_LIB_ARTIFACTS),$($(x)_SRCS))
TOOL_BUILD_LIB_HDRS = $(foreach x,$(TOOL_BUILD_LIB_ARTIFACTS),$($(x)_HDRS))
TOOL_BUILD_LIB_BINS = $(foreach x,$(TOOL_BUILD_LIB_ARTIFACTS),$($(x)_BINS))
TOOL_BUILD_LIB_CHECKS = $(foreach x,$(TOOL_BUILD_LIB_ARTIFACTS),$($(x)_CHECKS))
TOOL_BUILD_LIB_OBJS = $(foreach x,$(TOOL_BUILD_LIB_ARTIFACTS),$($(x)_OBJS))
TOOL_BUILD_LIB_TESTS = $(foreach x,$(TOOL_BUILD_LIB_ARTIFACTS),$($(x)_TESTS))

$(TOOL_BUILD_LIB_OBJS): tool/build/lib/BUILD.mk

.PHONY: o/$(MODE)/tool/build/lib
o/$(MODE)/tool/build/lib:				\
		$(TOOL_BUILD_LIB_COMS)			\
		$(TOOL_BUILD_LIB_CHECKS)		\
		$(TOOL_BUILD_LIB_A)
