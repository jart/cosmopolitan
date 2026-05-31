#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#── vi: set noet ft=make ts=8 sw=8 fenc=utf-8 :vi ────────────────────┘

PKGS += THIRD_PARTY_MBEDTLS4

THIRD_PARTY_MBEDTLS4_WRAPPER_SCRIPT = build/mkmbedtls4wrappers.py
THIRD_PARTY_MBEDTLS4_WRAPPERS := $(shell python3 $(THIRD_PARTY_MBEDTLS4_WRAPPER_SCRIPT) --print-targets)
THIRD_PARTY_MBEDTLS4_NO_STATIC_SRC = third_party/mbedtls4/generated/psa_crypto_driver_wrappers_no_static.c
THIRD_PARTY_MBEDTLS4_WRAPPER_DIRS =					\
	third_party/mbedtls4/generated/mbedtls/				\
	third_party/mbedtls4/generated/mbedtls/private/			\
	third_party/mbedtls4/generated/psa/				\
	third_party/mbedtls4/generated/test/drivers/			\
	third_party/mbedtls4/generated/tf-psa-crypto/			\
	third_party/mbedtls4/generated/tf-psa-crypto/private/		\
	third_party/mbedtls4/generated/tf-psa-crypto/private/everest/

THIRD_PARTY_MBEDTLS4_ARTIFACTS += THIRD_PARTY_MBEDTLS4_A
THIRD_PARTY_MBEDTLS4 = $(THIRD_PARTY_MBEDTLS4_A_DEPS) $(THIRD_PARTY_MBEDTLS4_A)
THIRD_PARTY_MBEDTLS4_A = o/$(MODE)/third_party/mbedtls4/mbedtls4.a

