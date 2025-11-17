#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#── vi: set noet ft=make ts=8 sw=8 fenc=utf-8 :vi ────────────────────┘

PKGS += THIRD_PARTY_DASH

THIRD_PARTY_DASH_FILES := $(wildcard third_party/dash/*)
THIRD_PARTY_DASH_HDRS = $(filter %.h,$(THIRD_PARTY_DASH_FILES))
THIRD_PARTY_DASH_SRCS = $(filter %.c,$(THIRD_PARTY_DASH_FILES))
THIRD_PARTY_DASH_OBJS = $(THIRD_PARTY_DASH_SRCS:%.c=o/$(MODE)/%.o)
THIRD_PARTY_DASH_BINS = o/$(MODE)/third_party/dash/dash

THIRD_PARTY_DASH_DIRECTDEPS =					\
	LIBC_CALLS						\
	LIBC_FMT						\
	LIBC_INTRIN						\
	LIBC_MEM						\
	LIBC_NEXGEN32E						\
	LIBC_RUNTIME						\
	LIBC_STDIO						\
	LIBC_PROC						\
	LIBC_STR						\
	LIBC_SYSV						\
	THIRD_PARTY_GDTOA					\
	THIRD_PARTY_MUSL					\

THIRD_PARTY_DASH_DEPS :=					\
	$(call uniq,$(foreach x,$(THIRD_PARTY_DASH_DIRECTDEPS),$($(x))))

o/$(MODE)/third_party/dash/dash.pkg:				\
		$(THIRD_PARTY_DASH_OBJS)			\
		$(foreach x,$(THIRD_PARTY_DASH_DIRECTDEPS),$($(x)_A).pkg)

o/$(MODE)/third_party/dash/dash.dbg:				\
		$(THIRD_PARTY_DASH_DEPS)			\
		o/$(MODE)/third_party/dash/dash.pkg		\
		$(THIRD_PARTY_DASH_OBJS)			\
		$(CRT)						\
		$(APE_NO_MODIFY_SELF)
	@$(APELINK)

$(THIRD_PARTY_DASH_OBJS):					\
		private CFLAGS +=				\
			-DHAVE_CONFIG_H				\
			-include third_party/dash/config.h	\
			-DSHELL					\
			-DBSD					\

o/$(MODE)/third_party/dash/miscbltin.o:				\
		private CFLAGS +=				\
			-fportcosmo

$(THIRD_PARTY_DASH_OBJS):					\
		third_party/dash/BUILD.mk			\
		third_party/dash/config.h

.PHONY: o/$(MODE)/third_party/dash
o/$(MODE)/third_party/dash: $(THIRD_PARTY_DASH_BINS)
