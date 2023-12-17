#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#── vi: set noet ft=make ts=8 sw=8 fenc=utf-8 :vi ────────────────────┘

PKGS += NET_HTTP

NET_HTTP_ARTIFACTS += NET_HTTP_A
NET_HTTP = $(NET_HTTP_A_DEPS) $(NET_HTTP_A)
NET_HTTP_A = o/$(MODE)/net/http/http.a
NET_HTTP_A_FILES := $(wildcard net/http/*)
NET_HTTP_A_HDRS = $(filter %.h,$(NET_HTTP_A_FILES))
NET_HTTP_A_INCS = $(filter %.inc,$(NET_HTTP_A_FILES))
NET_HTTP_A_SRCS = $(filter %.c,$(NET_HTTP_A_FILES))
NET_HTTP_A_OBJS = $(NET_HTTP_A_SRCS:%.c=o/$(MODE)/%.o)

NET_HTTP_A_CHECKS =				\
	$(NET_HTTP_A).pkg			\
	$(NET_HTTP_A_HDRS:%=o/$(MODE)/%.ok)

NET_HTTP_A_DIRECTDEPS =				\
	LIBC_FMT				\
	LIBC_INTRIN				\
	LIBC_MEM				\
	LIBC_NEXGEN32E				\
	LIBC_STR				\
	LIBC_SYSV				\
	LIBC_TIME

NET_HTTP_A_DEPS :=				\
	$(call uniq,$(foreach x,$(NET_HTTP_A_DIRECTDEPS),$($(x))))

$(NET_HTTP_A):	net/http/			\
		$(NET_HTTP_A).pkg		\
		$(NET_HTTP_A_OBJS)

$(NET_HTTP_A).pkg:				\
		$(NET_HTTP_A_OBJS)		\
		$(foreach x,$(NET_HTTP_A_DIRECTDEPS),$($(x)_A).pkg)

o/$(MODE)/net/http/gethttpheadername.o		\
o/$(MODE)/net/http/categorizeip.o		\
o/$(MODE)/net/http/getipcategoryname.o		\
o/$(MODE)/net/http/isafrinicip.o		\
o/$(MODE)/net/http/isanonymousip.o		\
o/$(MODE)/net/http/isapnicip.o			\
o/$(MODE)/net/http/isarinip.o			\
o/$(MODE)/net/http/isdodip.o			\
o/$(MODE)/net/http/islacnicip.o			\
o/$(MODE)/net/http/isloopbackip.o		\
o/$(MODE)/net/http/ismulticastip.o		\
o/$(MODE)/net/http/isripeip.o			\
o/$(MODE)/net/http/istestnetip.o: private	\
		CFLAGS +=			\
			-Os

# we need -O3 because:
#   we're dividing by constants
o/$(MODE)/net/http/formathttpdatetime.o: private\
		CFLAGS +=			\
			-O3

NET_HTTP_LIBS = $(foreach x,$(NET_HTTP_ARTIFACTS),$($(x)))
NET_HTTP_SRCS = $(foreach x,$(NET_HTTP_ARTIFACTS),$($(x)_SRCS))
NET_HTTP_HDRS = $(foreach x,$(NET_HTTP_ARTIFACTS),$($(x)_HDRS))
NET_HTTP_INCS = $(foreach x,$(NET_HTTP_ARTIFACTS),$($(x)_INCS))
NET_HTTP_OBJS = $(foreach x,$(NET_HTTP_ARTIFACTS),$($(x)_OBJS))
NET_HTTP_CHECKS = $(foreach x,$(NET_HTTP_ARTIFACTS),$($(x)_CHECKS))

.PHONY: o/$(MODE)/net/http
o/$(MODE)/net/http: $(NET_HTTP_CHECKS)
