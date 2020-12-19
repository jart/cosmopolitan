#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#───vi: set et ft=make ts=8 tw=8 fenc=utf-8 :vi───────────────────────┘

PKGS += LIBC_SYSV
LIBC_SYSV_LIBS = $(foreach x,$(LIBC_SYSV_ARTIFACTS),$($(x)_A))
LIBC_SYSV_ARCHIVES = $(foreach x,$(LIBC_SYSV_ARTIFACTS),$($(x)_A))
LIBC_SYSV_HDRS = $(foreach x,$(LIBC_SYSV_ARTIFACTS),$($(x)_HDRS))
LIBC_SYSV_BINS = $(foreach x,$(LIBC_SYSV_ARTIFACTS),$($(x)_BINS))
LIBC_SYSV_CHECKS = $(foreach x,$(LIBC_SYSV_ARTIFACTS),$($(x)_CHECKS))
LIBC_SYSV_OBJS = $(foreach x,$(LIBC_SYSV_ARTIFACTS),$($(x)_OBJS))
LIBC_SYSV_TESTS = $(foreach x,$(LIBC_SYSV_ARTIFACTS),$($(x)_TESTS))
LIBC_SYSV_SRCS = $(foreach x,$(LIBC_SYSV_ARTIFACTS),$($(x)_SRCS))

#───────────────────────────────────────────────────────────────────────────────

LIBC_SYSV =						\
	$(LIBC_SYSV_A_DEPS)				\
	$(LIBC_SYSV_A)

LIBC_SYSV_ARTIFACTS += LIBC_SYSV_A
LIBC_SYSV_A = o/$(MODE)/libc/sysv/sysv.a
LIBC_SYSV_A_HDRS = $(filter %.h,$(LIBC_SYSV_A_FILES))
LIBC_SYSV_A_SRCS_A = $(filter %.s,$(LIBC_SYSV_A_FILES))
LIBC_SYSV_A_SRCS_S = $(filter %.S,$(LIBC_SYSV_A_FILES))
LIBC_SYSV_A_CHECKS = $(LIBC_SYSV_A).pkg

LIBC_SYSV_A_DIRECTDEPS =				\
	LIBC_STUBS

