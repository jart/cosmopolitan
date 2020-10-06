#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#───vi: set et ft=make ts=8 tw=8 fenc=utf-8 :vi───────────────────────┘

PKGS += THIRD_PARTY_ZLIB

THIRD_PARTY_ZLIB_ARTIFACTS += THIRD_PARTY_ZLIB_A
THIRD_PARTY_ZLIB = $(THIRD_PARTY_ZLIB_A_DEPS) $(THIRD_PARTY_ZLIB_A)
THIRD_PARTY_ZLIB_A = o/$(MODE)/third_party/zlib/zlib.a
THIRD_PARTY_ZLIB_A_FILES := $(wildcard third_party/zlib/*)
THIRD_PARTY_ZLIB_A_HDRS = third_party/zlib/zlib.h
THIRD_PARTY_ZLIB_A_HDRS_ALL = $(filter %.h,$(THIRD_PARTY_ZLIB_A_FILES))
THIRD_PARTY_ZLIB_A_SRCS_S = $(filter %.S,$(THIRD_PARTY_ZLIB_A_FILES))
THIRD_PARTY_ZLIB_A_SRCS_C = $(filter %.c,$(THIRD_PARTY_ZLIB_A_FILES))

THIRD_PARTY_ZLIB_A_SRCS =				\
	$(THIRD_PARTY_ZLIB_A_SRCS_S)			\
	$(THIRD_PARTY_ZLIB_A_SRCS_C)

THIRD_PARTY_ZLIB_A_OBJS =				\
	$(THIRD_PARTY_ZLIB_A_SRCS:%=o/$(MODE)/%.zip.o)	\
	$(THIRD_PARTY_ZLIB_A_SRCS_S:%.S=o/$(MODE)/%.o)	\
	$(THIRD_PARTY_ZLIB_A_SRCS_C:%.c=o/$(MODE)/%.o)

THIRD_PARTY_ZLIB_A_CHECKS =				\
	$(THIRD_PARTY_ZLIB_A).pkg			\
	$(THIRD_PARTY_ZLIB_A_HDRS_ALL:%=o/$(MODE)/%.ok)

THIRD_PARTY_ZLIB_A_DIRECTDEPS =				\
	LIBC_STUBS					\
	LIBC_NEXGEN32E

THIRD_PARTY_ZLIB_A_DEPS :=				\
	$(call uniq,$(foreach x,$(THIRD_PARTY_ZLIB_A_DIRECTDEPS),$($(x))))

$(THIRD_PARTY_ZLIB_A):					\
		third_party/zlib/			\
		$(THIRD_PARTY_ZLIB_A).pkg		\
		$(THIRD_PARTY_ZLIB_A_OBJS)

$(THIRD_PARTY_ZLIB_A).pkg:				\
		$(THIRD_PARTY_ZLIB_A_OBJS)		\
		$(foreach x,$(THIRD_PARTY_ZLIB_A_DIRECTDEPS),$($(x)_A).pkg)

o/$(MODE)/third_party/zlib/adler32simd.o:		\
		OVERRIDE_CFLAGS +=			\
			-mssse3

o/$(MODE)/third_party/zlib/crcfold.o:			\
		OVERRIDE_CFLAGS +=			\
			-mpclmul			\
			-mssse3

o/$(MODE)/third_party/zlib/deflate.o			\
o/$(MODE)/third_party/zlib/inflate.o			\
o/$(MODE)/third_party/zlib/adler32.o:			\
		OVERRIDE_CFLAGS +=			\
			-ffunction-sections		\
			-fdata-sections

THIRD_PARTY_ZLIB_LIBS = $(foreach x,$(THIRD_PARTY_ZLIB_ARTIFACTS),$($(x)))
THIRD_PARTY_ZLIB_SRCS = $(foreach x,$(THIRD_PARTY_ZLIB_ARTIFACTS),$($(x)_SRCS))
THIRD_PARTY_ZLIB_HDRS = $(foreach x,$(THIRD_PARTY_ZLIB_ARTIFACTS),$($(x)_HDRS))
THIRD_PARTY_ZLIB_HDRS_ALL = $(foreach x,$(THIRD_PARTY_ZLIB_ARTIFACTS),$($(x)_HDRS_ALL))
THIRD_PARTY_ZLIB_BINS = $(foreach x,$(THIRD_PARTY_ZLIB_ARTIFACTS),$($(x)_BINS))
THIRD_PARTY_ZLIB_CHECKS = $(foreach x,$(THIRD_PARTY_ZLIB_ARTIFACTS),$($(x)_CHECKS))
THIRD_PARTY_ZLIB_OBJS = $(foreach x,$(THIRD_PARTY_ZLIB_ARTIFACTS),$($(x)_OBJS))
$(THIRD_PARTY_ZLIB_OBJS): $(BUILD_FILES) third_party/zlib/zlib.mk

.PHONY: o/$(MODE)/third_party/zlib
o/$(MODE)/third_party/zlib:				\
		$(THIRD_PARTY_ZLIB_A)			\
		$(THIRD_PARTY_ZLIB_CHECKS)
