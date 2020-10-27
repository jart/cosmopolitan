#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#───vi: set et ft=make ts=8 tw=8 fenc=utf-8 :vi───────────────────────┘

PKGS += LIBC_STR

LIBC_STR_ARTIFACTS += LIBC_STR_A
LIBC_STR = $(LIBC_STR_A_DEPS) $(LIBC_STR_A)
LIBC_STR_A = o/$(MODE)/libc/str/str.a
LIBC_STR_A_FILES := $(wildcard libc/str/*)
LIBC_STR_A_HDRS = $(filter %.h,$(LIBC_STR_A_FILES))
LIBC_STR_A_SRCS_A = $(filter %.s,$(LIBC_STR_A_FILES))
LIBC_STR_A_SRCS_S = $(filter %.S,$(LIBC_STR_A_FILES))
LIBC_STR_A_SRCS_C = $(filter %.c,$(LIBC_STR_A_FILES))

LIBC_STR_A_SRCS =						\
	$(LIBC_STR_A_SRCS_A)					\
	$(LIBC_STR_A_SRCS_S)					\
	$(LIBC_STR_A_SRCS_C)

LIBC_STR_A_OBJS =						\
	$(LIBC_STR_A_SRCS:%=o/$(MODE)/%.zip.o)			\
	$(LIBC_STR_A_SRCS_A:%.s=o/$(MODE)/%.o)			\
	$(LIBC_STR_A_SRCS_S:%.S=o/$(MODE)/%.o)			\
	$(LIBC_STR_A_SRCS_C:%.c=o/$(MODE)/%.o)

LIBC_STR_A_CHECKS =						\
	$(LIBC_STR_A).pkg					\
	$(LIBC_STR_A_HDRS:%=o/$(MODE)/%.ok)

LIBC_STR_A_DIRECTDEPS =						\
	LIBC_INTRIN						\
	LIBC_STUBS						\
	LIBC_NEXGEN32E

LIBC_STR_A_DEPS :=						\
	$(call uniq,$(foreach x,$(LIBC_STR_A_DIRECTDEPS),$($(x))))

$(LIBC_STR_A):	libc/str/					\
		$(LIBC_STR_A).pkg				\
		$(LIBC_STR_A_OBJS)

$(LIBC_STR_A).pkg:						\
		$(LIBC_STR_A_OBJS)				\
		$(foreach x,$(LIBC_STR_A_DIRECTDEPS),$($(x)_A).pkg)

LIBC_STR_LIBS = $(foreach x,$(LIBC_STR_ARTIFACTS),$($(x)))
LIBC_STR_SRCS = $(foreach x,$(LIBC_STR_ARTIFACTS),$($(x)_SRCS))
LIBC_STR_HDRS = $(foreach x,$(LIBC_STR_ARTIFACTS),$($(x)_HDRS))
LIBC_STR_BINS = $(foreach x,$(LIBC_STR_ARTIFACTS),$($(x)_BINS))
LIBC_STR_CHECKS = $(foreach x,$(LIBC_STR_ARTIFACTS),$($(x)_CHECKS))
LIBC_STR_OBJS = $(foreach x,$(LIBC_STR_ARTIFACTS),$($(x)_OBJS))
LIBC_STR_TESTS = $(foreach x,$(LIBC_STR_ARTIFACTS),$($(x)_TESTS))
$(LIBC_STR_OBJS): $(BUILD_FILES) libc/str/str.mk

.PHONY: o/$(MODE)/libc/str
o/$(MODE)/libc/str: $(LIBC_STR_CHECKS)
