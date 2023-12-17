#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#── vi: set noet ft=make ts=8 sw=8 fenc=utf-8 :vi ────────────────────┘

PKGS += LIBC_LOG

LIBC_LOG_ARTIFACTS += LIBC_LOG_A
LIBC_LOG = $(LIBC_LOG_A_DEPS) $(LIBC_LOG_A)
LIBC_LOG_A = o/$(MODE)/libc/log/log.a
LIBC_LOG_A_FILES := $(wildcard libc/log/*)
LIBC_LOG_A_HDRS = $(filter %.h,$(LIBC_LOG_A_FILES))
LIBC_LOG_A_SRCS_C = $(filter %.c,$(LIBC_LOG_A_FILES))
LIBC_LOG_A_SRCS_S = $(filter %.S,$(LIBC_LOG_A_FILES))

LIBC_LOG_A_SRCS =					\
	$(LIBC_LOG_A_SRCS_C)				\
	$(LIBC_LOG_A_SRCS_S)

LIBC_LOG_A_OBJS =					\
	$(LIBC_LOG_A_SRCS_C:%.c=o/$(MODE)/%.o)		\
	$(LIBC_LOG_A_SRCS_S:%.S=o/$(MODE)/%.o)

LIBC_LOG_A_CHECKS =					\
	$(LIBC_LOG_A).pkg				\
	$(LIBC_LOG_A_HDRS:%=o/$(MODE)/%.ok)

LIBC_LOG_A_DIRECTDEPS =					\
	LIBC_CALLS					\
	LIBC_ELF					\
	LIBC_FMT					\
	LIBC_INTRIN					\
	LIBC_MEM					\
	LIBC_NEXGEN32E					\
	LIBC_NT_KERNEL32				\
	LIBC_NT_NTDLL					\
	LIBC_PROC					\
	LIBC_RUNTIME					\
	LIBC_STDIO					\
	LIBC_STR					\
	LIBC_SYSV					\
	LIBC_SYSV_CALLS					\
	LIBC_THREAD					\
	LIBC_TIME					\
	LIBC_TINYMATH					\
	THIRD_PARTY_COMPILER_RT				\
	THIRD_PARTY_DLMALLOC				\
	THIRD_PARTY_GDTOA

LIBC_LOG_A_DEPS :=					\
	$(call uniq,$(foreach x,$(LIBC_LOG_A_DIRECTDEPS),$($(x))))

$(LIBC_LOG_A):	libc/log/				\
		$(LIBC_LOG_A).pkg			\
		$(LIBC_LOG_A_OBJS)

$(LIBC_LOG_A).pkg:					\
		$(LIBC_LOG_A_OBJS)			\
		$(foreach x,$(LIBC_LOG_A_DIRECTDEPS),$($(x)_A).pkg)

# offer assurances about the stack safety of cosmo libc
$(LIBC_LOG_A_OBJS): private COPTS += -Wframe-larger-than=4096 -Walloca-larger-than=4096

$(LIBC_RUNTIME_A_OBJS): private				\
		COPTS +=				\
			-fno-sanitize=all		\
			-Wframe-larger-than=4096	\
			-Walloca-larger-than=4096

o/$(MODE)/libc/log/checkfail.o: private			\
		CFLAGS +=				\
			-mgeneral-regs-only

o/$(MODE)/libc/log/watch.o: private			\
		CFLAGS +=				\
			-ffreestanding

LIBC_LOG_LIBS = $(foreach x,$(LIBC_LOG_ARTIFACTS),$($(x)))
LIBC_LOG_SRCS = $(foreach x,$(LIBC_LOG_ARTIFACTS),$($(x)_SRCS))
LIBC_LOG_HDRS = $(foreach x,$(LIBC_LOG_ARTIFACTS),$($(x)_HDRS))
LIBC_LOG_BINS = $(foreach x,$(LIBC_LOG_ARTIFACTS),$($(x)_BINS))
LIBC_LOG_CHECKS = $(foreach x,$(LIBC_LOG_ARTIFACTS),$($(x)_CHECKS))
LIBC_LOG_OBJS = $(foreach x,$(LIBC_LOG_ARTIFACTS),$($(x)_OBJS))
LIBC_LOG_TESTS = $(foreach x,$(LIBC_LOG_ARTIFACTS),$($(x)_TESTS))

$(LIBC_LOG_OBJS): $(BUILD_FILES) libc/log/BUILD.mk

.PHONY: o/$(MODE)/libc/log
o/$(MODE)/libc/log: $(LIBC_LOG_CHECKS)
