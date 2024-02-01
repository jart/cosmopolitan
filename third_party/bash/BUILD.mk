#-*-mode:bashfile-gbash;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#── vi: set noet ft=bash ts=8 sw=8 fenc=utf-8 :vi ────────────────────┘

PKGS += THIRD_PARTY_BASH

THIRD_PARTY_BASH_A = o/$(MODE)/third_party/bash/bash.a
THIRD_PARTY_BASH_FILES := $(wildcard third_party/bash/*)
THIRD_PARTY_BASH_HDRS = $(filter %.h,$(THIRD_PARTY_BASH_FILES))
THIRD_PARTY_BASH_INCS = $(filter %.inc,$(THIRD_PARTY_BASH_FILES))
THIRD_PARTY_BASH_SRCS = $(filter %.c,$(THIRD_PARTY_BASH_FILES))
THIRD_PARTY_BASH_OBJS = $(THIRD_PARTY_BASH_SRCS:%.c=o/$(MODE)/%.o)
THIRD_PARTY_BASH_COMS = o/$(MODE)/third_party/bash/bash.com
THIRD_PARTY_BASH_CHECKS = $(THIRD_PARTY_BASH_A).pkg

THIRD_PARTY_BASH_BINS =							\
	$(THIRD_PARTY_BASH_COMS)					\
	$(THIRD_PARTY_BASH_COMS:%=%.dbg)

THIRD_PARTY_BASH_DIRECTDEPS =						\
	LIBC_CALLS							\
	LIBC_FMT							\
	LIBC_INTRIN							\
	LIBC_MEM							\
	LIBC_NEXGEN32E							\
	LIBC_PROC							\
	LIBC_RUNTIME							\
	LIBC_SOCK							\
	LIBC_STDIO							\
	LIBC_STR							\
	LIBC_SYSV							\
	LIBC_THREAD							\
	LIBC_TIME							\
	THIRD_PARTY_COMPILER_RT						\
	THIRD_PARTY_GDTOA						\
	THIRD_PARTY_GETOPT						\
	THIRD_PARTY_MUSL						\
	THIRD_PARTY_NCURSES						\
	THIRD_PARTY_READLINE						\
	THIRD_PARTY_REGEX

THIRD_PARTY_BASH_DEPS :=						\
	$(call uniq,$(foreach x,$(THIRD_PARTY_BASH_DIRECTDEPS),$($(x))))

$(THIRD_PARTY_BASH_A).pkg:						\
	$(THIRD_PARTY_BASH_OBJS)					\
	$(foreach x,$(THIRD_PARTY_BASH_DIRECTDEPS),$($(x)_A).pkg)

$(THIRD_PARTY_BASH_A):							\
	third_party/bash/						\
	$(THIRD_PARTY_BASH_A).pkg					\
	$(filter-out %main.o,$(THIRD_PARTY_BASH_OBJS))

o/$(MODE)/third_party/bash/bash.com.dbg:				\
		$(THIRD_PARTY_BASH_DEPS)				\
		$(THIRD_PARTY_BASH_A)					\
		$(THIRD_PARTY_BASH_A).pkg				\
		o/$(MODE)/third_party/bash/shell.o			\
		$(CRT)							\
		$(APE_NO_MODIFY_SELF)
	@$(APELINK)

$(THIRD_PARTY_BASH_OBJS): private					\
		CPPFLAGS +=						\
			-DHAVE_CONFIG_H					\
			-DSHELL						\
			-DPACKAGE=\"bash\"				\
			-DLOCALEDIR=\"/zip/usr/share/locale\"		\
			-DCONF_HOSTTYPE=\"unknown\"			\
			-DCONF_OSTYPE=\"linux-cosmo\"			\
			-DCONF_MACHTYPE=\"unknown-pc-unknown-cosmo\"	\
			-DCONF_VENDOR=\"pc\"

$(THIRD_PARTY_BASH_OBJS): private					\
		CFLAGS +=						\
			-Wno-unused-but-set-variable			\
			-Wno-discarded-qualifiers			\
			-Wno-maybe-uninitialized			\
			-Wno-pointer-to-int-cast			\
			-Wno-stringop-truncation			\
			-Wno-format-zero-length				\
			-Wno-format-overflow				\
			-Wno-char-subscripts				\
			-Wno-nonnull-compare				\
			-Wno-unused-variable				\
			-Wno-missing-braces				\
			-Wno-use-after-free				\
			-Wno-unused-label				\
			-Wno-unused-value				\
			-Wno-return-type				\
			-Wno-parentheses				\
			-fportcosmo

$(THIRD_PARTY_BASH_OBJS): third_party/bash/BUILD.mk

.PHONY: o/$(MODE)/third_party/bash
o/$(MODE)/third_party/bash:						\
		$(THIRD_PARTY_BASH_BINS)				\
		$(THIRD_PARTY_BASH_CHECKS)

