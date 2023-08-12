#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#───vi: set et ft=make ts=8 tw=8 fenc=utf-8 :vi───────────────────────┘

PKGS += THIRD_PARTY_GGML

################################################################################
# single file machine learning framework written in c
# make -j8 o//third_party/ggml/ggml.a

THIRD_PARTY_GGML_ARTIFACTS += THIRD_PARTY_GGML_A
THIRD_PARTY_GGML = $(THIRD_PARTY_GGML_A_DEPS) $(THIRD_PARTY_GGML_A)
THIRD_PARTY_GGML_A = o/$(MODE)/third_party/ggml/ggml.a
THIRD_PARTY_GGML_A_OBJS = $(THIRD_PARTY_GGML_A_SRCS:%.c=o/$(MODE)/%.o)
THIRD_PARTY_GGML_A_FILES = $(THIRD_PARTY_GGML_A_SRCS) $(THIRD_PARTY_GGML_A_HDRS)
THIRD_PARTY_GGML_A_CHECKS = $(THIRD_PARTY_GGML_A).pkg $(THIRD_PARTY_GGML_A_HDRS:%=o/$(MODE)/%.ok)

THIRD_PARTY_GGML_A_HDRS =						\
	third_party/ggml/fp16.h						\
	third_party/ggml/ggml.h						\
	third_party/ggml/ggjt.v1.q4_0.h					\
	third_party/ggml/ggjt.v1.q4_1.h					\
	third_party/ggml/ggjt.v1.q4_2.h					\
	third_party/ggml/ggjt.v1.q5_0.h					\
	third_party/ggml/ggjt.v1.q5_1.h					\
	third_party/ggml/ggjt.v1.q8_0.h					\
	third_party/ggml/ggjt.v1.q8_1.h					\
	third_party/ggml/ggjt.v2.q4_0.h					\
	third_party/ggml/ggjt.v2.q4_1.h					\
	third_party/ggml/ggjt.v2.q5_0.h					\
	third_party/ggml/ggjt.v2.q5_1.h					\
	third_party/ggml/ggjt.v2.q8_0.h					\
	third_party/ggml/ggjt.v2.q8_1.h					\
	third_party/ggml/fp16.internal.h				\
	third_party/ggml/ggjt.v1.internal.h				\
	third_party/ggml/ggjt.v2.internal.h

THIRD_PARTY_GGML_A_SRCS =						\
	third_party/ggml/fp16.c						\
	third_party/ggml/ggml.c						\
	third_party/ggml/ggjt.v1.c					\
	third_party/ggml/ggjt.v1.q4_0.c					\
	third_party/ggml/ggjt.v1.q4_1.c					\
	third_party/ggml/ggjt.v1.q4_2.c					\
	third_party/ggml/ggjt.v1.q5_0.c					\
	third_party/ggml/ggjt.v1.q5_1.c					\
	third_party/ggml/ggjt.v1.q8_0.c					\
	third_party/ggml/ggjt.v1.q8_1.c					\
	third_party/ggml/ggjt.v2.c					\
	third_party/ggml/ggjt.v2.q4_0.c					\
	third_party/ggml/ggjt.v2.q4_1.c					\
	third_party/ggml/ggjt.v2.q5_0.c					\
	third_party/ggml/ggjt.v2.q5_1.c					\
	third_party/ggml/ggjt.v2.q8_0.c					\
	third_party/ggml/ggjt.v2.q8_1.c

THIRD_PARTY_GGML_A_DIRECTDEPS =						\
	LIBC_CALLS							\
	LIBC_INTRIN							\
	LIBC_FMT							\
	LIBC_MEM							\
	LIBC_NEXGEN32E							\
	LIBC_RUNTIME							\
	LIBC_STDIO							\
	LIBC_THREAD							\
	LIBC_STR							\
	LIBC_SYSV							\
	LIBC_TINYMATH							\
	THIRD_PARTY_COMPILER_RT

THIRD_PARTY_GGML_A_DEPS :=						\
	$(call uniq,$(foreach x,$(THIRD_PARTY_GGML_A_DIRECTDEPS),$($(x))))

$(THIRD_PARTY_GGML_A):							\
		third_party/ggml/					\
		$(THIRD_PARTY_GGML_A).pkg				\
		$(THIRD_PARTY_GGML_A_OBJS)

$(THIRD_PARTY_GGML_A).pkg:						\
		$(THIRD_PARTY_GGML_A_OBJS)				\
		$(foreach x,$(THIRD_PARTY_GGML_A_DIRECTDEPS),$($(x)_A).pkg)

$(THIRD_PARTY_GGML_A_OBJS): private					\
		CFLAGS +=						\
			-O3						\
			-ffunction-sections				\
			-fdata-sections

ifeq ($(ARCH), x86_64)
$(THIRD_PARTY_GGML_A_OBJS): private					\
		CFLAGS +=						\
			-msse3						\
			-mavx						\
			-mavx2						\
			-mf16c						\
			-mfma
endif

o/opt/third_party/ggml/ggml.o: private					\
		CFLAGS +=						\
			-x-no-pg

################################################################################
# command for running inference on large language models
# make -j8 o//third_party/ggml/llama.com