LIBC_SYSV_A_FILES :=					\
	libc/sysv/macros.internal.h			\
	libc/sysv/errfuns.h				\
	libc/sysv/g_syscount.S				\
	libc/sysv/restorert.S				\
	libc/sysv/syscall.S				\
	libc/sysv/systemfive.S				\
	$(wildcard libc/sysv/stubs/*)			\
	$(wildcard libc/sysv/consts/*)			\
	$(wildcard libc/sysv/errfuns/*)

LIBC_SYSV_A_SRCS =					\
	$(LIBC_SYSV_A_SRCS_A)				\
	$(LIBC_SYSV_A_SRCS_S)

LIBC_SYSV_A_OBJS =					\
	$(LIBC_SYSV_A_SRCS_A:%.s=o/$(MODE)/%.o)		\
	$(LIBC_SYSV_A_SRCS_S:%.S=o/$(MODE)/%.o)		\
	o/$(MODE)/libc/sysv/g_syscount.S.zip.o		\
	o/$(MODE)/libc/sysv/restorert.S.zip.o		\
	o/$(MODE)/libc/sysv/syscall.S.zip.o		\
	o/$(MODE)/libc/sysv/systemfive.S.zip.o

LIBC_SYSV_A_DEPS :=					\
	$(call uniq,$(foreach x,$(LIBC_SYSV_A_DIRECTDEPS),$($(x))))

$(LIBC_SYSV_A):	libc/sysv/				\
		libc/sysv/consts/			\
		$(LIBC_SYSV_A).pkg			\
		$(LIBC_SYSV_A_OBJS)

$(LIBC_SYSV_A).pkg:					\
		$(LIBC_SYSV_A_OBJS)			\
		$(foreach x,$(LIBC_SYSV_A_DIRECTDEPS),$($(x)_A).pkg)

$(LIBC_SYSV_A_OBJS):					\
		libc/sysv/consts/syscon.inc

libc/sysv/consts/syscon.inc: libc/macros.internal.inc

#───────────────────────────────────────────────────────────────────────────────

LIBC_SYSV_CALLS =					\
	$(LIBC_SYSV_CALLS_A_DEPS)			\
	$(LIBC_SYSV_CALLS_A)

LIBC_SYSV_ARTIFACTS += LIBC_SYSV_CALLS_A
LIBC_SYSV_CALLS_A = o/$(MODE)/libc/sysv/calls.a
LIBC_SYSV_CALLS_A_SRCS := $(wildcard libc/sysv/calls/*.s)
LIBC_SYSV_CALLS_A_OBJS = $(LIBC_SYSV_CALLS_A_SRCS:%.s=o/$(MODE)/%.o)
LIBC_SYSV_CALLS_A_CHECKS = $(LIBC_SYSV_CALLS_A).pkg

LIBC_SYSV_CALLS_A_DIRECTDEPS =				\
	LIBC_SYSV

LIBC_SYSV_CALLS_A_DEPS :=				\
	$(call uniq,$(foreach x,$(LIBC_SYSV_CALLS_A_DIRECTDEPS),$($(x))))

$(LIBC_SYSV_CALLS_A):					\
		libc/sysv/calls/			\
		$(LIBC_SYSV_CALLS_A).pkg		\
		$(LIBC_SYSV_CALLS_A_OBJS)

$(LIBC_SYSV_CALLS_A).pkg:				\
		$(LIBC_SYSV_CALLS_A_OBJS)		\
		$(foreach x,$(LIBC_SYSV_CALLS_A_DIRECTDEPS),$($(x)_A).pkg)

$(LIBC_SYSV_CALLS_A_OBJS):				\
		o/libc/sysv/macros.internal.inc

#───────────────────────────────────────────────────────────────────────────────

LIBC_SYSV_MACHCALLS =					\
	$(LIBC_SYSV_MACHCALLS_A_DEPS)			\
	$(LIBC_SYSV_MACHCALLS_A)

LIBC_SYSV_ARTIFACTS += LIBC_SYSV_MACHCALLS_A
LIBC_SYSV_MACHCALLS_A = o/$(MODE)/libc/sysv/machcalls.a
LIBC_SYSV_MACHCALLS_A_SRCS := $(wildcard libc/sysv/machcalls/*.s)
LIBC_SYSV_MACHCALLS_A_OBJS = $(LIBC_SYSV_MACHCALLS_A_SRCS:%.s=o/$(MODE)/%.o)
LIBC_SYSV_MACHCALLS_A_CHECKS = $(LIBC_SYSV_MACHCALLS_A).pkg

LIBC_SYSV_MACHCALLS_A_DIRECTDEPS =			\
	LIBC_SYSV

LIBC_SYSV_MACHCALLS_A_DEPS :=				\
	$(call uniq,$(foreach x,$(LIBC_SYSV_MACHCALLS_A_DIRECTDEPS),$($(x))))

$(LIBC_SYSV_MACHCALLS_A):				\
		libc/sysv/machcalls/			\
		$(LIBC_SYSV_MACHCALLS_A).pkg		\
		$(LIBC_SYSV_MACHCALLS_A_OBJS)

$(LIBC_SYSV_MACHCALLS_A).pkg:				\
		$(LIBC_SYSV_MACHCALLS_A_OBJS)		\
		$(foreach x,$(LIBC_SYSV_MACHCALLS_A_DIRECTDEPS),$($(x)_A).pkg)

$(LIBC_SYSV_MACHCALLS_A_OBJS):				\
		o/libc/sysv/macros.internal.inc

#───────────────────────────────────────────────────────────────────────────────

.PHONY: o/$(MODE)/libc/sysv
o/$(MODE)/libc/sysv: $(LIBC_SYSV_CHECKS)
