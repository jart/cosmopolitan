#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#── vi: set noet ft=make ts=8 sw=8 fenc=utf-8 :vi ────────────────────┘

PKGS += THIRD_PARTY_FINGER

THIRD_PARTY_FINGER_ARTIFACTS += THIRD_PARTY_FINGER_A
THIRD_PARTY_FINGER = $(THIRD_PARTY_FINGER_A_DEPS) $(THIRD_PARTY_FINGER_A)
THIRD_PARTY_FINGER_A = o/$(MODE)/third_party/finger/finger.a
THIRD_PARTY_FINGER_A_FILES := $(wildcard third_party/finger/*)
THIRD_PARTY_FINGER_A_HDRS = $(filter %.h,$(THIRD_PARTY_FINGER_A_FILES))
THIRD_PARTY_FINGER_A_INCS = $(filter %.inc,$(THIRD_PARTY_FINGER_A_FILES))
THIRD_PARTY_FINGER_A_SRCS = $(filter %.c,$(THIRD_PARTY_FINGER_A_FILES))
THIRD_PARTY_FINGER_A_OBJS = $(THIRD_PARTY_FINGER_A_SRCS:%.c=o/$(MODE)/%.o)

THIRD_PARTY_FINGER_A_DIRECTDEPS =			\
	LIBC_CALLS					\
	LIBC_FMT					\
	LIBC_INTRIN					\
	LIBC_MEM					\
	LIBC_NEXGEN32E					\
	LIBC_RUNTIME					\
	LIBC_STDIO					\
	LIBC_STR					\
	LIBC_SYSV					\
	LIBC_SOCK					\
	LIBC_TIME					\
	THIRD_PARTY_MUSL				\
	THIRD_PARTY_GETOPT

THIRD_PARTY_FINGER_A_DEPS :=				\
	$(call uniq,$(foreach x,$(THIRD_PARTY_FINGER_A_DIRECTDEPS),$($(x))))

THIRD_PARTY_FINGER_A_CHECKS =				\
	$(THIRD_PARTY_FINGER_A).pkg

$(THIRD_PARTY_FINGER_A):				\
		third_party/finger/			\
		$(THIRD_PARTY_FINGER_A).pkg		\
		$(THIRD_PARTY_FINGER_A_OBJS)

$(THIRD_PARTY_FINGER_A).pkg:				\
		$(THIRD_PARTY_FINGER_A_OBJS)		\
		$(foreach x,$(THIRD_PARTY_FINGER_A_DIRECTDEPS),$($(x)_A).pkg)

o/$(MODE)/third_party/finger/finger.com.dbg:		\
		$(THIRD_PARTY_FINGER)			\
		o/$(MODE)/third_party/finger/finger.o	\
		$(CRT)					\
		$(APE_NO_MODIFY_SELF)
	@$(APELINK)

THIRD_PARTY_FINGER_COMS = o/$(MODE)/third_party/finger/finger.com
THIRD_PARTY_FINGER_BINS = $(THIRD_PARTY_FINGER_COMS) $(THIRD_PARTY_FINGER_COMS:%=%.dbg)
THIRD_PARTY_FINGER_LIBS = $(foreach x,$(THIRD_PARTY_FINGER_ARTIFACTS),$($(x)))
THIRD_PARTY_FINGER_SRCS = $(foreach x,$(THIRD_PARTY_FINGER_ARTIFACTS),$($(x)_SRCS))
THIRD_PARTY_FINGER_HDRS = $(foreach x,$(THIRD_PARTY_FINGER_ARTIFACTS),$($(x)_HDRS))
THIRD_PARTY_FINGER_INCS = $(foreach x,$(THIRD_PARTY_FINGER_ARTIFACTS),$($(x)_INCS))
THIRD_PARTY_FINGER_CHECKS = $(foreach x,$(THIRD_PARTY_FINGER_ARTIFACTS),$($(x)_CHECKS))
THIRD_PARTY_FINGER_OBJS = $(foreach x,$(THIRD_PARTY_FINGER_ARTIFACTS),$($(x)_OBJS))
$(THIRD_PARTY_FINGER_OBJS): third_party/finger/BUILD.mk

.PHONY: o/$(MODE)/third_party/finger
o/$(MODE)/third_party/finger:				\
		$(THIRD_PARTY_FINGER_BINS)		\
		$(THIRD_PARTY_FINGER_CHECKS)
