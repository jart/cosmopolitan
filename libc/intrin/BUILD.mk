#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#── vi: set noet ft=make ts=8 sw=8 fenc=utf-8 :vi ────────────────────┘

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
LIBC_INTRIN_A_HDRS += libc/intrin/aarch64/asmdefs.h
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
	LIBC_NT_REALTIME				\
	LIBC_NT_SYNCHRONIZATION				\
	LIBC_NT_WS2_32					\
	LIBC_SYSV					\
	LIBC_SYSV_CALLS					\

LIBC_INTRIN_A_DEPS :=					\
	$(call uniq,$(foreach x,$(LIBC_INTRIN_A_DIRECTDEPS),$($(x))))

$(LIBC_INTRIN_A):					\
		libc/intrin/				\
		$(LIBC_INTRIN_A).pkg			\
		$(LIBC_INTRIN_A_OBJS)

$(LIBC_INTRIN_A).pkg:					\
		$(LIBC_INTRIN_A_OBJS)			\
		$(foreach x,$(LIBC_INTRIN_A_DIRECTDEPS),$($(x)_A).pkg)

o/$(MODE)/libc/intrin/mman.greg.o: private COPTS += -Os

$(LIBC_INTRIN_A_OBJS): private				\
		COPTS +=				\
			-x-no-pg			\
			-ffreestanding			\
			-fno-sanitize=all		\
			-fno-stack-protector		\
			-Wframe-larger-than=4096	\
			-Walloca-larger-than=4096

o/$(MODE)/libc/intrin/kprintf.o: private		\
		CFLAGS +=				\
			-Wframe-larger-than=128		\
			-Walloca-larger-than=128

o/$(MODE)/libc/intrin/cursor.o				\
o/$(MODE)/libc/intrin/mmap.o				\
o/$(MODE)/libc/intrin/tree.o: private			\
		CFLAGS +=				\
			-ffunction-sections

o//libc/intrin/memmove.o: private			\
		CFLAGS +=				\
			-fno-toplevel-reorder

o//libc/intrin/bzero.o					\
o//libc/intrin/strlen.o					\
o//libc/intrin/strchr.o					\
o//libc/intrin/memchr.o					\
o//libc/intrin/memrchr.o				\
o//libc/intrin/memcmp.o					\
o//libc/intrin/memset.o					\
o//libc/intrin/memmove.o: private			\
		CFLAGS +=				\
			-O2				\
			-finline			\
			-foptimize-sibling-calls

o/$(MODE)/libc/intrin/bzero.o				\
o/$(MODE)/libc/intrin/memcmp.o				\
o/$(MODE)/libc/intrin/memmove.o: private		\
		CFLAGS +=				\
			-fpie

o/$(MODE)/libc/intrin/x86.o: private			\
		CFLAGS +=				\
			-ffreestanding			\
			-fno-jump-tables		\
			-fpatchable-function-entry=0	\
			-Os

# avoid the legacy sse decoding penalty on avx systems
o//libc/intrin/dll.o					\
o//libc/intrin/fds.o					\
o//libc/intrin/mmap.o					\
o//libc/intrin/demangle.o: private			\
		CFLAGS +=				\
			-mgeneral-regs-only

# ensure that division is optimized
o/$(MODE)/libc/intrin/windowsdurationtotimeval.o	\
o/$(MODE)/libc/intrin/windowsdurationtotimespec.o	\
o/$(MODE)/libc/intrin/timevaltowindowstime.o		\
o/$(MODE)/libc/intrin/timespectowindowstime.o		\
o/$(MODE)/libc/intrin/windowstimetotimeval.o		\
o/$(MODE)/libc/intrin/windowstimetotimespec.o: private	\
		CFLAGS +=				\
			-O2

# these assembly files are safe to build on aarch64
o/$(MODE)/libc/intrin/getcontext.o: libc/intrin/getcontext.S
	@$(COMPILE) -AOBJECTIFY.S $(OBJECTIFY.S) $(OUTPUT_OPTION) -c $<
o/$(MODE)/libc/intrin/swapcontext.o: libc/intrin/swapcontext.S
	@$(COMPILE) -AOBJECTIFY.S $(OBJECTIFY.S) $(OUTPUT_OPTION) -c $<