THIRD_PARTY_GGML_ARTIFACTS += THIRD_PARTY_GGML_LLAMA
THIRD_PARTY_GGML_LLAMA = o/$(MODE)/third_party/ggml/llama.com
THIRD_PARTY_GGML_LLAMA_OBJS = $(THIRD_PARTY_GGML_LLAMA_SRCS:%.cc=o/$(MODE)/%.o)
THIRD_PARTY_GGML_LLAMA_FILES := $(THIRD_PARTY_GGML_LLAMA_SRCS) $(THIRD_PARTY_GGML_LLAMA_HDRS)
THIRD_PARTY_GGML_LLAMA_CHECKS = $(THIRD_PARTY_GGML_LLAMA).pkg $(THIRD_PARTY_GGML_LLAMA_HDRS:%=o/$(MODE)/%.okk)

THIRD_PARTY_GGML_LLAMA_HDRS =						\
	third_party/ggml/common.cc					\
	third_party/ggml/llama.h					\
	third_party/ggml/llama_util.h					\
	third_party/ggml/common.h

THIRD_PARTY_GGML_LLAMA_SRCS =						\
	third_party/ggml/main.cc					\
	third_party/ggml/llama.cc					\
	third_party/ggml/common.cc					\
	third_party/ggml/quantize.cc					\
	third_party/ggml/perplexity.cc

THIRD_PARTY_GGML_LLAMA_DIRECTDEPS =					\
	LIBC_CALLS							\
	LIBC_FMT							\
	LIBC_INTRIN							\
	LIBC_MEM							\
	LIBC_NEXGEN32E							\
	LIBC_RUNTIME							\
	LIBC_STDIO							\
	LIBC_LOG							\
	LIBC_STR							\
	LIBC_SYSV							\
	LIBC_SYSV_CALLS							\
	LIBC_THREAD							\
	LIBC_TINYMATH							\
	THIRD_PARTY_GGML						\
	THIRD_PARTY_LIBCXX

THIRD_PARTY_GGML_LLAMA_DEPS :=						\
	$(call uniq,$(foreach x,$(THIRD_PARTY_GGML_LLAMA_DIRECTDEPS),$($(x))))

$(THIRD_PARTY_GGML_LLAMA).dbg:						\
		$(THIRD_PARTY_GGML_LLAMA).pkg				\
		$(THIRD_PARTY_GGML_LLAMA_DEPS)				\
		o/$(MODE)/third_party/ggml/companionai.txt.zip.o	\
		o/$(MODE)/third_party/ggml/common.o			\
		o/$(MODE)/third_party/ggml/llama.o			\
		o/$(MODE)/third_party/ggml/main.o			\
		$(CRT)							\
		$(APE_NO_MODIFY_SELF)
	@$(APELINK)

o/$(MODE)/third_party/ggml/quantize.com.dbg:				\
		$(THIRD_PARTY_GGML_LLAMA).pkg				\
		$(THIRD_PARTY_GGML_LLAMA_DEPS)				\
		o/$(MODE)/third_party/ggml/common.o			\
		o/$(MODE)/third_party/ggml/llama.o			\
		o/$(MODE)/third_party/ggml/quantize.o			\
		$(CRT)							\
		$(APE_NO_MODIFY_SELF)
	@$(APELINK)

o/$(MODE)/third_party/ggml/perplexity.com.dbg:				\
		$(THIRD_PARTY_GGML_LLAMA).pkg				\
		$(THIRD_PARTY_GGML_LLAMA_DEPS)				\
		o/$(MODE)/third_party/ggml/common.o			\
		o/$(MODE)/third_party/ggml/llama.o			\
		o/$(MODE)/third_party/ggml/perplexity.o			\
		$(CRT)							\
		$(APE_NO_MODIFY_SELF)
	@$(APELINK)

$(THIRD_PARTY_GGML_LLAMA).pkg:						\
		$(THIRD_PARTY_GGML_LLAMA_OBJS)				\
		$(foreach x,$(THIRD_PARTY_GGML_LLAMA_DIRECTDEPS),$($(x)_A).pkg)

o/$(MODE)/third_party/ggml/companionai.txt.zip.o: private		\
		ZIPOBJ_FLAGS +=						\
			-B

o/$(MODE)/third_party/ggml/ggml.o: private QUOTA = -C64
o/$(MODE)/third_party/ggml/llama.o: private QUOTA = -C64

################################################################################

THIRD_PARTY_GGML_COMS =							\
	$(THIRD_PARTY_GGML_LLAMA)					\
	o/$(MODE)/third_party/ggml/quantize.com				\
	o/$(MODE)/third_party/ggml/perplexity.com

THIRD_PARTY_GGML_BINS = $(THIRD_PARTY_GGML_COMS) $(THIRD_PARTY_GGML_COMS:%=%.dbg)
THIRD_PARTY_GGML_LIBS = $(foreach x,$(THIRD_PARTY_GGML_ARTIFACTS),$($(x)))
THIRD_PARTY_GGML_SRCS = $(foreach x,$(THIRD_PARTY_GGML_ARTIFACTS),$($(x)_SRCS))
THIRD_PARTY_GGML_HDRS = $(foreach x,$(THIRD_PARTY_GGML_ARTIFACTS),$($(x)_HDRS))
THIRD_PARTY_GGML_OBJS = $(foreach x,$(THIRD_PARTY_GGML_ARTIFACTS),$($(x)_OBJS))
THIRD_PARTY_GGML_CHECKS = $(foreach x,$(THIRD_PARTY_GGML_ARTIFACTS),$($(x)_CHECKS))
$(THIRD_PARTY_GGML_OBJS): third_party/ggml/ggml.mk

.PHONY: o/$(MODE)/third_party/ggml
o/$(MODE)/third_party/ggml:						\
		$(THIRD_PARTY_GGML_BINS)				\
		$(THIRD_PARTY_GGML_CHECKS)
