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

$(LIBC_INTRIN_A_OBJS):					\
		OVERRIDE_CFLAGS +=			\
			-foptimize-sibling-calls

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

o/$(MODE)/libc/intrin/kprintf.greg.o:			\
		OVERRIDE_CFLAGS +=			\
			-fpie				\
			-ffreestanding			\
			$(NO_MAGIC)

o/$(MODE)/libc/intrin/createfile.greg.o			\
o/$(MODE)/libc/intrin/createpipe.greg.o			\
o/$(MODE)/libc/intrin/closehandle.greg.o		\
o/$(MODE)/libc/intrin/createthread.greg.o		\
o/$(MODE)/libc/intrin/describeflags.greg.o		\
o/$(MODE)/libc/intrin/createnamedpipe.greg.o		\
o/$(MODE)/libc/intrin/unmapviewoffile.greg.o		\
o/$(MODE)/libc/intrin/flushviewoffile.greg.o		\
o/$(MODE)/libc/intrin/flushfilebuffers.greg.o		\
o/$(MODE)/libc/intrin/mapviewoffileexnuma.greg.o	\
o/$(MODE)/libc/intrin/createfilemappingnuma.greg.o	\
o/$(MODE)/libc/intrin/kstarttsc.o			\
o/$(MODE)/libc/intrin/nomultics.o			\
o/$(MODE)/libc/intrin/ntconsolemode.o:			\
		OVERRIDE_CFLAGS +=			\
			-Os				\
			-ffreestanding			\
			$(NO_MAGIC)

o/$(MODE)/libc/intrin/asan.o				\
o/$(MODE)/libc/intrin/ubsan.o:				\
		OVERRIDE_CFLAGS +=			\
			-fno-sanitize=all		\
			-fno-stack-protector

o//libc/intrin/memmove.o:				\
		OVERRIDE_CFLAGS +=			\
			-fno-toplevel-reorder

o//libc/intrin/bzero.o					\
o//libc/intrin/memcmp.o					\
o//libc/intrin/memset.o					\
o//libc/intrin/memmove.o:				\
		OVERRIDE_CFLAGS +=			\
			-O2

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
