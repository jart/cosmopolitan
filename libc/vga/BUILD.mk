#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#── vi: set noet ft=make ts=8 sw=8 fenc=utf-8 :vi ────────────────────┘

PKGS += LIBC_VGA

LIBC_VGA_ARTIFACTS += LIBC_VGA_A
LIBC_VGA_A = o/$(MODE)/libc/vga/vga.a
LIBC_VGA_A_FILES := $(wildcard libc/vga/*)
LIBC_VGA_A_HDRS = $(filter %.h,$(LIBC_VGA_A_FILES))
LIBC_VGA_A_INCS = $(filter %.inc,$(LIBC_VGA_A_FILES))
LIBC_VGA_A_SRCS_S = $(filter %.S,$(LIBC_VGA_A_FILES))
LIBC_VGA_A_SRCS_C = $(filter %.c,$(LIBC_VGA_A_FILES))

LIBC_VGA =						\
	$(LIBC_VGA_A_DEPS)				\
	$(LIBC_VGA_A)

LIBC_VGA_A_SRCS =					\
	$(LIBC_VGA_A_SRCS_S)				\
	$(LIBC_VGA_A_SRCS_C)

LIBC_VGA_A_OBJS =					\
	$(LIBC_VGA_A_SRCS_S:%.S=o/$(MODE)/%.o)		\
	$(LIBC_VGA_A_SRCS_C:%.c=o/$(MODE)/%.o)

LIBC_VGA_A_CHECKS =					\
	$(LIBC_VGA_A).pkg				\
	$(LIBC_VGA_A_HDRS:%=o/$(MODE)/%.ok)

LIBC_VGA_A_DIRECTDEPS =					\
	LIBC_FMT					\
	LIBC_INTRIN					\
	LIBC_NEXGEN32E					\
	LIBC_STR					\
	LIBC_SYSV

LIBC_VGA_A_DEPS :=					\
	$(call uniq,$(foreach x,$(LIBC_VGA_A_DIRECTDEPS),$($(x))))

$(LIBC_VGA_A):libc/vga/					\
		$(LIBC_VGA_A).pkg			\
		$(LIBC_VGA_A_OBJS)

$(LIBC_VGA_A).pkg:					\
		$(LIBC_VGA_A_OBJS)			\
		$(foreach x,$(LIBC_VGA_A_DIRECTDEPS),$($(x)_A).pkg)

LIBC_VGA_LIBS = $(foreach x,$(LIBC_VGA_ARTIFACTS),$($(x)))
LIBC_VGA_SRCS = $(foreach x,$(LIBC_VGA_ARTIFACTS),$($(x)_SRCS))
LIBC_VGA_HDRS = $(foreach x,$(LIBC_VGA_ARTIFACTS),$($(x)_HDRS))
LIBC_VGA_INCS = $(foreach x,$(LIBC_VGA_ARTIFACTS),$($(x)_INCS))
LIBC_VGA_BINS = $(foreach x,$(LIBC_VGA_ARTIFACTS),$($(x)_BINS))
LIBC_VGA_CHECKS = $(foreach x,$(LIBC_VGA_ARTIFACTS),$($(x)_CHECKS))
LIBC_VGA_OBJS = $(foreach x,$(LIBC_VGA_ARTIFACTS),$($(x)_OBJS))
LIBC_VGA_TESTS = $(foreach x,$(LIBC_VGA_ARTIFACTS),$($(x)_TESTS))
$(LIBC_VGA_OBJS): $(BUILD_FILES) libc/vga/BUILD.mk

.PHONY: o/$(MODE)/libc/vga
o/$(MODE)/libc/vga: $(LIBC_VGA_CHECKS)
