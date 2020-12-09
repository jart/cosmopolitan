#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#───vi: set et ft=make ts=8 tw=8 fenc=utf-8 :vi───────────────────────┘

PKGS += THIRD_PARTY_F2C

THIRD_PARTY_F2C_ARTIFACTS += THIRD_PARTY_F2C_A
THIRD_PARTY_F2C = $(THIRD_PARTY_F2C_A_DEPS) $(THIRD_PARTY_F2C_A)
THIRD_PARTY_F2C_A = o/$(MODE)/third_party/f2c/f2c.a
THIRD_PARTY_F2C_A_FILES := $(wildcard third_party/f2c/*)
THIRD_PARTY_F2C_A_HDRS = $(filter %.h,$(THIRD_PARTY_F2C_A_FILES))
THIRD_PARTY_F2C_A_SRCS_C = $(filter %.c,$(THIRD_PARTY_F2C_A_FILES))

THIRD_PARTY_F2C_A_SRCS =				\
	$(THIRD_PARTY_F2C_A_SRCS_C)

THIRD_PARTY_F2C_A_OBJS =				\
	$(THIRD_PARTY_F2C_A_SRCS:%=o/$(MODE)/%.zip.o)	\
	$(THIRD_PARTY_F2C_A_SRCS_C:%.c=o/$(MODE)/%.o)

THIRD_PARTY_F2C_A_CHECKS =				\
	$(THIRD_PARTY_F2C_A).pkg			\
	$(THIRD_PARTY_F2C_A_HDRS:%=o/$(MODE)/%.ok)

THIRD_PARTY_F2C_A_DIRECTDEPS =				\
	LIBC_CALLS					\
	LIBC_CALLS_HEFTY				\
	LIBC_FMT					\
	LIBC_MEM					\
	LIBC_NEXGEN32E					\
	LIBC_RUNTIME					\
	LIBC_STDIO					\
	LIBC_STR					\
	LIBC_STUBS					\
	LIBC_UNICODE

THIRD_PARTY_F2C_A_DEPS :=				\
	$(call uniq,$(foreach x,$(THIRD_PARTY_F2C_A_DIRECTDEPS),$($(x))))

$(THIRD_PARTY_F2C_A):					\
		third_party/f2c/			\
		$(THIRD_PARTY_F2C_A).pkg		\
		$(THIRD_PARTY_F2C_A_OBJS)

$(THIRD_PARTY_F2C_A).pkg:				\
		$(THIRD_PARTY_F2C_A_OBJS)		\
		$(foreach x,$(THIRD_PARTY_F2C_A_DIRECTDEPS),$($(x)_A).pkg)

THIRD_PARTY_F2C_LIBS = $(foreach x,$(THIRD_PARTY_F2C_ARTIFACTS),$($(x)))
THIRD_PARTY_F2C_SRCS = $(foreach x,$(THIRD_PARTY_F2C_ARTIFACTS),$($(x)_SRCS))
# THIRD_PARTY_F2C_HDRS = $(foreach x,$(THIRD_PARTY_F2C_ARTIFACTS),$($(x)_HDRS))
THIRD_PARTY_F2C_CHECKS = $(foreach x,$(THIRD_PARTY_F2C_ARTIFACTS),$($(x)_CHECKS))
THIRD_PARTY_F2C_OBJS = $(foreach x,$(THIRD_PARTY_F2C_ARTIFACTS),$($(x)_OBJS))
$(THIRD_PARTY_F2C_OBJS): third_party/f2c/f2c.mk

.PHONY: o/$(MODE)/third_party/f2c
o/$(MODE)/third_party/f2c: $(THIRD_PARTY_F2C_CHECKS)
