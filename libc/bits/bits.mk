#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#───vi: set et ft=make ts=8 tw=8 fenc=utf-8 :vi───────────────────────┘

PKGS += LIBC_BITS

LIBC_BITS_ARTIFACTS += LIBC_BITS_A
LIBC_BITS = $(LIBC_BITS_A_DEPS) $(LIBC_BITS_A)
LIBC_BITS_A = o/$(MODE)/libc/bits/bits.a
LIBC_BITS_A_FILES := $(wildcard libc/bits/*)
LIBC_BITS_A_HDRS = $(filter %.h,$(LIBC_BITS_A_FILES))
LIBC_BITS_A_SRCS_S = $(filter %.S,$(LIBC_BITS_A_FILES))
LIBC_BITS_A_SRCS_C = $(filter %.c,$(LIBC_BITS_A_FILES))

LIBC_BITS_A_SRCS =				\
	$(LIBC_BITS_A_SRCS_S)			\
	$(LIBC_BITS_A_SRCS_C)

LIBC_BITS_A_OBJS =				\
	$(LIBC_BITS_A_SRCS:%=o/$(MODE)/%.zip.o)	\
	$(LIBC_BITS_A_SRCS_S:%.S=o/$(MODE)/%.o)	\
	$(LIBC_BITS_A_SRCS_C:%.c=o/$(MODE)/%.o)

LIBC_BITS_A_CHECKS =				\
	$(LIBC_BITS_A).pkg			\
	$(LIBC_BITS_A_HDRS:%=o/$(MODE)/%.ok)

LIBC_BITS_A_DIRECTDEPS =			\
	LIBC_STUBS				\
	LIBC_NEXGEN32E

LIBC_BITS_A_DEPS :=				\
	$(call uniq,$(foreach x,$(LIBC_BITS_A_DIRECTDEPS),$($(x))))

$(LIBC_BITS_A):	libc/bits/			\
		$(LIBC_BITS_A).pkg		\
		$(LIBC_BITS_A_OBJS)

$(LIBC_BITS_A).pkg:				\
		$(LIBC_BITS_A_OBJS)		\
		$(foreach x,$(LIBC_BITS_A_DIRECTDEPS),$($(x)_A).pkg)

LIBC_BITS_LIBS = $(foreach x,$(LIBC_BITS_ARTIFACTS),$($(x)))
LIBC_BITS_SRCS = $(foreach x,$(LIBC_BITS_ARTIFACTS),$($(x)_SRCS))
LIBC_BITS_HDRS = $(foreach x,$(LIBC_BITS_ARTIFACTS),$($(x)_HDRS))
LIBC_BITS_CHECKS = $(foreach x,$(LIBC_BITS_ARTIFACTS),$($(x)_CHECKS))
LIBC_BITS_OBJS = $(foreach x,$(LIBC_BITS_ARTIFACTS),$($(x)_OBJS))
$(LIBC_BITS_OBJS): $(BUILD_FILES) libc/bits/bits.mk

.PHONY: o/$(MODE)/libc/bits
o/$(MODE)/libc/bits: $(LIBC_BITS_CHECKS)
