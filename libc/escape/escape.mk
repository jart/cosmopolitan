#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#───vi: set et ft=make ts=8 tw=8 fenc=utf-8 :vi───────────────────────┘

PKGS += LIBC_ESCAPE

LIBC_ESCAPE_ARTIFACTS += LIBC_ESCAPE_A
LIBC_ESCAPE = $(LIBC_ESCAPE_A_DEPS) $(LIBC_ESCAPE_A)
LIBC_ESCAPE_A = o/$(MODE)/libc/escape/escape.a
LIBC_ESCAPE_A_FILES := $(wildcard libc/escape/*)
LIBC_ESCAPE_A_HDRS = $(filter %.h,$(LIBC_ESCAPE_A_FILES))
LIBC_ESCAPE_A_SRCS_S = $(filter %.S,$(LIBC_ESCAPE_A_FILES))
LIBC_ESCAPE_A_SRCS_C = $(filter %.c,$(LIBC_ESCAPE_A_FILES))

LIBC_ESCAPE_A_SRCS =					\
	$(LIBC_ESCAPE_A_SRCS_S)				\
	$(LIBC_ESCAPE_A_SRCS_C)

LIBC_ESCAPE_A_OBJS =					\
	$(LIBC_ESCAPE_A_SRCS:%=o/$(MODE)/%.zip.o)	\
	$(LIBC_ESCAPE_A_SRCS_S:%.S=o/$(MODE)/%.o)	\
	$(LIBC_ESCAPE_A_SRCS_C:%.c=o/$(MODE)/%.o)

LIBC_ESCAPE_A_CHECKS =					\
	$(LIBC_ESCAPE_A).pkg				\
	$(LIBC_ESCAPE_A_HDRS:%=o/$(MODE)/%.ok)

LIBC_ESCAPE_A_DIRECTDEPS =				\
	LIBC_MEM					\
	LIBC_NEXGEN32E					\
	LIBC_RUNTIME					\
	LIBC_STR					\
	LIBC_STUBS					\
	LIBC_SYSV

LIBC_ESCAPE_A_DEPS :=					\
	$(call uniq,$(foreach x,$(LIBC_ESCAPE_A_DIRECTDEPS),$($(x))))

$(LIBC_ESCAPE_A):					\
		libc/escape/				\
		$(LIBC_ESCAPE_A).pkg			\
		$(LIBC_ESCAPE_A_OBJS)

$(LIBC_ESCAPE_A).pkg:					\
		$(LIBC_ESCAPE_A_OBJS)			\
		$(foreach x,$(LIBC_ESCAPE_A_DIRECTDEPS),$($(x)_A).pkg)

LIBC_ESCAPE_LIBS = $(foreach x,$(LIBC_ESCAPE_ARTIFACTS),$($(x)))
LIBC_ESCAPE_SRCS = $(foreach x,$(LIBC_ESCAPE_ARTIFACTS),$($(x)_SRCS))
LIBC_ESCAPE_HDRS = $(foreach x,$(LIBC_ESCAPE_ARTIFACTS),$($(x)_HDRS))
LIBC_ESCAPE_BINS = $(foreach x,$(LIBC_ESCAPE_ARTIFACTS),$($(x)_BINS))
LIBC_ESCAPE_CHECKS = $(foreach x,$(LIBC_ESCAPE_ARTIFACTS),$($(x)_CHECKS))
LIBC_ESCAPE_OBJS = $(foreach x,$(LIBC_ESCAPE_ARTIFACTS),$($(x)_OBJS))
LIBC_ESCAPE_TESTS = $(foreach x,$(LIBC_ESCAPE_ARTIFACTS),$($(x)_TESTS))
$(LIBC_ESCAPE_OBJS): $(BUILD_FILES) libc/escape/escape.mk

.PHONY: o/$(MODE)/libc/escape
o/$(MODE)/libc/escape: $(LIBC_ESCAPE_CHECKS)
