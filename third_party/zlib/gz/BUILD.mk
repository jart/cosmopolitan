#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#── vi: set noet ft=make ts=8 sw=8 fenc=utf-8 :vi ────────────────────┘

PKGS += THIRD_PARTY_ZLIB_GZ

THIRD_PARTY_ZLIB_GZ_ARTIFACTS += THIRD_PARTY_ZLIB_GZ_A
THIRD_PARTY_ZLIB_GZ = $(THIRD_PARTY_ZLIB_GZ_A_DEPS) $(THIRD_PARTY_ZLIB_GZ_A)
THIRD_PARTY_ZLIB_GZ_A = o/$(MODE)/third_party/zlib/gz/gz.a
THIRD_PARTY_ZLIB_GZ_A_FILES := $(wildcard third_party/zlib/gz/*)
THIRD_PARTY_ZLIB_GZ_A_HDRS = $(filter %.h,$(THIRD_PARTY_ZLIB_GZ_A_FILES))
THIRD_PARTY_ZLIB_GZ_A_INCS = $(filter %.inc,$(THIRD_PARTY_ZLIB_GZ_A_FILES))
THIRD_PARTY_ZLIB_GZ_A_SRCS = $(filter %.c,$(THIRD_PARTY_ZLIB_GZ_A_FILES))
THIRD_PARTY_ZLIB_GZ_A_OBJS = $(THIRD_PARTY_ZLIB_GZ_A_SRCS:%.c=o/$(MODE)/%.o)

THIRD_PARTY_ZLIB_GZ_A_CHECKS =			\
	$(THIRD_PARTY_ZLIB_GZ_A).pkg		\
	$(THIRD_PARTY_ZLIB_GZ_A_HDRS:%=o/$(MODE)/%.ok)

THIRD_PARTY_ZLIB_GZ_A_DIRECTDEPS =		\
	LIBC_CALLS				\
	LIBC_FMT				\
	LIBC_INTRIN				\
	LIBC_MEM				\
	LIBC_NEXGEN32E				\
	LIBC_STDIO				\
	LIBC_STR				\
	LIBC_SYSV				\
	THIRD_PARTY_ZLIB

THIRD_PARTY_ZLIB_GZ_A_DEPS :=			\
	$(call uniq,$(foreach x,$(THIRD_PARTY_ZLIB_GZ_A_DIRECTDEPS),$($(x))))

$(THIRD_PARTY_ZLIB_GZ_A):			\
		third_party/zlib/gz/		\
		$(THIRD_PARTY_ZLIB_GZ_A).pkg	\
		$(THIRD_PARTY_ZLIB_GZ_A_OBJS)

$(THIRD_PARTY_ZLIB_GZ_A).pkg:			\
		$(THIRD_PARTY_ZLIB_GZ_A_OBJS)	\
		$(foreach x,$(THIRD_PARTY_ZLIB_GZ_A_DIRECTDEPS),$($(x)_A).pkg)

$(THIRD_PARTY_ZLIB_GZ_OBJS): private		\
		CFLAGS +=			\
			-ffunction-sections	\
			-fdata-sections

THIRD_PARTY_ZLIB_GZ_LIBS = $(foreach x,$(THIRD_PARTY_ZLIB_GZ_ARTIFACTS),$($(x)))
THIRD_PARTY_ZLIB_GZ_SRCS = $(foreach x,$(THIRD_PARTY_ZLIB_GZ_ARTIFACTS),$($(x)_SRCS))
THIRD_PARTY_ZLIB_GZ_HDRS = $(foreach x,$(THIRD_PARTY_ZLIB_GZ_ARTIFACTS),$($(x)_HDRS))
THIRD_PARTY_ZLIB_GZ_INCS = $(foreach x,$(THIRD_PARTY_ZLIB_GZ_ARTIFACTS),$($(x)_INCS))
THIRD_PARTY_ZLIB_GZ_BINS = $(foreach x,$(THIRD_PARTY_ZLIB_GZ_ARTIFACTS),$($(x)_BINS))
THIRD_PARTY_ZLIB_GZ_CHECKS = $(foreach x,$(THIRD_PARTY_ZLIB_GZ_ARTIFACTS),$($(x)_CHECKS))
THIRD_PARTY_ZLIB_GZ_OBJS = $(foreach x,$(THIRD_PARTY_ZLIB_GZ_ARTIFACTS),$($(x)_OBJS))
$(THIRD_PARTY_ZLIB_GZ_OBJS): $(BUILD_FILES) third_party/zlib/gz/BUILD.mk

.PHONY: o/$(MODE)/third_party/zlib/gz
o/$(MODE)/third_party/zlib/gz:			\
		$(THIRD_PARTY_ZLIB_GZ_A)	\
		$(THIRD_PARTY_ZLIB_GZ_CHECKS)
