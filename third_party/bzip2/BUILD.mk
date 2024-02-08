#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#── vi: set noet ft=make ts=8 sw=8 fenc=utf-8 :vi ────────────────────┘

PKGS += THIRD_PARTY_BZIP2

THIRD_PARTY_BZIP2_ARTIFACTS += THIRD_PARTY_BZIP2_A
THIRD_PARTY_BZIP2 = $(THIRD_PARTY_BZIP2_A_DEPS) $(THIRD_PARTY_BZIP2_A)
THIRD_PARTY_BZIP2_A = o/$(MODE)/third_party/bzip2/bzip2.a
THIRD_PARTY_BZIP2_A_FILES := $(wildcard third_party/bzip2/*)
THIRD_PARTY_BZIP2_A_HDRS = $(filter %.h,$(THIRD_PARTY_BZIP2_A_FILES))
THIRD_PARTY_BZIP2_A_INCS = $(filter %.inc,$(THIRD_PARTY_BZIP2_A_FILES))
THIRD_PARTY_BZIP2_A_SRCS = $(filter %.c,$(THIRD_PARTY_BZIP2_A_FILES))
THIRD_PARTY_BZIP2_A_OBJS = $(THIRD_PARTY_BZIP2_A_SRCS:%.c=o/$(MODE)/%.o)
THIRD_PARTY_BZIP2_BINS = $(THIRD_PARTY_BZIP2_COMS) $(THIRD_PARTY_BZIP2_COMS:%=%.dbg)

THIRD_PARTY_BZIP2_COMS =					\
	o/$(MODE)/third_party/bzip2/bzip2.com			\
	o/$(MODE)/third_party/bzip2/bzip2recover.com

THIRD_PARTY_BZIP2_A_CHECKS =					\
	$(THIRD_PARTY_BZIP2_A).pkg				\
	$(THIRD_PARTY_BZIP2_A_HDRS:%=o/$(MODE)/%.ok)

THIRD_PARTY_BZIP2_A_DIRECTDEPS =				\
	LIBC_CALLS						\
	LIBC_FMT						\
	LIBC_INTRIN						\
	LIBC_LOG						\
	LIBC_MEM						\
	LIBC_NEXGEN32E						\
	LIBC_RUNTIME						\
	LIBC_STDIO						\
	LIBC_STR						\
	LIBC_SYSV

THIRD_PARTY_BZIP2_A_DEPS :=					\
	$(call uniq,$(foreach x,$(THIRD_PARTY_BZIP2_A_DIRECTDEPS),$($(x))))

$(THIRD_PARTY_BZIP2_A):						\
		third_party/bzip2/				\
		$(THIRD_PARTY_BZIP2_A).pkg			\
		$(THIRD_PARTY_BZIP2_A_OBJS)

$(THIRD_PARTY_BZIP2_A).pkg:					\
		$(THIRD_PARTY_BZIP2_A_OBJS)			\
		$(foreach x,$(THIRD_PARTY_BZIP2_A_DIRECTDEPS),$($(x)_A).pkg)

o/$(MODE)/third_party/bzip2/bzip2.com.dbg:			\
		$(THIRD_PARTY_BZIP2)				\
		o/$(MODE)/third_party/bzip2/bzip2.o		\
		o/$(MODE)/third_party/bzip2/bzip2.a.pkg		\
		$(CRT)						\
		$(APE_NO_MODIFY_SELF)
	@$(APELINK)

o/$(MODE)/third_party/bzip2/bzip2recover.com.dbg:		\
		$(THIRD_PARTY_BZIP2)				\
		o/$(MODE)/third_party/bzip2/bzip2recover.o	\
		o/$(MODE)/third_party/bzip2/bzip2.a.pkg		\
		$(CRT)						\
		$(APE_NO_MODIFY_SELF)
	@$(APELINK)

$(THIRD_PARTY_BZIP2_A_OBJS): private				\
		CFLAGS +=					\
			-w					\
			-ffunction-sections			\
			-fdata-sections

THIRD_PARTY_BZIP2_LIBS = $(foreach x,$(THIRD_PARTY_BZIP2_ARTIFACTS),$($(x)))
THIRD_PARTY_BZIP2_SRCS = $(foreach x,$(THIRD_PARTY_BZIP2_ARTIFACTS),$($(x)_SRCS))
THIRD_PARTY_BZIP2_HDRS = $(foreach x,$(THIRD_PARTY_BZIP2_ARTIFACTS),$($(x)_HDRS))
THIRD_PARTY_BZIP2_INCS = $(foreach x,$(THIRD_PARTY_BZIP2_ARTIFACTS),$($(x)_INCS))
THIRD_PARTY_BZIP2_BINS = $(foreach x,$(THIRD_PARTY_BZIP2_ARTIFACTS),$($(x)_BINS))
THIRD_PARTY_BZIP2_CHECKS = $(foreach x,$(THIRD_PARTY_BZIP2_ARTIFACTS),$($(x)_CHECKS))
THIRD_PARTY_BZIP2_OBJS = $(foreach x,$(THIRD_PARTY_BZIP2_ARTIFACTS),$($(x)_OBJS))
$(THIRD_PARTY_BZIP2_OBJS): $(BUILD_FILES) third_party/bzip2/BUILD.mk

.PHONY: o/$(MODE)/third_party/bzip2
o/$(MODE)/third_party/bzip2:					\
		$(THIRD_PARTY_BZIP2_COMS)			\
		$(THIRD_PARTY_BZIP2_CHECKS)

# TODO(jart): write regression test
# master jart@nightmare:~/cosmo$ o//third_party/bzip2/bzip2.com -1 <third_party/bzip2/sample1.ref >a
# master jart@nightmare:~/cosmo$ cmp -s third_party/bzip2/sample1.bz2 a
