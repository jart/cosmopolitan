#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#───vi: set et ft=make ts=8 tw=8 fenc=utf-8 :vi───────────────────────┘

PKGS += LIBC_RAND

LIBC_RAND_ARTIFACTS += LIBC_RAND_A
LIBC_RAND = $(LIBC_RAND_A_DEPS) $(LIBC_RAND_A)
LIBC_RAND_A = o/$(MODE)/libc/rand/rand.a
LIBC_RAND_A_FILES := $(wildcard libc/rand/*)
LIBC_RAND_A_HDRS = $(filter %.h,$(LIBC_RAND_A_FILES))
LIBC_RAND_A_SRCS_S = $(filter %.S,$(LIBC_RAND_A_FILES))
LIBC_RAND_A_SRCS_C = $(filter %.c,$(LIBC_RAND_A_FILES))

LIBC_RAND_A_SRCS =				\
	$(LIBC_RAND_A_SRCS_S)			\
	$(LIBC_RAND_A_SRCS_C)

LIBC_RAND_A_OBJS =				\
	$(LIBC_RAND_A_SRCS:%=o/$(MODE)/%.zip.o)	\
	$(LIBC_RAND_A_SRCS_S:%.S=o/$(MODE)/%.o)	\
	$(LIBC_RAND_A_SRCS_C:%.c=o/$(MODE)/%.o)

LIBC_RAND_A_CHECKS =				\
	$(LIBC_RAND_A).pkg			\
	$(LIBC_RAND_A_HDRS:%=o/$(MODE)/%.ok)

LIBC_RAND_A_DIRECTDEPS =			\
	LIBC_STUBS				\
	LIBC_TINYMATH				\
	LIBC_NEXGEN32E				\
	LIBC_NT_KERNEL32			\
	LIBC_STR				\
	LIBC_SYSV_CALLS				\
	LIBC_SYSV

LIBC_RAND_A_DEPS :=				\
	$(call uniq,$(foreach x,$(LIBC_RAND_A_DIRECTDEPS),$($(x))))

$(LIBC_RAND_A):	libc/rand/			\
		$(LIBC_RAND_A).pkg		\
		$(LIBC_RAND_A_OBJS)

$(LIBC_RAND_A).pkg:				\
		$(LIBC_RAND_A_OBJS)		\
		$(foreach x,$(LIBC_RAND_A_DIRECTDEPS),$($(x)_A).pkg)

LIBC_RAND_LIBS = $(foreach x,$(LIBC_RAND_ARTIFACTS),$($(x)))
LIBC_RAND_SRCS = $(foreach x,$(LIBC_RAND_ARTIFACTS),$($(x)_SRCS))
LIBC_RAND_HDRS = $(foreach x,$(LIBC_RAND_ARTIFACTS),$($(x)_HDRS))
LIBC_RAND_CHECKS = $(foreach x,$(LIBC_RAND_ARTIFACTS),$($(x)_CHECKS))
LIBC_RAND_OBJS = $(foreach x,$(LIBC_RAND_ARTIFACTS),$($(x)_OBJS))
$(LIBC_RAND_OBJS): $(BUILD_FILES) libc/rand/rand.mk

.PHONY: o/$(MODE)/libc/rand
o/$(MODE)/libc/rand: $(LIBC_RAND_CHECKS)
