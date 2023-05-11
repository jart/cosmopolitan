#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#───vi: set et ft=make ts=8 tw=8 fenc=utf-8 :vi───────────────────────┘

PKGS += THIRD_PARTY_REDPAJAMA

################################################################################
# single file machine learning framework written in c
# make -j8 o//third_party/radpajama/ggml.a

THIRD_PARTY_REDPAJAMA_ARTIFACTS += THIRD_PARTY_REDPAJAMA_A
THIRD_PARTY_REDPAJAMA = $(THIRD_PARTY_REDPAJAMA_A_DEPS) $(THIRD_PARTY_REDPAJAMA_A)
THIRD_PARTY_REDPAJAMA_A = o/$(MODE)/third_party/radpajama/ggml.a
THIRD_PARTY_REDPAJAMA_A_HDRS = third_party/radpajama/ggml.h
THIRD_PARTY_REDPAJAMA_A_SRCS = third_party/radpajama/ggml.c
THIRD_PARTY_REDPAJAMA_A_OBJS = $(THIRD_PARTY_REDPAJAMA_A_SRCS:%.c=o/$(MODE)/%.o)
THIRD_PARTY_REDPAJAMA_A_FILES = $(THIRD_PARTY_REDPAJAMA_A_SRCS) $(THIRD_PARTY_REDPAJAMA_A_HDRS)
THIRD_PARTY_REDPAJAMA_A_CHECKS = $(THIRD_PARTY_REDPAJAMA_A).pkg $(THIRD_PARTY_REDPAJAMA_A_HDRS:%=o/$(MODE)/%.ok)

THIRD_PARTY_REDPAJAMA_A_DIRECTDEPS =						\
	LIBC_CALLS							\
	LIBC_INTRIN							\
	LIBC_MEM							\
	LIBC_NEXGEN32E							\
	LIBC_RUNTIME							\
	LIBC_STDIO							\
	LIBC_THREAD							\
	LIBC_STR							\
	LIBC_STUBS							\
	LIBC_SYSV							\
	LIBC_TINYMATH							\
	THIRD_PARTY_COMPILER_RT

THIRD_PARTY_REDPAJAMA_A_DEPS :=						\
	$(call uniq,$(foreach x,$(THIRD_PARTY_REDPAJAMA_A_DIRECTDEPS),$($(x))))

$(THIRD_PARTY_REDPAJAMA_A):							\
		third_party/radpajama/					\
		$(THIRD_PARTY_REDPAJAMA_A).pkg				\
		$(THIRD_PARTY_REDPAJAMA_A_OBJS)

$(THIRD_PARTY_REDPAJAMA_A).pkg:						\
		$(THIRD_PARTY_REDPAJAMA_A_OBJS)				\
		$(foreach x,$(THIRD_PARTY_REDPAJAMA_A_DIRECTDEPS),$($(x)_A).pkg)

$(THIRD_PARTY_REDPAJAMA_A_OBJS): private					\
		OVERRIDE_CFLAGS +=					\
			-O3						\
			-ffunction-sections				\
			-fdata-sections

ifeq ($(ARCH), x86_64)
$(THIRD_PARTY_REDPAJAMA_A_OBJS): private					\
		OVERRIDE_CFLAGS +=					\
			-msse3						\
			-mavx						\
			-mavx2						\
			-mf16c						\
			-mfma
endif

################################################################################
# command for running inference on large language models
# make -j8 o//third_party/radpajama/radpajama.com

THIRD_PARTY_REDPAJAMA_ARTIFACTS += THIRD_PARTY_REDPAJAMA_LLAMA
THIRD_PARTY_REDPAJAMA_LLAMA = o/$(MODE)/third_party/radpajama/radpajama.com
THIRD_PARTY_REDPAJAMA_LLAMA_HDRS = third_party/radpajama/llama.h third_party/radpajama/llama_util.h third_party/radpajama/common.h
THIRD_PARTY_REDPAJAMA_LLAMA_SRCS = third_party/radpajama/radpajama.cc third_party/radpajama/common.cc
THIRD_PARTY_REDPAJAMA_LLAMA_OBJS = $(THIRD_PARTY_REDPAJAMA_LLAMA_SRCS:%.cc=o/$(MODE)/%.o)
THIRD_PARTY_REDPAJAMA_LLAMA_FILES := $(THIRD_PARTY_REDPAJAMA_LLAMA_SRCS) $(THIRD_PARTY_REDPAJAMA_LLAMA_HDRS)
THIRD_PARTY_REDPAJAMA_LLAMA_CHECKS = $(THIRD_PARTY_REDPAJAMA_LLAMA).pkg $(THIRD_PARTY_REDPAJAMA_LLAMA_HDRS:%=o/$(MODE)/%.okk)

