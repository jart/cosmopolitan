#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#───vi: set et ft=make ts=8 tw=8 fenc=utf-8 :vi───────────────────────┘

PKGS += APE_LIB

APE_LIB_ARTIFACTS += APE_LIB_A
APE_LIB = $(APE_LIB_A_DEPS) $(APE_LIB_A)
APE_LIB_A = o/$(MODE)/ape/lib/apelib.a
APE_LIB_A_FILES := $(wildcard ape/lib/*)
APE_LIB_A_HDRS = $(filter %.h,$(APE_LIB_A_FILES))
APE_LIB_A_SRCS_S = $(filter %.S,$(APE_LIB_A_FILES))
APE_LIB_A_SRCS_C = $(filter %.c,$(APE_LIB_A_FILES))

APE_LIB_A_SRCS =					\
	$(APE_LIB_A_SRCS_S)				\
	$(APE_LIB_A_SRCS_C)

APE_LIB_A_OBJS =					\
	$(APE_LIB_A_SRCS_S:%.S=o/$(MODE)/%.o)		\
	$(APE_LIB_A_SRCS_C:%.c=o/$(MODE)/%.o)		\
	$(APE_LIB_A_SRCS:%=o/$(MODE)/%.zip.o)		\
	o/$(MODE)/ape/ape.lds.zip.o			\
	o/$(MODE)/ape/ape.S.zip.o			\
	o/$(MODE)/NOTICE.zip.o

APE_LIB_A_CHECKS = $(APE_LIB_A_HDRS:%=o/$(MODE)/%.ok)
APE_LIB_A_DIRECTDEPS = LIBC_STR LIBC_STUBS
APE_LIB_A_DEPS = $(call uniq,$(foreach x,$(APE_LIB_A_DIRECTDEPS),$($(x))))

$(APE_LIB_A): ape/lib/ $(APE_LIB_A).pkg $(APE_LIB_A_OBJS)
$(APE_LIB_A).pkg: $(APE_LIB_A_OBJS) $(foreach x,$(APE_LIB_A_DIRECTDEPS),$($(x)_A).pkg)

APE_LIB_LIBS = $(foreach x,$(APE_LIB_ARTIFACTS),$($(x)))
APE_LIB_SRCS = $(foreach x,$(APE_LIB_ARTIFACTS),$($(x)_SRCS))
APE_LIB_HDRS = $(foreach x,$(APE_LIB_ARTIFACTS),$($(x)_HDRS))
APE_LIB_BINS = $(foreach x,$(APE_LIB_ARTIFACTS),$($(x)_BINS))
APE_LIB_CHECKS = $(foreach x,$(APE_LIB_ARTIFACTS),$($(x)_CHECKS))
APE_LIB_OBJS = $(foreach x,$(APE_LIB_ARTIFACTS),$($(x)_OBJS))
$(APE_LIB_OBJS): $(BUILD_FILES) libc/str/str.mk

.PHONY: o/$(MODE)/ape/lib
o/$(MODE)/ape/lib:			\
		$(APE_LIB_CHECKS)	\
		$(APE_LIB_A)
