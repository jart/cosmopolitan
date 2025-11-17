#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#── vi: set noet ft=make ts=8 sw=8 fenc=utf-8 :vi ────────────────────┘

PKGS += THIRD_PARTY_SED

THIRD_PARTY_SED = $(THIRD_PARTY_SED_A_DEPS) $(THIRD_PARTY_SED_A)
THIRD_PARTY_SED_A = o/$(MODE)/third_party/sed/sed.a
THIRD_PARTY_SED_FILES := $(wildcard third_party/sed/*)
THIRD_PARTY_SED_OBJS = $(THIRD_PARTY_SED_SRCS:%.c=o/$(MODE)/%.o)

THIRD_PARTY_SED_HDRS =					\
	third_party/sed/cmd.h				\
	third_party/sed/defs.h				\
	third_party/sed/extern.h			\
	third_party/sed/shade.h				\

THIRD_PARTY_SED_SRCS =					\
	third_party/sed/compile.c			\
	third_party/sed/main.c				\
	third_party/sed/misc.c				\
	third_party/sed/process.c			\

THIRD_PARTY_SED_A_DIRECTDEPS =				\
	LIBC_FMT					\
	LIBC_INTRIN					\
	LIBC_MEM					\
	LIBC_NEXGEN32E					\
	LIBC_RUNTIME					\
	LIBC_CALLS					\
	LIBC_STDIO					\
	LIBC_SYSV					\
	LIBC_STR					\
	LIBC_LOG					\
	THIRD_PARTY_GETOPT				\
	THIRD_PARTY_MUSL				\
	THIRD_PARTY_REGEX

THIRD_PARTY_SED_A_DEPS :=				\
	$(call uniq,$(foreach x,$(THIRD_PARTY_SED_A_DIRECTDEPS),$($(x))))

THIRD_PARTY_SED_CHECKS =				\
	$(THIRD_PARTY_SED_A).pkg			\
	$(THIRD_PARTY_SED_HDRS:%=o/$(MODE)/%.ok)

$(THIRD_PARTY_SED_A):					\
		third_party/sed/			\
		$(THIRD_PARTY_SED_A).pkg		\
		$(THIRD_PARTY_SED_OBJS)

$(THIRD_PARTY_SED_A).pkg:				\
		$(THIRD_PARTY_SED_OBJS)			\
		$(foreach x,$(THIRD_PARTY_SED_A_DIRECTDEPS),$($(x)_A).pkg)

o/$(MODE)/third_party/sed/sed.dbg:			\
		$(THIRD_PARTY_SED)			\
		o/$(MODE)/third_party/sed/cmd.o		\
		$(CRT)					\
		$(APE_NO_MODIFY_SELF)
	@$(APELINK)

o/$(MODE)/third_party/sed/cmd.o:			\
		third_party/sed/cmd.c			\
		third_party/sed/cmd.h			\

THIRD_PARTY_SED_BINS = $(THIRD_PARTY_SED_COMS) $(THIRD_PARTY_SED_COMS:%=%.dbg)
THIRD_PARTY_SED_COMS = o/$(MODE)/third_party/sed/sed
THIRD_PARTY_SED_LIBS = $(THIRD_PARTY_SED_A)
$(THIRD_PARTY_SED_OBJS): $(BUILD_FILES) third_party/sed/BUILD.mk

.PHONY: o/$(MODE)/third_party/sed
o/$(MODE)/third_party/sed:				\
		$(THIRD_PARTY_SED_BINS)			\
		$(THIRD_PARTY_SED_CHECKS)
