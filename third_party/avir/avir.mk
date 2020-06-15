#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#───vi: set et ft=make ts=8 tw=8 fenc=utf-8 :vi───────────────────────┘

PKGS += THIRD_PARTY_AVIR

THIRD_PARTY_AVIR_ARTIFACTS += THIRD_PARTY_AVIR_A
THIRD_PARTY_AVIR = $(THIRD_PARTY_AVIR_A_DEPS) $(THIRD_PARTY_AVIR_A)
THIRD_PARTY_AVIR_A = o/$(MODE)/third_party/avir/avir.a
THIRD_PARTY_AVIR_A_CHECKS = $(THIRD_PARTY_AVIR_A).pkg
THIRD_PARTY_AVIR_A_FILES := $(wildcard third_party/avir/*)
THIRD_PARTY_AVIR_A_SRCS_S = $(filter %.S,$(THIRD_PARTY_AVIR_A_FILES))
THIRD_PARTY_AVIR_A_SRCS_C = $(filter %.c,$(THIRD_PARTY_AVIR_A_FILES))
THIRD_PARTY_AVIR_A_SRCS_X = $(filter %.cc,$(THIRD_PARTY_AVIR_A_FILES))

THIRD_PARTY_AVIR_A_HDRS =				\
	$(filter %.h,$(THIRD_PARTY_AVIR_A_FILES))	\
	$(filter %.hpp,$(THIRD_PARTY_AVIR_A_FILES))

THIRD_PARTY_AVIR_A_SRCS =				\
	$(THIRD_PARTY_AVIR_A_SRCS_S)			\
	$(THIRD_PARTY_AVIR_A_SRCS_C)			\
	$(THIRD_PARTY_AVIR_A_SRCS_X)

THIRD_PARTY_AVIR_A_OBJS =				\
	$(THIRD_PARTY_AVIR_A_SRCS:%=o/$(MODE)/%.zip.o)	\
	$(THIRD_PARTY_AVIR_A_SRCS_S:%.S=o/$(MODE)/%.o)	\
	$(THIRD_PARTY_AVIR_A_SRCS_C:%.c=o/$(MODE)/%.o)	\
	$(THIRD_PARTY_AVIR_A_SRCS_X:%.cc=o/$(MODE)/%.o)

THIRD_PARTY_AVIR_A_DIRECTDEPS =				\
	DSP_CORE					\
	LIBC_NEXGEN32E					\
	LIBC_BITS					\
	LIBC_MEM					\
	LIBC_CALLS					\
	LIBC_STUBS					\
	LIBC_SYSV					\
	LIBC_FMT					\
	LIBC_UNICODE					\
	LIBC_LOG					\
	LIBC_TINYMATH

$(THIRD_PARTY_AVIR_A).pkg:				\
		$(THIRD_PARTY_AVIR_A_OBJS)		\
		$(foreach x,$(THIRD_PARTY_AVIR_A_DIRECTDEPS),$($(x)_A).pkg)

$(THIRD_PARTY_AVIR_A):					\
		third_party/avir/			\
		$(THIRD_PARTY_AVIR_A).pkg		\
		$(THIRD_PARTY_AVIR_A_OBJS)

#o/$(MODE)/third_party/avir/lanczos1b.o:			\
		CXX = clang++-10

o/$(MODE)/third_party/avir/lanczos1b.o			\
o/$(MODE)/third_party/avir/lanczos.o:			\
		OVERRIDE_CXXFLAGS +=			\
			$(MATHEMATICAL)

THIRD_PARTY_AVIR_A_DEPS :=				\
	$(call uniq,$(foreach x,$(THIRD_PARTY_AVIR_A_DIRECTDEPS),$($(x))))

THIRD_PARTY_AVIR_LIBS = $(foreach x,$(THIRD_PARTY_AVIR_ARTIFACTS),$($(x)))
THIRD_PARTY_AVIR_SRCS = $(foreach x,$(THIRD_PARTY_AVIR_ARTIFACTS),$($(x)_SRCS))
THIRD_PARTY_AVIR_HDRS = $(foreach x,$(THIRD_PARTY_AVIR_ARTIFACTS),$($(x)_HDRS))
THIRD_PARTY_AVIR_CHECKS = $(foreach x,$(THIRD_PARTY_AVIR_ARTIFACTS),$($(x)_CHECKS))
THIRD_PARTY_AVIR_OBJS = $(foreach x,$(THIRD_PARTY_AVIR_ARTIFACTS),$($(x)_OBJS))
THIRD_PARTY_AVIR_TESTS = $(foreach x,$(THIRD_PARTY_AVIR_ARTIFACTS),$($(x)_TESTS))

.PHONY: o/$(MODE)/third_party/avir
o/$(MODE)/third_party/avir: $(THIRD_PARTY_AVIR_A_CHECKS)
