#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#── vi: set noet ft=make ts=8 sw=8 fenc=utf-8 :vi ────────────────────┘

PKGS += THIRD_PARTY_TR

THIRD_PARTY_TR_ARTIFACTS += THIRD_PARTY_TR_A
THIRD_PARTY_TR = $(THIRD_PARTY_TR_DEPS) $(THIRD_PARTY_TR_A)
THIRD_PARTY_TR_A = o/$(MODE)/third_party/tr/tr.a
THIRD_PARTY_TR_FILES := $(wildcard third_party/tr/*)
THIRD_PARTY_TR_HDRS = $(filter %.h,$(THIRD_PARTY_TR_FILES))
THIRD_PARTY_TR_INCS = $(filter %.inc,$(THIRD_PARTY_TR_FILES))
THIRD_PARTY_TR_SRCS = $(filter %.c,$(THIRD_PARTY_TR_FILES))
THIRD_PARTY_TR_OBJS = $(THIRD_PARTY_TR_SRCS:%.c=o/$(MODE)/%.o)

THIRD_PARTY_TR_DIRECTDEPS =			\
	LIBC_CALLS				\
	LIBC_FMT				\
	LIBC_INTRIN				\
	LIBC_LOG				\
	LIBC_MEM				\
	LIBC_NEXGEN32E				\
	LIBC_RUNTIME				\
	LIBC_STDIO				\
	LIBC_STR				\
	THIRD_PARTY_GETOPT

THIRD_PARTY_TR_DEPS :=				\
	$(call uniq,$(foreach x,$(THIRD_PARTY_TR_DIRECTDEPS),$($(x))))

THIRD_PARTY_TR_CHECKS =				\
	$(THIRD_PARTY_TR_A).pkg			\
	$(THIRD_PARTY_TR_HDRS:%=o/$(MODE)/%.ok)

$(THIRD_PARTY_TR_A):				\
		third_party/tr/			\
		$(THIRD_PARTY_TR_A).pkg		\
		$(THIRD_PARTY_TR_OBJS)

$(THIRD_PARTY_TR_A).pkg:			\
		$(THIRD_PARTY_TR_OBJS)		\
		$(foreach x,$(THIRD_PARTY_TR_DIRECTDEPS),$($(x)_A).pkg)

o/$(MODE)/third_party/tr/tr.com.dbg:		\
		$(THIRD_PARTY_TR)		\
		o/$(MODE)/third_party/tr/cmd.o	\
		$(CRT)				\
		$(APE_NO_MODIFY_SELF)
	@$(APELINK)

THIRD_PARTY_TR_LIBS = $(THIRD_PARTY_TR_A)
THIRD_PARTY_TR_BINS = $(THIRD_PARTY_TR_COMS) $(THIRD_PARTY_TR_COMS:%=%.dbg)
THIRD_PARTY_TR_COMS = o/$(MODE)/third_party/tr/tr.com
$(THIRD_PARTY_TR_OBJS): $(BUILD_FILES) third_party/tr/BUILD.mk

.PHONY: o/$(MODE)/third_party/tr
o/$(MODE)/third_party/tr:			\
		$(THIRD_PARTY_TR_BINS)		\
		$(THIRD_PARTY_TR_CHECKS)
