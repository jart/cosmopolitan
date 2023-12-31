#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#── vi: set noet ft=make ts=8 sw=8 fenc=utf-8 :vi ────────────────────┘

PKGS += THIRD_PARTY_MUSL

THIRD_PARTY_MUSL_SRCS = $(THIRD_PARTY_MUSL_A_SRCS)
THIRD_PARTY_MUSL_HDRS = $(THIRD_PARTY_MUSL_A_HDRS)

THIRD_PARTY_MUSL_ARTIFACTS += THIRD_PARTY_MUSL_A
THIRD_PARTY_MUSL = $(THIRD_PARTY_MUSL_A_DEPS) $(THIRD_PARTY_MUSL_A)
THIRD_PARTY_MUSL_A = o/$(MODE)/third_party/musl/musl.a
THIRD_PARTY_MUSL_A_FILES := $(wildcard third_party/musl/*)
THIRD_PARTY_MUSL_A_HDRS = $(filter %.h,$(THIRD_PARTY_MUSL_A_FILES))
THIRD_PARTY_MUSL_A_SRCS = $(filter %.c,$(THIRD_PARTY_MUSL_A_FILES))

THIRD_PARTY_MUSL_A_OBJS =				\
	$(THIRD_PARTY_MUSL_A_SRCS:%.c=o/$(MODE)/%.o)

THIRD_PARTY_MUSL_A_DIRECTDEPS =				\
	LIBC_CALLS					\
	LIBC_FMT					\
	LIBC_INTRIN					\
	LIBC_MEM					\
	LIBC_NEXGEN32E					\
	LIBC_NT_ADVAPI32				\
	LIBC_PROC					\
	LIBC_RUNTIME					\
	LIBC_SOCK					\
	LIBC_STDIO					\
	LIBC_STR					\
	LIBC_SYSV					\
	LIBC_THREAD					\
	THIRD_PARTY_ZLIB

THIRD_PARTY_MUSL_A_DEPS :=				\
	$(call uniq,$(foreach x,$(THIRD_PARTY_MUSL_A_DIRECTDEPS),$($(x))))

THIRD_PARTY_MUSL_A_CHECKS =				\
	$(THIRD_PARTY_MUSL_A).pkg			\
	$(THIRD_PARTY_MUSL_A_HDRS:%=o/$(MODE)/%.ok)

$(THIRD_PARTY_MUSL_A):					\
		third_party/musl/			\
		$(THIRD_PARTY_MUSL_A).pkg		\
		$(THIRD_PARTY_MUSL_A_OBJS)

$(THIRD_PARTY_MUSL_A).pkg:				\
		$(THIRD_PARTY_MUSL_A_OBJS)		\
		$(foreach x,$(THIRD_PARTY_MUSL_A_DIRECTDEPS),$($(x)_A).pkg)

o/$(MODE)/third_party/musl/getnameinfo.o		\
o/$(MODE)/third_party/musl/lookup_name.o		\
o/$(MODE)/third_party/musl/lookup_serv.o:		\
		private CFLAGS +=			\
			-fportcosmo

# offer assurances about the stack safety of cosmo libc
$(THIRD_PARTY_MUSL_A_OBJS): private COPTS += -Wframe-larger-than=4096 -Walloca-larger-than=4096

THIRD_PARTY_MUSL_LIBS = $(foreach x,$(THIRD_PARTY_MUSL_ARTIFACTS),$($(x)))
THIRD_PARTY_MUSL_SRCS = $(foreach x,$(THIRD_PARTY_MUSL_ARTIFACTS),$($(x)_SRCS))
THIRD_PARTY_MUSL_CHECKS = $(foreach x,$(THIRD_PARTY_MUSL_ARTIFACTS),$($(x)_CHECKS))
THIRD_PARTY_MUSL_OBJS = $(foreach x,$(THIRD_PARTY_MUSL_ARTIFACTS),$($(x)_OBJS))
$(THIRD_PARTY_MUSL_OBJS): third_party/musl/BUILD.mk

.PHONY: o/$(MODE)/third_party/musl
o/$(MODE)/third_party/musl: $(THIRD_PARTY_MUSL_CHECKS)
