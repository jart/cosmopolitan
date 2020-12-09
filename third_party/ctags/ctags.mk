#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#───vi: set et ft=make ts=8 tw=8 fenc=utf-8 :vi───────────────────────┘

THIRD_PARTY_CTAGS_FILES := $(wildcard third_party/ctags/*)
THIRD_PARTY_CTAGS_HDRS = $(filter %.h,$(THIRD_PARTY_CTAGS_FILES))
THIRD_PARTY_CTAGS_SRCS = $(filter %.c,$(THIRD_PARTY_CTAGS_FILES))

THIRD_PARTY_CTAGS_BINS =					\
	o/$(MODE)/third_party/ctags/ctags.com

THIRD_PARTY_CTAGS_CHECKS =					\
	o/$(MODE)/third_party/ctags/ctags.pkg			\
	$(THIRD_PARTY_CTAGS_HDRS:%=o/$(MODE)/%.ok)

THIRD_PARTY_CTAGS_DIRECTDEPS =					\
	LIBC_CALLS						\
	LIBC_CALLS_HEFTY					\
	LIBC_FMT						\
	LIBC_LOG						\
	LIBC_MEM						\
	LIBC_NEXGEN32E						\
	LIBC_RUNTIME						\
	LIBC_STDIO						\
	LIBC_STR						\
	LIBC_STUBS						\
	LIBC_SYSV						\
	LIBC_TIME						\
	LIBC_UNICODE						\
	THIRD_PARTY_MUSL					\
	THIRD_PARTY_REGEX

THIRD_PARTY_CTAGS_DEPS :=					\
	$(call uniq,$(foreach x,$(THIRD_PARTY_CTAGS_DIRECTDEPS),$($(x))))

o/$(MODE)/third_party/ctags/ctags.srcs.a:			\
		third_party/ctags/				\
		$(THIRD_PARTY_CTAGS_SRCS:%=o/$(MODE)/%.zip.o)

o/$(MODE)/third_party/ctags/ctags.com.dbg:			\
		o/$(MODE)/third_party/ctags/ctags.pkg		\
		o/$(MODE)/third_party/ctags/ctags.srcs.a	\
		$(THIRD_PARTY_CTAGS_DEPS)			\
		$(THIRD_PARTY_CTAGS_SRCS:%.c=o/$(MODE)/%.o)	\
		$(CRT)						\
		$(APE)
	@$(APELINK)

o/$(MODE)/third_party/ctags/ctags.pkg:				\
		$(THIRD_PARTY_CTAGS_SRCS:%.c=o/$(MODE)/%.o)	\
		o/$(MODE)/third_party/ctags/ctags.srcs.pkg	\
		$(foreach x,$(THIRD_PARTY_CTAGS_DIRECTDEPS),$($(x)_A).pkg)

o/$(MODE)/third_party/ctags/ctags.srcs.pkg:			\
		$(THIRD_PARTY_CTAGS_SRCS:%=o/$(MODE)/%.zip.o)	\
		$(foreach x,$(THIRD_PARTY_CTAGS_DIRECTDEPS),$($(x)_A).pkg)

.PHONY: o/$(MODE)/third_party/ctags
o/$(MODE)/third_party/ctags:					\
		$(THIRD_PARTY_CTAGS_BINS)			\
		$(THIRD_PARTY_CTAGS_CHECKS)
