#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#── vi: set noet ft=make ts=8 sw=8 fenc=utf-8 :vi ────────────────────┘

PKGS += THIRD_PARTY_CTAGS

THIRD_PARTY_CTAGS_ARTIFACTS += THIRD_PARTY_CTAGS_A
THIRD_PARTY_CTAGS = $(THIRD_PARTY_CTAGS_DEPS) $(THIRD_PARTY_CTAGS_A)
THIRD_PARTY_CTAGS_A = o/$(MODE)/third_party/ctags/ctags.a
THIRD_PARTY_CTAGS_FILES := $(wildcard third_party/ctags/*)
THIRD_PARTY_CTAGS_HDRS = $(filter %.h,$(THIRD_PARTY_CTAGS_FILES))
THIRD_PARTY_CTAGS_INCS = $(filter %.inc,$(THIRD_PARTY_CTAGS_FILES))
THIRD_PARTY_CTAGS_SRCS = $(filter %.c,$(THIRD_PARTY_CTAGS_FILES))
THIRD_PARTY_CTAGS_OBJS = $(THIRD_PARTY_CTAGS_SRCS:%.c=o/$(MODE)/%.o)

THIRD_PARTY_CTAGS_DIRECTDEPS =				\
	LIBC_CALLS					\
	LIBC_FMT					\
	LIBC_INTRIN					\
	LIBC_LOG					\
	LIBC_MEM					\
	LIBC_NEXGEN32E					\
	LIBC_RUNTIME					\
	LIBC_PROC					\
	LIBC_STDIO					\
	LIBC_STR					\
	LIBC_SYSV					\
	THIRD_PARTY_MUSL				\
	THIRD_PARTY_REGEX

THIRD_PARTY_CTAGS_DEPS :=				\
	$(call uniq,$(foreach x,$(THIRD_PARTY_CTAGS_DIRECTDEPS),$($(x))))

THIRD_PARTY_CTAGS_CHECKS =				\
	$(THIRD_PARTY_CTAGS_A).pkg			\
	$(THIRD_PARTY_CTAGS_HDRS:%=o/$(MODE)/%.ok)

$(THIRD_PARTY_CTAGS_A):					\
		third_party/ctags/			\
		$(THIRD_PARTY_CTAGS_A).pkg		\
		$(THIRD_PARTY_CTAGS_OBJS)

$(THIRD_PARTY_CTAGS_A).pkg:				\
		$(THIRD_PARTY_CTAGS_OBJS)		\
		$(foreach x,$(THIRD_PARTY_CTAGS_DIRECTDEPS),$($(x)_A).pkg)

o/$(MODE)/third_party/ctags/ctags.com.dbg:		\
		$(THIRD_PARTY_CTAGS)			\
		o/$(MODE)/third_party/ctags/main.o	\
		$(CRT)					\
		$(APE_NO_MODIFY_SELF)
	@$(APELINK)

THIRD_PARTY_CTAGS_LIBS = $(THIRD_PARTY_CTAGS_A)
THIRD_PARTY_CTAGS_BINS = $(THIRD_PARTY_CTAGS_COMS) $(THIRD_PARTY_CTAGS_COMS:%=%.dbg)
THIRD_PARTY_CTAGS_COMS = o/$(MODE)/third_party/ctags/ctags.com
$(THIRD_PARTY_CTAGS_OBJS): $(BUILD_FILES) third_party/ctags/BUILD.mk

.PHONY: o/$(MODE)/third_party/ctags
o/$(MODE)/third_party/ctags:				\
		$(THIRD_PARTY_CTAGS_BINS)		\
		$(THIRD_PARTY_CTAGS_CHECKS)
