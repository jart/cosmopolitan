#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#── vi: set noet ft=make ts=8 sw=8 fenc=utf-8 :vi ────────────────────┘

PKGS += LIBC_IRQ

LIBC_IRQ_ARTIFACTS += LIBC_IRQ_A
LIBC_IRQ_A = o/$(MODE)/libc/irq/irq.a
LIBC_IRQ_A_FILES := $(wildcard libc/irq/*)
LIBC_IRQ_A_HDRS = $(filter %.h,$(LIBC_IRQ_A_FILES))
LIBC_IRQ_A_INCS = $(filter %.inc,$(LIBC_IRQ_A_FILES))
LIBC_IRQ_A_SRCS_S = $(filter %.S,$(LIBC_IRQ_A_FILES))
LIBC_IRQ_A_SRCS_C = $(filter %.c,$(LIBC_IRQ_A_FILES))

LIBC_IRQ =						\
	$(LIBC_IRQ_A_DEPS)				\
	$(LIBC_IRQ_A)

LIBC_IRQ_A_SRCS =					\
	$(LIBC_IRQ_A_SRCS_S)				\
	$(LIBC_IRQ_A_SRCS_C)

LIBC_IRQ_A_OBJS =					\
	$(LIBC_IRQ_A_SRCS_S:%.S=o/$(MODE)/%.o)		\
	$(LIBC_IRQ_A_SRCS_C:%.c=o/$(MODE)/%.o)

LIBC_IRQ_A_CHECKS =					\
	$(LIBC_IRQ_A).pkg				\
	$(LIBC_IRQ_A_HDRS:%=o/$(MODE)/%.ok)

LIBC_IRQ_A_DIRECTDEPS =					\
	LIBC_INTRIN					\
	LIBC_STR					\
	LIBC_SYSV

LIBC_IRQ_A_DEPS :=					\
	$(call uniq,$(foreach x,$(LIBC_IRQ_A_DIRECTDEPS),$($(x))))

$(LIBC_IRQ_A):libc/irq/					\
		$(LIBC_IRQ_A).pkg			\
		$(LIBC_IRQ_A_OBJS)

$(LIBC_IRQ_A).pkg:					\
		$(LIBC_IRQ_A_OBJS)			\
		$(foreach x,$(LIBC_IRQ_A_DIRECTDEPS),$($(x)_A).pkg)

LIBC_IRQ_LIBS = $(foreach x,$(LIBC_IRQ_ARTIFACTS),$($(x)))
LIBC_IRQ_SRCS = $(foreach x,$(LIBC_IRQ_ARTIFACTS),$($(x)_SRCS))
LIBC_IRQ_HDRS = $(foreach x,$(LIBC_IRQ_ARTIFACTS),$($(x)_HDRS))
LIBC_IRQ_INCS = $(foreach x,$(LIBC_IRQ_ARTIFACTS),$($(x)_INCS))
LIBC_IRQ_BINS = $(foreach x,$(LIBC_IRQ_ARTIFACTS),$($(x)_BINS))
LIBC_IRQ_CHECKS = $(foreach x,$(LIBC_IRQ_ARTIFACTS),$($(x)_CHECKS))
LIBC_IRQ_OBJS = $(foreach x,$(LIBC_IRQ_ARTIFACTS),$($(x)_OBJS))
LIBC_IRQ_TESTS = $(foreach x,$(LIBC_IRQ_ARTIFACTS),$($(x)_TESTS))
$(LIBC_IRQ_OBJS): $(BUILD_FILES) libc/irq/BUILD.mk

.PHONY: o/$(MODE)/libc/irq
o/$(MODE)/libc/irq: $(LIBC_IRQ_CHECKS)
