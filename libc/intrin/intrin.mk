#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#───vi: set et ft=make ts=8 tw=8 fenc=utf-8 :vi───────────────────────┘

PKGS += LIBC_INTRIN

LIBC_INTRIN_ARTIFACTS += LIBC_INTRIN_A
LIBC_INTRIN = $(LIBC_INTRIN_A_DEPS) $(LIBC_INTRIN_A)
LIBC_INTRIN_A = o/$(MODE)/libc/intrin/intrin.a
LIBC_INTRIN_A_HDRS = $(filter %.h,$(LIBC_INTRIN_A_FILES))
LIBC_INTRIN_A_INCS = $(filter %.inc,$(LIBC_INTRIN_A_FILES))
LIBC_INTRIN_A_SRCS_S = $(filter %.S,$(LIBC_INTRIN_A_FILES))
LIBC_INTRIN_A_SRCS_C = $(filter %.c,$(LIBC_INTRIN_A_FILES))
LIBC_INTRIN_A_SRCS = $(LIBC_INTRIN_A_SRCS_S) $(LIBC_INTRIN_A_SRCS_C)
LIBC_INTRIN_A_CHECKS = $(LIBC_INTRIN_A).pkg

LIBC_INTRIN_A_FILES :=					\
	$(wildcard libc/intrin/*)

LIBC_INTRIN_A_OBJS =					\
	$(LIBC_INTRIN_A_SRCS_S:%.S=o/$(MODE)/%.o)	\
	$(LIBC_INTRIN_A_SRCS_C:%.c=o/$(MODE)/%.o)

LIBC_INTRIN_A_CHECKS =					\
	$(LIBC_INTRIN_A).pkg				\
	$(LIBC_INTRIN_A_HDRS:%=o/$(MODE)/%.ok)

LIBC_INTRIN_A_DIRECTDEPS =				\
	LIBC_STUBS					\
	LIBC_SYSV					\
	LIBC_SYSV_CALLS					\
	LIBC_NEXGEN32E					\
	LIBC_NT_KERNEL32				\
	LIBC_NT_WS2_32

LIBC_INTRIN_A_DEPS :=					\
	$(call uniq,$(foreach x,$(LIBC_INTRIN_A_DIRECTDEPS),$($(x))))

$(LIBC_INTRIN_A):					\
		libc/intrin/				\
		$(LIBC_INTRIN_A).pkg			\
		$(LIBC_INTRIN_A_OBJS)

$(LIBC_INTRIN_A).pkg:					\
		$(LIBC_INTRIN_A_OBJS)			\
		$(foreach x,$(LIBC_INTRIN_A_DIRECTDEPS),$($(x)_A).pkg)

# we can't use asan because:
#   __strace_init() calls this before asan is initialized
o/$(MODE)/libc/intrin/strace_enabled.o: private		\
		OVERRIDE_COPTS +=			\
			-fno-sanitize=address

# we can't use asan because:
#   asan guard pages haven't been allocated yet
o/$(MODE)/libc/intrin/directmap.o			\
o/$(MODE)/libc/intrin/directmap-nt.o: private		\
		OVERRIDE_COPTS +=			\
			-ffreestanding			\
			-fno-sanitize=address

# we want small code size because:
#   to keep .text.head under 4096 bytes
o/$(MODE)/libc/intrin/mman.greg.o: private		\
		OVERRIDE_COPTS +=			\
			-Os

# we can't use asan and ubsan because:
#   this is asan and ubsan
o/$(MODE)/libc/intrin/asan.o				\
o/$(MODE)/libc/intrin/ubsan.o: private			\
		OVERRIDE_CFLAGS +=			\
			-fno-sanitize=all		\
			-fno-stack-protector

o/$(MODE)/libc/intrin/asan.o: private			\
		OVERRIDE_CFLAGS +=			\
			-O2				\
			-finline			\
			-finline-functions

# we can't use compiler magic because:
#   kprintf() is mission critical to error reporting
o/$(MODE)/libc/intrin/getmagnumstr.greg.o		\
o/$(MODE)/libc/intrin/strerrno.greg.o			\
o/$(MODE)/libc/intrin/strerrdoc.greg.o			\
o/$(MODE)/libc/intrin/strerror_wr.greg.o		\
o/$(MODE)/libc/intrin/kprintf.greg.o: private		\
		OVERRIDE_CFLAGS +=			\
			-fpie				\
			-fwrapv				\
			-x-no-pg			\
			-mno-fentry			\
			-ffreestanding			\
			-fno-sanitize=all		\
			-fno-stack-protector

# TODO(jart): Do we really need these?
# synchronization primitives are intended to be magic free
o/$(MODE)/libc/intrin/futex_wait.o			\
o/$(MODE)/libc/intrin/futex_wake.o			\
o/$(MODE)/libc/intrin/gettid.greg.o			\
o/$(MODE)/libc/intrin/sys_gettid.greg.o			\
o/$(MODE)/libc/intrin/_trylock_debug_4.o		\
o/$(MODE)/libc/intrin/_spinlock_debug_4.o: private	\
		OVERRIDE_CFLAGS +=			\
			-fwrapv				\
			-x-no-pg			\
			-mno-fentry			\
			-ffreestanding			\
			-fno-sanitize=all		\
			-mgeneral-regs-only		\
			-fno-stack-protector

# we can't use asan because:
#   global gone could be raised
o/$(MODE)/libc/intrin/exit.o				\
o/$(MODE)/libc/intrin/restorewintty.o: private		\
		OVERRIDE_CFLAGS +=			\
			-fno-sanitize=all

# we can't use asan because:
#   sys_mmap() calls these which sets up shadow memory
o/$(MODE)/libc/intrin/describeflags.o			\
o/$(MODE)/libc/intrin/describeframe.o			\
o/$(MODE)/libc/intrin/describemapflags.o		\
o/$(MODE)/libc/intrin/describeprotflags.o: private	\
		OVERRIDE_CFLAGS +=			\
			-fno-sanitize=address

o/$(MODE)/libc/intrin/exit1.greg.o			\
o/$(MODE)/libc/intrin/wsarecv.o				\
o/$(MODE)/libc/intrin/wsarecvfrom.o			\
o/$(MODE)/libc/intrin/createfile.o			\
o/$(MODE)/libc/intrin/reopenfile.o			\
o/$(MODE)/libc/intrin/deletefile.o			\
o/$(MODE)/libc/intrin/createpipe.o			\
o/$(MODE)/libc/intrin/closehandle.o			\
o/$(MODE)/libc/intrin/openprocess.o			\
o/$(MODE)/libc/intrin/createthread.o			\
o/$(MODE)/libc/intrin/findclose.o			\
o/$(MODE)/libc/intrin/findnextfile.o			\
o/$(MODE)/libc/intrin/createprocess.o			\
o/$(MODE)/libc/intrin/findfirstfile.o			\
o/$(MODE)/libc/intrin/removedirectory.o			\
o/$(MODE)/libc/intrin/createsymboliclink.o		\
o/$(MODE)/libc/intrin/createnamedpipe.o			\
o/$(MODE)/libc/intrin/unmapviewoffile.o			\
o/$(MODE)/libc/intrin/virtualprotect.o			\
o/$(MODE)/libc/intrin/flushviewoffile.o			\
o/$(MODE)/libc/intrin/createdirectory.o			\
o/$(MODE)/libc/intrin/flushfilebuffers.o		\
o/$(MODE)/libc/intrin/terminateprocess.o		\
o/$(MODE)/libc/intrin/getfileattributes.o		\
o/$(MODE)/libc/intrin/getexitcodeprocess.o		\
o/$(MODE)/libc/intrin/waitforsingleobject.o		\
o/$(MODE)/libc/intrin/setcurrentdirectory.o		\
o/$(MODE)/libc/intrin/mapviewoffileex.o			\
o/$(MODE)/libc/intrin/movefileex.o			\
o/$(MODE)/libc/intrin/mapviewoffileexnuma.o		\
o/$(MODE)/libc/intrin/createfilemapping.o		\
o/$(MODE)/libc/intrin/createfilemappingnuma.o		\
o/$(MODE)/libc/intrin/waitformultipleobjects.o		\
o/$(MODE)/libc/intrin/generateconsolectrlevent.o	\
o/$(MODE)/libc/intrin/wsawaitformultipleevents.o: private\
		OVERRIDE_CFLAGS +=			\
			-Os				\
			-fwrapv				\
			-ffreestanding			\
			-fno-stack-protector		\
			-fno-sanitize=all

o//libc/intrin/memmove.o: private			\
		OVERRIDE_CFLAGS +=			\
			-fno-toplevel-reorder

o//libc/intrin/bzero.o					\
o//libc/intrin/memcmp.o					\
o//libc/intrin/memset.o					\
o//libc/intrin/memmove.o: private			\
		OVERRIDE_CFLAGS +=			\
			-O2 -finline

o/$(MODE)/libc/intrin/bzero.o				\
o/$(MODE)/libc/intrin/memcmp.o				\
o/$(MODE)/libc/intrin/memmove.o: private		\
		OVERRIDE_CFLAGS +=			\
			-fpie

LIBC_INTRIN_LIBS = $(foreach x,$(LIBC_INTRIN_ARTIFACTS),$($(x)))
LIBC_INTRIN_HDRS = $(foreach x,$(LIBC_INTRIN_ARTIFACTS),$($(x)_HDRS))
LIBC_INTRIN_INCS = $(foreach x,$(LIBC_INTRIN_ARTIFACTS),$($(x)_INCS))
LIBC_INTRIN_SRCS = $(foreach x,$(LIBC_INTRIN_ARTIFACTS),$($(x)_SRCS))
LIBC_INTRIN_CHECKS = $(foreach x,$(LIBC_INTRIN_ARTIFACTS),$($(x)_CHECKS))
LIBC_INTRIN_OBJS = $(foreach x,$(LIBC_INTRIN_ARTIFACTS),$($(x)_OBJS))
$(LIBC_INTRIN_OBJS): $(BUILD_FILES) libc/intrin/intrin.mk

.PHONY: o/$(MODE)/libc/intrin
o/$(MODE)/libc/intrin: $(LIBC_INTRIN_CHECKS)
