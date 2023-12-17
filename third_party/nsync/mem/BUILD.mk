#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#── vi: set noet ft=make ts=8 sw=8 fenc=utf-8 :vi ────────────────────┘

PKGS += THIRD_PARTY_NSYNC_MEM

THIRD_PARTY_NSYNC_MEM_SRCS = $(THIRD_PARTY_NSYNC_MEM_A_SRCS)
THIRD_PARTY_NSYNC_MEM_HDRS = $(THIRD_PARTY_NSYNC_MEM_A_HDRS)

THIRD_PARTY_NSYNC_MEM_ARTIFACTS += THIRD_PARTY_NSYNC_MEM_A
THIRD_PARTY_NSYNC_MEM = $(THIRD_PARTY_NSYNC_MEM_A_DEPS) $(THIRD_PARTY_NSYNC_MEM_A)
THIRD_PARTY_NSYNC_MEM_A = o/$(MODE)/third_party/nsync/mem/nsync.a
THIRD_PARTY_NSYNC_MEM_A_FILES := $(wildcard third_party/nsync/mem/*)
THIRD_PARTY_NSYNC_MEM_A_HDRS = $(filter %.h,$(THIRD_PARTY_NSYNC_MEM_A_FILES))
THIRD_PARTY_NSYNC_MEM_A_SRCS = $(filter %.c,$(THIRD_PARTY_NSYNC_MEM_A_FILES))
THIRD_PARTY_NSYNC_MEM_A_OBJS = $(THIRD_PARTY_NSYNC_MEM_A_SRCS:%.c=o/$(MODE)/%.o)

THIRD_PARTY_NSYNC_MEM_A_DIRECTDEPS =		\
	LIBC_CALLS				\
	LIBC_INTRIN				\
	LIBC_NEXGEN32E				\
	LIBC_MEM				\
	LIBC_SYSV				\
	THIRD_PARTY_NSYNC

THIRD_PARTY_NSYNC_MEM_A_DEPS :=			\
	$(call uniq,$(foreach x,$(THIRD_PARTY_NSYNC_MEM_A_DIRECTDEPS),$($(x))))

THIRD_PARTY_NSYNC_MEM_A_CHECKS =		\
	$(THIRD_PARTY_NSYNC_MEM_A).pkg		\
	$(THIRD_PARTY_NSYNC_MEM_A_HDRS:%=o/$(MODE)/%.ok)

$(THIRD_PARTY_NSYNC_MEM_A):			\
		third_party/nsync/mem/		\
		$(THIRD_PARTY_NSYNC_MEM_A).pkg	\
		$(THIRD_PARTY_NSYNC_MEM_A_OBJS)

$(THIRD_PARTY_NSYNC_MEM_A).pkg:			\
		$(THIRD_PARTY_NSYNC_MEM_A_OBJS)	\
		$(foreach x,$(THIRD_PARTY_NSYNC_MEM_A_DIRECTDEPS),$($(x)_A).pkg)

# offer assurances about the stack safety of cosmo libc
$(THIRD_PARTY_NSYNC_MEM_A_OBJS): private COPTS += -Wframe-larger-than=4096 -Walloca-larger-than=4096

$(THIRD_PARTY_NSYNC_MEM_A_OBJS): private		\
		COPTS +=				\
			-ffreestanding			\
			-fdata-sections			\
			-ffunction-sections		\
			-fno-sanitize=address		\
			-Wframe-larger-than=4096	\
			-Walloca-larger-than=4096

THIRD_PARTY_NSYNC_MEM_LIBS = $(foreach x,$(THIRD_PARTY_NSYNC_MEM_ARTIFACTS),$($(x)))
THIRD_PARTY_NSYNC_MEM_SRCS = $(foreach x,$(THIRD_PARTY_NSYNC_MEM_ARTIFACTS),$($(x)_SRCS))
THIRD_PARTY_NSYNC_MEM_CHECKS = $(foreach x,$(THIRD_PARTY_NSYNC_MEM_ARTIFACTS),$($(x)_CHECKS))
THIRD_PARTY_NSYNC_MEM_OBJS = $(foreach x,$(THIRD_PARTY_NSYNC_MEM_ARTIFACTS),$($(x)_OBJS))
$(THIRD_PARTY_NSYNC_MEM_OBJS): third_party/nsync/mem/BUILD.mk

.PHONY: o/$(MODE)/third_party/nsync/mem
o/$(MODE)/third_party/nsync/mem: $(THIRD_PARTY_NSYNC_MEM_CHECKS)

