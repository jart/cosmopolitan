#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#── vi: set noet ft=make ts=8 sw=8 fenc=utf-8 :vi ────────────────────┘

PKGS += LIBC_SYSV
LIBC_SYSV_LIBS = $(foreach x,$(LIBC_SYSV_ARTIFACTS),$($(x)_A))
LIBC_SYSV_ARCHIVES = $(foreach x,$(LIBC_SYSV_ARTIFACTS),$($(x)_A))
LIBC_SYSV_HDRS = $(foreach x,$(LIBC_SYSV_ARTIFACTS),$($(x)_HDRS))
LIBC_SYSV_INCS = $(foreach x,$(LIBC_SYSV_ARTIFACTS),$($(x)_INCS))
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
LIBC_SYSV_A_INCS = $(filter %.inc,$(LIBC_SYSV_A_FILES))
LIBC_SYSV_A_SRCS_C = $(filter %.c,$(LIBC_SYSV_A_FILES))
LIBC_SYSV_A_SRCS_A = $(filter %.s,$(LIBC_SYSV_A_FILES))
LIBC_SYSV_A_SRCS_S = $(filter %.S,$(LIBC_SYSV_A_FILES))
LIBC_SYSV_A_CHECKS = $(LIBC_SYSV_A).pkg

LIBC_SYSV_A_DIRECTDEPS =				\
	LIBC_NEXGEN32E

