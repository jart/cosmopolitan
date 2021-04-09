#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#───vi: set et ft=make ts=8 tw=8 fenc=utf-8 :vi───────────────────────┘

PKGS += THIRD_PARTY_QUICKJS

THIRD_PARTY_QUICKJS_FILES := $(wildcard third_party/quickjs/*)
THIRD_PARTY_QUICKJS_SRCS = $(filter %.c,$(THIRD_PARTY_QUICKJS_FILES))
THIRD_PARTY_QUICKJS_HDRS = $(filter %.h,$(THIRD_PARTY_QUICKJS_FILES))
THIRD_PARTY_QUICKJS_BINS = $(THIRD_PARTY_QUICKJS_COMS) $(THIRD_PARTY_QUICKJS_COMS:%=%.dbg)
THIRD_PARTY_QUICKJS = $(THIRD_PARTY_QUICKJS_DEPS) $(THIRD_PARTY_QUICKJS_A)
THIRD_PARTY_QUICKJS_A = o/$(MODE)/third_party/quickjs/quickjs.a

THIRD_PARTY_QUICKJS_OBJS =				\
	$(THIRD_PARTY_QUICKJS_SRCS:%.c=o/$(MODE)/%.o)

THIRD_PARTY_QUICKJS_COMS =				\
	o/$(MODE)/third_party/quickjs/qjs.com

THIRD_PARTY_QUICKJS_CHECKS =				\
	$(THIRD_PARTY_QUICKJS_A).pkg			\
	$(THIRD_PARTY_QUICKJS_HDRS:%=o/$(MODE)/%.ok)

THIRD_PARTY_QUICKJS_DIRECTDEPS =			\
	LIBC_ALG					\
	LIBC_CALLS					\
	LIBC_FMT					\
	LIBC_INTRIN					\
	LIBC_LOG					\
	LIBC_MEM					\
	LIBC_NEXGEN32E					\
	LIBC_RUNTIME					\
	LIBC_SOCK					\
	LIBC_STDIO					\
	LIBC_STR					\
	LIBC_SYSV					\
	LIBC_SYSV_CALLS					\
	LIBC_TIME					\
	LIBC_TINYMATH					\
	LIBC_UNICODE					\
	LIBC_X						\
	THIRD_PARTY_COMPILER_RT				\
	THIRD_PARTY_GDTOA				\
	THIRD_PARTY_GETOPT				\
	THIRD_PARTY_MUSL

THIRD_PARTY_QUICKJS_DEPS :=				\
	$(call uniq,$(foreach x,$(THIRD_PARTY_QUICKJS_DIRECTDEPS),$($(x))))

$(THIRD_PARTY_QUICKJS_A):				\
		third_party/quickjs/			\
		$(THIRD_PARTY_QUICKJS_A).pkg		\
		$(filter-out %/quickjs.c,$(THIRD_PARTY_QUICKJS_OBJS))

$(THIRD_PARTY_QUICKJS_A).pkg:				\
		$(THIRD_PARTY_QUICKJS_OBJS)		\
		$(foreach x,$(THIRD_PARTY_QUICKJS_DIRECTDEPS),$($(x)_A).pkg)

o/$(MODE)/third_party/quickjs/qjs.com.dbg:		\
		$(THIRD_PARTY_QUICKJS_DEPS)		\
		$(THIRD_PARTY_QUICKJS_A)		\
		$(THIRD_PARTY_QUICKJS_A).pkg		\
		o/$(MODE)/third_party/quickjs/qjs.o	\
		$(CRT)					\
		$(APE)
	-@$(APELINK)

$(THIRD_PARTY_QUICKJS_OBJS):				\
		OVERRIDE_CPPFLAGS +=			\
			-DCONFIG_BIGNUM			\
			-DCONFIG_VERSION=\"$(shell cat third_party/quickjs/VERSION)\"

o/$(MODE)/third_party/quickjs/unicode_gen.o:		\
		OVERRIDE_CPPFLAGS +=			\
			-DSTACK_FRAME_UNLIMITED

# TODO(jart): Replace alloca() calls with malloc().
o/$(MODE)/third_party/quickjs/libregexp.o		\
o/$(MODE)/third_party/quickjs/quickjs.o:		\
		OVERRIDE_CPPFLAGS +=			\
			-DSTACK_FRAME_UNLIMITED

.PHONY: o/$(MODE)/third_party/quickjs
o/$(MODE)/third_party/quickjs:				\
		$(THIRD_PARTY_QUICKJS_BINS)		\
		$(THIRD_PARTY_QUICKJS_CHECKS)
