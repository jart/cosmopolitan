#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#── vi: set noet ft=make ts=8 sw=8 fenc=utf-8 :vi ────────────────────┘

PKGS += THIRD_PARTY_NETCAT

THIRD_PARTY_NETCAT_BINS = o/$(MODE)/third_party/netcat/nc

THIRD_PARTY_NETCAT_FILES := $(wildcard third_party/netcat/*)
THIRD_PARTY_NETCAT_HDRS = $(filter %.h,$(THIRD_PARTY_NETCAT_FILES))
THIRD_PARTY_NETCAT_SRCS = $(filter %.c,$(THIRD_PARTY_NETCAT_FILES))
THIRD_PARTY_NETCAT_OBJS = $(THIRD_PARTY_NETCAT_SRCS:%.c=o/$(MODE)/%.o)

THIRD_PARTY_NETCAT_DIRECTDEPS =					\
	LIBC_CALLS						\
	LIBC_FMT						\
	LIBC_INTRIN						\
	LIBC_LOG						\
	LIBC_MEM						\
	LIBC_NEXGEN32E						\
	LIBC_RUNTIME						\
	LIBC_SOCK						\
	LIBC_STDIO						\
	LIBC_STR						\
	LIBC_SYSV						\
	LIBC_THREAD						\
	THIRD_PARTY_GETOPT					\
	THIRD_PARTY_MUSL					\

THIRD_PARTY_NETCAT_DEPS :=					\
	$(call uniq,$(foreach x,$(THIRD_PARTY_NETCAT_DIRECTDEPS),$($(x))))

o/$(MODE)/third_party/netcat/nc.pkg:				\
		$(THIRD_PARTY_NETCAT_OBJS)			\
		$(foreach x,$(THIRD_PARTY_NETCAT_DIRECTDEPS),$($(x)_A).pkg)

o/$(MODE)/third_party/netcat/nc.dbg:				\
		$(THIRD_PARTY_NETCAT_DEPS)			\
		o/$(MODE)/third_party/netcat/nc.pkg		\
		$(THIRD_PARTY_NETCAT_OBJS)			\
		$(CRT)						\
		$(APE_NO_MODIFY_SELF)
	@$(APELINK)

.PHONY: o/$(MODE)/third_party/netcat
o/$(MODE)/third_party/netcat: $(THIRD_PARTY_NETCAT_BINS)
