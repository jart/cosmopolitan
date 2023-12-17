#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#── vi: set noet ft=make ts=8 sw=8 fenc=utf-8 :vi ────────────────────┘

PKGS += THIRD_PARTY_TREE

THIRD_PARTY_TREE = $(THIRD_PARTY_TREE_A_DEPS) $(THIRD_PARTY_TREE_A)
THIRD_PARTY_TREE_A = o/$(MODE)/third_party/tree/tree.a
THIRD_PARTY_TREE_FILES := $(wildcard third_party/tree/*)
THIRD_PARTY_TREE_HDRS = $(filter %.h,$(THIRD_PARTY_TREE_FILES))
THIRD_PARTY_TREE_INCS = $(filter %.inc,$(THIRD_PARTY_TREE_FILES))
THIRD_PARTY_TREE_SRCS = $(filter %.c,$(THIRD_PARTY_TREE_FILES))
THIRD_PARTY_TREE_OBJS = $(THIRD_PARTY_TREE_SRCS:%.c=o/$(MODE)/%.o)

THIRD_PARTY_TREE_A_DIRECTDEPS =				\
	LIBC_CALLS					\
	LIBC_FMT					\
	LIBC_INTRIN					\
	LIBC_MEM					\
	LIBC_NEXGEN32E					\
	LIBC_RUNTIME					\
	LIBC_STDIO					\
	LIBC_STR					\
	LIBC_SYSV					\
	LIBC_TIME					\
	THIRD_PARTY_MUSL

THIRD_PARTY_TREE_A_DEPS :=				\
	$(call uniq,$(foreach x,$(THIRD_PARTY_TREE_A_DIRECTDEPS),$($(x))))

THIRD_PARTY_TREE_CHECKS =				\
	$(THIRD_PARTY_TREE_A).pkg			\
	$(THIRD_PARTY_TREE_HDRS:%=o/$(MODE)/%.ok)

$(THIRD_PARTY_TREE_A):					\
		third_party/tree/			\
		$(THIRD_PARTY_TREE_A).pkg		\
		$(THIRD_PARTY_TREE_OBJS)

$(THIRD_PARTY_TREE_A).pkg:				\
		$(THIRD_PARTY_TREE_OBJS)		\
		$(foreach x,$(THIRD_PARTY_TREE_A_DIRECTDEPS),$($(x)_A).pkg)

o/$(MODE)/third_party/tree/tree.com.dbg:		\
		$(THIRD_PARTY_TREE)			\
		o/$(MODE)/third_party/tree/tree.o	\
		$(CRT)					\
		$(APE_NO_MODIFY_SELF)
	@$(APELINK)

THIRD_PARTY_TREE_BINS = $(THIRD_PARTY_TREE_COMS) $(THIRD_PARTY_TREE_COMS:%=%.dbg)
THIRD_PARTY_TREE_COMS = o/$(MODE)/third_party/tree/tree.com
THIRD_PARTY_TREE_LIBS = $(THIRD_PARTY_TREE_A)
$(THIRD_PARTY_TREE_OBJS): $(BUILD_FILES) third_party/tree/BUILD.mk

.PHONY: o/$(MODE)/third_party/tree
o/$(MODE)/third_party/tree:				\
		$(THIRD_PARTY_TREE_BINS)		\
		$(THIRD_PARTY_TREE_CHECKS)