LIBC_SYSV_A_FILES :=					\
	libc/sysv/macros.internal.h			\
	libc/sysv/errfuns.h				\
	libc/sysv/hostos.S				\
	libc/sysv/syscon.S				\
	libc/sysv/syslib.S				\
	libc/sysv/syscount.S				\
	libc/sysv/restorert.S				\
	libc/sysv/syscall2.S				\
	libc/sysv/syscall3.S				\
	libc/sysv/syscall4.S				\
	libc/sysv/systemfive.S				\
	libc/sysv/sysret.c				\
	libc/sysv/sysv.c				\
	libc/sysv/errno.c				\
	libc/sysv/errfun.S				\
	libc/sysv/errfun2.c				\
	libc/sysv/strace.greg.c				\
	libc/sysv/describeos.greg.c			\
	$(wildcard libc/sysv/consts/*)			\
	$(wildcard libc/sysv/errfuns/*)			\
	$(wildcard libc/sysv/dos2errno/*)

LIBC_SYSV_A_SRCS =					\
	$(LIBC_SYSV_A_SRCS_A)				\
	$(LIBC_SYSV_A_SRCS_C)				\
	$(LIBC_SYSV_A_SRCS_S)

LIBC_SYSV_A_OBJS =					\
	$(LIBC_SYSV_A_SRCS_A:%.s=o/$(MODE)/%.o)		\
	$(LIBC_SYSV_A_SRCS_C:%.c=o/$(MODE)/%.o)		\
	$(LIBC_SYSV_A_SRCS_S:%.S=o/$(MODE)/%.o)

LIBC_SYSV_A_DEPS :=					\
	$(call uniq,$(foreach x,$(LIBC_SYSV_A_DIRECTDEPS),$($(x))))

$(LIBC_SYSV_A):	libc/sysv/				\
		libc/sysv/consts/			\
		libc/sysv/errfuns/			\
		$(LIBC_SYSV_A).pkg			\
		$(LIBC_SYSV_A_OBJS)

$(LIBC_SYSV_A).pkg:					\
		$(LIBC_SYSV_A_OBJS)			\
		$(foreach x,$(LIBC_SYSV_A_DIRECTDEPS),$($(x)_A).pkg)

o/$(MODE)/libc/sysv/sysv.o				\
o/$(MODE)/libc/sysv/errno.o				\
o/$(MODE)/libc/sysv/sysret.o				\
o/$(MODE)/libc/sysv/errfun2.o				\
o/$(MODE)/libc/sysv/sysret.o: private			\
		CFLAGS +=				\
			-ffreestanding			\
			-fno-stack-protector		\
			-fno-sanitize=all

ifeq ($(ARCH),aarch64)
o/$(MODE)/libc/sysv/sysv.o: private			\
		CFLAGS +=				\
			-ffixed-x0			\
			-ffixed-x1			\
			-ffixed-x2			\
			-ffixed-x3			\
			-ffixed-x4			\
			-ffixed-x5			\
			-ffixed-x8			\
			-ffixed-x9			\
			-ffixed-x16			\
			-fomit-frame-pointer		\
			-foptimize-sibling-calls	\
			-Os
endif

#───────────────────────────────────────────────────────────────────────────────

LIBC_SYSV_CALLS =					\
	$(LIBC_SYSV_CALLS_A_DEPS)			\
	$(LIBC_SYSV_CALLS_A)

LIBC_SYSV_ARTIFACTS += LIBC_SYSV_CALLS_A
LIBC_SYSV_CALLS_A = o/$(MODE)/libc/sysv/calls.a
LIBC_SYSV_CALLS_A_SRCS := $(wildcard libc/sysv/calls/*.S)
LIBC_SYSV_CALLS_A_OBJS = $(LIBC_SYSV_CALLS_A_SRCS:%.S=o/$(MODE)/%.o)
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

#───────────────────────────────────────────────────────────────────────────────

# let aarch64 compile these
o/$(MODE)/libc/sysv/syscon.o: libc/sysv/syscon.S
	@$(COMPILE) -AOBJECTIFY.S $(OBJECTIFY.S) $(OUTPUT_OPTION) $<
o/$(MODE)/libc/sysv/hostos.o: libc/sysv/hostos.S
	@$(COMPILE) -AOBJECTIFY.S $(OBJECTIFY.S) $(OUTPUT_OPTION) $<
o/$(MODE)/libc/sysv/syslib.o: libc/sysv/syslib.S
	@$(COMPILE) -AOBJECTIFY.S $(OBJECTIFY.S) $(OUTPUT_OPTION) $<
o/$(MODE)/libc/sysv/syscount.o: libc/sysv/syscount.S
	@$(COMPILE) -AOBJECTIFY.S $(OBJECTIFY.S) $(OUTPUT_OPTION) $<
o/$(MODE)/libc/sysv/syscall2.o: libc/sysv/syscall2.S
	@$(COMPILE) -AOBJECTIFY.S $(OBJECTIFY.S) $(OUTPUT_OPTION) $<
o/$(MODE)/libc/sysv/syscall3.o: libc/sysv/syscall3.S
	@$(COMPILE) -AOBJECTIFY.S $(OBJECTIFY.S) $(OUTPUT_OPTION) $<
o/$(MODE)/libc/sysv/syscall4.o: libc/sysv/syscall4.S
	@$(COMPILE) -AOBJECTIFY.S $(OBJECTIFY.S) $(OUTPUT_OPTION) $<
o/$(MODE)/libc/sysv/restorert.o: libc/sysv/restorert.S
	@$(COMPILE) -AOBJECTIFY.S $(OBJECTIFY.S) $(OUTPUT_OPTION) $<
o/$(MODE)/libc/sysv/calls/%.o: libc/sysv/calls/%.S
	@$(COMPILE) -AOBJECTIFY.S $(OBJECTIFY.S) $(OUTPUT_OPTION) $<
o/$(MODE)/libc/sysv/consts/%.o: libc/sysv/consts/%.S
	@$(COMPILE) -AOBJECTIFY.S $(OBJECTIFY.S) $(OUTPUT_OPTION) $<
o/$(MODE)/libc/sysv/errfuns/%.o: libc/sysv/errfuns/%.S
	@$(COMPILE) -AOBJECTIFY.S $(OBJECTIFY.S) $(OUTPUT_OPTION) $<
o/$(MODE)/libc/sysv/dos2errno/%.o: libc/sysv/dos2errno/%.S
	@$(COMPILE) -AOBJECTIFY.S $(OBJECTIFY.S) $(OUTPUT_OPTION) $<

#───────────────────────────────────────────────────────────────────────────────

.PHONY: o/$(MODE)/libc/sysv
o/$(MODE)/libc/sysv: $(LIBC_SYSV_CHECKS)
