#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#───vi: set et ft=make ts=8 tw=8 fenc=utf-8 :vi───────────────────────┘

PKGS += THIRD_PARTY_BLAS

THIRD_PARTY_BLAS_ARTIFACTS += THIRD_PARTY_BLAS_A
THIRD_PARTY_BLAS = $(THIRD_PARTY_BLAS_A_DEPS) $(THIRD_PARTY_BLAS_A)
THIRD_PARTY_BLAS_A = o/$(MODE)/third_party/blas/blas.a
THIRD_PARTY_BLAS_A_FILES := $(wildcard third_party/blas/*)
THIRD_PARTY_BLAS_A_HDRS = $(filter %.h,$(THIRD_PARTY_BLAS_A_FILES))
THIRD_PARTY_BLAS_A_SRCS_S = $(filter %.S,$(THIRD_PARTY_BLAS_A_FILES))
THIRD_PARTY_BLAS_A_SRCS_C = $(filter %.c,$(THIRD_PARTY_BLAS_A_FILES))

THIRD_PARTY_BLAS_A_SRCS =				\
	$(THIRD_PARTY_BLAS_A_SRCS_S)			\
	$(THIRD_PARTY_BLAS_A_SRCS_C)

THIRD_PARTY_BLAS_A_OBJS =				\
	$(THIRD_PARTY_BLAS_A_SRCS:%=o/$(MODE)/%.zip.o)	\
	$(THIRD_PARTY_BLAS_A_SRCS_S:%.S=o/$(MODE)/%.o)	\
	$(THIRD_PARTY_BLAS_A_SRCS_C:%.c=o/$(MODE)/%.o)

THIRD_PARTY_BLAS_A_CHECKS =				\
	$(THIRD_PARTY_BLAS_A).pkg			\
	$(THIRD_PARTY_BLAS_A_HDRS:%=o/$(MODE)/%.ok)

THIRD_PARTY_BLAS_A_DIRECTDEPS =				\
	LIBC_STUBS					\
	LIBC_NEXGEN32E					\
	THIRD_PARTY_F2C

THIRD_PARTY_BLAS_A_DEPS :=				\
	$(call uniq,$(foreach x,$(THIRD_PARTY_BLAS_A_DIRECTDEPS),$($(x))))

$(THIRD_PARTY_BLAS_A_OBJS):				\
		OVERRIDE_CFLAGS +=			\
			$(MATHEMATICAL)

$(THIRD_PARTY_BLAS_A):					\
		third_party/blas/			\
		$(THIRD_PARTY_BLAS_A).pkg		\
		$(THIRD_PARTY_BLAS_A_OBJS)

$(THIRD_PARTY_BLAS_A).pkg:				\
		$(THIRD_PARTY_BLAS_A_OBJS)		\
		$(foreach x,$(THIRD_PARTY_BLAS_A_DIRECTDEPS),$($(x)_A).pkg)

THIRD_PARTY_BLAS_LIBS = $(foreach x,$(THIRD_PARTY_BLAS_ARTIFACTS),$($(x)))
THIRD_PARTY_BLAS_SRCS = $(foreach x,$(THIRD_PARTY_BLAS_ARTIFACTS),$($(x)_SRCS))
THIRD_PARTY_BLAS_HDRS = $(foreach x,$(THIRD_PARTY_BLAS_ARTIFACTS),$($(x)_HDRS))
THIRD_PARTY_BLAS_CHECKS = $(foreach x,$(THIRD_PARTY_BLAS_ARTIFACTS),$($(x)_CHECKS))
THIRD_PARTY_BLAS_OBJS = $(foreach x,$(THIRD_PARTY_BLAS_ARTIFACTS),$($(x)_OBJS))
$(THIRD_PARTY_BLAS_OBJS): $(BUILD_FILES) third_party/blas/blas.mk

.PHONY: o/$(MODE)/third_party/blas
o/$(MODE)/third_party/blas: $(THIRD_PARTY_BLAS_CHECKS)
