#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#───vi: set et ft=make ts=8 tw=8 fenc=utf-8 :vi───────────────────────┘

PKGS += LIBC_SOCK

LIBC_SOCK_ARTIFACTS += LIBC_SOCK_A
LIBC_SOCK = $(LIBC_SOCK_A_DEPS) $(LIBC_SOCK_A)
LIBC_SOCK_A = o/$(MODE)/libc/sock/sock.a
LIBC_SOCK_A_FILES := $(wildcard libc/sock/*)
LIBC_SOCK_A_HDRS = $(filter %.h,$(LIBC_SOCK_A_FILES))
LIBC_SOCK_A_SRCS = $(filter %.c,$(LIBC_SOCK_A_FILES))

LIBC_SOCK_A_OBJS =				\
	$(LIBC_SOCK_A_SRCS:%=o/$(MODE)/%.zip.o)	\
	$(LIBC_SOCK_A_SRCS:%.c=o/$(MODE)/%.o)

LIBC_SOCK_A_CHECKS =				\
	$(LIBC_SOCK_A).pkg			\
	$(LIBC_SOCK_A_HDRS:%=o/$(MODE)/%.ok)

LIBC_SOCK_A_DIRECTDEPS =			\
	LIBC_BITS				\
	LIBC_CALLS				\
	LIBC_FMT				\
	LIBC_MEM				\
	LIBC_NEXGEN32E				\
	LIBC_NT_KERNEL32			\
	LIBC_NT_KERNEL32			\
	LIBC_NT_MSWSOCK				\
	LIBC_NT_NTDLL				\
	LIBC_NT_WS2_32				\
	LIBC_RUNTIME				\
	LIBC_STDIO				\
	LIBC_STUBS				\
	LIBC_SYSV_CALLS				\
	LIBC_SYSV

LIBC_SOCK_A_DEPS :=				\
	$(call uniq,$(foreach x,$(LIBC_SOCK_A_DIRECTDEPS),$($(x))))

$(LIBC_SOCK_A):	libc/sock/			\
		$(LIBC_SOCK_A).pkg		\
		$(LIBC_SOCK_A_OBJS)

$(LIBC_SOCK_A).pkg:				\
		$(LIBC_SOCK_A_OBJS)		\
		$(foreach x,$(LIBC_SOCK_A_DIRECTDEPS),$($(x)_A).pkg)

LIBC_SOCK_LIBS = $(foreach x,$(LIBC_SOCK_ARTIFACTS),$($(x)))
LIBC_SOCK_SRCS = $(foreach x,$(LIBC_SOCK_ARTIFACTS),$($(x)_SRCS))
LIBC_SOCK_HDRS = $(foreach x,$(LIBC_SOCK_ARTIFACTS),$($(x)_HDRS))
LIBC_SOCK_CHECKS = $(foreach x,$(LIBC_SOCK_ARTIFACTS),$($(x)_CHECKS))
LIBC_SOCK_OBJS = $(foreach x,$(LIBC_SOCK_ARTIFACTS),$($(x)_OBJS))
$(LIBC_SOCK_OBJS): $(BUILD_FILES) libc/sock/sock.mk

.PHONY: o/$(MODE)/libc/sock
o/$(MODE)/libc/sock: $(LIBC_SOCK_CHECKS)
