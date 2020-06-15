#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#───vi: set et ft=make ts=8 tw=8 fenc=utf-8 :vi───────────────────────┘

PKGS += LIBC_NEXGEN32E

LIBC_NEXGEN32E_ARTIFACTS += LIBC_NEXGEN32E_A
LIBC_NEXGEN32E = $(LIBC_NEXGEN32E_A_DEPS) $(LIBC_NEXGEN32E_A)
LIBC_NEXGEN32E_A = o/$(MODE)/libc/nexgen32e/nexgen32e.a
LIBC_NEXGEN32E_A_FILES := $(wildcard libc/nexgen32e/*)
LIBC_NEXGEN32E_A_HDRS = $(filter %.h,$(LIBC_NEXGEN32E_A_FILES))
LIBC_NEXGEN32E_A_SRCS_A = $(filter %.s,$(LIBC_NEXGEN32E_A_FILES))
LIBC_NEXGEN32E_A_SRCS_S = $(filter %.S,$(LIBC_NEXGEN32E_A_FILES))
LIBC_NEXGEN32E_A_SRCS_C = $(filter %.c,$(LIBC_NEXGEN32E_A_FILES))

LIBC_NEXGEN32E_A_SRCS =					\
	$(LIBC_NEXGEN32E_A_SRCS_A)			\
	$(LIBC_NEXGEN32E_A_SRCS_S)			\
	$(LIBC_NEXGEN32E_A_SRCS_C)

LIBC_NEXGEN32E_A_OBJS =					\
	$(LIBC_NEXGEN32E_A_SRCS:%=o/$(MODE)/%.zip.o)	\
	$(LIBC_NEXGEN32E_A_SRCS_A:%.s=o/$(MODE)/%.o)	\
	$(LIBC_NEXGEN32E_A_SRCS_S:%.S=o/$(MODE)/%.o)	\
	$(LIBC_NEXGEN32E_A_SRCS_C:%.c=o/$(MODE)/%.o)

LIBC_NEXGEN32E_A_CHECKS =				\
	$(LIBC_NEXGEN32E_A).pkg				\
	$(LIBC_NEXGEN32E_A_HDRS:%=o/$(MODE)/%.ok)

LIBC_NEXGEN32E_A_DIRECTDEPS =				\
	LIBC_STUBS

LIBC_NEXGEN32E_A_DEPS :=				\
	$(call uniq,$(foreach x,$(LIBC_NEXGEN32E_A_DIRECTDEPS),$($(x))))

$(LIBC_NEXGEN32E_A):					\
		libc/nexgen32e/				\
		$(LIBC_NEXGEN32E_A).pkg			\
		$(LIBC_NEXGEN32E_A_OBJS)

$(LIBC_NEXGEN32E_A).pkg:				\
		$(LIBC_NEXGEN32E_A_OBJS)		\
		$(foreach x,$(LIBC_NEXGEN32E_A_DIRECTDEPS),$($(x)_A).pkg)

o/$(MODE)/libc/nexgen32e/tinystrlen.ncabi.o		\
o/$(MODE)/libc/nexgen32e/tinystrncmp.ncabi.o:		\
		OVERRIDE_CFLAGS +=			\
			-Os

LIBC_NEXGEN32E_LIBS = $(foreach x,$(LIBC_NEXGEN32E_ARTIFACTS),$($(x)))
LIBC_NEXGEN32E_SRCS = $(foreach x,$(LIBC_NEXGEN32E_ARTIFACTS),$($(x)_SRCS))
LIBC_NEXGEN32E_HDRS = $(foreach x,$(LIBC_NEXGEN32E_ARTIFACTS),$($(x)_HDRS))
LIBC_NEXGEN32E_CHECKS = $(foreach x,$(LIBC_NEXGEN32E_ARTIFACTS),$($(x)_CHECKS))
LIBC_NEXGEN32E_OBJS = $(foreach x,$(LIBC_NEXGEN32E_ARTIFACTS),$($(x)_OBJS))
$(LIBC_NEXGEN32E_OBJS): $(BUILD_FILES) libc/nexgen32e/nexgen32e.mk

.PHONY: o/$(MODE)/libc/nexgen32e
o/$(MODE)/libc/nexgen32e: $(LIBC_NEXGEN32E_CHECKS)
