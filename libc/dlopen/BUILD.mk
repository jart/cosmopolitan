#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#── vi: set noet ft=make ts=8 sw=8 fenc=utf-8 :vi ────────────────────┘

PKGS += LIBC_DLOPEN

LIBC_DLOPEN_ARTIFACTS += LIBC_DLOPEN_A
LIBC_DLOPEN = $(LIBC_DLOPEN_A_DEPS) $(LIBC_DLOPEN_A)
LIBC_DLOPEN_A = o/$(MODE)/libc/dlopen/dlopen.a
LIBC_DLOPEN_A_FILES := $(wildcard libc/dlopen/*)
LIBC_DLOPEN_A_HDRS = $(filter %.h,$(LIBC_DLOPEN_A_FILES))
LIBC_DLOPEN_A_SRCS_C = $(filter %.c,$(LIBC_DLOPEN_A_FILES))
LIBC_DLOPEN_A_SRCS_S = $(filter %.S,$(LIBC_DLOPEN_A_FILES))
LIBC_DLOPEN_A_SRCS = $(LIBC_DLOPEN_A_SRCS_C) $(LIBC_DLOPEN_A_SRCS_S)

LIBC_DLOPEN_A_OBJS =							\
	$(LIBC_DLOPEN_A_SRCS_C:%.c=o/$(MODE)/%.o)			\
	$(LIBC_DLOPEN_A_SRCS_S:%.S=o/$(MODE)/%.o)

LIBC_DLOPEN_A_CHECKS =							\
	$(LIBC_DLOPEN_A).pkg						\
	$(LIBC_DLOPEN_A_HDRS:%=o/$(MODE)/%.ok)

LIBC_DLOPEN_A_DIRECTDEPS =						\
	LIBC_CALLS							\
	LIBC_ELF							\
	LIBC_INTRIN							\
	LIBC_NEXGEN32E							\
	LIBC_NT_KERNEL32						\
	LIBC_PROC							\
	LIBC_RUNTIME							\
	LIBC_SYSV							\
	LIBC_SYSV_CALLS							\
	LIBC_STR							\
	THIRD_PARTY_COMPILER_RT

LIBC_DLOPEN_A_DEPS :=							\
	$(call uniq,$(foreach x,$(LIBC_DLOPEN_A_DIRECTDEPS),$($(x))))

$(LIBC_DLOPEN_A):							\
		libc/dlopen/						\
		$(LIBC_DLOPEN_A).pkg					\
		$(LIBC_DLOPEN_A_OBJS)

$(LIBC_DLOPEN_A).pkg:							\
		$(LIBC_DLOPEN_A_OBJS)					\
		$(foreach x,$(LIBC_DLOPEN_A_DIRECTDEPS),$($(x)_A).pkg)

$(LIBC_DLOPEN_A_OBJS): private						\
		COPTS +=						\
			-Wframe-larger-than=4096			\
			-Walloca-larger-than=4096

# these assembly files are safe to build on aarch64
o/$(MODE)/libc/dlopen/foreign_tramp.o: libc/dlopen/foreign_tramp.S
	@$(COMPILE) -AOBJECTIFY.S $(OBJECTIFY.S) $(OUTPUT_OPTION) -c $<

LIBC_DLOPEN_LIBS = $(foreach x,$(LIBC_DLOPEN_ARTIFACTS),$($(x)))
LIBC_DLOPEN_SRCS = $(foreach x,$(LIBC_DLOPEN_ARTIFACTS),$($(x)_SRCS))
LIBC_DLOPEN_HDRS = $(foreach x,$(LIBC_DLOPEN_ARTIFACTS),$($(x)_HDRS))
LIBC_DLOPEN_CHECKS = $(foreach x,$(LIBC_DLOPEN_ARTIFACTS),$($(x)_CHECKS))
LIBC_DLOPEN_OBJS = $(foreach x,$(LIBC_DLOPEN_ARTIFACTS),$($(x)_OBJS))
$(LIBC_DLOPEN_OBJS): $(BUILD_FILES) libc/dlopen/BUILD.mk

.PHONY: o/$(MODE)/libc/dlopen
o/$(MODE)/libc/dlopen: $(LIBC_DLOPEN_CHECKS)
