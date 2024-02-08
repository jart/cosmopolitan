#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#── vi: set noet ft=make ts=8 sw=8 fenc=utf-8 :vi ────────────────────┘

PKGS += THIRD_PARTY_ZLIB

THIRD_PARTY_ZLIB_ARTIFACTS += THIRD_PARTY_ZLIB_A
THIRD_PARTY_ZLIB = $(THIRD_PARTY_ZLIB_A_DEPS) $(THIRD_PARTY_ZLIB_A)
THIRD_PARTY_ZLIB_A = o/$(MODE)/third_party/zlib/zlib.a
THIRD_PARTY_ZLIB_A_FILES := $(wildcard third_party/zlib/*)
THIRD_PARTY_ZLIB_A_HDRS = $(filter %.h,$(THIRD_PARTY_ZLIB_A_FILES))
THIRD_PARTY_ZLIB_A_SRCS = $(filter %.c,$(THIRD_PARTY_ZLIB_A_FILES))
THIRD_PARTY_ZLIB_A_INCS = $(filter %.inc,$(THIRD_PARTY_ZLIB_A_FILES))
THIRD_PARTY_ZLIB_A_OBJS = $(THIRD_PARTY_ZLIB_A_SRCS:%.c=o/$(MODE)/%.o)

THIRD_PARTY_ZLIB_A_CHECKS =				\
	$(THIRD_PARTY_ZLIB_A).pkg			\
	$(THIRD_PARTY_ZLIB_A_HDRS:%=o/$(MODE)/%.ok)

THIRD_PARTY_ZLIB_A_DIRECTDEPS =				\
	LIBC_INTRIN					\
	LIBC_NEXGEN32E					\
	LIBC_MEM					\
	LIBC_STR					\
	LIBC_SYSV

THIRD_PARTY_ZLIB_A_DEPS :=				\
	$(call uniq,$(foreach x,$(THIRD_PARTY_ZLIB_A_DIRECTDEPS),$($(x))))

$(THIRD_PARTY_ZLIB_A):					\
		third_party/zlib/			\
		$(THIRD_PARTY_ZLIB_A).pkg		\
		$(THIRD_PARTY_ZLIB_A_OBJS)

$(THIRD_PARTY_ZLIB_A).pkg:				\
		$(THIRD_PARTY_ZLIB_A_OBJS)		\
		$(foreach x,$(THIRD_PARTY_ZLIB_A_DIRECTDEPS),$($(x)_A).pkg)

ifeq ($(ARCH), x86_64)
o/$(MODE)/third_party/zlib/adler32_simd.o: private	\
		TARGET_ARCH +=				\
			-mssse3
o/$(MODE)/third_party/zlib/crc_folding.o		\
o/$(MODE)/third_party/zlib/crc32_simd.o: private	\
		TARGET_ARCH +=				\
			-msse4.2			\
			-mpclmul
$(THIRD_PARTY_ZLIB_A_OBJS): private			\
		CPPFLAGS +=				\
			-DADLER32_SIMD_SSSE3		\
			-DCRC32_SIMD_SSE42_PCLMUL	\
			-DDEFLATE_SLIDE_HASH_SSE2	\
			-DINFLATE_CHUNK_SIMD_SSE2	\
			-DINFLATE_CHUNK_READ_64LE
endif

ifeq ($(ARCH), aarch64)
o/$(MODE)/third_party/zlib/deflate.o			\
o/$(MODE)/third_party/zlib/crc32_simd.o: private	\
		TARGET_ARCH +=				\
			-march=armv8-a+aes+crc
$(THIRD_PARTY_ZLIB_A_OBJS): private			\
		CPPFLAGS +=				\
			-DADLER32_SIMD_NEON		\
			-DCRC32_ARMV8_CRC32		\
			-DDEFLATE_SLIDE_HASH_NEON	\
			-DINFLATE_CHUNK_SIMD_NEON	\
			-DINFLATE_CHUNK_READ_64LE
endif

$(THIRD_PARTY_ZLIB_A_OBJS): private			\
		CFLAGS +=				\
			-ffunction-sections		\
			-fdata-sections

THIRD_PARTY_ZLIB_LIBS = $(foreach x,$(THIRD_PARTY_ZLIB_ARTIFACTS),$($(x)))
THIRD_PARTY_ZLIB_SRCS = $(foreach x,$(THIRD_PARTY_ZLIB_ARTIFACTS),$($(x)_SRCS))
THIRD_PARTY_ZLIB_HDRS = $(foreach x,$(THIRD_PARTY_ZLIB_ARTIFACTS),$($(x)_HDRS))
THIRD_PARTY_ZLIB_INCS = $(foreach x,$(THIRD_PARTY_ZLIB_ARTIFACTS),$($(x)_INCS))
THIRD_PARTY_ZLIB_BINS = $(foreach x,$(THIRD_PARTY_ZLIB_ARTIFACTS),$($(x)_BINS))
THIRD_PARTY_ZLIB_CHECKS = $(foreach x,$(THIRD_PARTY_ZLIB_ARTIFACTS),$($(x)_CHECKS))
THIRD_PARTY_ZLIB_OBJS = $(foreach x,$(THIRD_PARTY_ZLIB_ARTIFACTS),$($(x)_OBJS))
$(THIRD_PARTY_ZLIB_OBJS): $(BUILD_FILES) third_party/zlib/BUILD.mk

.PHONY: o/$(MODE)/third_party/zlib
o/$(MODE)/third_party/zlib:				\
		o/$(MODE)/third_party/zlib/gz		\
		$(THIRD_PARTY_ZLIB_A)			\
		$(THIRD_PARTY_ZLIB_CHECKS)
