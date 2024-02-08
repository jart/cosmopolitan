#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#── vi: set noet ft=make ts=8 sw=8 fenc=utf-8 :vi ────────────────────┘
#
# SYNOPSIS
#
#   C Compiler
#
# OVERVIEW
#
#   This makefile compiles and runs each test twice. The first with
#   GCC-built chibicc, and a second time with chibicc-built chibicc

ifeq ($(ARCH), x86_64)

CHIBICC = o/$(MODE)/third_party/chibicc/chibicc.com
CHIBICC_FLAGS =								\
	-fno-common							\
	-include libc/integral/normalize.inc				\
	-DMODE='"$(MODE)"'

o/$(MODE)/%.chibicc.o: private .UNSANDBOXED = true
o/$(MODE)/%.chibicc.o: %.c $(CHIBICC)
	@$(COMPILE) $(CHIBICC) $(OBJECTIFY.c.flags) -c $< $(OUTPUT_OPTION)

PKGS += THIRD_PARTY_CHIBICC
THIRD_PARTY_CHIBICC_ARTIFACTS += THIRD_PARTY_CHIBICC_A
THIRD_PARTY_CHIBICC = $(THIRD_PARTY_CHIBICC_A_DEPS) $(THIRD_PARTY_CHIBICC_A)
THIRD_PARTY_CHIBICC_A = o/$(MODE)/third_party/chibicc/chibicc.a
THIRD_PARTY_CHIBICC_A_FILES := $(wildcard third_party/chibicc/*)
THIRD_PARTY_CHIBICC_A_HDRS = $(filter %.h,$(THIRD_PARTY_CHIBICC_A_FILES))
THIRD_PARTY_CHIBICC_A_SRCS = $(filter %.c,$(THIRD_PARTY_CHIBICC_A_FILES))
THIRD_PARTY_CHIBICC_A_INCS = $(filter %.inc,$(THIRD_PARTY_CHIBICC_A_FILES))

THIRD_PARTY_CHIBICC_DEFINES =						\
	-DCRT=\"$(CRT)\"						\
	-DAPE=\"o/$(MODE)/ape/ape.o\"					\
	-DLDS=\"o/$(MODE)/ape/ape.lds\"

THIRD_PARTY_CHIBICC_BINS =						\
	o/$(MODE)/third_party/chibicc/chibicc.com.dbg			\
	o/$(MODE)/third_party/chibicc/chibicc.com

THIRD_PARTY_CHIBICC_A_OBJS =						\
	$(THIRD_PARTY_CHIBICC_A_SRCS:%.c=o/$(MODE)/%.o)

THIRD_PARTY_CHIBICC_A_CHECKS =						\
	$(THIRD_PARTY_CHIBICC_A).pkg					\
	$(THIRD_PARTY_CHIBICC_A_HDRS:%=o/$(MODE)/%.ok)

THIRD_PARTY_CHIBICC_A_DIRECTDEPS =					\
	LIBC_CALLS							\
	LIBC_FMT							\
	LIBC_INTRIN							\
	LIBC_LOG							\
	LIBC_MEM							\
	LIBC_NEXGEN32E							\
	LIBC_PROC							\
	LIBC_RUNTIME							\
	LIBC_STDIO							\
	LIBC_STR							\
	LIBC_SYSV							\
	LIBC_TIME							\
	LIBC_X								\
	THIRD_PARTY_COMPILER_RT						\
	THIRD_PARTY_DLMALLOC						\
	TOOL_BUILD_LIB							\
	THIRD_PARTY_GDTOA

THIRD_PARTY_CHIBICC_A_DEPS :=						\
	$(call uniq,$(foreach x,$(THIRD_PARTY_CHIBICC_A_DIRECTDEPS),$($(x))))

$(THIRD_PARTY_CHIBICC_A):						\
		third_party/chibicc/					\
		$(THIRD_PARTY_CHIBICC_A).pkg				\
		$(THIRD_PARTY_CHIBICC_A_OBJS)
$(THIRD_PARTY_CHIBICC_A).pkg:						\
		$(THIRD_PARTY_CHIBICC_A_OBJS)				\
		$(foreach x,$(THIRD_PARTY_CHIBICC_A_DIRECTDEPS),$($(x)_A).pkg)

o/$(MODE)/third_party/chibicc/chibicc.com.dbg:				\
		$(THIRD_PARTY_CHIBICC_A_DEPS)				\
		$(THIRD_PARTY_CHIBICC_A)				\
		$(APE_NO_MODIFY_SELF)					\
		$(CRT)							\
		o/$(MODE)/third_party/chibicc/help.txt.zip.o		\
		o/$(MODE)/third_party/chibicc/chibicc.main.o		\
		$(THIRD_PARTY_CHIBICC_A).pkg
	@$(APELINK)

o/$(MODE)/third_party/chibicc/chibicc.com:				\
		o/$(MODE)/third_party/chibicc/chibicc.com.dbg		\
		o/$(MODE)/third_party/zip/zip.com			\
		o/$(MODE)/tool/build/symtab.com
	@$(MAKE_OBJCOPY)
	@$(MAKE_SYMTAB_CREATE)
	@$(MAKE_SYMTAB_ZIP)

o/$(MODE)/third_party/chibicc/as.com.dbg:				\
		$(THIRD_PARTY_CHIBICC_A_DEPS)				\
		$(THIRD_PARTY_CHIBICC_A)				\
		$(APE_NO_MODIFY_SELF)					\
		$(CRT)							\
		o/$(MODE)/third_party/chibicc/as.main.o			\
		$(THIRD_PARTY_CHIBICC_A).pkg
	@$(APELINK)

o/$(MODE)/third_party/chibicc/chibicc.o: private			\
		CPPFLAGS += $(THIRD_PARTY_CHIBICC_DEFINES)

THIRD_PARTY_CHIBICC_LIBS = $(foreach x,$(THIRD_PARTY_CHIBICC_ARTIFACTS),$($(x)))
THIRD_PARTY_CHIBICC_SRCS = $(foreach x,$(THIRD_PARTY_CHIBICC_ARTIFACTS),$($(x)_SRCS))
THIRD_PARTY_CHIBICC_HDRS = $(foreach x,$(THIRD_PARTY_CHIBICC_ARTIFACTS),$($(x)_HDRS))
THIRD_PARTY_CHIBICC_INCS = $(foreach x,$(THIRD_PARTY_CHIBICC_ARTIFACTS),$($(x)_INCS))
THIRD_PARTY_CHIBICC_CHECKS = $(foreach x,$(THIRD_PARTY_CHIBICC_ARTIFACTS),$($(x)_CHECKS))
THIRD_PARTY_CHIBICC_OBJS = $(foreach x,$(THIRD_PARTY_CHIBICC_ARTIFACTS),$($(x)_OBJS))
$(THIRD_PARTY_CHIBICC_OBJS): $(BUILD_FILES) third_party/chibicc/BUILD.mk

endif

.PHONY: o/$(MODE)/third_party/chibicc
o/$(MODE)/third_party/chibicc:						\
		o/$(MODE)/third_party/chibicc/test			\
		$(THIRD_PARTY_CHIBICC_BINS)				\
		$(THIRD_PARTY_CHIBICC_CHECKS)