o/$(MODE)/libc/intrin/tailcontext.o: libc/intrin/tailcontext.S
	@$(COMPILE) -AOBJECTIFY.S $(OBJECTIFY.S) $(OUTPUT_OPTION) -c $<
o/$(MODE)/libc/intrin/aarch64/%.o: libc/intrin/aarch64/%.S
	@$(COMPILE) -AOBJECTIFY.S $(OBJECTIFY.S) $(OUTPUT_OPTION) -c $<
o/$(MODE)/libc/intrin/fenv.o: libc/intrin/fenv.S
	@$(COMPILE) -AOBJECTIFY.S $(OBJECTIFY.S) $(OUTPUT_OPTION) -c $<
o/$(MODE)/libc/intrin/gcov.o: libc/intrin/gcov.S
	@$(COMPILE) -AOBJECTIFY.S $(OBJECTIFY.S) $(OUTPUT_OPTION) -c $<
o/$(MODE)/libc/intrin/cosmo_futex_thunk.o: libc/intrin/cosmo_futex_thunk.S
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
o/$(MODE)/libc/intrin/kipv6optnames.o: libc/intrin/kipv6optnames.S
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
o/$(MODE)/libc/intrin/stackcall.o: libc/intrin/stackcall.S
	@$(COMPILE) -AOBJECTIFY.S $(OBJECTIFY.S) $(OUTPUT_OPTION) -c $<
o/$(MODE)/libc/intrin/kmonthname.o: libc/intrin/kmonthname.S
	@$(COMPILE) -AOBJECTIFY.S $(OBJECTIFY.S) $(OUTPUT_OPTION) -c $<
o/$(MODE)/libc/intrin/kmonthnameshort.o: libc/intrin/kmonthnameshort.S
	@$(COMPILE) -AOBJECTIFY.S $(OBJECTIFY.S) $(OUTPUT_OPTION) -c $<
o/$(MODE)/libc/intrin/kweekdayname.o: libc/intrin/kweekdayname.S
	@$(COMPILE) -AOBJECTIFY.S $(OBJECTIFY.S) $(OUTPUT_OPTION) -c $<
o/$(MODE)/libc/intrin/kweekdaynameshort.o: libc/intrin/kweekdaynameshort.S
	@$(COMPILE) -AOBJECTIFY.S $(OBJECTIFY.S) $(OUTPUT_OPTION) -c $<
o/$(MODE)/libc/intrin/sched_yield.o: libc/intrin/sched_yield.S
	@$(COMPILE) -AOBJECTIFY.S $(OBJECTIFY.S) $(OUTPUT_OPTION) -c $<
o/$(MODE)/libc/intrin/dsohandle.o: libc/intrin/dsohandle.S
	@$(COMPILE) -AOBJECTIFY.S $(OBJECTIFY.S) $(OUTPUT_OPTION) -c $<
o/$(MODE)/libc/intrin/getpagesize_freebsd.o: libc/intrin/getpagesize_freebsd.S
	@$(COMPILE) -AOBJECTIFY.S $(OBJECTIFY.S) $(OUTPUT_OPTION) -c $<

LIBC_INTRIN_LIBS = $(foreach x,$(LIBC_INTRIN_ARTIFACTS),$($(x)))
LIBC_INTRIN_HDRS = $(foreach x,$(LIBC_INTRIN_ARTIFACTS),$($(x)_HDRS))
LIBC_INTRIN_INCS = $(foreach x,$(LIBC_INTRIN_ARTIFACTS),$($(x)_INCS))
LIBC_INTRIN_SRCS = $(foreach x,$(LIBC_INTRIN_ARTIFACTS),$($(x)_SRCS))
LIBC_INTRIN_CHECKS = $(foreach x,$(LIBC_INTRIN_ARTIFACTS),$($(x)_CHECKS))
LIBC_INTRIN_OBJS = $(foreach x,$(LIBC_INTRIN_ARTIFACTS),$($(x)_OBJS))
$(LIBC_INTRIN_OBJS): $(BUILD_FILES) libc/intrin/BUILD.mk

.PHONY: o/$(MODE)/libc/intrin
o/$(MODE)/libc/intrin: $(LIBC_INTRIN_CHECKS)
