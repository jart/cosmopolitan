#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#───vi: set et ft=make ts=8 tw=8 fenc=utf-8 :vi───────────────────────┘

PKGS += LIBC_MATH

LIBC_MATH_ARTIFACTS += LIBC_MATH_A
LIBC_MATH = $(LIBC_MATH_A_DEPS) $(LIBC_MATH_A)
LIBC_MATH_A = o/$(MODE)/libc/math/math.a
LIBC_MATH_A_FILES := $(wildcard libc/math/*)
LIBC_MATH_A_SRCS_A = $(filter %.s,$(LIBC_MATH_A_FILES))
LIBC_MATH_A_SRCS_S = $(filter %.S,$(LIBC_MATH_A_FILES))
LIBC_MATH_A_SRCS_C = $(filter %.c,$(LIBC_MATH_A_FILES))

LIBC_MATH_A_SRCS =					\
	$(LIBC_MATH_A_SRCS_A)				\
	$(LIBC_MATH_A_SRCS_S)				\
	$(LIBC_MATH_A_SRCS_C)

LIBC_MATH_A_OBJS =					\
	$(LIBC_MATH_A_SRCS:%=o/$(MODE)/%.zip.o)		\
	$(LIBC_MATH_A_SRCS_A:%.s=o/$(MODE)/%.o)		\
	$(LIBC_MATH_A_SRCS_S:%.S=o/$(MODE)/%.o)		\
	$(LIBC_MATH_A_SRCS_C:%.c=o/$(MODE)/%.o)

LIBC_MATH_A_CHECKS = $(LIBC_MATH_A).pkg

LIBC_MATH_A_DIRECTDEPS =				\
	LIBC_STUBS					\
	LIBC_NEXGEN32E

LIBC_MATH_A_DEPS :=					\
	$(call uniq,$(foreach x,$(LIBC_MATH_A_DIRECTDEPS),$($(x))))

$(LIBC_MATH_A_OBJS):					\
		OVERRIDE_CFLAGS +=			\
			$(OLD_CODE)			\
			-Wno-unknown-pragmas		\
			-Wno-parentheses

$(LIBC_MATH_A):	libc/math/				\
		$(LIBC_MATH_A).pkg			\
		$(LIBC_MATH_A_OBJS)

$(LIBC_MATH_A).pkg:					\
		$(LIBC_MATH_A_OBJS)			\
		$(foreach x,$(LIBC_MATH_A_DIRECTDEPS),$($(x)_A).pkg)

LIBC_MATH_LIBS = $(foreach x,$(LIBC_MATH_ARTIFACTS),$($(x)))
LIBC_MATH_SRCS = $(foreach x,$(LIBC_MATH_ARTIFACTS),$($(x)_SRCS))
LIBC_MATH_CHECKS = $(foreach x,$(LIBC_MATH_ARTIFACTS),$($(x)_CHECKS))
LIBC_MATH_OBJS = $(foreach x,$(LIBC_MATH_ARTIFACTS),$($(x)_OBJS))
$(LIBC_MATH_OBJS): $(BUILD_FILES) libc/math/math.mk

.PHONY: o/$(MODE)/libc/math
o/$(MODE)/libc/math: $(LIBC_MATH_CHECKS)
