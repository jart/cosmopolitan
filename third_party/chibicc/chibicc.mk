#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#───vi: set et ft=make ts=8 tw=8 fenc=utf-8 :vi───────────────────────┘
#
# SYNOPSIS
#
#   C Compiler
#
# OVERVIEW
#
#   This makefile compiles and runs each test twice. The first with
#   GCC-built chibicc, and a second time with chibicc-built chibicc

CHIBICC = o/$(MODE)/third_party/chibicc/chibicc.com.dbg
CHIBICC2 = o/$(MODE)/third_party/chibicc/chibicc2.com.dbg
CHIBICC_FLAGS =								\
	-fno-common							\
	-include libc/integral/normalize.inc

PKGS += THIRD_PARTY_CHIBICC
THIRD_PARTY_CHIBICC_ARTIFACTS += THIRD_PARTY_CHIBICC_A
THIRD_PARTY_CHIBICC = $(THIRD_PARTY_CHIBICC_A_DEPS) $(THIRD_PARTY_CHIBICC_A)
THIRD_PARTY_CHIBICC_A = o/$(MODE)/third_party/chibicc/chibicc.a
THIRD_PARTY_CHIBICC2_A = o/$(MODE)/third_party/chibicc/chibicc2.a
THIRD_PARTY_CHIBICC_A_FILES := $(wildcard third_party/chibicc/*)
THIRD_PARTY_CHIBICC_A_HDRS = $(filter %.h,$(THIRD_PARTY_CHIBICC_A_FILES))
THIRD_PARTY_CHIBICC_A_SRCS = $(filter %.c,$(THIRD_PARTY_CHIBICC_A_FILES))

THIRD_PARTY_CHIBICC_DEFINES =						\
	-DCRT=\"$(CRT)\"						\
	-DAPE=\"o/$(MODE)/ape/ape.o\"					\
	-DLDS=\"o/$(MODE)/ape/ape.lds\"

THIRD_PARTY_CHIBICC_BINS =						\
	o/$(MODE)/third_party/chibicc/chibicc.com.dbg			\
	o/$(MODE)/third_party/chibicc/chibicc.com			\
	o/$(MODE)/third_party/chibicc/chibicc2.com.dbg			\
	o/$(MODE)/third_party/chibicc/chibicc2.com

THIRD_PARTY_CHIBICC_A_OBJS =						\
	$(THIRD_PARTY_CHIBICC_A_SRCS:%=o/$(MODE)/%.zip.o)		\
	$(THIRD_PARTY_CHIBICC_A_SRCS:%.c=o/$(MODE)/%.o)
THIRD_PARTY_CHIBICC2_A_OBJS =						\
	$(THIRD_PARTY_CHIBICC_A_SRCS:%=o/$(MODE)/%.zip.o)		\
	$(THIRD_PARTY_CHIBICC_A_SRCS:%.c=o/$(MODE)/%.chibicc.o)

THIRD_PARTY_CHIBICC_A_CHECKS =						\
	$(THIRD_PARTY_CHIBICC_A).pkg					\
	$(THIRD_PARTY_CHIBICC2_A).pkg					\
	$(THIRD_PARTY_CHIBICC_A_HDRS:%=o/$(MODE)/%.ok)

THIRD_PARTY_CHIBICC_A_DIRECTDEPS =					\
	LIBC_ALG							\
	LIBC_BITS							\
	LIBC_CALLS							\
	LIBC_CALLS_HEFTY						\
	LIBC_FMT							\
	LIBC_LOG							\
	LIBC_MEM							\
	LIBC_NEXGEN32E							\
	LIBC_RUNTIME							\
	LIBC_STDIO							\
	LIBC_LOG							\
	LIBC_STR							\
	LIBC_STUBS							\
	LIBC_TIME							\
	LIBC_UNICODE							\
	LIBC_SYSV							\
	LIBC_X								\
	TOOL_BUILD_LIB							\
	THIRD_PARTY_COMPILER_RT						\
	THIRD_PARTY_DLMALLOC						\
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

$(THIRD_PARTY_CHIBICC2_A):						\
		third_party/chibicc/					\
		$(THIRD_PARTY_CHIBICC2_A).pkg				\
		$(THIRD_PARTY_CHIBICC2_A_OBJS)
$(THIRD_PARTY_CHIBICC2_A).pkg:						\
		$(THIRD_PARTY_CHIBICC2_A_OBJS)				\
		$(foreach x,$(THIRD_PARTY_CHIBICC_A_DIRECTDEPS),$($(x)_A).pkg)

o/$(MODE)/third_party/chibicc/chibicc.com.dbg:				\
		$(THIRD_PARTY_CHIBICC_A_DEPS)				\
		$(THIRD_PARTY_CHIBICC_A)				\
		$(APE)							\
		$(CRT)							\
		o/$(MODE)/third_party/chibicc/chibicc.main.o		\
		$(THIRD_PARTY_CHIBICC_A).pkg
	@$(APELINK)
o/$(MODE)/third_party/chibicc/chibicc2.com.dbg:				\
		$(THIRD_PARTY_CHIBICC_A_DEPS)				\
		$(THIRD_PARTY_CHIBICC2_A)				\
		$(APE)							\
		$(CRT)							\
		o/$(MODE)/third_party/chibicc/chibicc.main.chibicc.o	\
		$(THIRD_PARTY_CHIBICC2_A).pkg
	@$(APELINK)

o/$(MODE)/third_party/chibicc/as.com.dbg:				\
		$(THIRD_PARTY_CHIBICC_A_DEPS)				\
		$(THIRD_PARTY_CHIBICC_A)				\
		$(APE)							\
		$(CRT)							\
		o/$(MODE)/third_party/chibicc/as.main.o			\
		$(THIRD_PARTY_CHIBICC_A).pkg
	@$(APELINK)

o/$(MODE)/third_party/chibicc/chibicc.o:				\
		CPPFLAGS += $(THIRD_PARTY_CHIBICC_DEFINES)

o/$(MODE)/third_party/chibicc/chibicc.chibicc.o:			\
		CHIBICC_FLAGS += $(THIRD_PARTY_CHIBICC_DEFINES)

o/$(MODE)/%.chibicc.o: %.c o/$(MODE)/third_party/chibicc/chibicc.com.dbg
	@ACTION=CHIBICC TARGET=$@ build/do $(CHIBICC) $(CHIBICC_FLAGS) -c -o $@ $<
o/$(MODE)/%.chibicc2.o: %.c o/$(MODE)/third_party/chibicc/chibicc2.com.dbg
	@ACTION=CHIBICC2 TARGET=$@ build/do $(CHIBICC2) $(CHIBICC_FLAGS) -c -o $@ $<

THIRD_PARTY_CHIBICC_LIBS = $(foreach x,$(THIRD_PARTY_CHIBICC_ARTIFACTS),$($(x)))
THIRD_PARTY_CHIBICC_SRCS = $(foreach x,$(THIRD_PARTY_CHIBICC_ARTIFACTS),$($(x)_SRCS))
THIRD_PARTY_CHIBICC_HDRS = $(foreach x,$(THIRD_PARTY_CHIBICC_ARTIFACTS),$($(x)_HDRS))
THIRD_PARTY_CHIBICC_CHECKS = $(foreach x,$(THIRD_PARTY_CHIBICC_ARTIFACTS),$($(x)_CHECKS))
THIRD_PARTY_CHIBICC_OBJS = $(foreach x,$(THIRD_PARTY_CHIBICC_ARTIFACTS),$($(x)_OBJS))
$(THIRD_PARTY_CHIBICC_OBJS): $(BUILD_FILES) third_party/chibicc/chibicc.mk

.PHONY: o/$(MODE)/third_party/chibicc
o/$(MODE)/third_party/chibicc:						\
		o/$(MODE)/third_party/chibicc/test			\
		$(THIRD_PARTY_CHIBICC_BINS)				\
		$(THIRD_PARTY_CHIBICC_CHECKS)
