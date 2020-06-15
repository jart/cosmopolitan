#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#───vi: set et ft=make ts=8 tw=8 fenc=utf-8 :vi───────────────────────┘

PKGS += LIBC_ALG

LIBC_ALG_ARTIFACTS += LIBC_ALG_A
LIBC_ALG = $(LIBC_ALG_A_DEPS) $(LIBC_ALG_A)
LIBC_ALG_A = o/$(MODE)/libc/alg/alg.a
LIBC_ALG_A_FILES := $(wildcard libc/alg/*)
LIBC_ALG_A_HDRS = $(filter %.h,$(LIBC_ALG_A_FILES))
LIBC_ALG_A_SRCS_S = $(filter %.S,$(LIBC_ALG_A_FILES))
LIBC_ALG_A_SRCS_C = $(filter %.c,$(LIBC_ALG_A_FILES))

LIBC_ALG_A_SRCS =				\
	$(LIBC_ALG_A_SRCS_S)			\
	$(LIBC_ALG_A_SRCS_C)

LIBC_ALG_A_OBJS =				\
	$(LIBC_ALG_A_SRCS:%=o/$(MODE)/%.zip.o)	\
	$(LIBC_ALG_A_SRCS_S:%.S=o/$(MODE)/%.o)	\
	$(LIBC_ALG_A_SRCS_C:%.c=o/$(MODE)/%.o)

LIBC_ALG_A_CHECKS =				\
	$(LIBC_ALG_A).pkg			\
	$(LIBC_ALG_A_HDRS:%=o/$(MODE)/%.ok)

LIBC_ALG_A_DIRECTDEPS =				\
	LIBC_MEM				\
	LIBC_NEXGEN32E				\
	LIBC_RUNTIME				\
	LIBC_STR				\
	LIBC_STUBS				\
	LIBC_SYSV

LIBC_ALG_A_DEPS :=				\
	$(call uniq,$(foreach x,$(LIBC_ALG_A_DIRECTDEPS),$($(x))))

$(LIBC_ALG_A):	libc/alg/			\
		$(LIBC_ALG_A).pkg		\
		$(LIBC_ALG_A_OBJS)

$(LIBC_ALG_A).pkg:				\
		$(LIBC_ALG_A_OBJS)		\
		$(foreach x,$(LIBC_ALG_A_DIRECTDEPS),$($(x)_A).pkg)

o/$(MODE)/libc/alg/critbit0.o:			\
	DEFAULT_CFLAGS +=			\
		-ffunction-sections		\
		-fdata-sections

LIBC_ALG_LIBS = $(foreach x,$(LIBC_ALG_ARTIFACTS),$($(x)))
LIBC_ALG_SRCS = $(foreach x,$(LIBC_ALG_ARTIFACTS),$($(x)_SRCS))
LIBC_ALG_HDRS = $(foreach x,$(LIBC_ALG_ARTIFACTS),$($(x)_HDRS))
LIBC_ALG_CHECKS = $(foreach x,$(LIBC_ALG_ARTIFACTS),$($(x)_CHECKS))
LIBC_ALG_OBJS = $(foreach x,$(LIBC_ALG_ARTIFACTS),$($(x)_OBJS))
$(LIBC_ALG_OBJS): $(BUILD_FILES) libc/alg/alg.mk

.PHONY: o/$(MODE)/libc/alg
o/$(MODE)/libc/alg: $(LIBC_ALG_CHECKS)
