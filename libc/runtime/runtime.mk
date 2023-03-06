#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#───vi: set et ft=make ts=8 tw=8 fenc=utf-8 :vi───────────────────────┘
#
# SYNOPSIS
#
#   Cosmopolitan Runtime
#
# DESCRIPTION
#
#   This package exports essential routines for userspace process
#   initialization.

PKGS += LIBC_RUNTIME

LIBC_RUNTIME = $(LIBC_RUNTIME_A_DEPS) $(LIBC_RUNTIME_A)
LIBC_RUNTIME_ARTIFACTS += LIBC_RUNTIME_A
LIBC_RUNTIME_A = o/$(MODE)/libc/runtime/runtime.a
LIBC_RUNTIME_A_FILES := $(wildcard libc/runtime/*)
LIBC_RUNTIME_A_HDRS = $(filter %.h,$(LIBC_RUNTIME_A_FILES))
LIBC_RUNTIME_A_SRCS_S = $(filter %.S,$(LIBC_RUNTIME_A_FILES))
LIBC_RUNTIME_A_SRCS_C = $(filter %.c,$(LIBC_RUNTIME_A_FILES))

LIBC_RUNTIME_A_SRCS =					\
	$(LIBC_RUNTIME_A_SRCS_S)			\
	$(LIBC_RUNTIME_A_SRCS_C)

LIBC_RUNTIME_A_OBJS =					\
	$(LIBC_RUNTIME_A_SRCS_S:%.S=o/$(MODE)/%.o)	\
	$(LIBC_RUNTIME_A_SRCS_C:%.c=o/$(MODE)/%.o)

LIBC_RUNTIME_A_CHECKS =					\
	$(LIBC_RUNTIME_A).pkg				\
	$(LIBC_RUNTIME_A_HDRS:%=o/$(MODE)/%.ok)

LIBC_RUNTIME_A_DIRECTDEPS =				\
	LIBC_CALLS					\
	LIBC_ELF					\
	LIBC_FMT					\
	LIBC_INTRIN					\
	LIBC_NEXGEN32E					\
	LIBC_NT_ADVAPI32				\
	LIBC_NT_KERNEL32				\
	LIBC_NT_SYNCHRONIZATION				\
	LIBC_STR					\
	LIBC_STUBS					\
	LIBC_SYSV					\
	LIBC_SYSV_CALLS					\
	THIRD_PARTY_PUFF				\
	THIRD_PARTY_XED

LIBC_RUNTIME_A_DEPS :=					\
	$(call uniq,$(foreach x,$(LIBC_RUNTIME_A_DIRECTDEPS),$($(x))))

$(LIBC_RUNTIME_A):					\
		libc/runtime/				\
		$(LIBC_RUNTIME_A).pkg			\
		$(LIBC_RUNTIME_A_OBJS)

$(LIBC_RUNTIME_A).pkg:					\
		$(LIBC_RUNTIME_A_OBJS)			\
		$(foreach x,$(LIBC_RUNTIME_A_DIRECTDEPS),$($(x)_A).pkg)

# we can't use asan and ubsan because:
#   asan and ubsan can be function traced
# we can't use function tracing because:
#   this is the function tracing runtime
o/$(MODE)/libc/runtime/ftracer.o: private		\
		OVERRIDE_CFLAGS +=			\
			-x-no-pg			\
			-mno-fentry			\
			-ffreestanding			\
			-fno-sanitize=all

o/$(MODE)/libc/runtime/fork-nt.o			\
o/$(MODE)/libc/runtime/printmemoryintervals.o		\
o/$(MODE)/libc/runtime/arememoryintervalsok.o		\
o/$(MODE)/libc/runtime/findmemoryinterval.o		\
o/$(MODE)/libc/runtime/sys_mprotect.greg.o		\
o/$(MODE)/libc/runtime/getdosargv.o			\
o/$(MODE)/libc/runtime/getdosenviron.o			\
o/$(MODE)/libc/runtime/hook.greg.o			\
o/$(MODE)/libc/runtime/ismemtracked.greg.o		\
o/$(MODE)/libc/runtime/memtracknt.o			\
o/$(MODE)/libc/runtime/memtrack.greg.o			\
o/$(MODE)/libc/runtime/metalprintf.greg.o		\
o/$(MODE)/libc/runtime/printargs.greg.o			\
o/$(MODE)/libc/runtime/mman.greg.o			\
o/$(MODE)/libc/runtime/print.greg.o			\
o/$(MODE)/libc/runtime/stackchkfail.o			\
o/$(MODE)/libc/runtime/stackchkfaillocal.o		\
o/$(MODE)/libc/runtime/winmain.greg.o			\
o/$(MODE)/libc/runtime/opensymboltable.o: private	\
		OVERRIDE_CFLAGS +=			\
			-Os				\
			-ffreestanding			\
			$(NO_MAGIC)

# must use alloca()
# can't use asan or any runtime services
o/$(MODE)/libc/runtime/fork-nt.o: private		\
		OVERRIDE_CPPFLAGS +=			\
			-DSTACK_FRAME_UNLIMITED

o/$(MODE)/libc/runtime/qsort.o: private			\
		OVERRIDE_CFLAGS +=			\
			-Og

# make always linked runtimes less huge when it's profitable
o//libc/runtime/mmap.o					\
o//libc/runtime/munmap.o				\
o//libc/runtime/memtrack.greg.o				\
o//libc/runtime/opensymboltable.greg.o: private		\
		OVERRIDE_CFLAGS +=			\
			-Os

LIBC_RUNTIME_LIBS = $(foreach x,$(LIBC_RUNTIME_ARTIFACTS),$($(x)))
LIBC_RUNTIME_SRCS = $(foreach x,$(LIBC_RUNTIME_ARTIFACTS),$($(x)_SRCS))
LIBC_RUNTIME_HDRS = $(foreach x,$(LIBC_RUNTIME_ARTIFACTS),$($(x)_HDRS))
LIBC_RUNTIME_BINS = $(foreach x,$(LIBC_RUNTIME_ARTIFACTS),$($(x)_BINS))
LIBC_RUNTIME_CHECKS = $(foreach x,$(LIBC_RUNTIME_ARTIFACTS),$($(x)_CHECKS))
LIBC_RUNTIME_OBJS = $(foreach x,$(LIBC_RUNTIME_ARTIFACTS),$($(x)_OBJS))
LIBC_RUNTIME_TESTS = $(foreach x,$(LIBC_RUNTIME_ARTIFACTS),$($(x)_TESTS))
$(LIBC_RUNTIME_OBJS): $(BUILD_FILES) libc/runtime/runtime.mk

.PHONY: o/$(MODE)/libc/runtime
o/$(MODE)/libc/runtime: $(LIBC_RUNTIME_CHECKS)
