#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#── vi: set noet ft=make ts=8 sw=8 fenc=utf-8 :vi ────────────────────┘

PKGS += THIRD_PARTY_MBEDTLS

THIRD_PARTY_MBEDTLS_ARTIFACTS += THIRD_PARTY_MBEDTLS_A
THIRD_PARTY_MBEDTLS = $(THIRD_PARTY_MBEDTLS_A_DEPS) $(THIRD_PARTY_MBEDTLS_A)
THIRD_PARTY_MBEDTLS_A = o/$(MODE)/third_party/mbedtls/mbedtls.a
THIRD_PARTY_MBEDTLS_A_FILES := $(wildcard third_party/mbedtls/*)
THIRD_PARTY_MBEDTLS_A_INCS = $(filter %.inc,$(THIRD_PARTY_MBEDTLS_A_FILES))
THIRD_PARTY_MBEDTLS_A_HDRS = $(filter %.h,$(THIRD_PARTY_MBEDTLS_A_FILES))
THIRD_PARTY_MBEDTLS_A_SRCS = $(filter %.c,$(THIRD_PARTY_MBEDTLS_A_FILES))
THIRD_PARTY_MBEDTLS_A_OBJS = $(THIRD_PARTY_MBEDTLS_A_SRCS:%.c=o/$(MODE)/%.o)

THIRD_PARTY_MBEDTLS_A_CHECKS =						\
	$(THIRD_PARTY_MBEDTLS_A).pkg					\
	$(THIRD_PARTY_MBEDTLS_A_HDRS:%=o/$(MODE)/%.ok)

THIRD_PARTY_MBEDTLS_A_DIRECTDEPS =					\
	LIBC_CALLS							\
	LIBC_FMT							\
	LIBC_INTRIN							\
	LIBC_MEM							\
	LIBC_NEXGEN32E							\
	LIBC_RUNTIME							\
	LIBC_SOCK							\
	LIBC_STDIO							\
	LIBC_STR							\
	LIBC_SYSV							\
	LIBC_TIME							\
	NET_HTTP							\
	THIRD_PARTY_COMPILER_RT						\
	THIRD_PARTY_MUSL						\
	THIRD_PARTY_ZLIB

THIRD_PARTY_MBEDTLS_A_DEPS :=						\
	$(call uniq,$(foreach x,$(THIRD_PARTY_MBEDTLS_A_DIRECTDEPS),$($(x))))

$(THIRD_PARTY_MBEDTLS_A):						\
		third_party/mbedtls/					\
		$(THIRD_PARTY_MBEDTLS_A).pkg				\
		$(THIRD_PARTY_MBEDTLS_A_OBJS)

$(THIRD_PARTY_MBEDTLS_A).pkg:						\
		$(THIRD_PARTY_MBEDTLS_A_OBJS)				\
		$(foreach x,$(THIRD_PARTY_MBEDTLS_A_DIRECTDEPS),$($(x)_A).pkg)

$(THIRD_PARTY_MBEDTLS_A_OBJS): private					\
			CFLAGS +=					\
				-fdata-sections				\
				-ffunction-sections

o/$(MODE)/third_party/mbedtls/everest.o: private			\
			CFLAGS +=					\
				-O3

o/$(MODE)/third_party/mbedtls/bigmul4.o					\
o/$(MODE)/third_party/mbedtls/bigmul6.o: private			\
			CFLAGS +=					\
				-O2

ifeq ($(ARCH), x86_64)
o/$(MODE)/third_party/mbedtls/shiftright-avx.o: private			\
			CFLAGS +=					\
				-O3 -mavx
endif

o/$(MODE)/third_party/mbedtls/zeroize.o: private			\
			CFLAGS +=					\
				-O3					\
				-x-no-pg				\
				-fomit-frame-pointer			\
				-foptimize-sibling-calls

THIRD_PARTY_MBEDTLS_LIBS = $(foreach x,$(THIRD_PARTY_MBEDTLS_ARTIFACTS),$($(x)))
THIRD_PARTY_MBEDTLS_SRCS = $(foreach x,$(THIRD_PARTY_MBEDTLS_ARTIFACTS),$($(x)_SRCS))
THIRD_PARTY_MBEDTLS_HDRS = $(foreach x,$(THIRD_PARTY_MBEDTLS_ARTIFACTS),$($(x)_HDRS))
THIRD_PARTY_MBEDTLS_INCS = $(foreach x,$(THIRD_PARTY_MBEDTLS_ARTIFACTS),$($(x)_INCS))
THIRD_PARTY_MBEDTLS_CHECKS = $(foreach x,$(THIRD_PARTY_MBEDTLS_ARTIFACTS),$($(x)_CHECKS))
THIRD_PARTY_MBEDTLS_OBJS = $(foreach x,$(THIRD_PARTY_MBEDTLS_ARTIFACTS),$($(x)_OBJS))
$(THIRD_PARTY_MBEDTLS_A_OBJS): third_party/mbedtls/BUILD.mk

.PHONY: o/$(MODE)/third_party/mbedtls
o/$(MODE)/third_party/mbedtls:						\
		o/$(MODE)/third_party/mbedtls/test			\
		$(THIRD_PARTY_MBEDTLS_CHECKS)
