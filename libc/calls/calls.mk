#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#───vi: set et ft=make ts=8 tw=8 fenc=utf-8 :vi───────────────────────┘
#
# SYNOPSIS
#
#   System Call Compatibility Layer
#
# DESCRIPTION
#
#   This package exports a familiar system call interface that works
#   across platforms.

PKGS += LIBC_CALLS

LIBC_CALLS_ARTIFACTS += LIBC_CALLS_A
LIBC_CALLS = $(LIBC_CALLS_A_DEPS) $(LIBC_CALLS_A)
LIBC_CALLS_A = o/$(MODE)/libc/calls/syscalls.a
LIBC_CALLS_A_FILES :=					\
	$(wildcard libc/calls/typedef/*)		\
	$(wildcard libc/calls/struct/*)			\
	$(wildcard libc/calls/*)
LIBC_CALLS_A_HDRS = $(filter %.h,$(LIBC_CALLS_A_FILES))
LIBC_CALLS_A_INCS = $(filter %.inc,$(LIBC_CALLS_A_FILES))
LIBC_CALLS_A_SRCS_S = $(filter %.S,$(LIBC_CALLS_A_FILES))
LIBC_CALLS_A_SRCS_C = $(filter %.c,$(LIBC_CALLS_A_FILES))

LIBC_CALLS_A_SRCS =					\
	$(LIBC_CALLS_A_SRCS_S)				\
	$(LIBC_CALLS_A_SRCS_C)

LIBC_CALLS_A_OBJS =					\
	$(LIBC_CALLS_A_SRCS_S:%.S=o/$(MODE)/%.o)	\
	$(LIBC_CALLS_A_SRCS_C:%.c=o/$(MODE)/%.o)

LIBC_CALLS_A_CHECKS =					\
	$(LIBC_CALLS_A).pkg				\
	$(LIBC_CALLS_A_HDRS:%=o/$(MODE)/%.ok)

LIBC_CALLS_A_DIRECTDEPS =				\
	LIBC_FMT					\
	LIBC_INTRIN					\
	LIBC_NEXGEN32E					\
	LIBC_NT_ADVAPI32				\
	LIBC_NT_IPHLPAPI				\
	LIBC_NT_KERNEL32				\
	LIBC_NT_NTDLL					\
	LIBC_NT_PDH					\
	LIBC_NT_POWRPROF				\
	LIBC_NT_PSAPI					\
	LIBC_NT_WS2_32					\
	LIBC_STR					\
	LIBC_SYSV					\
	LIBC_SYSV_CALLS					\
	THIRD_PARTY_COMPILER_RT

LIBC_CALLS_A_DEPS :=					\
	$(call uniq,$(foreach x,$(LIBC_CALLS_A_DIRECTDEPS),$($(x))))

$(LIBC_CALLS_A):					\
		libc/calls/				\
		$(LIBC_CALLS_A).pkg			\
		$(LIBC_CALLS_A_OBJS)

$(LIBC_CALLS_A).pkg:					\
		$(LIBC_CALLS_A_OBJS)			\
		$(foreach x,$(LIBC_CALLS_A_DIRECTDEPS),$($(x)_A).pkg)

# we can't use asan because:
#   siginfo_t memory is owned by kernels
o/$(MODE)/libc/calls/siginfo2cosmo.o: private		\
		COPTS +=				\
			-ffreestanding			\
			-fno-sanitize=address

# we can't use asan because:
#   ucontext_t memory is owned by xnu kernel
o/$(MODE)/libc/calls/sigenter-xnu.o: private		\
		COPTS +=				\
			-ffreestanding			\
			-fno-sanitize=address

# we can't use asan because:
#   vdso memory is owned by linux kernel
o/$(MODE)/libc/calls/vdsofunc.greg.o: private		\
		COPTS +=				\
			-ffreestanding			\
			-fno-sanitize=address

# we can't use asan because:
#   ntspawn allocates 128kb of heap memory via win32
o/$(MODE)/libc/calls/ntspawn.o				\
o/$(MODE)/libc/calls/mkntcmdline.o			\
o/$(MODE)/libc/calls/mkntenvblock.o: private		\
		COPTS +=				\
			-ffreestanding			\
			-fno-sanitize=address

# we can't use sanitizers because:
#   windows owns the data structure
o/$(MODE)/libc/calls/wincrash.o				\
o/$(MODE)/libc/calls/ntcontext2linux.o: private		\
		COPTS +=				\
			-fno-sanitize=all		\
			-fpatchable-function-entry=0,0

ifneq ($(ARCH), aarch64)
# we always want -O3 because:
#   it makes the code size smaller too
# we need -mstringop-strategy=loop because:
#   privileged code might generate memcpy call
o/$(MODE)/libc/calls/termios2host.o			\
o/$(MODE)/libc/calls/sigenter-freebsd.o			\
o/$(MODE)/libc/calls/sigenter-netbsd.o			\
o/$(MODE)/libc/calls/sigenter-openbsd.o			\
o/$(MODE)/libc/calls/sigenter-xnu.o			\
o/$(MODE)/libc/calls/ntcontext2linux.o: private		\
		COPTS +=				\
			-O3				\
			-mstringop-strategy=loop
endif

# we must disable static stack safety because:
#   these functions use alloca(n)
o/$(MODE)/libc/calls/execl.o				\
o/$(MODE)/libc/calls/execle.o				\
o/$(MODE)/libc/calls/execlp.o				\
o/$(MODE)/libc/calls/execvpe.o				\
o/$(MODE)/libc/calls/statfs.o				\
o/$(MODE)/libc/calls/fstatfs.o				\
o/$(MODE)/libc/calls/execve-sysv.o			\
o/$(MODE)/libc/calls/readlinkat-nt.o			\
o/$(MODE)/libc/calls/execve-nt.greg.o			\
o/$(MODE)/libc/calls/mkntenvblock.o: private		\
		CPPFLAGS +=				\
			-DSTACK_FRAME_UNLIMITED

# we always want -Os because:
#   va_arg codegen is very bloated in default mode
o//libc/calls/open.o					\
o//libc/calls/openat.o					\
o//libc/calls/prctl.o:					\
		CFLAGS +=				\
			-Os

# we always want -Os because:
#   it's early runtime mandatory and quite huge without it
o//libc/calls/getcwd.greg.o				\
o//libc/calls/getcwd-nt.greg.o				\
o//libc/calls/getcwd-xnu.greg.o				\
o//libc/calls/statfs2cosmo.o: private			\
		CFLAGS +=				\
			-Os

# we always want -O2 because:
#   division is expensive if not optimized
o/$(MODE)/libc/calls/clock.o				\
o/$(MODE)/libc/calls/timespec_tomillis.o		\
o/$(MODE)/libc/calls/timespec_tomicros.o		\
o/$(MODE)/libc/calls/timespec_totimeval.o		\
o/$(MODE)/libc/calls/timespec_fromnanos.o		\
o/$(MODE)/libc/calls/timespec_frommillis.o		\
o/$(MODE)/libc/calls/timespec_frommicros.o		\
o/$(MODE)/libc/calls/timeval_tomillis.o			\
o/$(MODE)/libc/calls/timeval_frommillis.o		\
o/$(MODE)/libc/calls/timeval_frommicros.o: private	\
		CFLAGS +=				\
			-O2

# privileged functions
o/$(MODE)/libc/calls/sigenter-freebsd.o			\
o/$(MODE)/libc/calls/sigenter-netbsd.o			\
o/$(MODE)/libc/calls/sigenter-openbsd.o			\
o/$(MODE)/libc/calls/sigenter-linux.o			\
o/$(MODE)/libc/calls/sigenter-xnu.o			\
o/$(MODE)/libc/calls/pledge-linux.o			\
o/$(MODE)/libc/calls/siginfo2cosmo.o: private		\
		CFLAGS +=				\
			-ffreestanding			\
			-fno-sanitize=all		\
			-fno-stack-protector

o/$(MODE)/libc/calls/pledge-linux.o			\
o/$(MODE)/libc/calls/unveil.o: private			\
		CFLAGS +=				\
			-DSTACK_FRAME_UNLIMITED

ifeq ($(ARCH), aarch64)
o/$(MODE)/libc/calls/sigaction.o: private CFLAGS += -mcmodel=large
o/$(MODE)/libc/calls/getloadavg-nt.o: private CFLAGS += -ffreestanding
endif

# we want -Os because:
#   it makes a big difference
# we need pic because:
#   so it can be an LD_PRELOAD payload
o/$(MODE)/libc/calls/pledge-linux.o: private		\
		CFLAGS +=				\
			-Os				\
			-fPIC

# these assembly files are safe to build on aarch64
o/$(MODE)/libc/calls/getcontext.o: libc/calls/getcontext.S
	@$(COMPILE) -AOBJECTIFY.S $(OBJECTIFY.S) $(OUTPUT_OPTION) -c $<
o/$(MODE)/libc/calls/swapcontext.o: libc/calls/swapcontext.S
	@$(COMPILE) -AOBJECTIFY.S $(OBJECTIFY.S) $(OUTPUT_OPTION) -c $<
o/$(MODE)/libc/calls/tailcontext.o: libc/calls/tailcontext.S
	@$(COMPILE) -AOBJECTIFY.S $(OBJECTIFY.S) $(OUTPUT_OPTION) -c $<

LIBC_CALLS_LIBS = $(foreach x,$(LIBC_CALLS_ARTIFACTS),$($(x)))
LIBC_CALLS_SRCS = $(foreach x,$(LIBC_CALLS_ARTIFACTS),$($(x)_SRCS))
LIBC_CALLS_HDRS = $(foreach x,$(LIBC_CALLS_ARTIFACTS),$($(x)_HDRS))
LIBC_CALLS_INCS = $(foreach x,$(LIBC_CALLS_ARTIFACTS),$($(x)_INCS))
LIBC_CALLS_BINS = $(foreach x,$(LIBC_CALLS_ARTIFACTS),$($(x)_BINS))
LIBC_CALLS_CHECKS = $(foreach x,$(LIBC_CALLS_ARTIFACTS),$($(x)_CHECKS))
LIBC_CALLS_OBJS = $(foreach x,$(LIBC_CALLS_ARTIFACTS),$($(x)_OBJS))
LIBC_CALLS_TESTS = $(foreach x,$(LIBC_CALLS_ARTIFACTS),$($(x)_TESTS))

.PHONY: o/$(MODE)/libc/calls
o/$(MODE)/libc/calls: $(LIBC_CALLS_CHECKS)
