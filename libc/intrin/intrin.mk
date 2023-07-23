#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#───vi: set et ft=make ts=8 tw=8 fenc=utf-8 :vi───────────────────────┘

PKGS += LIBC_INTRIN

LIBC_INTRIN_ARTIFACTS += LIBC_INTRIN_A
LIBC_INTRIN = $(LIBC_INTRIN_A_DEPS) $(LIBC_INTRIN_A)
LIBC_INTRIN_A = o/$(MODE)/libc/intrin/intrin.a
LIBC_INTRIN_A_FILES := $(wildcard libc/intrin/*)
LIBC_INTRIN_A_HDRS = $(filter %.h,$(LIBC_INTRIN_A_FILES))
LIBC_INTRIN_A_INCS = $(filter %.inc,$(LIBC_INTRIN_A_FILES))
LIBC_INTRIN_A_SRCS_S = $(filter %.S,$(LIBC_INTRIN_A_FILES))
LIBC_INTRIN_A_SRCS_C = $(filter %.c,$(LIBC_INTRIN_A_FILES))
LIBC_INTRIN_A_SRCS = $(LIBC_INTRIN_A_SRCS_S) $(LIBC_INTRIN_A_SRCS_C)
LIBC_INTRIN_A_CHECKS = $(LIBC_INTRIN_A).pkg

ifeq ($(ARCH), aarch64)
LIBC_INTRIN_A_SRCS_S += $(wildcard libc/intrin/aarch64/*.S)
LIBC_INTRIN_A_HDRS += libc/intrin/aarch64/asmdefs.internal.h
endif

LIBC_INTRIN_A_OBJS =					\
	$(LIBC_INTRIN_A_SRCS_S:%.S=o/$(MODE)/%.o)	\
	$(LIBC_INTRIN_A_SRCS_C:%.c=o/$(MODE)/%.o)

LIBC_INTRIN_A_CHECKS =					\
	$(LIBC_INTRIN_A).pkg				\
	$(LIBC_INTRIN_A_HDRS:%=o/$(MODE)/%.ok)

LIBC_INTRIN_A_DIRECTDEPS =				\
	LIBC_NEXGEN32E					\
	LIBC_NT_KERNEL32				\
	LIBC_NT_WS2_32					\
	LIBC_SYSV					\
	LIBC_SYSV_CALLS

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
		COPTS +=				\
			-fno-sanitize=address

# we can't use asan because:
#   asan guard pages haven't been allocated yet
o/$(MODE)/libc/intrin/directmap.o			\
o/$(MODE)/libc/intrin/directmap-nt.o: private		\
		COPTS +=				\
			-ffreestanding			\
			-fno-sanitize=address

# we want small code size because:
#   to keep .text.head under 4096 bytes
o/$(MODE)/libc/intrin/mman.greg.o: private		\
		COPTS +=				\
			-Os

# we can't use asan and ubsan because:
#   this is asan and ubsan
o/$(MODE)/libc/intrin/asan.o				\
o/$(MODE)/libc/intrin/ubsan.o: private			\
		CFLAGS +=				\
			-ffreestanding			\
			-fno-sanitize=all		\
			-fno-stack-protector

o/$(MODE)/libc/intrin/asan.o: private			\
		CFLAGS +=				\
			-O2				\
			-finline			\
			-finline-functions

o/$(MODE)/libc/intrin/asanthunk.o: private		\
		CFLAGS +=				\
			-x-no-pg			\
			-ffreestanding			\
			-fno-sanitize=all		\
			-fno-stack-protector

# we can't use compiler magic because:
#   kprintf() is mission critical to error reporting
o/$(MODE)/libc/intrin/getmagnumstr.greg.o		\
o/$(MODE)/libc/intrin/strerrno.greg.o			\
o/$(MODE)/libc/intrin/strerrdoc.greg.o			\
o/$(MODE)/libc/intrin/strerror_wr.greg.o		\
o/$(MODE)/libc/intrin/kprintf.greg.o: private		\
		CFLAGS +=				\
			-fpie				\
			-fwrapv				\
			-x-no-pg			\
			-ffreestanding			\
			-fno-sanitize=all		\
			-fno-stack-protector

# TODO(jart): Do we really need these?
# synchronization primitives are intended to be magic free
o/$(MODE)/libc/intrin/futex_wait.o			\
o/$(MODE)/libc/intrin/futex_wake.o			\
o/$(MODE)/libc/intrin/gettid.greg.o			\
o/$(MODE)/libc/intrin/_trylock_debug_4.o		\
o/$(MODE)/libc/intrin/_spinlock_debug_4.o: private	\
		CFLAGS +=				\
			-fwrapv				\
			-x-no-pg			\
			-ffreestanding			\
			-fno-sanitize=all		\
			-mgeneral-regs-only		\
			-fno-stack-protector

# we can't use asan because:
#   global gone could be raised
o/$(MODE)/libc/intrin/exit.o				\
o/$(MODE)/libc/intrin/restorewintty.o: private		\
		CFLAGS +=				\
			-fno-sanitize=all

# we can't use -ftrapv because:
#   this file implements it
o/$(MODE)/libc/intrin/ftrapv.o: private			\
		CFLAGS +=				\
			-ffunction-sections		\
			-ffreestanding			\
			-fwrapv

# we can't use asan because:
#   sys_mmap() calls these which sets up shadow memory
o/$(MODE)/libc/intrin/describeflags.o			\
o/$(MODE)/libc/intrin/describeframe.o			\
o/$(MODE)/libc/intrin/describemapflags.o		\
o/$(MODE)/libc/intrin/describeprotflags.o: private	\
		CFLAGS +=				\
			-fno-sanitize=address

o/$(MODE)/libc/intrin/exit1.greg.o			\
o/$(MODE)/libc/intrin/wsarecv.o				\
o/$(MODE)/libc/intrin/wsarecvfrom.o			\
o/$(MODE)/libc/intrin/createfile.o			\
o/$(MODE)/libc/intrin/cancelioex.o			\
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
o/$(MODE)/libc/intrin/wsagetoverlappedresult.o		\
o/$(MODE)/libc/intrin/generateconsolectrlevent.o	\
o/$(MODE)/libc/intrin/wsawaitformultipleevents.o: private\
		CFLAGS +=				\
			-Os				\
			-fwrapv				\
			-ffreestanding			\
			-fno-stack-protector		\
			-fno-sanitize=all

# privileged functions
o/$(MODE)/libc/intrin/dos2errno.o			\
o/$(MODE)/libc/intrin/have_fsgsbase.o			\
o/$(MODE)/libc/intrin/getmagnumstr.o			\
o/$(MODE)/libc/intrin/formatint32.o			\
o/$(MODE)/libc/intrin/strsignal_r.o			\
o/$(MODE)/libc/intrin/strerror_wr.o: private		\
		CFLAGS +=				\
			-ffreestanding			\
			-fno-sanitize=all

o//libc/intrin/memmove.o: private			\
		CFLAGS +=				\
			-fno-toplevel-reorder

o//libc/intrin/bzero.o					\
o//libc/intrin/memcmp.o					\
o//libc/intrin/memset.o					\
o//libc/intrin/memmove.o: private			\
		CFLAGS +=				\
			-O2 -finline

o/$(MODE)/libc/intrin/bzero.o				\
o/$(MODE)/libc/intrin/memcmp.o				\
o/$(MODE)/libc/intrin/memmove.o: private		\
		CFLAGS +=				\
			-fpie

# these assembly files are safe to build on aarch64
o/$(MODE)/libc/intrin/aarch64/%.o: libc/intrin/aarch64/%.S
	@$(COMPILE) -AOBJECTIFY.S $(OBJECTIFY.S) $(OUTPUT_OPTION) -c $<
o/$(MODE)/libc/intrin/fenv.o: libc/intrin/fenv.S
	@$(COMPILE) -AOBJECTIFY.S $(OBJECTIFY.S) $(OUTPUT_OPTION) -c $<
o/$(MODE)/libc/intrin/gcov.o: libc/intrin/gcov.S
	@$(COMPILE) -AOBJECTIFY.S $(OBJECTIFY.S) $(OUTPUT_OPTION) -c $<
o/$(MODE)/libc/intrin/futex.o: libc/intrin/futex.S
	@$(COMPILE) -AOBJECTIFY.S $(OBJECTIFY.S) $(OUTPUT_OPTION) -c $<
o/$(MODE)/libc/intrin/typeinfo.o: libc/intrin/typeinfo.S
	@$(COMPILE) -AOBJECTIFY.S $(OBJECTIFY.S) $(OUTPUT_OPTION) -c $<
o/$(MODE)/libc/intrin/kclocknames.o: libc/intrin/kclocknames.S
	@$(COMPILE) -AOBJECTIFY.S $(OBJECTIFY.S) $(OUTPUT_OPTION) -c $<
o/$(MODE)/libc/intrin/kdos2errno.o: libc/intrin/kdos2errno.S
	@$(COMPILE) -AOBJECTIFY.S $(OBJECTIFY.S) $(OUTPUT_OPTION) -c $<
o/$(MODE)/libc/intrin/kerrnodocs.o: libc/intrin/kerrnodocs.S
	@$(COMPILE) -AOBJECTIFY.S $(OBJECTIFY.S) $(OUTPUT_OPTION) -c $<
o/$(MODE)/libc/intrin/kipoptnames.o: libc/intrin/kipoptnames.S
	@$(COMPILE) -AOBJECTIFY.S $(OBJECTIFY.S) $(OUTPUT_OPTION) -c $<
o/$(MODE)/libc/intrin/kerrnonames.o: libc/intrin/kerrnonames.S
	@$(COMPILE) -AOBJECTIFY.S $(OBJECTIFY.S) $(OUTPUT_OPTION) -c $<
o/$(MODE)/libc/intrin/kfcntlcmds.o: libc/intrin/kfcntlcmds.S
	@$(COMPILE) -AOBJECTIFY.S $(OBJECTIFY.S) $(OUTPUT_OPTION) -c $<
o/$(MODE)/libc/intrin/kopenflags.o: libc/intrin/kopenflags.S
	@$(COMPILE) -AOBJECTIFY.S $(OBJECTIFY.S) $(OUTPUT_OPTION) -c $<
o/$(MODE)/libc/intrin/krlimitnames.o: libc/intrin/krlimitnames.S
	@$(COMPILE) -AOBJECTIFY.S $(OBJECTIFY.S) $(OUTPUT_OPTION) -c $<
o/$(MODE)/libc/intrin/ksignalnames.o: libc/intrin/ksignalnames.S
	@$(COMPILE) -AOBJECTIFY.S $(OBJECTIFY.S) $(OUTPUT_OPTION) -c $<
o/$(MODE)/libc/intrin/ksockoptnames.o: libc/intrin/ksockoptnames.S
	@$(COMPILE) -AOBJECTIFY.S $(OBJECTIFY.S) $(OUTPUT_OPTION) -c $<
o/$(MODE)/libc/intrin/ktcpoptnames.o: libc/intrin/ktcpoptnames.S
	@$(COMPILE) -AOBJECTIFY.S $(OBJECTIFY.S) $(OUTPUT_OPTION) -c $<
o/$(MODE)/libc/intrin/sched_yield.o: libc/intrin/sched_yield.S
	@$(COMPILE) -AOBJECTIFY.S $(OBJECTIFY.S) $(OUTPUT_OPTION) -c $<

LIBC_INTRIN_LIBS = $(foreach x,$(LIBC_INTRIN_ARTIFACTS),$($(x)))
LIBC_INTRIN_HDRS = $(foreach x,$(LIBC_INTRIN_ARTIFACTS),$($(x)_HDRS))
LIBC_INTRIN_INCS = $(foreach x,$(LIBC_INTRIN_ARTIFACTS),$($(x)_INCS))
LIBC_INTRIN_SRCS = $(foreach x,$(LIBC_INTRIN_ARTIFACTS),$($(x)_SRCS))
LIBC_INTRIN_CHECKS = $(foreach x,$(LIBC_INTRIN_ARTIFACTS),$($(x)_CHECKS))
LIBC_INTRIN_OBJS = $(foreach x,$(LIBC_INTRIN_ARTIFACTS),$($(x)_OBJS))
$(LIBC_INTRIN_OBJS): $(BUILD_FILES) libc/intrin/intrin.mk

.PHONY: o/$(MODE)/libc/intrin
o/$(MODE)/libc/intrin: $(LIBC_INTRIN_CHECKS)
