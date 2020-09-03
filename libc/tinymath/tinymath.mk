#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#───vi: set et ft=make ts=8 tw=8 fenc=utf-8 :vi───────────────────────┘

PKGS += LIBC_TINYMATH

LIBC_TINYMATH_ARTIFACTS += LIBC_TINYMATH_A
LIBC_TINYMATH = $(LIBC_TINYMATH_A_DEPS) $(LIBC_TINYMATH_A)
LIBC_TINYMATH_A = o/$(MODE)/libc/tinymath/tinymath.a
LIBC_TINYMATH_A_FILES := $(wildcard libc/tinymath/*)
LIBC_TINYMATH_A_HDRS = $(filter %.h,$(LIBC_TINYMATH_A_FILES))
LIBC_TINYMATH_A_SRCS_A = $(filter %.s,$(LIBC_TINYMATH_A_FILES))
LIBC_TINYMATH_A_SRCS_S = $(filter %.S,$(LIBC_TINYMATH_A_FILES))
LIBC_TINYMATH_A_SRCS_C = $(filter %.c,$(LIBC_TINYMATH_A_FILES))
LIBC_TINYMATH_A_SRCS = $(LIBC_TINYMATH_A_SRCS_S) $(LIBC_TINYMATH_A_SRCS_C)
LIBC_TINYMATH_A_CHECKS = $(LIBC_TINYMATH_A).pkg

LIBC_TINYMATH_A_OBJS =					\
	$(LIBC_TINYMATH_A_SRCS:%=o/$(MODE)/%.zip.o)	\
	$(LIBC_TINYMATH_A_SRCS_A:%.s=o/$(MODE)/%.o)	\
	$(LIBC_TINYMATH_A_SRCS_S:%.S=o/$(MODE)/%.o)	\
	$(LIBC_TINYMATH_A_SRCS_C:%.c=o/$(MODE)/%.o)

LIBC_TINYMATH_A_CHECKS =				\
	$(LIBC_TINYMATH_A).pkg				\
	$(LIBC_TINYMATH_A_HDRS:%=o/$(MODE)/%.ok)

LIBC_TINYMATH_A_DIRECTDEPS =				\
	LIBC_STUBS					\
	LIBC_NEXGEN32E

LIBC_TINYMATH_A_DEPS :=					\
	$(call uniq,$(foreach x,$(LIBC_TINYMATH_A_DIRECTDEPS),$($(x))))

$(LIBC_TINYMATH_A):					\
		libc/tinymath/				\
		$(LIBC_TINYMATH_A).pkg			\
		$(LIBC_TINYMATH_A_OBJS)

$(LIBC_TINYMATH_A).pkg:					\
		$(LIBC_TINYMATH_A_OBJS)			\
		$(foreach x,$(LIBC_TINYMATH_A_DIRECTDEPS),$($(x)_A).pkg)

LIBC_TINYMATH_LIBS = $(foreach x,$(LIBC_TINYMATH_ARTIFACTS),$($(x)))
LIBC_TINYMATH_HDRS = $(foreach x,$(LIBC_TINYMATH_ARTIFACTS),$($(x)_HDRS))
LIBC_TINYMATH_SRCS = $(foreach x,$(LIBC_TINYMATH_ARTIFACTS),$($(x)_SRCS))
LIBC_TINYMATH_CHECKS = $(foreach x,$(LIBC_TINYMATH_ARTIFACTS),$($(x)_CHECKS))
LIBC_TINYMATH_OBJS = $(foreach x,$(LIBC_TINYMATH_ARTIFACTS),$($(x)_OBJS))
LIBC_TINYMATH_CHECKS = $(LIBC_TINYMATH_HDRS:%=o/$(MODE)/%.ok)
$(LIBC_TINYMATH_OBJS): $(BUILD_FILES) libc/tinymath/tinymath.mk

.PHONY: o/$(MODE)/libc/tinymath
o/$(MODE)/libc/tinymath: $(LIBC_TINYMATH_CHECKS)
