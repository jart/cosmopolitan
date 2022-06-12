#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#───vi: set et ft=make ts=8 tw=8 fenc=utf-8 :vi───────────────────────┘

PKGS += LIBC_INTRIN

LIBC_INTRIN_ARTIFACTS += LIBC_INTRIN_A
LIBC_INTRIN = $(LIBC_INTRIN_A_DEPS) $(LIBC_INTRIN_A)
LIBC_INTRIN_A = o/$(MODE)/libc/intrin/intrin.a
LIBC_INTRIN_A_HDRS = $(filter %.h,$(LIBC_INTRIN_A_FILES))
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
	LIBC_NT_KERNEL32

LIBC_INTRIN_A_DEPS :=					\
	$(call uniq,$(foreach x,$(LIBC_INTRIN_A_DIRECTDEPS),$($(x))))

$(LIBC_INTRIN_A):					\
		libc/intrin/				\
		$(LIBC_INTRIN_A).pkg			\
		$(LIBC_INTRIN_A_OBJS)

$(LIBC_INTRIN_A).pkg:					\
		$(LIBC_INTRIN_A_OBJS)			\
		$(foreach x,$(LIBC_INTRIN_A_DIRECTDEPS),$($(x)_A).pkg)

# we can't use asan and ubsan because:
#   this is asan and ubsan
o/$(MODE)/libc/intrin/asan.o				\
o/$(MODE)/libc/intrin/ubsan.o:				\
		OVERRIDE_CFLAGS +=			\
			-fno-sanitize=all		\
			-fno-stack-protector

o/$(MODE)/libc/intrin/asan.o:				\
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
o/$(MODE)/libc/intrin/kprintf.greg.o:			\
		OVERRIDE_CFLAGS +=			\
			-fpie				\
			-fwrapv				\
			-x-no-pg			\
			-mno-fentry			\
			-ffreestanding			\
			-fno-sanitize=all		\
			-fno-stack-protector

# synchronization primitives are intended to be magic free
o/$(MODE)/libc/intrin/gettid.greg.o			\
o/$(MODE)/libc/intrin/pthread_mutex_lock.o		\
o/$(MODE)/libc/intrin/pthread_mutex_unlock.o		\
o/$(MODE)/libc/intrin/pthread_mutex_trylock.o		\
o/$(MODE)/libc/intrin/_trylock_debug_4.o		\
o/$(MODE)/libc/intrin/_spinlock_debug_4.o:		\
		OVERRIDE_CFLAGS +=			\
			-fwrapv				\
			-x-no-pg			\
			-mno-fentry			\
			-ffreestanding			\
			-fno-sanitize=all		\
			-fno-stack-protector

o/$(MODE)/libc/intrin/tls.greg.o			\
o/$(MODE)/libc/intrin/exit.greg.o			\
o/$(MODE)/libc/intrin/exit1.greg.o			\
o/$(MODE)/libc/intrin/getenv.greg.o			\
o/$(MODE)/libc/intrin/assertfail.greg.o			\
o/$(MODE)/libc/intrin/describeiov.greg.o		\
o/$(MODE)/libc/intrin/describestat.greg.o		\
o/$(MODE)/libc/intrin/describeflags.greg.o		\
o/$(MODE)/libc/intrin/describerlimit.greg.o		\
o/$(MODE)/libc/intrin/deviceiocontrol.greg.o		\
o/$(MODE)/libc/intrin/describemapflags.greg.o		\
o/$(MODE)/libc/intrin/describetimespec.greg.o		\
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
o/$(MODE)/libc/intrin/kstarttsc.o			\
o/$(MODE)/libc/intrin/nomultics.o			\
o/$(MODE)/libc/intrin/ntconsolemode.o:			\
		OVERRIDE_CFLAGS +=			\
			-Os				\
			-fwrapv				\
			-ffreestanding			\
			-fno-stack-protector		\
			-fno-sanitize=all

o/$(MODE)/libc/intrin/describeopenflags.greg.o:		\
		OVERRIDE_CPPFLAGS +=			\
			-DSTACK_FRAME_UNLIMITED

o//libc/intrin/memmove.o:				\
		OVERRIDE_CFLAGS +=			\
			-fno-toplevel-reorder

o//libc/intrin/bzero.o					\
o//libc/intrin/memcmp.o					\
o//libc/intrin/memset.o					\
o//libc/intrin/memmove.o:				\
		OVERRIDE_CFLAGS +=			\
			-O2 -finline

o/$(MODE)/libc/intrin/bzero.o				\
o/$(MODE)/libc/intrin/memcmp.o				\
o/$(MODE)/libc/intrin/memmove.o:			\
		OVERRIDE_CFLAGS +=			\
			-fpie

LIBC_INTRIN_LIBS = $(foreach x,$(LIBC_INTRIN_ARTIFACTS),$($(x)))
LIBC_INTRIN_HDRS = $(foreach x,$(LIBC_INTRIN_ARTIFACTS),$($(x)_HDRS))
LIBC_INTRIN_SRCS = $(foreach x,$(LIBC_INTRIN_ARTIFACTS),$($(x)_SRCS))
LIBC_INTRIN_CHECKS = $(foreach x,$(LIBC_INTRIN_ARTIFACTS),$($(x)_CHECKS))
LIBC_INTRIN_OBJS = $(foreach x,$(LIBC_INTRIN_ARTIFACTS),$($(x)_OBJS))
$(LIBC_INTRIN_OBJS): $(BUILD_FILES) libc/intrin/intrin.mk

.PHONY: o/$(MODE)/libc/intrin
o/$(MODE)/libc/intrin: $(LIBC_INTRIN_CHECKS)
