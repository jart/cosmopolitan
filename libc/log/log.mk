#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#───vi: set et ft=make ts=8 tw=8 fenc=utf-8 :vi───────────────────────┘

PKGS += LIBC_LOG

LIBC_LOG_ARTIFACTS += LIBC_LOG_A
LIBC_LOG = $(LIBC_LOG_A_DEPS) $(LIBC_LOG_A)
LIBC_LOG_A = o/$(MODE)/libc/log/log.a
LIBC_LOG_A_FILES :=					\
	$(wildcard libc/log/thunks/*)			\
	$(wildcard libc/log/elf/*)			\
	$(wildcard libc/log/*)	
LIBC_LOG_A_HDRS = $(filter %.h,$(LIBC_LOG_A_FILES))
LIBC_LOG_A_SRCS_C = $(filter %.c,$(LIBC_LOG_A_FILES))
LIBC_LOG_A_SRCS_S = $(filter %.S,$(LIBC_LOG_A_FILES))

LIBC_LOG_A_SRCS =					\
	$(LIBC_LOG_A_SRCS_C)				\
	$(LIBC_LOG_A_SRCS_S)

LIBC_LOG_A_OBJS =					\
	$(LIBC_LOG_A_SRCS:%=o/$(MODE)/%.zip.o)		\
	$(LIBC_LOG_A_SRCS_C:%.c=o/$(MODE)/%.o)		\
	$(LIBC_LOG_A_SRCS_S:%.S=o/$(MODE)/%.o)

LIBC_LOG_A_CHECKS =					\
	$(LIBC_LOG_A).pkg				\
	$(LIBC_LOG_A_HDRS:%=o/$(MODE)/%.ok)

LIBC_LOG_A_DIRECTDEPS =					\
	LIBC_ALG					\
	LIBC_CALLS					\
	LIBC_CALLS_HEFTY				\
	LIBC_CONV					\
	LIBC_ELF					\
	LIBC_ESCAPE					\
	LIBC_FMT					\
	LIBC_TINYMATH					\
	LIBC_NEXGEN32E					\
	LIBC_NT_KERNELBASE				\
	LIBC_RAND					\
	LIBC_RUNTIME					\
	LIBC_STDIO					\
	LIBC_STR					\
	LIBC_STUBS					\
	LIBC_NT_NTDLL					\
	LIBC_UNICODE					\
	LIBC_SYSV					\
	LIBC_TIME					\
	THIRD_PARTY_DLMALLOC

LIBC_LOG_A_DEPS :=					\
	$(call uniq,$(foreach x,$(LIBC_LOG_A_DIRECTDEPS),$($(x))))

$(LIBC_LOG_A):	libc/log/				\
		$(LIBC_LOG_A).pkg			\
		$(LIBC_LOG_A_OBJS)

$(LIBC_LOG_A).pkg:					\
		$(LIBC_LOG_A_OBJS)			\
		$(foreach x,$(LIBC_LOG_A_DIRECTDEPS),$($(x)_A).pkg)

o/$(MODE)/libc/log/die.o				\
o/$(MODE)/libc/log/perror.o				\
o/$(MODE)/libc/log/ftrace.o				\
o/$(MODE)/libc/log/ubsan.o				\
o/$(MODE)/libc/log/symbols.o				\
o/$(MODE)/libc/log/backtrace.o				\
o/$(MODE)/libc/log/oncrash.o				\
o/$(MODE)/libc/log/shadowargs.o				\
o/$(MODE)/libc/log/thunks/__check_fail_ndebug.o:	\
	OVERRIDE_COPTS +=				\
		$(NO_MAGIC)

LIBC_LOG_LIBS = $(foreach x,$(LIBC_LOG_ARTIFACTS),$($(x)))
LIBC_LOG_SRCS = $(foreach x,$(LIBC_LOG_ARTIFACTS),$($(x)_SRCS))
LIBC_LOG_HDRS = $(foreach x,$(LIBC_LOG_ARTIFACTS),$($(x)_HDRS))
LIBC_LOG_BINS = $(foreach x,$(LIBC_LOG_ARTIFACTS),$($(x)_BINS))
LIBC_LOG_CHECKS = $(foreach x,$(LIBC_LOG_ARTIFACTS),$($(x)_CHECKS))
LIBC_LOG_OBJS = $(foreach x,$(LIBC_LOG_ARTIFACTS),$($(x)_OBJS))
LIBC_LOG_TESTS = $(foreach x,$(LIBC_LOG_ARTIFACTS),$($(x)_TESTS))

$(LIBC_LOG_OBJS): $(BUILD_FILES) libc/log/log.mk

.PHONY: o/$(MODE)/libc/log
o/$(MODE)/libc/log: $(LIBC_LOG_CHECKS)
