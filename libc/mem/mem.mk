#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#───vi: set et ft=make ts=8 tw=8 fenc=utf-8 :vi───────────────────────┘

PKGS += LIBC_MEM

LIBC_MEM_ARTIFACTS += LIBC_MEM_A
LIBC_MEM = $(LIBC_MEM_A_DEPS) $(LIBC_MEM_A)
LIBC_MEM_A = o/$(MODE)/libc/mem/mem.a
LIBC_MEM_A_HDRS = $(filter %.h,$(LIBC_MEM_A_FILES))
LIBC_MEM_A_SRCS_S = $(filter %.S,$(LIBC_MEM_A_FILES))
LIBC_MEM_A_SRCS_C = $(filter %.c,$(LIBC_MEM_A_FILES))

LIBC_MEM_A_FILES :=				\
	$(wildcard libc/mem/*)			\
	$(wildcard libc/mem/cxx/*)		\
	$(wildcard libc/mem/hook/*)

LIBC_MEM_A_SRCS =				\
	$(LIBC_MEM_A_SRCS_S)			\
	$(LIBC_MEM_A_SRCS_C)

LIBC_MEM_A_OBJS =				\
	$(LIBC_MEM_A_SRCS:%=o/$(MODE)/%.zip.o)	\
	$(LIBC_MEM_A_SRCS_S:%.S=o/$(MODE)/%.o)	\
	$(LIBC_MEM_A_SRCS_C:%.c=o/$(MODE)/%.o)

LIBC_MEM_A_CHECKS =				\
	$(LIBC_MEM_A).pkg			\
	$(LIBC_MEM_A_HDRS:%=o/$(MODE)/%.ok)

LIBC_MEM_A_DIRECTDEPS =				\
	LIBC_CALLS				\
	LIBC_FMT				\
	LIBC_NEXGEN32E				\
	LIBC_RAND				\
	LIBC_RUNTIME				\
	LIBC_STR				\
	LIBC_STUBS				\
	LIBC_SYSV				\
	THIRD_PARTY_DLMALLOC

LIBC_MEM_A_DEPS :=				\
	$(call uniq,$(foreach x,$(LIBC_MEM_A_DIRECTDEPS),$($(x))))

$(LIBC_MEM_A):	libc/mem/			\
		$(LIBC_MEM_A).pkg		\
		$(LIBC_MEM_A_OBJS)

$(LIBC_MEM_A).pkg:				\
		$(LIBC_MEM_A_OBJS)		\
		$(foreach x,$(LIBC_MEM_A_DIRECTDEPS),$($(x)_A).pkg)

LIBC_MEM_LIBS = $(foreach x,$(LIBC_MEM_ARTIFACTS),$($(x)))
LIBC_MEM_SRCS = $(foreach x,$(LIBC_MEM_ARTIFACTS),$($(x)_SRCS))
LIBC_MEM_HDRS = $(foreach x,$(LIBC_MEM_ARTIFACTS),$($(x)_HDRS))
LIBC_MEM_BINS = $(foreach x,$(LIBC_MEM_ARTIFACTS),$($(x)_BINS))
LIBC_MEM_CHECKS = $(foreach x,$(LIBC_MEM_ARTIFACTS),$($(x)_CHECKS))
LIBC_MEM_OBJS = $(foreach x,$(LIBC_MEM_ARTIFACTS),$($(x)_OBJS))
LIBC_MEM_TESTS = $(foreach x,$(LIBC_MEM_ARTIFACTS),$($(x)_TESTS))
$(LIBC_MEM_OBJS): $(BUILD_FILES) libc/mem/mem.mk

.PHONY: o/$(MODE)/libc/mem
o/$(MODE)/libc/mem: $(LIBC_MEM_CHECKS)
