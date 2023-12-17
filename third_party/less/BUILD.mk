#-*-mode:lessfile-gless;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#── vi: set noet ft=less ts=8 sw=8 fenc=utf-8 :vi ────────────────────┘

PKGS += THIRD_PARTY_LESS

THIRD_PARTY_LESS_A = o/$(MODE)/third_party/less/less.a
THIRD_PARTY_LESS_FILES := $(wildcard third_party/less/*)
THIRD_PARTY_LESS_HDRS = $(filter %.h,$(THIRD_PARTY_LESS_FILES))
THIRD_PARTY_LESS_INCS = $(filter %.inc,$(THIRD_PARTY_LESS_FILES))
THIRD_PARTY_LESS_SRCS = $(filter %.c,$(THIRD_PARTY_LESS_FILES))
THIRD_PARTY_LESS_OBJS = $(THIRD_PARTY_LESS_SRCS:%.c=o/$(MODE)/%.o)
THIRD_PARTY_LESS_COMS = o/$(MODE)/third_party/less/less.com
THIRD_PARTY_LESS_CHECKS = $(THIRD_PARTY_LESS_A).pkg

THIRD_PARTY_LESS_BINS =						\
	$(THIRD_PARTY_LESS_COMS)				\
	$(THIRD_PARTY_LESS_COMS:%=%.dbg)

THIRD_PARTY_LESS_DIRECTDEPS =					\
	LIBC_CALLS						\
	LIBC_FMT						\
	LIBC_INTRIN						\
	LIBC_MEM						\
	LIBC_NEXGEN32E						\
	LIBC_PROC						\
	LIBC_RUNTIME						\
	LIBC_STDIO						\
	LIBC_STR						\
	LIBC_SYSV						\
	THIRD_PARTY_NCURSES					\
	THIRD_PARTY_PCRE

THIRD_PARTY_LESS_DEPS :=					\
	$(call uniq,$(foreach x,$(THIRD_PARTY_LESS_DIRECTDEPS),$($(x))))

$(THIRD_PARTY_LESS_A).pkg:					\
	$(THIRD_PARTY_LESS_OBJS)				\
	$(foreach x,$(THIRD_PARTY_LESS_DIRECTDEPS),$($(x)_A).pkg)

$(THIRD_PARTY_LESS_A):						\
	third_party/less/					\
	$(THIRD_PARTY_LESS_A).pkg				\
	$(filter-out %main.o,$(THIRD_PARTY_LESS_OBJS))

o/$(MODE)/third_party/less/less.com.dbg:			\
		$(THIRD_PARTY_LESS_DEPS)			\
		$(THIRD_PARTY_LESS_A)				\
		$(THIRD_PARTY_LESS_A).pkg			\
		o/$(MODE)/third_party/less/main.o		\
		$(CRT)						\
		$(APE_NO_MODIFY_SELF)
	@$(APELINK)

o/$(MODE)/third_party/less/lesskey.com.dbg:			\
		$(THIRD_PARTY_LESS_DEPS)			\
		$(THIRD_PARTY_LESS_A)				\
		$(THIRD_PARTY_LESS_A).pkg			\
		o/$(MODE)/third_party/less/lesskey.o		\
		$(CRT)						\
		$(APE_NO_MODIFY_SELF)
	@$(APELINK)

o/$(MODE)/third_party/less/lessecho.com.dbg:			\
		$(THIRD_PARTY_LESS_DEPS)			\
		$(THIRD_PARTY_LESS_A)				\
		$(THIRD_PARTY_LESS_A).pkg			\
		o/$(MODE)/third_party/less/lessecho.o		\
		$(CRT)						\
		$(APE_NO_MODIFY_SELF)
	@$(APELINK)

o/$(MODE)/third_party/less/screen.o: private			\
		CFLAGS +=					\
			-fportcosmo

$(THIRD_PARTY_LESS_OBJS): private				\
		CPPFLAGS +=					\
			-DBINDIR=\"/opt/cosmos/bin\"		\
			-DSYSDIR=\"/opt/cosmos/etc\"

$(THIRD_PARTY_LESS_OBJS): third_party/less/BUILD.mk

.PHONY: o/$(MODE)/third_party/less
o/$(MODE)/third_party/less:					\
		$(THIRD_PARTY_LESS_BINS)			\
		$(THIRD_PARTY_LESS_CHECKS)
