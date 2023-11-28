#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#───vi: set et ft=make ts=8 tw=8 fenc=utf-8 :vi───────────────────────┘

PKGS += THIRD_PARTY_RADPAJAMA

################################################################################
# redpajama library code common to both executables below

THIRD_PARTY_RADPAJAMA_ARTIFACTS += THIRD_PARTY_RADPAJAMA_A
THIRD_PARTY_RADPAJAMA = $(THIRD_PARTY_RADPAJAMA_A_DEPS) $(THIRD_PARTY_RADPAJAMA_A)
THIRD_PARTY_RADPAJAMA_A = o/$(MODE)/third_party/radpajama/radpajama.a
THIRD_PARTY_RADPAJAMA_A_OBJS = $(THIRD_PARTY_RADPAJAMA_A_SRCS:%.cc=o/$(MODE)/%.o)
THIRD_PARTY_RADPAJAMA_A_FILES = $(THIRD_PARTY_RADPAJAMA_A_SRCS) $(THIRD_PARTY_RADPAJAMA_A_HDRS)
THIRD_PARTY_RADPAJAMA_A_CHECKS = $(THIRD_PARTY_RADPAJAMA_A).pkg $(THIRD_PARTY_RADPAJAMA_A_HDRS:%=o/$(MODE)/%.okk)

THIRD_PARTY_RADPAJAMA_A_HDRS =						\
	third_party/radpajama/common-gptneox.h				\
	third_party/radpajama/gptneox-util.h				\
	third_party/radpajama/gptneox.h

THIRD_PARTY_RADPAJAMA_A_SRCS =						\
	third_party/radpajama/common-gptneox.cc				\
	third_party/radpajama/gptneox.cc				\

THIRD_PARTY_RADPAJAMA_A_DIRECTDEPS =					\
	LIBC_CALLS							\
	LIBC_FMT							\
	LIBC_INTRIN							\
	LIBC_MEM							\
	LIBC_NEXGEN32E							\
	LIBC_RUNTIME							\
	LIBC_STDIO							\
	LIBC_STR							\
	LIBC_SYSV							\
	LIBC_THREAD							\
	LIBC_TINYMATH							\
	THIRD_PARTY_COMPILER_RT						\
	THIRD_PARTY_GGML						\
	THIRD_PARTY_LIBCXX

THIRD_PARTY_RADPAJAMA_A_DEPS :=						\
	$(call uniq,$(foreach x,$(THIRD_PARTY_RADPAJAMA_A_DIRECTDEPS),$($(x))))

$(THIRD_PARTY_RADPAJAMA_A):						\
		third_party/radpajama/					\
		$(THIRD_PARTY_RADPAJAMA_A).pkg				\
		$(THIRD_PARTY_RADPAJAMA_A_OBJS)

$(THIRD_PARTY_RADPAJAMA_A).pkg:						\
		$(THIRD_PARTY_RADPAJAMA_A_OBJS)				\
		$(foreach x,$(THIRD_PARTY_RADPAJAMA_A_DIRECTDEPS),$($(x)_A).pkg)

################################################################################
# two executable programs for running inference on redpajama models
#
#     make -j8 o//third_party/radpajama/radpajama.com
#     make -j8 o//third_party/radpajama/radpajama-chat.com
#     make -j8 o//third_party/radpajama/radpajama-copy.com
#     make -j8 o//third_party/radpajama/radpajama-quantize.com

THIRD_PARTY_RADPAJAMA_ARTIFACTS += THIRD_PARTY_RADPAJAMA_MAIN
THIRD_PARTY_RADPAJAMA_MAIN_OBJS = $(THIRD_PARTY_RADPAJAMA_MAIN_SRCS:%.cc=o/$(MODE)/%.o)
THIRD_PARTY_RADPAJAMA_MAIN_BINS = $(THIRD_PARTY_RADPAJAMA_COMS) $(THIRD_PARTY_RADPAJAMA_COMS:%=%.dbg)

THIRD_PARTY_RADPAJAMA_MAIN_COMS =					\
	o/$(MODE)/third_party/radpajama/radpajama.com			\
	o/$(MODE)/third_party/radpajama/radpajama-chat.com		\
	o/$(MODE)/third_party/radpajama/radpajama-copy.com		\
	o/$(MODE)/third_party/radpajama/radpajama-quantize.com

THIRD_PARTY_RADPAJAMA_MAIN_SRCS =					\
	third_party/radpajama/main-redpajama.cc				\
	third_party/radpajama/main-redpajama-chat.cc			\
	third_party/radpajama/copy-gptneox.cc				\
	third_party/radpajama/quantize-gptneox.cc

