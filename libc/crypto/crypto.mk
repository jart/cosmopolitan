#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#───vi: set et ft=make ts=8 tw=8 fenc=utf-8 :vi───────────────────────┘

PKGS += LIBC_CRYPTO

LIBC_CRYPTO_ARTIFACTS += LIBC_CRYPTO_A
LIBC_CRYPTO = $(LIBC_CRYPTO_A_DEPS) $(LIBC_CRYPTO_A)
LIBC_CRYPTO_A = o/$(MODE)/libc/crypto/crypto.a
LIBC_CRYPTO_A_FILES := $(wildcard libc/crypto/*)
LIBC_CRYPTO_A_HDRS = $(filter %.h,$(LIBC_CRYPTO_A_FILES))
LIBC_CRYPTO_A_SRCS_A = $(filter %.s,$(LIBC_CRYPTO_A_FILES))
LIBC_CRYPTO_A_SRCS_S = $(filter %.S,$(LIBC_CRYPTO_A_FILES))
LIBC_CRYPTO_A_SRCS_C = $(filter %.c,$(LIBC_CRYPTO_A_FILES))

LIBC_CRYPTO_A_SRCS =					\
	$(LIBC_CRYPTO_A_SRCS_A)				\
	$(LIBC_CRYPTO_A_SRCS_S)				\
	$(LIBC_CRYPTO_A_SRCS_C)

LIBC_CRYPTO_A_OBJS =					\
	$(LIBC_CRYPTO_A_SRCS:%=o/$(MODE)/%.zip.o)	\
	$(LIBC_CRYPTO_A_SRCS_A:%.s=o/$(MODE)/%.o)	\
	$(LIBC_CRYPTO_A_SRCS_S:%.S=o/$(MODE)/%.o)	\
	$(LIBC_CRYPTO_A_SRCS_C:%.c=o/$(MODE)/%.o)

LIBC_CRYPTO_A_CHECKS =					\
	$(LIBC_CRYPTO_A).pkg				\
	$(LIBC_CRYPTO_A_HDRS:%=o/$(MODE)/%.ok)

LIBC_CRYPTO_A_DIRECTDEPS =				\
	LIBC_STUBS					\
	LIBC_NEXGEN32E

LIBC_CRYPTO_A_DEPS :=					\
	$(call uniq,$(foreach x,$(LIBC_CRYPTO_A_DIRECTDEPS),$($(x))))

$(LIBC_CRYPTO_A):					\
		libc/crypto/				\
		$(LIBC_CRYPTO_A).pkg			\
		$(LIBC_CRYPTO_A_OBJS)

$(LIBC_CRYPTO_A).pkg:					\
		$(LIBC_CRYPTO_A_OBJS)			\
		$(foreach x,$(LIBC_CRYPTO_A_DIRECTDEPS),$($(x)_A).pkg)

LIBC_CRYPTO_LIBS = $(foreach x,$(LIBC_CRYPTO_ARTIFACTS),$($(x)))
LIBC_CRYPTO_SRCS = $(foreach x,$(LIBC_CRYPTO_ARTIFACTS),$($(x)_SRCS))
LIBC_CRYPTO_HDRS = $(foreach x,$(LIBC_CRYPTO_ARTIFACTS),$($(x)_HDRS))
LIBC_CRYPTO_BINS = $(foreach x,$(LIBC_CRYPTO_ARTIFACTS),$($(x)_BINS))
LIBC_CRYPTO_CHECKS = $(foreach x,$(LIBC_CRYPTO_ARTIFACTS),$($(x)_CHECKS))
LIBC_CRYPTO_OBJS = $(foreach x,$(LIBC_CRYPTO_ARTIFACTS),$($(x)_OBJS))
LIBC_CRYPTO_TESTS = $(foreach x,$(LIBC_CRYPTO_ARTIFACTS),$($(x)_TESTS))
$(LIBC_CRYPTO_OBJS): $(BUILD_FILES) libc/crypto/crypto.mk

.PHONY: o/$(MODE)/libc/crypto
o/$(MODE)/libc/crypto: $(LIBC_CRYPTO_CHECKS)
