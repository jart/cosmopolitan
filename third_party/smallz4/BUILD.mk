#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#── vi: set noet ft=make ts=8 sw=8 fenc=utf-8 :vi ────────────────────┘

PKGS += THIRD_PARTY_SMALLZ4

THIRD_PARTY_SMALLZ4_SRCS = $(THIRD_PARTY_SMALLZ4_A_SRCS)
THIRD_PARTY_SMALLZ4_HDRS = $(THIRD_PARTY_SMALLZ4_A_HDRS)
THIRD_PARTY_SMALLZ4_BINS = $(THIRD_PARTY_SMALLZ4_COMS) $(THIRD_PARTY_SMALLZ4_COMS:%=%.dbg)

THIRD_PARTY_SMALLZ4_ARTIFACTS += THIRD_PARTY_SMALLZ4_A
THIRD_PARTY_SMALLZ4 = $(THIRD_PARTY_SMALLZ4_A_DEPS) $(THIRD_PARTY_SMALLZ4_A)
THIRD_PARTY_SMALLZ4_A = o/$(MODE)/third_party/smallz4/smallz4.a
THIRD_PARTY_SMALLZ4_A_FILES := $(wildcard third_party/smallz4/*)
THIRD_PARTY_SMALLZ4_A_HDRS = $(filter %.hh,$(THIRD_PARTY_SMALLZ4_A_FILES))
THIRD_PARTY_SMALLZ4_A_SRCS_S = $(filter %.S,$(THIRD_PARTY_SMALLZ4_A_FILES))
THIRD_PARTY_SMALLZ4_A_SRCS_C = $(filter %.c,$(THIRD_PARTY_SMALLZ4_A_FILES))
THIRD_PARTY_SMALLZ4_A_SRCS_CC = $(filter %.cc,$(THIRD_PARTY_SMALLZ4_A_FILES))

THIRD_PARTY_SMALLZ4_A_SRCS =					\
	$(THIRD_PARTY_SMALLZ4_A_SRCS_S)				\
	$(THIRD_PARTY_SMALLZ4_A_SRCS_C)				\
	$(THIRD_PARTY_SMALLZ4_A_SRCS_CC)

THIRD_PARTY_SMALLZ4_A_OBJS =					\
	$(THIRD_PARTY_SMALLZ4_A_SRCS_S:%.S=o/$(MODE)/%.o)	\
	$(THIRD_PARTY_SMALLZ4_A_SRCS_C:%.c=o/$(MODE)/%.o)	\
	$(THIRD_PARTY_SMALLZ4_A_SRCS_CC:%.cc=o/$(MODE)/%.o)

THIRD_PARTY_SMALLZ4_A_DIRECTDEPS =				\
	LIBC_FMT						\
	LIBC_INTRIN						\
	LIBC_LOG						\
	LIBC_MEM						\
	LIBC_NEXGEN32E						\
	LIBC_RUNTIME						\
	LIBC_CALLS						\
	LIBC_STDIO						\
	LIBC_STR						\
	THIRD_PARTY_LIBCXX

THIRD_PARTY_SMALLZ4_A_DEPS :=					\
	$(call uniq,$(foreach x,$(THIRD_PARTY_SMALLZ4_A_DIRECTDEPS),$($(x))))

THIRD_PARTY_SMALLZ4_A_CHECKS =					\
	$(THIRD_PARTY_SMALLZ4_A).pkg				\
	$(THIRD_PARTY_SMALLZ4_A_HDRS:%=o/$(MODE)/%.ok)

$(THIRD_PARTY_SMALLZ4_A):					\
		third_party/smallz4/				\
		$(THIRD_PARTY_SMALLZ4_A).pkg			\
		$(THIRD_PARTY_SMALLZ4_A_OBJS)

$(THIRD_PARTY_SMALLZ4_A).pkg:					\
		$(THIRD_PARTY_SMALLZ4_A_OBJS)			\
		$(foreach x,$(THIRD_PARTY_SMALLZ4_A_DIRECTDEPS),$($(x)_A).pkg)

o/$(MODE)/third_party/smallz4/smallz4.com.dbg:			\
		$(THIRD_PARTY_SMALLZ4)				\
		o/$(MODE)/third_party/smallz4/smallz4.o		\
		$(CRT)						\
		$(APE_NO_MODIFY_SELF)
	@$(APELINK)

o/$(MODE)/third_party/smallz4/smallz4cat.com.dbg:		\
		$(THIRD_PARTY_SMALLZ4)				\
		o/$(MODE)/third_party/smallz4/smallz4cat.o	\
		$(CRT)						\
		$(APE_NO_MODIFY_SELF)
	@$(APELINK)

THIRD_PARTY_SMALLZ4_COMS =					\
	o/$(MODE)/third_party/smallz4/smallz4.com		\
	o/$(MODE)/third_party/smallz4/smallz4cat.com

THIRD_PARTY_SMALLZ4_LIBS = $(foreach x,$(THIRD_PARTY_SMALLZ4_ARTIFACTS),$($(x)))
THIRD_PARTY_SMALLZ4_SRCS = $(foreach x,$(THIRD_PARTY_SMALLZ4_ARTIFACTS),$($(x)_SRCS))
THIRD_PARTY_SMALLZ4_CHECKS = $(foreach x,$(THIRD_PARTY_SMALLZ4_ARTIFACTS),$($(x)_CHECKS))
THIRD_PARTY_SMALLZ4_OBJS = $(foreach x,$(THIRD_PARTY_SMALLZ4_ARTIFACTS),$($(x)_OBJS))
$(THIRD_PARTY_SMALLZ4_OBJS): $(BUILD_FILES) third_party/smallz4/BUILD.mk

.PHONY: o/$(MODE)/third_party/smallz4
o/$(MODE)/third_party/smallz4:					\
		$(THIRD_PARTY_SMALLZ4_BINS)			\
		$(THIRD_PARTY_SMALLZ4_CHECKS)