THIRD_PARTY_RADPAJAMA_MAIN_DIRECTDEPS =					\
	LIBC_CALLS							\
	LIBC_FMT							\
	LIBC_INTRIN							\
	LIBC_LOG							\
	LIBC_NEXGEN32E							\
	LIBC_RUNTIME							\
        LIBC_PROC							\
	LIBC_STDIO							\
	LIBC_STR							\
	THIRD_PARTY_GGML						\
	THIRD_PARTY_RADPAJAMA						\
	THIRD_PARTY_LIBCXX

THIRD_PARTY_RADPAJAMA_MAIN_DEPS :=					\
	$(call uniq,$(foreach x,$(THIRD_PARTY_RADPAJAMA_MAIN_DIRECTDEPS),$($(x))))

o/$(MODE)/third_party/radpajama/main.pkg:				\
		$(THIRD_PARTY_RADPAJAMA_MAIN_OBJS)			\
		$(foreach x,$(THIRD_PARTY_RADPAJAMA_MAIN_DIRECTDEPS),$($(x)_A).pkg)

o/$(MODE)/third_party/radpajama/radpajama.com.dbg:			\
		o/$(MODE)/third_party/radpajama/main.pkg		\
		$(THIRD_PARTY_RADPAJAMA_MAIN_DEPS)			\
		o/$(MODE)/third_party/radpajama/main-redpajama.o	\
		$(CRT)							\
		$(APE_NO_MODIFY_SELF)
	@$(APELINK)

o/$(MODE)/third_party/radpajama/radpajama-chat.com.dbg:			\
		o/$(MODE)/third_party/radpajama/main.pkg		\
		$(THIRD_PARTY_RADPAJAMA_MAIN_DEPS)			\
		o/$(MODE)/third_party/radpajama/main-redpajama-chat.o	\
		$(CRT)							\
		$(APE_NO_MODIFY_SELF)
	@$(APELINK)

o/$(MODE)/third_party/radpajama/radpajama-copy.com.dbg:			\
		o/$(MODE)/third_party/radpajama/main.pkg		\
		$(THIRD_PARTY_RADPAJAMA_MAIN_DEPS)			\
		o/$(MODE)/third_party/radpajama/copy-gptneox.o		\
		$(CRT)							\
		$(APE_NO_MODIFY_SELF)
	@$(APELINK)

o/$(MODE)/third_party/radpajama/radpajama-quantize.com.dbg:		\
		o/$(MODE)/third_party/radpajama/main.pkg		\
		$(THIRD_PARTY_RADPAJAMA_MAIN_DEPS)			\
		o/$(MODE)/third_party/radpajama/quantize-gptneox.o	\
		$(CRT)							\
		$(APE_NO_MODIFY_SELF)
	@$(APELINK)

################################################################################
# package level definitions

THIRD_PARTY_RADPAJAMA_LIBS = $(foreach x,$(THIRD_PARTY_RADPAJAMA_ARTIFACTS),$($(x)))
THIRD_PARTY_RADPAJAMA_COMS = $(foreach x,$(THIRD_PARTY_RADPAJAMA_ARTIFACTS),$($(x)_COMS))
THIRD_PARTY_RADPAJAMA_BINS = $(foreach x,$(THIRD_PARTY_RADPAJAMA_ARTIFACTS),$($(x)_BINS))
THIRD_PARTY_RADPAJAMA_SRCS = $(foreach x,$(THIRD_PARTY_RADPAJAMA_ARTIFACTS),$($(x)_SRCS))
THIRD_PARTY_RADPAJAMA_HDRS = $(foreach x,$(THIRD_PARTY_RADPAJAMA_ARTIFACTS),$($(x)_HDRS))
THIRD_PARTY_RADPAJAMA_OBJS = $(foreach x,$(THIRD_PARTY_RADPAJAMA_ARTIFACTS),$($(x)_OBJS))
THIRD_PARTY_RADPAJAMA_CHECKS = $(foreach x,$(THIRD_PARTY_RADPAJAMA_ARTIFACTS),$($(x)_CHECKS))
$(THIRD_PARTY_RADPAJAMA_OBJS): third_party/radpajama/BUILD.mk

.PHONY: o/$(MODE)/third_party/radpajama
o/$(MODE)/third_party/radpajama:					\
		$(THIRD_PARTY_RADPAJAMA_BINS)				\
		$(THIRD_PARTY_RADPAJAMA_CHECKS)
