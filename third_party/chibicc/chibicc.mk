#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#───vi: set et ft=make ts=8 tw=8 fenc=utf-8 :vi───────────────────────┘

PKGS += THIRD_PARTY_CHIBICC

THIRD_PARTY_CHIBICC_ARTIFACTS += THIRD_PARTY_CHIBICC_A
THIRD_PARTY_CHIBICC = $(THIRD_PARTY_CHIBICC_A_DEPS) $(THIRD_PARTY_CHIBICC_A)
THIRD_PARTY_CHIBICC_A = o/$(MODE)/third_party/chibicc/chibicc.a
THIRD_PARTY_CHIBICC_A_FILES := $(wildcard third_party/chibicc/*)
THIRD_PARTY_CHIBICC_A_HDRS = $(filter %.h,$(THIRD_PARTY_CHIBICC_A_FILES))
THIRD_PARTY_CHIBICC_A_SRCS_S = $(filter %.S,$(THIRD_PARTY_CHIBICC_A_FILES))
THIRD_PARTY_CHIBICC_A_SRCS_C = $(filter %.c,$(THIRD_PARTY_CHIBICC_A_FILES))

THIRD_PARTY_CHIBICC_BINS =					\
	o/$(MODE)/third_party/chibicc/chibicc.com

THIRD_PARTY_CHIBICC_A_SRCS =					\
	$(THIRD_PARTY_CHIBICC_A_SRCS_S)				\
	$(THIRD_PARTY_CHIBICC_A_SRCS_C)

THIRD_PARTY_CHIBICC_A_OBJS =					\
	$(THIRD_PARTY_CHIBICC_A_SRCS:%=o/$(MODE)/%.zip.o)	\
	$(THIRD_PARTY_CHIBICC_A_SRCS_S:%.S=o/$(MODE)/%.o)	\
	$(THIRD_PARTY_CHIBICC_A_SRCS_C:%.c=o/$(MODE)/%.o)

THIRD_PARTY_CHIBICC_A_CHECKS =					\
	$(THIRD_PARTY_CHIBICC_A).pkg				\
	$(THIRD_PARTY_CHIBICC_A_HDRS:%=o/$(MODE)/%.ok)

THIRD_PARTY_CHIBICC_A_DIRECTDEPS =				\
	LIBC_STR						\
	LIBC_STUBS						\
	LIBC_FMT						\
	LIBC_NEXGEN32E						\
	LIBC_UNICODE						\
	LIBC_STDIO						\
	LIBC_MEM						\
	LIBC_LOG						\
	LIBC_CALLS						\
	LIBC_CALLS_HEFTY					\
	LIBC_TIME						\
	LIBC_X							\
	LIBC_CONV						\
	LIBC_RUNTIME						\
	THIRD_PARTY_GDTOA

THIRD_PARTY_CHIBICC_A_DEPS :=					\
	$(call uniq,$(foreach x,$(THIRD_PARTY_CHIBICC_A_DIRECTDEPS),$($(x))))

$(THIRD_PARTY_CHIBICC_A):					\
		third_party/chibicc/				\
		$(THIRD_PARTY_CHIBICC_A).pkg			\
		$(THIRD_PARTY_CHIBICC_A_OBJS)

$(THIRD_PARTY_CHIBICC_A).pkg:					\
		$(THIRD_PARTY_CHIBICC_A_OBJS)			\
		$(foreach x,$(THIRD_PARTY_CHIBICC_A_DIRECTDEPS),$($(x)_A).pkg)

o/$(MODE)/third_party/chibicc/%.com.dbg:			\
		$(THIRD_PARTY_CHIBICC_A_DEPS)			\
		$(THIRD_PARTY_CHIBICC_A)			\
		o/$(MODE)/third_party/chibicc/%.o		\
		$(THIRD_PARTY_CHIBICC_A).pkg			\
		$(CRT)						\
		$(APE)
	@$(APELINK)

THIRD_PARTY_CHIBICC_LIBS = $(foreach x,$(THIRD_PARTY_CHIBICC_ARTIFACTS),$($(x)))
THIRD_PARTY_CHIBICC_SRCS = $(foreach x,$(THIRD_PARTY_CHIBICC_ARTIFACTS),$($(x)_SRCS))
THIRD_PARTY_CHIBICC_HDRS = $(foreach x,$(THIRD_PARTY_CHIBICC_ARTIFACTS),$($(x)_HDRS))
THIRD_PARTY_CHIBICC_CHECKS = $(foreach x,$(THIRD_PARTY_CHIBICC_ARTIFACTS),$($(x)_CHECKS))
THIRD_PARTY_CHIBICC_OBJS = $(foreach x,$(THIRD_PARTY_CHIBICC_ARTIFACTS),$($(x)_OBJS))
$(THIRD_PARTY_CHIBICC_OBJS): $(BUILD_FILES) third_party/chibicc/chibicc.mk

.PHONY: o/$(MODE)/third_party/chibicc
o/$(MODE)/third_party/chibicc:					\
		$(THIRD_PARTY_CHIBICC_BINS)			\
		$(THIRD_PARTY_CHIBICC_CHECKS)