THIRD_PARTY_MBEDTLS4_A_INCS =					\
	$(THIRD_PARTY_MBEDTLS4_WRAPPERS)				\
	$(wildcard third_party/mbedtls4/include/mbedtls/*.h)		\
	$(wildcard third_party/mbedtls4/tf-psa-crypto/include/mbedtls/*.h) \
	$(wildcard third_party/mbedtls4/tf-psa-crypto/include/psa/*.h)	\
	$(wildcard third_party/mbedtls4/tf-psa-crypto/drivers/builtin/include/mbedtls/*.h)

THIRD_PARTY_MBEDTLS4_A_HDRS =						\
	$(THIRD_PARTY_MBEDTLS4_WRAPPERS)				\
	$(wildcard third_party/mbedtls4/include/mbedtls/*.h)		\
	$(wildcard third_party/mbedtls4/library/*.h)			\
	$(wildcard third_party/mbedtls4/tf-psa-crypto/include/mbedtls/*.h) \
	$(wildcard third_party/mbedtls4/tf-psa-crypto/include/psa/*.h)	\
	$(wildcard third_party/mbedtls4/tf-psa-crypto/core/*.h)		\
	$(wildcard third_party/mbedtls4/tf-psa-crypto/drivers/builtin/include/mbedtls/*.h) \
	$(wildcard third_party/mbedtls4/tf-psa-crypto/drivers/builtin/include/mbedtls/private/*.h) \
	$(wildcard third_party/mbedtls4/tf-psa-crypto/drivers/builtin/src/*.h)

THIRD_PARTY_MBEDTLS4_A_CHECKS =					\
	$(THIRD_PARTY_MBEDTLS4_A).pkg

THIRD_PARTY_MBEDTLS4_A_SRCS =						\
	$(wildcard third_party/mbedtls4/library/*.c)			\
	$(filter-out third_party/mbedtls4/tf-psa-crypto/core/psa_crypto_driver_wrappers_no_static.c,$(wildcard third_party/mbedtls4/tf-psa-crypto/core/*.c)) \
	$(THIRD_PARTY_MBEDTLS4_NO_STATIC_SRC)				\
	$(wildcard third_party/mbedtls4/tf-psa-crypto/drivers/builtin/src/*.c) \
	$(wildcard third_party/mbedtls4/tf-psa-crypto/extras/*.c)	\
	$(wildcard third_party/mbedtls4/tf-psa-crypto/platform/*.c)	\
	$(wildcard third_party/mbedtls4/tf-psa-crypto/utilities/*.c)

THIRD_PARTY_MBEDTLS4_A_OBJS = $(THIRD_PARTY_MBEDTLS4_A_SRCS:%.c=o/$(MODE)/%.o)

THIRD_PARTY_MBEDTLS4_A_DIRECTDEPS =					\
	LIBC_CALLS							\
	LIBC_FMT							\
	LIBC_INTRIN							\
	LIBC_MEM							\
	LIBC_NEXGEN32E							\
	LIBC_PROC							\
	LIBC_RUNTIME							\
	LIBC_SOCK							\
	LIBC_STDIO							\
	LIBC_STR							\
	LIBC_SYSV							\
	LIBC_THREAD							\
	THIRD_PARTY_COMPILER_RT						\
	THIRD_PARTY_MUSL						\
	THIRD_PARTY_TZ							\
	THIRD_PARTY_ZLIB

THIRD_PARTY_MBEDTLS4_A_DEPS :=						\
	$(call uniq,$(foreach x,$(THIRD_PARTY_MBEDTLS4_A_DIRECTDEPS),$($(x))))

$(THIRD_PARTY_MBEDTLS4_A):						\
		third_party/mbedtls4/					\
		$(THIRD_PARTY_MBEDTLS4_A).pkg				\
		$(THIRD_PARTY_MBEDTLS4_A_OBJS)
	$(file >$(TMPDIR)/$(subst /,_,$@),$^)
	@$(COMPILE) -AARCHIVE -wT$@ $(AR) $(ARFLAGS) $@ @$(TMPDIR)/$(subst /,_,$@)

$(THIRD_PARTY_MBEDTLS4_A).pkg:						\
		$(THIRD_PARTY_MBEDTLS4_A_OBJS)				\
		$(foreach x,$(THIRD_PARTY_MBEDTLS4_A_DIRECTDEPS),$($(x)_A).pkg)

$(THIRD_PARTY_MBEDTLS4_WRAPPERS) $(THIRD_PARTY_MBEDTLS4_WRAPPER_DIRS) $(THIRD_PARTY_MBEDTLS4_NO_STATIC_SRC) &: \
		$(THIRD_PARTY_MBEDTLS4_WRAPPER_SCRIPT)			\
		third_party/mbedtls4/
	python3 $(THIRD_PARTY_MBEDTLS4_WRAPPER_SCRIPT) --generate

$(THIRD_PARTY_MBEDTLS4_A_OBJS): private					\
		CPPFLAGS +=						\
			-iquotethird_party/mbedtls4/generated		\
			-Ithird_party/mbedtls4/include			\
			-Ithird_party/mbedtls4/library			\
			-Ithird_party/mbedtls4/tf-psa-crypto/include	\
			-Ithird_party/mbedtls4/tf-psa-crypto/core	\
			-Ithird_party/mbedtls4/tf-psa-crypto/dispatch	\
			-Ithird_party/mbedtls4/tf-psa-crypto/drivers/builtin/include \
			-Ithird_party/mbedtls4/tf-psa-crypto/drivers/builtin/src \
			-Ithird_party/mbedtls4/tf-psa-crypto/drivers/everest/include \
			-Ithird_party/mbedtls4/tf-psa-crypto/drivers/p256-m	\
			-Ithird_party/mbedtls4/tf-psa-crypto/drivers/pqcp/include \
			-Ithird_party/mbedtls4/tf-psa-crypto/drivers/pqcp/src

$(THIRD_PARTY_MBEDTLS4_A_OBJS): private					\
		CFLAGS +=						\
			-fdata-sections					\
			-ffunction-sections

THIRD_PARTY_MBEDTLS4_LIBS = $(foreach x,$(THIRD_PARTY_MBEDTLS4_ARTIFACTS),$($(x)))
THIRD_PARTY_MBEDTLS4_SRCS = $(foreach x,$(THIRD_PARTY_MBEDTLS4_ARTIFACTS),$($(x)_SRCS))
THIRD_PARTY_MBEDTLS4_HDRS = $(foreach x,$(THIRD_PARTY_MBEDTLS4_ARTIFACTS),$($(x)_HDRS))
THIRD_PARTY_MBEDTLS4_INCS = $(foreach x,$(THIRD_PARTY_MBEDTLS4_ARTIFACTS),$($(x)_INCS))
THIRD_PARTY_MBEDTLS4_CHECKS = $(foreach x,$(THIRD_PARTY_MBEDTLS4_ARTIFACTS),$($(x)_CHECKS))
THIRD_PARTY_MBEDTLS4_OBJS = $(foreach x,$(THIRD_PARTY_MBEDTLS4_ARTIFACTS),$($(x)_OBJS))
$(THIRD_PARTY_MBEDTLS4_OBJS): $(BUILD_FILES) third_party/mbedtls4/BUILD.mk

.PHONY: o/$(MODE)/third_party/mbedtls4
o/$(MODE)/third_party/mbedtls4:						\
		$(THIRD_PARTY_MBEDTLS4_WRAPPERS)			\
		$(THIRD_PARTY_MBEDTLS4_CHECKS)
