#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#── vi: set noet ft=make ts=8 sw=8 fenc=utf-8 :vi ────────────────────┘

PKGS += THIRD_PARTY_XXHASH

THIRD_PARTY_XXHASH_ARTIFACTS += THIRD_PARTY_XXHASH_A
THIRD_PARTY_XXHASH = $(THIRD_PARTY_XXHASH_A_DEPS) $(THIRD_PARTY_XXHASH_A)
THIRD_PARTY_XXHASH_A = o/$(MODE)/third_party/xxhash/libxxhash.a
THIRD_PARTY_XXHASH_FILES := $(wildcard third_party/xxhash/*) $(wildcard third_party/xxhash/cli/*)
THIRD_PARTY_XXHASH_HDRS = $(filter %.h,$(THIRD_PARTY_XXHASH_FILES))
THIRD_PARTY_XXHASH_SRCS = $(filter %.c,$(THIRD_PARTY_XXHASH_FILES))
THIRD_PARTY_XXHASH_OBJS = $(THIRD_PARTY_XXHASH_SRCS:%.c=o/$(MODE)/%.o)

THIRD_PARTY_XXHASH_A_DIRECTDEPS =				\
        LIBC_CALLS						\
        LIBC_FMT						\
        LIBC_INTRIN						\
        LIBC_MEM						\
        LIBC_RUNTIME						\
        LIBC_PROC						\
        LIBC_STDIO						\
        LIBC_STR						\
        LIBC_SYSV						\
        LIBC_TIME                       

THIRD_PARTY_XXHASH_A_DEPS :=					\
	$(call uniq,$(foreach x,$(THIRD_PARTY_XXHASH_A_DIRECTDEPS),$($(x))))

THIRD_PARTY_XXHASH_CHECKS =					\
	$(THIRD_PARTY_XXHASH_A).pkg				\
	$(THIRD_PARTY_XXHASH_HDRS:%=o/$(MODE)/%.ok)

$(THIRD_PARTY_XXHASH_A):					\
		third_party/xxhash/				\
		$(THIRD_PARTY_XXHASH_A).pkg			\
		$(THIRD_PARTY_XXHASH_OBJS)

$(THIRD_PARTY_XXHASH_A).pkg:					\
		$(THIRD_PARTY_XXHASH_OBJS)			\
		$(foreach x,$(THIRD_PARTY_XXHASH_A_DIRECTDEPS),$($(x)_A).pkg)

o/$(MODE)/third_party/xxhash/xxhsum.com.dbg:			\
		$(THIRD_PARTY_XXHASH)				\
		o/$(MODE)/third_party/xxhash/cli/xxhsum.o	\
		$(CRT)						\
		$(APE_NO_MODIFY_SELF)
	@$(APELINK)

THIRD_PARTY_XXHASH_BINS = $(THIRD_PARTY_XXHASH_COMS) $(THIRD_PARTY_XXHASH_COMS:%=%.dbg)
THIRD_PARTY_XXHASH_COMS = o/$(MODE)/third_party/xxhash/xxhsum.com
THIRD_PARTY_XXHASH_LIBS = $(THIRD_PARTY_XXHASH_A)

.PHONY: o/$(MODE)/third_party/xxhash
o/$(MODE)/third_party/xxhash:					\
		$(THIRD_PARTY_XXHASH_BINS)			\
		$(THIRD_PARTY_XXHASH_CHECKS)
