#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#── vi: set noet ft=make ts=8 sw=8 fenc=utf-8 :vi ────────────────────┘

PKGS += NET_FINGER

NET_FINGER_ARTIFACTS += NET_FINGER_A
NET_FINGER = $(NET_FINGER_A_DEPS) $(NET_FINGER_A)
NET_FINGER_A = o/$(MODE)/net/finger/finger.a
NET_FINGER_A_FILES := $(wildcard net/finger/*)
NET_FINGER_A_HDRS = $(filter %.h,$(NET_FINGER_A_FILES))
NET_FINGER_A_INCS := $(filter %.inc,$(NET_FINGER_A_FILES))
NET_FINGER_A_SRCS_C = $(filter %.c,$(NET_FINGER_A_FILES))
NET_FINGER_A_SRCS_S = $(filter %.S,$(NET_FINGER_A_FILES))
NET_FINGER_A_SRCS = $(NET_FINGER_A_SRCS_S) $(NET_FINGER_A_SRCS_C)
NET_FINGER_A_OBJS_C = $(NET_FINGER_A_SRCS_C:%.c=o/$(MODE)/%.o)
NET_FINGER_A_OBJS_S = $(NET_FINGER_A_SRCS_S:%.S=o/$(MODE)/%.o)
NET_FINGER_A_OBJS = $(NET_FINGER_A_OBJS_S) $(NET_FINGER_A_OBJS_C)

NET_FINGER_A_CHECKS =			\
	$(NET_FINGER_A).pkg		\
	$(NET_FINGER_A_HDRS:%=o/$(MODE)/%.ok)

NET_FINGER_A_DIRECTDEPS =		\
	LIBC_INTRIN			\
	LIBC_NEXGEN32E

NET_FINGER_A_DEPS :=			\
	$(call uniq,$(foreach x,$(NET_FINGER_A_DIRECTDEPS),$($(x))))

$(NET_FINGER_A):			\
		net/finger/		\
		$(NET_FINGER_A).pkg	\
		$(NET_FINGER_A_OBJS)

$(NET_FINGER_A).pkg:			\
		$(NET_FINGER_A_OBJS)	\
		$(foreach x,$(NET_FINGER_A_DIRECTDEPS),$($(x)_A).pkg)

NET_FINGER_LIBS = $(foreach x,$(NET_FINGER_ARTIFACTS),$($(x)))
NET_FINGER_SRCS = $(foreach x,$(NET_FINGER_ARTIFACTS),$($(x)_SRCS))
NET_FINGER_HDRS = $(foreach x,$(NET_FINGER_ARTIFACTS),$($(x)_HDRS))
NET_FINGER_OBJS = $(foreach x,$(NET_FINGER_ARTIFACTS),$($(x)_OBJS))
NET_FINGER_CHECKS = $(foreach x,$(NET_FINGER_ARTIFACTS),$($(x)_CHECKS))

.PHONY: o/$(MODE)/net/finger
o/$(MODE)/net/finger:			\
		$(NET_FINGER_CHECKS)
