#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#───vi: set et ft=make ts=8 tw=8 fenc=utf-8 :vi───────────────────────┘

PKGS += LIBC_DNS

LIBC_DNS_ARTIFACTS += LIBC_DNS_A
LIBC_DNS = $(LIBC_DNS_A_DEPS) $(LIBC_DNS_A)
LIBC_DNS_A = o/$(MODE)/libc/dns/dns.a
LIBC_DNS_A_FILES := $(wildcard libc/dns/*)
LIBC_DNS_A_HDRS = $(filter %.h,$(LIBC_DNS_A_FILES))
LIBC_DNS_A_SRCS_C = $(filter %.c,$(LIBC_DNS_A_FILES))
LIBC_DNS_A_SRCS_S = $(filter %.S,$(LIBC_DNS_A_FILES))

LIBC_DNS_A_SRCS =				\
	$(LIBC_DNS_A_SRCS_S)			\
	$(LIBC_DNS_A_SRCS_C)

LIBC_DNS_A_OBJS =				\
	$(LIBC_DNS_A_SRCS:%=o/$(MODE)/%.zip.o)	\
	$(LIBC_DNS_A_SRCS_S:%.S=o/$(MODE)/%.o)	\
	$(LIBC_DNS_A_SRCS_C:%.c=o/$(MODE)/%.o)

LIBC_DNS_A_CHECKS =				\
	$(LIBC_DNS_A).pkg			\
	$(LIBC_DNS_A_HDRS:%=o/$(MODE)/%.ok)

LIBC_DNS_A_DIRECTDEPS =				\
	LIBC_ALG				\
	LIBC_CALLS				\
	LIBC_FMT				\
	LIBC_MEM				\
	LIBC_NEXGEN32E				\
	LIBC_RAND				\
	LIBC_RUNTIME				\
	LIBC_SOCK				\
	LIBC_STDIO				\
	LIBC_STUBS				\
	LIBC_STR				\
	LIBC_SYSV				\
	LIBC_NT_ADVAPI32			\
	LIBC_NT_KERNEL32

LIBC_DNS_A_DEPS :=				\
	$(call uniq,$(foreach x,$(LIBC_DNS_A_DIRECTDEPS),$($(x))))

$(LIBC_DNS_A):	libc/dns/			\
		$(LIBC_DNS_A).pkg		\
		$(LIBC_DNS_A_OBJS)

$(LIBC_DNS_A).pkg:				\
		$(LIBC_DNS_A_OBJS)		\
		$(foreach x,$(LIBC_DNS_A_DIRECTDEPS),$($(x)_A).pkg)

LIBC_DNS_LIBS = $(foreach x,$(LIBC_DNS_ARTIFACTS),$($(x)))
LIBC_DNS_SRCS = $(foreach x,$(LIBC_DNS_ARTIFACTS),$($(x)_SRCS))
LIBC_DNS_HDRS = $(foreach x,$(LIBC_DNS_ARTIFACTS),$($(x)_HDRS))
LIBC_DNS_CHECKS = $(foreach x,$(LIBC_DNS_ARTIFACTS),$($(x)_CHECKS))
LIBC_DNS_OBJS = $(foreach x,$(LIBC_DNS_ARTIFACTS),$($(x)_OBJS))
$(LIBC_DNS_OBJS): $(BUILD_FILES) libc/dns/dns.mk

.PHONY: o/$(MODE)/libc/dns
o/$(MODE)/libc/dns: $(LIBC_DNS_CHECKS)
