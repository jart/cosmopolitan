#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#───vi: set et ft=make ts=8 tw=8 fenc=utf-8 :vi───────────────────────┘

PKGS += LIBC_OHMYPLUS

LIBC_OHMYPLUS_ARTIFACTS += LIBC_OHMYPLUS_A
LIBC_OHMYPLUS = $(LIBC_OHMYPLUS_A_DEPS) $(LIBC_OHMYPLUS_A)
LIBC_OHMYPLUS_A = o/$(MODE)/libc/ohmyplus/ohmyplus.a
LIBC_OHMYPLUS_A_FILES := $(wildcard libc/ohmyplus/*)
LIBC_OHMYPLUS_A_HDRS = $(filter %.h,$(LIBC_OHMYPLUS_A_FILES))
LIBC_OHMYPLUS_A_SRCS_S = $(filter %.S,$(LIBC_OHMYPLUS_A_FILES))
LIBC_OHMYPLUS_A_SRCS_C = $(filter %.c,$(LIBC_OHMYPLUS_A_FILES))
LIBC_OHMYPLUS_A_SRCS_CXX = $(filter %.cc,$(LIBC_OHMYPLUS_A_FILES))

LIBC_OHMYPLUS_A_SRCS =					\
	$(LIBC_OHMYPLUS_A_SRCS_S)			\
	$(LIBC_OHMYPLUS_A_SRCS_C)			\
	$(LIBC_OHMYPLUS_A_SRCS_CXX)

LIBC_OHMYPLUS_A_OBJS =					\
	$(LIBC_OHMYPLUS_A_SRCS:%=o/$(MODE)/%.zip.o)	\
	$(LIBC_OHMYPLUS_A_SRCS_S:%.S=o/$(MODE)/%.o)	\
	$(LIBC_OHMYPLUS_A_SRCS_C:%.c=o/$(MODE)/%.o)	\
	$(LIBC_OHMYPLUS_A_SRCS_CXX:%.cc=o/$(MODE)/%.o)

LIBC_OHMYPLUS_A_CHECKS =				\
	$(LIBC_OHMYPLUS_A).pkg				\
	$(LIBC_OHMYPLUS_A_HDRS:%=o/$(MODE)/%.okk)

LIBC_OHMYPLUS_A_DIRECTDEPS =				\
	LIBC_BITS					\
	LIBC_MEM					\
	LIBC_STUBS

LIBC_OHMYPLUS_A_DEPS :=					\
	$(call uniq,$(foreach x,$(LIBC_OHMYPLUS_A_DIRECTDEPS),$($(x))))

$(LIBC_OHMYPLUS_A):					\
		libc/ohmyplus/				\
		$(LIBC_OHMYPLUS_A).pkg			\
		$(LIBC_OHMYPLUS_A_OBJS)

$(LIBC_OHMYPLUS_A).pkg:					\
		$(LIBC_OHMYPLUS_A_OBJS)			\
		$(foreach x,$(LIBC_OHMYPLUS_A_DIRECTDEPS),$($(x)_A).pkg)

LIBC_OHMYPLUS_LIBS = $(foreach x,$(LIBC_OHMYPLUS_ARTIFACTS),$($(x)))
LIBC_OHMYPLUS_SRCS = $(foreach x,$(LIBC_OHMYPLUS_ARTIFACTS),$($(x)_SRCS))
LIBC_OHMYPLUS_HDRS = $(foreach x,$(LIBC_OHMYPLUS_ARTIFACTS),$($(x)_HDRS))
LIBC_OHMYPLUS_CHECKS = $(foreach x,$(LIBC_OHMYPLUS_ARTIFACTS),$($(x)_CHECKS))
LIBC_OHMYPLUS_OBJS = $(foreach x,$(LIBC_OHMYPLUS_ARTIFACTS),$($(x)_OBJS))
$(LIBC_OHMYPLUS_OBJS): $(BUILD_FILES) libc/ohmyplus/ohmyplus.mk

.PHONY: o/$(MODE)/libc/ohmyplus
o/$(MODE)/libc/ohmyplus: $(LIBC_OHMYPLUS_CHECKS)
