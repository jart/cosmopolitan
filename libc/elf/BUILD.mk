#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#── vi: set noet ft=make ts=8 sw=8 fenc=utf-8 :vi ────────────────────┘

PKGS += LIBC_ELF

LIBC_ELF_ARTIFACTS += LIBC_ELF_A
LIBC_ELF = $(LIBC_ELF_A_DEPS) $(LIBC_ELF_A)
LIBC_ELF_A = o/$(MODE)/libc/elf/elf.a
LIBC_ELF_A_FILES := $(wildcard libc/elf/*) $(wildcard libc/elf/struct/*)
LIBC_ELF_A_HDRS = $(filter %.h,$(LIBC_ELF_A_FILES))
LIBC_ELF_A_SRCS_S = $(filter %.S,$(LIBC_ELF_A_FILES))
LIBC_ELF_A_SRCS_C = $(filter %.c,$(LIBC_ELF_A_FILES))

LIBC_ELF_A_SRCS =				\
	$(LIBC_ELF_A_SRCS_S)			\
	$(LIBC_ELF_A_SRCS_C)

LIBC_ELF_A_OBJS =				\
	$(LIBC_ELF_A_SRCS_S:%.S=o/$(MODE)/%.o)	\
	$(LIBC_ELF_A_SRCS_C:%.c=o/$(MODE)/%.o)

LIBC_ELF_A_CHECKS =				\
	$(LIBC_ELF_A).pkg			\
	$(LIBC_ELF_A_HDRS:%=o/$(MODE)/%.ok)

LIBC_ELF_A_DIRECTDEPS =				\
	LIBC_INTRIN				\
	LIBC_NEXGEN32E				\
	LIBC_STR				\

LIBC_ELF_A_DEPS :=				\
	$(call uniq,$(foreach x,$(LIBC_ELF_A_DIRECTDEPS),$($(x))))

$(LIBC_ELF_A):	libc/elf/			\
		$(LIBC_ELF_A).pkg		\
		$(LIBC_ELF_A_OBJS)

$(LIBC_ELF_A).pkg:				\
		$(LIBC_ELF_A_OBJS)		\
		$(foreach x,$(LIBC_ELF_A_DIRECTDEPS),$($(x)_A).pkg)

$(LIBC_ELF_A_OBJS): private				\
		COPTS +=				\
			-fno-sanitize=all		\
			-Wframe-larger-than=4096	\
			-Walloca-larger-than=4096

LIBC_ELF_LIBS = $(foreach x,$(LIBC_ELF_ARTIFACTS),$($(x)))
LIBC_ELF_SRCS = $(foreach x,$(LIBC_ELF_ARTIFACTS),$($(x)_SRCS))
LIBC_ELF_HDRS = $(foreach x,$(LIBC_ELF_ARTIFACTS),$($(x)_HDRS))
LIBC_ELF_CHECKS = $(foreach x,$(LIBC_ELF_ARTIFACTS),$($(x)_CHECKS))
LIBC_ELF_OBJS = $(foreach x,$(LIBC_ELF_ARTIFACTS),$($(x)_OBJS))
$(LIBC_ELF_OBJS): $(BUILD_FILES) libc/elf/BUILD.mk

.PHONY: o/$(MODE)/libc/elf
o/$(MODE)/libc/elf: $(LIBC_ELF_CHECKS)
