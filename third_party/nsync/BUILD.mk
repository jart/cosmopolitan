#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#── vi: set noet ft=make ts=8 sw=8 fenc=utf-8 :vi ────────────────────┘

PKGS += THIRD_PARTY_NSYNC

THIRD_PARTY_NSYNC_SRCS = $(THIRD_PARTY_NSYNC_A_SRCS)
THIRD_PARTY_NSYNC_HDRS = $(THIRD_PARTY_NSYNC_A_HDRS)

THIRD_PARTY_NSYNC_ARTIFACTS += THIRD_PARTY_NSYNC_A
THIRD_PARTY_NSYNC = $(THIRD_PARTY_NSYNC_A_DEPS) $(THIRD_PARTY_NSYNC_A)
THIRD_PARTY_NSYNC_A = o/$(MODE)/third_party/nsync/nsync.a
THIRD_PARTY_NSYNC_A_FILES := $(wildcard third_party/nsync/*)
THIRD_PARTY_NSYNC_A_HDRS = $(filter %.h,$(THIRD_PARTY_NSYNC_A_FILES))
THIRD_PARTY_NSYNC_A_SRCS_C = $(filter %.c,$(THIRD_PARTY_NSYNC_A_FILES))
THIRD_PARTY_NSYNC_A_SRCS_S = $(filter %.S,$(THIRD_PARTY_NSYNC_A_FILES))

THIRD_PARTY_NSYNC_A_SRCS =				\
	$(THIRD_PARTY_NSYNC_A_SRCS_S)			\
	$(THIRD_PARTY_NSYNC_A_SRCS_C)

THIRD_PARTY_NSYNC_A_OBJS =				\
	$(THIRD_PARTY_NSYNC_A_SRCS_C:%.c=o/$(MODE)/%.o)	\
	$(THIRD_PARTY_NSYNC_A_SRCS_S:%.S=o/$(MODE)/%.o)

THIRD_PARTY_NSYNC_A_DIRECTDEPS =			\
	LIBC_CALLS					\
	LIBC_INTRIN					\
	LIBC_NEXGEN32E					\
	LIBC_NT_KERNEL32				\
	LIBC_NT_SYNCHRONIZATION				\
	LIBC_STR					\
	LIBC_SYSV					\
	LIBC_SYSV_CALLS

THIRD_PARTY_NSYNC_A_DEPS :=				\
	$(call uniq,$(foreach x,$(THIRD_PARTY_NSYNC_A_DIRECTDEPS),$($(x))))

THIRD_PARTY_NSYNC_A_CHECKS =				\
	$(THIRD_PARTY_NSYNC_A).pkg			\
	$(THIRD_PARTY_NSYNC_A_HDRS:%=o/$(MODE)/%.ok)

$(THIRD_PARTY_NSYNC_A):					\
		third_party/nsync/			\
		$(THIRD_PARTY_NSYNC_A).pkg		\
		$(THIRD_PARTY_NSYNC_A_OBJS)

$(THIRD_PARTY_NSYNC_A).pkg:				\
		$(THIRD_PARTY_NSYNC_A_OBJS)		\
		$(foreach x,$(THIRD_PARTY_NSYNC_A_DIRECTDEPS),$($(x)_A).pkg)

$(THIRD_PARTY_NSYNC_A_OBJS): private			\
		COPTS +=				\
			-ffreestanding			\
			-fdata-sections			\
			-ffunction-sections		\
			-fno-sanitize=address		\
			-Wframe-larger-than=4096	\
			-Walloca-larger-than=4096

# these assembly files are safe to build on aarch64
o/$(MODE)/third_party/nsync/compat.o: third_party/nsync/compat.S
	@$(COMPILE) -AOBJECTIFY.S $(OBJECTIFY.S) $(OUTPUT_OPTION) -c $<

THIRD_PARTY_NSYNC_LIBS = $(foreach x,$(THIRD_PARTY_NSYNC_ARTIFACTS),$($(x)))
THIRD_PARTY_NSYNC_SRCS = $(foreach x,$(THIRD_PARTY_NSYNC_ARTIFACTS),$($(x)_SRCS))
THIRD_PARTY_NSYNC_CHECKS = $(foreach x,$(THIRD_PARTY_NSYNC_ARTIFACTS),$($(x)_CHECKS))
THIRD_PARTY_NSYNC_OBJS = $(foreach x,$(THIRD_PARTY_NSYNC_ARTIFACTS),$($(x)_OBJS))
$(THIRD_PARTY_NSYNC_OBJS): third_party/nsync/BUILD.mk

.PHONY: o/$(MODE)/third_party/nsync
o/$(MODE)/third_party/nsync:				\
	o/$(MODE)/third_party/nsync/mem			\
	$(THIRD_PARTY_NSYNC_CHECKS)
