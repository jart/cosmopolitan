#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#── vi: set noet ft=make ts=8 sw=8 fenc=utf-8 :vi ────────────────────┘

PKGS += CTL

CTL_ARTIFACTS += CTL_A
CTL = $(CTL_A_DEPS) $(CTL_A)
CTL_A = o/$(MODE)/ctl/ctl.a
CTL_A_FILES := $(wildcard ctl/*)
CTL_A_HDRS = $(filter %.h,$(CTL_A_FILES))
CTL_A_SRCS = $(filter %.cc,$(CTL_A_FILES))
CTL_A_OBJS = $(CTL_A_SRCS:%.cc=o/$(MODE)/%.o)

CTL_A_CHECKS =						\
	$(CTL_A).pkg					\
	$(CTL_A_HDRS:%=o/$(MODE)/%.okk)			\

CTL_A_DIRECTDEPS =					\
	LIBC_INTRIN					\
	LIBC_MEM					\
	LIBC_NEXGEN32E					\
	LIBC_STDIO					\
	LIBC_STR					\
	THIRD_PARTY_DOUBLECONVERSION			\
	THIRD_PARTY_GDTOA				\
	THIRD_PARTY_LIBCXXABI				\
	THIRD_PARTY_LIBUNWIND				\

CTL_A_DEPS := $(call uniq,$(foreach x,$(CTL_A_DIRECTDEPS),$($(x))))

$(CTL_A):	ctl/					\
		$(CTL_A).pkg				\
		$(CTL_A_OBJS)

$(CTL_A).pkg:						\
		$(CTL_A_OBJS)				\
		$(foreach x,$(CTL_A_DIRECTDEPS),$($(x)_A).pkg)

$(CTL_A_OBJS): private					\
		OVERRIDE_CXXFLAGS +=			\
			-Wframe-larger-than=4096	\
			-Walloca-larger-than=4096	\
			-ffunction-sections		\
			-fdata-sections			\
			-fexceptions			\

CTL_LIBS = $(foreach x,$(CTL_ARTIFACTS),$($(x)))
CTL_SRCS = $(foreach x,$(CTL_ARTIFACTS),$($(x)_SRCS))
CTL_HDRS = $(foreach x,$(CTL_ARTIFACTS),$($(x)_HDRS))
CTL_CHECKS = $(foreach x,$(CTL_ARTIFACTS),$($(x)_CHECKS))
CTL_OBJS = $(foreach x,$(CTL_ARTIFACTS),$($(x)_OBJS))
$(CTL_OBJS): $(BUILD_FILES) ctl/BUILD.mk

.PHONY: o/$(MODE)/ctl
o/$(MODE)/ctl: $(CTL_CHECKS)
