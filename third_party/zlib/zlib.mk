#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#───vi: set et ft=make ts=8 tw=8 fenc=utf-8 :vi───────────────────────┘

PKGS += THIRD_PARTY_ZLIB

THIRD_PARTY_ZLIB_ARTIFACTS += THIRD_PARTY_ZLIB_A
THIRD_PARTY_ZLIB = $(THIRD_PARTY_ZLIB_A_DEPS) $(THIRD_PARTY_ZLIB_A)
THIRD_PARTY_ZLIB_A = o/$(MODE)/third_party/zlib/zlib.a
THIRD_PARTY_ZLIB_A_FILES := $(wildcard third_party/zlib/*)
THIRD_PARTY_ZLIB_A_HDRS = $(filter %.h,$(THIRD_PARTY_ZLIB_A_FILES))
THIRD_PARTY_ZLIB_A_SRCS_S = $(filter %.S,$(THIRD_PARTY_ZLIB_A_FILES))
THIRD_PARTY_ZLIB_A_SRCS_C = $(filter %.c,$(THIRD_PARTY_ZLIB_A_FILES))

THIRD_PARTY_ZLIB_A_SRCS =				\
	$(THIRD_PARTY_ZLIB_A_SRCS_S)			\
	$(THIRD_PARTY_ZLIB_A_SRCS_C)

THIRD_PARTY_ZLIB_A_OBJS =				\
	$(THIRD_PARTY_ZLIB_A_SRCS_S:%.S=o/$(MODE)/%.o)	\
	$(THIRD_PARTY_ZLIB_A_SRCS_C:%.c=o/$(MODE)/%.o)

THIRD_PARTY_ZLIB_A_CHECKS =				\
	$(THIRD_PARTY_ZLIB_A).pkg			\
	$(THIRD_PARTY_ZLIB_A_HDRS:%=o/$(MODE)/%.ok)

THIRD_PARTY_ZLIB_A_DIRECTDEPS =				\
	LIBC_INTRIN					\
	LIBC_NEXGEN32E					\
	LIBC_STR					\
	LIBC_STUBS

THIRD_PARTY_ZLIB_A_DEPS :=				\
	$(call uniq,$(foreach x,$(THIRD_PARTY_ZLIB_A_DIRECTDEPS),$($(x))))

$(THIRD_PARTY_ZLIB_A):					\
		third_party/zlib/			\
		$(THIRD_PARTY_ZLIB_A).pkg		\
		$(THIRD_PARTY_ZLIB_A_OBJS)

$(THIRD_PARTY_ZLIB_A).pkg:				\
		$(THIRD_PARTY_ZLIB_A_OBJS)		\
		$(foreach x,$(THIRD_PARTY_ZLIB_A_DIRECTDEPS),$($(x)_A).pkg)

o/$(MODE)/third_party/zlib/adler32simd.o: private	\
		OVERRIDE_CFLAGS +=			\
			-mssse3

o/$(MODE)/third_party/zlib/adler32.o: private		\
		OVERRIDE_CPPFLAGS +=			\
			-DADLER32_SIMD_SSSE3

o/$(MODE)/third_party/zlib/deflate.o: private		\
		OVERRIDE_CPPFLAGS +=			\
			-DCRC32_SIMD_SSE42_PCLMUL

$(THIRD_PARTY_ZLIB_A_OBJS): private			\
		OVERRIDE_CFLAGS +=			\
			-ffunction-sections		\
			-fdata-sections

THIRD_PARTY_ZLIB_LIBS = $(foreach x,$(THIRD_PARTY_ZLIB_ARTIFACTS),$($(x)))
THIRD_PARTY_ZLIB_SRCS = $(foreach x,$(THIRD_PARTY_ZLIB_ARTIFACTS),$($(x)_SRCS))
THIRD_PARTY_ZLIB_HDRS = $(foreach x,$(THIRD_PARTY_ZLIB_ARTIFACTS),$($(x)_HDRS))
THIRD_PARTY_ZLIB_BINS = $(foreach x,$(THIRD_PARTY_ZLIB_ARTIFACTS),$($(x)_BINS))
THIRD_PARTY_ZLIB_CHECKS = $(foreach x,$(THIRD_PARTY_ZLIB_ARTIFACTS),$($(x)_CHECKS))
THIRD_PARTY_ZLIB_OBJS = $(foreach x,$(THIRD_PARTY_ZLIB_ARTIFACTS),$($(x)_OBJS))
$(THIRD_PARTY_ZLIB_OBJS): $(BUILD_FILES) third_party/zlib/zlib.mk

.PHONY: o/$(MODE)/third_party/zlib
o/$(MODE)/third_party/zlib:				\
		o/$(MODE)/third_party/zlib/gz		\
		$(THIRD_PARTY_ZLIB_A)			\
		$(THIRD_PARTY_ZLIB_CHECKS)
