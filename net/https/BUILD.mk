#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#── vi: set noet ft=make ts=8 sw=8 fenc=utf-8 :vi ────────────────────┘

PKGS += NET_HTTPS

NET_HTTPS_ARTIFACTS += NET_HTTPS_A
NET_HTTPS = $(NET_HTTPS_A_DEPS) $(NET_HTTPS_A)
NET_HTTPS_A = o/$(MODE)/net/https/https.a
NET_HTTPS_A_FILES := $(wildcard net/https/*)
NET_HTTPS_A_CERTS := $(wildcard usr/share/ssl/root/*.pem)
NET_HTTPS_A_HDRS = $(filter %.h,$(NET_HTTPS_A_FILES))
NET_HTTPS_A_SRCS = $(filter %.c,$(NET_HTTPS_A_FILES))

NET_HTTPS_A_OBJS =				\
	o/$(MODE)/usr/share/ssl/root/.zip.o	\
	$(NET_HTTPS_A_SRCS:%.c=o/$(MODE)/%.o)	\
	$(NET_HTTPS_A_CERTS:%=o/$(MODE)/%.zip.o)

NET_HTTPS_A_CHECKS =				\
	$(NET_HTTPS_A).pkg			\
	$(NET_HTTPS_A_HDRS:%=o/$(MODE)/%.ok)

NET_HTTPS_A_DIRECTDEPS =			\
	LIBC_CALLS				\
	LIBC_FMT				\
	LIBC_INTRIN				\
	LIBC_LOG				\
	LIBC_MEM				\
	LIBC_NEXGEN32E				\
	LIBC_RUNTIME				\
	LIBC_STDIO				\
	LIBC_STR				\
	LIBC_SYSV				\
	LIBC_THREAD				\
	LIBC_TIME				\
	LIBC_X					\
	THIRD_PARTY_COMPILER_RT			\
	THIRD_PARTY_MBEDTLS

NET_HTTPS_A_DEPS :=				\
	$(call uniq,$(foreach x,$(NET_HTTPS_A_DIRECTDEPS),$($(x))))

$(NET_HTTPS_A):	net/https/			\
		$(NET_HTTPS_A).pkg		\
		$(NET_HTTPS_A_OBJS)

$(NET_HTTPS_A).pkg:				\
		$(NET_HTTPS_A_OBJS)		\
		$(foreach x,$(NET_HTTPS_A_DIRECTDEPS),$($(x)_A).pkg)

o/$(MODE)/usr/share/ssl/root/.zip.o:		\
		usr/share/ssl/root

NET_HTTPS_LIBS = $(foreach x,$(NET_HTTPS_ARTIFACTS),$($(x)))
NET_HTTPS_SRCS = $(foreach x,$(NET_HTTPS_ARTIFACTS),$($(x)_SRCS))
NET_HTTPS_HDRS = $(foreach x,$(NET_HTTPS_ARTIFACTS),$($(x)_HDRS))
NET_HTTPS_OBJS = $(foreach x,$(NET_HTTPS_ARTIFACTS),$($(x)_OBJS))
NET_HTTPS_CHECKS = $(foreach x,$(NET_HTTPS_ARTIFACTS),$($(x)_CHECKS))

.PHONY: o/$(MODE)/net/https
o/$(MODE)/net/https: $(NET_HTTPS_CHECKS)
