#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#── vi: set noet ft=make ts=8 sw=8 fenc=utf-8 :vi ────────────────────┘

PKGS += LIBC_PROC

LIBC_PROC_ARTIFACTS += LIBC_PROC_A
LIBC_PROC = $(LIBC_PROC_A_DEPS) $(LIBC_PROC_A)
LIBC_PROC_A = o/$(MODE)/libc/proc/proc.a
LIBC_PROC_A_FILES := $(wildcard libc/proc/*) $(wildcard libc/proc/unlocked/*)
LIBC_PROC_A_HDRS = $(filter %.h,$(LIBC_PROC_A_FILES))
LIBC_PROC_A_SRCS_S = $(filter %.S,$(LIBC_PROC_A_FILES))
LIBC_PROC_A_SRCS_C = $(filter %.c,$(LIBC_PROC_A_FILES))

LIBC_PROC_A_SRCS =					\
	$(LIBC_PROC_A_SRCS_S)				\
	$(LIBC_PROC_A_SRCS_C)

LIBC_PROC_A_OBJS =					\
	$(LIBC_PROC_A_SRCS_S:%.S=o/$(MODE)/%.o)	\
	$(LIBC_PROC_A_SRCS_C:%.c=o/$(MODE)/%.o)

LIBC_PROC_A_CHECKS =					\
	$(LIBC_PROC_A).pkg				\
	$(LIBC_PROC_A_HDRS:%=o/$(MODE)/%.ok)

LIBC_PROC_A_DIRECTDEPS =				\
	LIBC_CALLS					\
	LIBC_FMT					\
	LIBC_INTRIN					\
	LIBC_MEM					\
	LIBC_NEXGEN32E					\
	LIBC_NT_KERNEL32				\
	LIBC_NT_PSAPI					\
	LIBC_RUNTIME					\
	LIBC_STR					\
	LIBC_SYSV					\
	LIBC_SYSV_CALLS					\
	THIRD_PARTY_NSYNC

LIBC_PROC_A_DEPS :=					\
	$(call uniq,$(foreach x,$(LIBC_PROC_A_DIRECTDEPS),$($(x))))

$(LIBC_PROC_A):libc/proc/				\
		$(LIBC_PROC_A).pkg			\
		$(LIBC_PROC_A_OBJS)

$(LIBC_PROC_A).pkg:					\
		$(LIBC_PROC_A_OBJS)			\
		$(foreach x,$(LIBC_PROC_A_DIRECTDEPS),$($(x)_A).pkg)

$(LIBC_PROC_A_OBJS): private				\
		COPTS +=				\
			-fno-sanitize=address		\
			-Wframe-larger-than=4096	\
			-Walloca-larger-than=4096

# aarch64 friendly assembly code
o/$(MODE)/libc/proc/vfork.o: libc/proc/vfork.S
	@$(COMPILE) -AOBJECTIFY.S $(OBJECTIFY.S) $(OUTPUT_OPTION) -c $<

LIBC_PROC_LIBS = $(foreach x,$(LIBC_PROC_ARTIFACTS),$($(x)))
LIBC_PROC_SRCS = $(foreach x,$(LIBC_PROC_ARTIFACTS),$($(x)_SRCS))
LIBC_PROC_HDRS = $(foreach x,$(LIBC_PROC_ARTIFACTS),$($(x)_HDRS))
LIBC_PROC_CHECKS = $(foreach x,$(LIBC_PROC_ARTIFACTS),$($(x)_CHECKS))
LIBC_PROC_OBJS = $(foreach x,$(LIBC_PROC_ARTIFACTS),$($(x)_OBJS))
$(LIBC_PROC_OBJS): $(BUILD_FILES) libc/proc/BUILD.mk

.PHONY: o/$(MODE)/libc/proc
o/$(MODE)/libc/proc: $(LIBC_PROC_CHECKS)
