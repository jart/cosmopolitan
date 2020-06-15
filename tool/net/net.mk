#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#───vi: set et ft=make ts=8 tw=8 fenc=utf-8 :vi───────────────────────┘

PKGS += TOOL_NET

TOOL_NET_FILES := $(wildcard tool/net/*)
TOOL_NET_SRCS = $(filter %.c,$(TOOL_NET_FILES))
TOOL_NET_HDRS = $(filter %.h,$(TOOL_NET_FILES))
TOOL_NET_COMS = $(TOOL_NET_OBJS:%.o=%.com)

TOOL_NET_OBJS =					\
	$(TOOL_NET_SRCS:%=o/$(MODE)/%.zip.o)	\
	$(TOOL_NET_SRCS:%.c=o/$(MODE)/%.o)

TOOL_NET_BINS =					\
	$(TOOL_NET_COMS)			\
	$(TOOL_NET_COMS:%=%.dbg)

TOOL_NET_DIRECTDEPS =				\
	LIBC_CALLS				\
	LIBC_DNS				\
	LIBC_FMT				\
	LIBC_LOG				\
	LIBC_NEXGEN32E				\
	LIBC_RUNTIME				\
	LIBC_SOCK				\
	LIBC_STDIO				\
	LIBC_STR				\
	LIBC_STUBS				\
	LIBC_SYSV				\
	LIBC_UNICODE				\
	LIBC_X					\
	THIRD_PARTY_GETOPT			\
	TOOL_DECODE_LIB

TOOL_NET_DEPS :=				\
	$(call uniq,$(foreach x,$(TOOL_NET_DIRECTDEPS),$($(x))))

o/$(MODE)/tool/net/net.pkg:			\
		$(TOOL_NET_OBJS)		\
		$(foreach x,$(TOOL_NET_DIRECTDEPS),$($(x)_A).pkg)

o/$(MODE)/tool/net/%.com.dbg:			\
		$(TOOL_NET_DEPS)		\
		o/$(MODE)/tool/net/%.o		\
		o/$(MODE)/tool/net/net.pkg	\
		$(CRT)				\
		$(APE)
	@$(APELINK)

$(TOOL_NET_OBJS):				\
		$(BUILD_FILES)			\
		tool/net/net.mk

.PHONY: o/$(MODE)/tool/net
o/$(MODE)/tool/net: $(TOOL_NET_BINS) $(TOOL_NET_CHECKS)
