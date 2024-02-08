#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#── vi: set noet ft=make ts=8 sw=8 fenc=utf-8 :vi ────────────────────┘

PKGS += THIRD_PARTY_OPENMP

THIRD_PARTY_OPENMP_ARTIFACTS += THIRD_PARTY_OPENMP_A
THIRD_PARTY_OPENMP = $(THIRD_PARTY_OPENMP_A_DEPS) $(THIRD_PARTY_OPENMP_A)
THIRD_PARTY_OPENMP_A = o/$(MODE)/third_party/openmp/openmp.a
THIRD_PARTY_OPENMP_A_FILES := $(wildcard third_party/openmp/*)
THIRD_PARTY_OPENMP_A_HDRS = $(filter %.h,$(THIRD_PARTY_OPENMP_A_FILES))
THIRD_PARTY_OPENMP_A_INCS = $(filter %.inc,$(THIRD_PARTY_OPENMP_A_FILES))
THIRD_PARTY_OPENMP_A_SRCS_CPP = $(filter %.cpp,$(THIRD_PARTY_OPENMP_A_FILES))
THIRD_PARTY_OPENMP_A_SRCS_S = $(filter %.S,$(THIRD_PARTY_OPENMP_A_FILES))
THIRD_PARTY_OPENMP_A_SRCS = $(THIRD_PARTY_OPENMP_A_SRCS_CPP) $(THIRD_PARTY_OPENMP_A_SRCS_S)
THIRD_PARTY_OPENMP_A_OBJS_CPP = $(THIRD_PARTY_OPENMP_A_SRCS_CPP:%.cpp=o/$(MODE)/%.o)
THIRD_PARTY_OPENMP_A_OBJS_S = $(THIRD_PARTY_OPENMP_A_SRCS_S:%.S=o/$(MODE)/%.o)
THIRD_PARTY_OPENMP_A_OBJS = $(THIRD_PARTY_OPENMP_A_OBJS_CPP) $(THIRD_PARTY_OPENMP_A_OBJS_S)

THIRD_PARTY_OPENMP_A_DIRECTDEPS =				\
	LIBC_CALLS						\
	LIBC_DLOPEN						\
	LIBC_FMT						\
	LIBC_INTRIN						\
	LIBC_MEM						\
	LIBC_NEXGEN32E						\
	LIBC_PROC						\
	LIBC_RUNTIME						\
	LIBC_STDIO						\
	LIBC_STR						\
	LIBC_SYSV						\
	LIBC_SYSV_CALLS						\
	LIBC_THREAD						\
	THIRD_PARTY_COMPILER_RT					\
	THIRD_PARTY_GDTOA					\
	THIRD_PARTY_LIBCXX					\
	THIRD_PARTY_NSYNC					\
	THIRD_PARTY_MUSL

THIRD_PARTY_OPENMP_A_DEPS :=					\
	$(call uniq,$(foreach x,$(THIRD_PARTY_OPENMP_A_DIRECTDEPS),$($(x))))

THIRD_PARTY_OPENMP_A_CHECKS =					\
	$(THIRD_PARTY_OPENMP_A).pkg

$(THIRD_PARTY_OPENMP_A):					\
		third_party/openmp/				\
		$(THIRD_PARTY_OPENMP_A).pkg			\
		$(THIRD_PARTY_OPENMP_A_OBJS)

$(THIRD_PARTY_OPENMP_A).pkg:					\
		$(THIRD_PARTY_OPENMP_A_OBJS)			\
		$(foreach x,$(THIRD_PARTY_OPENMP_A_DIRECTDEPS),$($(x)_A).pkg)

$(THIRD_PARTY_OPENMP_A_OBJS): private				\
		COPTS +=					\
			-fno-sanitize=all			\
			-fdata-sections				\
			-ffunction-sections			\
			-Wno-maybe-uninitialized		\
			-Wno-stringop-truncation		\
			-Wno-unused-but-set-variable		\
			-Wno-class-memaccess			\
			-fno-strict-aliasing			\
			-Wno-frame-address			\
			-Wno-sign-compare

o/$(MODE)/third_party/openmp/util1.o: private COPTS += -fportcosmo

# these assembly files are safe to build on aarch64
o/$(MODE)/third_party/openmp/util2.o: third_party/openmp/util2.S
	@$(COMPILE) -AOBJECTIFY.S $(OBJECTIFY.S) $(OUTPUT_OPTION) -c $<

THIRD_PARTY_OPENMP_LIBS = $(foreach x,$(THIRD_PARTY_OPENMP_ARTIFACTS),$($(x)))
THIRD_PARTY_OPENMP_SRCS = $(foreach x,$(THIRD_PARTY_OPENMP_ARTIFACTS),$($(x)_SRCS))
THIRD_PARTY_OPENMP_HDRS = $(foreach x,$(THIRD_PARTY_OPENMP_ARTIFACTS),$($(x)_HDRS))
THIRD_PARTY_OPENMP_INCS = $(foreach x,$(THIRD_PARTY_OPENMP_ARTIFACTS),$($(x)_INCS))
THIRD_PARTY_OPENMP_CHECKS = $(foreach x,$(THIRD_PARTY_OPENMP_ARTIFACTS),$($(x)_CHECKS))
THIRD_PARTY_OPENMP_OBJS = $(foreach x,$(THIRD_PARTY_OPENMP_ARTIFACTS),$($(x)_OBJS))
$(THIRD_PARTY_OPENMP_OBJS): third_party/openmp/BUILD.mk

.PHONY: o/$(MODE)/third_party/openmp
o/$(MODE)/third_party/openmp:					\
	$(THIRD_PARTY_OPENMP_CHECKS)
