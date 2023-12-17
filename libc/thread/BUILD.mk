#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#── vi: set noet ft=make ts=8 sw=8 fenc=utf-8 :vi ────────────────────┘

PKGS += LIBC_THREAD

LIBC_THREAD_ARTIFACTS += LIBC_THREAD_A
LIBC_THREAD = $(LIBC_THREAD_A_DEPS) $(LIBC_THREAD_A)
LIBC_THREAD_A = o/$(MODE)/libc/thread/thread.a
LIBC_THREAD_A_FILES := $(wildcard libc/thread/*)
LIBC_THREAD_A_HDRS = $(filter %.h,$(LIBC_THREAD_A_FILES))
LIBC_THREAD_A_SRCS_C = $(filter %.c,$(LIBC_THREAD_A_FILES))
LIBC_THREAD_A_SRCS_S = $(filter %.S,$(LIBC_THREAD_A_FILES))

LIBC_THREAD_A_SRCS =					\
	$(LIBC_THREAD_A_SRCS_S)				\
	$(LIBC_THREAD_A_SRCS_C)

LIBC_THREAD_A_OBJS =					\
	$(LIBC_THREAD_A_SRCS_S:%.S=o/$(MODE)/%.o)	\
	$(LIBC_THREAD_A_SRCS_C:%.c=o/$(MODE)/%.o)

LIBC_THREAD_A_CHECKS =					\
	$(LIBC_THREAD_A).pkg				\
	$(LIBC_THREAD_A_HDRS:%=o/$(MODE)/%.ok)

LIBC_THREAD_A_DIRECTDEPS =				\
	LIBC_CALLS					\
	LIBC_INTRIN					\
	LIBC_MEM					\
	LIBC_NEXGEN32E					\
	LIBC_NT_KERNEL32				\
	LIBC_NT_SYNCHRONIZATION				\
	LIBC_RUNTIME					\
	LIBC_STR					\
	LIBC_SYSV					\
	LIBC_SYSV_CALLS					\
	THIRD_PARTY_NSYNC				\
	THIRD_PARTY_NSYNC_MEM

LIBC_THREAD_A_DEPS :=					\
	$(call uniq,$(foreach x,$(LIBC_THREAD_A_DIRECTDEPS),$($(x))))

$(LIBC_THREAD_A):					\
		libc/thread/				\
		$(LIBC_THREAD_A).pkg			\
		$(LIBC_THREAD_A_OBJS)

$(LIBC_THREAD_A).pkg:					\
		$(LIBC_THREAD_A_OBJS)			\
		$(foreach x,$(LIBC_THREAD_A_DIRECTDEPS),$($(x)_A).pkg)

$(LIBC_THREAD_A_OBJS): private				\
		COPTS +=				\
			-fno-sanitize=all		\
			-Wframe-larger-than=4096	\
			-Walloca-larger-than=4096

LIBC_THREAD_LIBS = $(foreach x,$(LIBC_THREAD_ARTIFACTS),$($(x)))
LIBC_THREAD_SRCS = $(foreach x,$(LIBC_THREAD_ARTIFACTS),$($(x)_SRCS))
LIBC_THREAD_HDRS = $(foreach x,$(LIBC_THREAD_ARTIFACTS),$($(x)_HDRS))
LIBC_THREAD_CHECKS = $(foreach x,$(LIBC_THREAD_ARTIFACTS),$($(x)_CHECKS))
LIBC_THREAD_OBJS = $(foreach x,$(LIBC_THREAD_ARTIFACTS),$($(x)_OBJS))
$(LIBC_THREAD_OBJS): $(BUILD_FILES) libc/thread/BUILD.mk

.PHONY: o/$(MODE)/libc/thread
o/$(MODE)/libc/thread: $(LIBC_THREAD_CHECKS)
