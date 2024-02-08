#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#── vi: set noet ft=make ts=8 sw=8 fenc=utf-8 :vi ────────────────────┘

PKGS += THIRD_PARTY_MAKE

THIRD_PARTY_MAKE_A = o/$(MODE)/third_party/make/make.a
THIRD_PARTY_MAKE_FILES := $(wildcard third_party/make/*)
THIRD_PARTY_MAKE_HDRS = $(filter %.h,$(THIRD_PARTY_MAKE_FILES))
THIRD_PARTY_MAKE_SRCS = $(filter %.c,$(THIRD_PARTY_MAKE_FILES))
THIRD_PARTY_MAKE_OBJS = $(THIRD_PARTY_MAKE_SRCS:%.c=o/$(MODE)/%.o)
THIRD_PARTY_MAKE_COMS = o/$(MODE)/third_party/make/make.com
THIRD_PARTY_MAKE_CHECKS = $(THIRD_PARTY_MAKE_A).pkg

THIRD_PARTY_MAKE_BINS =					\
	$(THIRD_PARTY_MAKE_COMS)			\
	$(THIRD_PARTY_MAKE_COMS:%=%.dbg)

THIRD_PARTY_MAKE_DIRECTDEPS =				\
	LIBC_CALLS					\
	LIBC_FMT					\
	LIBC_INTRIN					\
	LIBC_LOG					\
	LIBC_MEM					\
	LIBC_NEXGEN32E					\
	LIBC_PROC					\
	LIBC_RUNTIME					\
	LIBC_STDIO					\
	LIBC_STR					\
	LIBC_SYSV					\
	LIBC_TIME					\
	LIBC_TINYMATH					\
	THIRD_PARTY_MUSL

THIRD_PARTY_MAKE_DEPS :=				\
	$(call uniq,$(foreach x,$(THIRD_PARTY_MAKE_DIRECTDEPS),$($(x))))

$(THIRD_PARTY_MAKE_A).pkg:				\
	$(THIRD_PARTY_MAKE_OBJS)			\
	$(foreach x,$(THIRD_PARTY_MAKE_DIRECTDEPS),$($(x)_A).pkg)

$(THIRD_PARTY_MAKE_A):					\
	third_party/make/				\
	$(THIRD_PARTY_MAKE_A).pkg			\
	$(filter-out %main.o,$(THIRD_PARTY_MAKE_OBJS))

o/$(MODE)/third_party/make/make.com.dbg:		\
		$(THIRD_PARTY_MAKE_DEPS)		\
		$(THIRD_PARTY_MAKE_A)			\
		$(THIRD_PARTY_MAKE_A).pkg		\
		o/$(MODE)/third_party/make/main.o	\
		$(CRT)					\
		$(APE_NO_MODIFY_SELF)
	@$(APELINK)

o/$(MODE)/third_party/make/strcache.o			\
o/$(MODE)/third_party/make/expand.o			\
o/$(MODE)/third_party/make/read.o: private		\
		CFLAGS +=				\
			-O2

o/$(MODE)/third_party/make/hash.o: private		\
		CFLAGS +=				\
			-O3

$(THIRD_PARTY_MAKE_OBJS): private			\
		CFLAGS +=				\
			-fportcosmo			\
			-DNO_ARCHIVES			\
			-DHAVE_CONFIG_H			\
			-DSET_STACK_SIZE

$(THIRD_PARTY_MAKE_OBJS): third_party/make/BUILD.mk

.PHONY: o/$(MODE)/third_party/make
o/$(MODE)/third_party/make:				\
		$(THIRD_PARTY_MAKE_BINS)		\
		$(THIRD_PARTY_MAKE_CHECKS)
