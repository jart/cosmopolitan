#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#── vi: set noet ft=make ts=8 sw=8 fenc=utf-8 :vi ────────────────────┘

PKGS += THIRD_PARTY_UNZIP

THIRD_PARTY_UNZIP_ARTIFACTS += THIRD_PARTY_UNZIP_A
THIRD_PARTY_UNZIP = $(THIRD_PARTY_UNZIP_A_DEPS) $(THIRD_PARTY_UNZIP_A)
THIRD_PARTY_UNZIP_A = o/$(MODE)/third_party/unzip/unzip.a
THIRD_PARTY_UNZIP_A_FILES := $(wildcard third_party/unzip/*)
THIRD_PARTY_UNZIP_A_HDRS = $(filter %.h,$(THIRD_PARTY_UNZIP_A_FILES))
THIRD_PARTY_UNZIP_A_INCS = $(filter %.inc,$(THIRD_PARTY_UNZIP_A_FILES))
THIRD_PARTY_UNZIP_A_SRCS = $(filter %.c,$(THIRD_PARTY_UNZIP_A_FILES))
THIRD_PARTY_UNZIP_A_OBJS = $(THIRD_PARTY_UNZIP_A_SRCS:%.c=o/$(MODE)/%.o)

THIRD_PARTY_UNZIP_A_DIRECTDEPS =						\
	LIBC_CALLS								\
	LIBC_FMT								\
	LIBC_INTRIN								\
	LIBC_LOG								\
	LIBC_MEM								\
	LIBC_NEXGEN32E								\
	LIBC_RUNTIME								\
	LIBC_STDIO								\
	LIBC_STR								\
	LIBC_SYSV								\
	LIBC_TIME								\
	THIRD_PARTY_BZIP2

THIRD_PARTY_UNZIP_A_DEPS :=							\
	$(call uniq,$(foreach x,$(THIRD_PARTY_UNZIP_A_DIRECTDEPS),$($(x))))

THIRD_PARTY_UNZIP_A_CHECKS =							\
	$(THIRD_PARTY_UNZIP_A).pkg

$(THIRD_PARTY_UNZIP_A):								\
		third_party/unzip/						\
		$(THIRD_PARTY_UNZIP_A).pkg					\
		$(THIRD_PARTY_UNZIP_A_OBJS)

$(THIRD_PARTY_UNZIP_A).pkg:							\
		$(THIRD_PARTY_UNZIP_A_OBJS)					\
		$(foreach x,$(THIRD_PARTY_UNZIP_A_DIRECTDEPS),$($(x)_A).pkg)

o/$(MODE)/third_party/unzip/unzip.com.dbg:					\
		$(THIRD_PARTY_UNZIP)						\
		o/$(MODE)/third_party/unzip/unzip.o				\
		$(CRT)								\
		$(APE_NO_MODIFY_SELF)
	@$(APELINK)

$(THIRD_PARTY_UNZIP_A_OBJS): private						\
		CPPFLAGS +=							\
			-w							\
			-DUSE_BZIP2						\
			-DUNICODE_SUPPORT					\
			-DHAVE_UNLINK						\
			-DLARGE_FILE_SUPPORT					\
			-DHAVE_TERMIOS_H					\
			-DNO_LCHMOD

$(THIRD_PARTY_UNZIP_A_OBJS): private						\
		CFLAGS +=							\
			$(OLD_CODE)

THIRD_PARTY_UNZIP_COMS = o/$(MODE)/third_party/unzip/unzip.com
THIRD_PARTY_UNZIP_BINS = $(THIRD_PARTY_UNZIP_COMS) $(THIRD_PARTY_UNZIP_COMS:%=%.dbg)
THIRD_PARTY_UNZIP_LIBS = $(foreach x,$(THIRD_PARTY_UNZIP_ARTIFACTS),$($(x)))
THIRD_PARTY_UNZIP_SRCS = $(foreach x,$(THIRD_PARTY_UNZIP_ARTIFACTS),$($(x)_SRCS))
THIRD_PARTY_UNZIP_HDRS = $(foreach x,$(THIRD_PARTY_UNZIP_ARTIFACTS),$($(x)_HDRS))
THIRD_PARTY_UNZIP_INCS = $(foreach x,$(THIRD_PARTY_UNZIP_ARTIFACTS),$($(x)_INCS))
THIRD_PARTY_UNZIP_CHECKS = $(foreach x,$(THIRD_PARTY_UNZIP_ARTIFACTS),$($(x)_CHECKS))
THIRD_PARTY_UNZIP_OBJS = $(foreach x,$(THIRD_PARTY_UNZIP_ARTIFACTS),$($(x)_OBJS))
$(THIRD_PARTY_UNZIP_OBJS): third_party/unzip/BUILD.mk

.PHONY: o/$(MODE)/third_party/unzip
o/$(MODE)/third_party/unzip:							\
		$(THIRD_PARTY_UNZIP_BINS)					\
		$(THIRD_PARTY_UNZIP_CHECKS)
