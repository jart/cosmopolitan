#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#───vi: set et ft=make ts=8 tw=8 fenc=utf-8 :vi───────────────────────┘

PKGS += NET_HTTP

NET_HTTP_ARTIFACTS += NET_HTTP_A
NET_HTTP = $(NET_HTTP_A_DEPS) $(NET_HTTP_A)
NET_HTTP_A = o/$(MODE)/net/http/http.a
NET_HTTP_A_FILES := $(wildcard net/http/*)
NET_HTTP_A_HDRS = $(filter %.h,$(NET_HTTP_A_FILES))
NET_HTTP_A_SRCS_S = $(filter %.S,$(NET_HTTP_A_FILES))
NET_HTTP_A_SRCS_C = $(filter %.c,$(NET_HTTP_A_FILES))
NET_HTTP_A_SRCS_R = $(filter %.rl,$(NET_HTTP_A_FILES))

NET_HTTP_A_SRCS =				\
	$(NET_HTTP_A_SRCS_S)			\
	$(NET_HTTP_A_SRCS_C)			\
	$(NET_HTTP_A_SRCS_R)

NET_HTTP_A_OBJS =				\
	$(NET_HTTP_A_SRCS:%=o/$(MODE)/%.zip.o)	\
	$(NET_HTTP_A_SRCS_S:%.S=o/$(MODE)/%.o)	\
	$(NET_HTTP_A_SRCS_C:%.c=o/$(MODE)/%.o)	\
	$(NET_HTTP_A_SRCS_R:%.rl=o/$(MODE)/%.o)

NET_HTTP_A_CHECKS =				\
	$(NET_HTTP_A).pkg			\
	$(NET_HTTP_A_HDRS:%=o/$(MODE)/%.ok)

NET_HTTP_A_DIRECTDEPS =				\
	LIBC_ALG				\
	LIBC_CALLS				\
	LIBC_FMT				\
	LIBC_LOG				\
	LIBC_LOG_ASAN				\
	LIBC_NEXGEN32E				\
	LIBC_RUNTIME				\
	LIBC_SOCK				\
	LIBC_STDIO				\
	LIBC_STR				\
	LIBC_STUBS				\
	LIBC_SYSV				\
	LIBC_TIME				\
	LIBC_X

NET_HTTP_A_DEPS :=				\
	$(call uniq,$(foreach x,$(NET_HTTP_A_DIRECTDEPS),$($(x))))

$(NET_HTTP_A):	net/http/			\
		$(NET_HTTP_A).pkg		\
		$(NET_HTTP_A_OBJS)

$(NET_HTTP_A).pkg:				\
		$(NET_HTTP_A_OBJS)		\
		$(foreach x,$(NET_HTTP_A_DIRECTDEPS),$($(x)_A).pkg)

o/$(MODE)/net/http/formathttpdatetime.o:	\
		OVERRIDE_CFLAGS +=		\
			-O3

# ifeq (,$(MODE))
# $(NET_HTTP_A_OBJS):				\
# 		OVERRIDE_CFLAGS +=		\
# 			-fsanitize=address
# endif

NET_HTTP_LIBS = $(foreach x,$(NET_HTTP_ARTIFACTS),$($(x)))
NET_HTTP_SRCS = $(foreach x,$(NET_HTTP_ARTIFACTS),$($(x)_SRCS))
NET_HTTP_HDRS = $(foreach x,$(NET_HTTP_ARTIFACTS),$($(x)_HDRS))
NET_HTTP_CHECKS = $(foreach x,$(NET_HTTP_ARTIFACTS),$($(x)_CHECKS))
NET_HTTP_OBJS = $(foreach x,$(NET_HTTP_ARTIFACTS),$($(x)_OBJS))

.PRECIOUS:					\
	$(NET_HTTP_A_SRCS_R:%.rl=o/$(MODE)/%.c)	\
	o/$(MODE)/net/http/uricspn.s		\
	o/$(MODE)/net/http/uricspn.i		\
	o/$(MODE)/net/http/uricspn.c

.PHONY: o/$(MODE)/net/http
o/$(MODE)/net/http:				\
		$(NET_HTTP_CHECKS)		\
		$(NET_HTTP_A_SRCS_R:%.rl=%.svgz)