THIRD_PARTY_REDPAJAMA_LLAMA_DIRECTDEPS =					\
	LIBC_CALLS							\
	LIBC_FMT							\
	LIBC_INTRIN							\
	LIBC_MEM							\
	LIBC_NEXGEN32E							\
	LIBC_RUNTIME							\
	LIBC_STDIO							\
	LIBC_LOG							\
	LIBC_STR							\
	LIBC_STUBS							\
	LIBC_SYSV							\
	LIBC_THREAD							\
	LIBC_TINYMATH							\
	LIBC_ZIPOS							\
	THIRD_PARTY_REDPAJAMA						\
	THIRD_PARTY_LIBCXX

THIRD_PARTY_REDPAJAMA_LLAMA_DEPS :=						\
	$(call uniq,$(foreach x,$(THIRD_PARTY_REDPAJAMA_LLAMA_DIRECTDEPS),$($(x))))

$(THIRD_PARTY_REDPAJAMA_LLAMA).dbg:						\
		$(THIRD_PARTY_REDPAJAMA_LLAMA).pkg				\
		$(THIRD_PARTY_REDPAJAMA_LLAMA_DEPS)				\
		o/$(MODE)/third_party/radpajama/radpajama.txt.zip.o	\
		o/$(MODE)/third_party/radpajama/common.o			\
		o/$(MODE)/third_party/radpajama/llama.o			\
		o/$(MODE)/third_party/radpajama/radpajama.o			\
		$(CRT)							\
		$(APE_NO_MODIFY_SELF)
	@$(APELINK)

$(THIRD_PARTY_REDPAJAMA_LLAMA).pkg:						\
		$(THIRD_PARTY_REDPAJAMA_LLAMA_OBJS)				\
		$(foreach x,$(THIRD_PARTY_REDPAJAMA_LLAMA_DIRECTDEPS),$($(x)_A).pkg)

o/$(MODE)/third_party/radpajama/radpajama.txt.zip.o: private		\
		ZIPOBJ_FLAGS +=						\
			-B

################################################################################

THIRD_PARTY_REDPAJAMA_COMS = $(THIRD_PARTY_REDPAJAMA_LLAMA)
THIRD_PARTY_REDPAJAMA_BINS = $(THIRD_PARTY_REDPAJAMA_COMS) $(THIRD_PARTY_REDPAJAMA_COMS:%=%.dbg)
THIRD_PARTY_REDPAJAMA_LIBS = $(foreach x,$(THIRD_PARTY_REDPAJAMA_ARTIFACTS),$($(x)))
THIRD_PARTY_REDPAJAMA_SRCS = $(foreach x,$(THIRD_PARTY_REDPAJAMA_ARTIFACTS),$($(x)_SRCS))
THIRD_PARTY_REDPAJAMA_HDRS = $(foreach x,$(THIRD_PARTY_REDPAJAMA_ARTIFACTS),$($(x)_HDRS))
THIRD_PARTY_REDPAJAMA_OBJS = $(foreach x,$(THIRD_PARTY_REDPAJAMA_ARTIFACTS),$($(x)_OBJS))
THIRD_PARTY_REDPAJAMA_CHECKS = $(foreach x,$(THIRD_PARTY_REDPAJAMA_ARTIFACTS),$($(x)_CHECKS))
$(THIRD_PARTY_REDPAJAMA_OBJS): third_party/radpajama/radpajama.mk

.PHONY: o/$(MODE)/third_party/radpajama
o/$(MODE)/third_party/radpajama:						\
		$(THIRD_PARTY_REDPAJAMA_BINS)				\
		$(THIRD_PARTY_REDPAJAMA_CHECKS)
