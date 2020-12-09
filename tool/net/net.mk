#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#───vi: set et ft=make ts=8 tw=8 fenc=utf-8 :vi───────────────────────┘

PKGS += TOOL_NET

TOOL_NET_FILES := $(wildcard tool/net/*)
TOOL_NET_SRCS = $(filter %.c,$(TOOL_NET_FILES))
TOOL_NET_HDRS = $(filter %.h,$(TOOL_NET_FILES))

TOOL_NET_OBJS =							\
	$(TOOL_NET_SRCS:%=o/$(MODE)/%.zip.o)			\
	$(TOOL_NET_SRCS:%.c=o/$(MODE)/%.o)

TOOL_NET_COMS =							\
	$(TOOL_NET_SRCS:%.c=o/$(MODE)/%.com)

TOOL_NET_BINS =							\
	$(TOOL_NET_COMS)					\
	$(TOOL_NET_COMS:%=%.dbg)

TOOL_NET_DIRECTDEPS =						\
	LIBC_ALG						\
	LIBC_BITS						\
	LIBC_CALLS						\
	LIBC_CALLS_HEFTY					\
	LIBC_DNS						\
	LIBC_FMT						\
	LIBC_LOG						\
	LIBC_LOG_ASAN						\
	LIBC_MEM						\
	LIBC_NEXGEN32E						\
	LIBC_RAND						\
	LIBC_RUNTIME						\
	LIBC_SOCK						\
	LIBC_STDIO						\
	LIBC_STR						\
	LIBC_STUBS						\
	LIBC_SYSV						\
	LIBC_SYSV_CALLS						\
	LIBC_TIME						\
	LIBC_UNICODE						\
	LIBC_X							\
	NET_HTTP						\
	THIRD_PARTY_GETOPT					\
	THIRD_PARTY_ZLIB					\
	TOOL_DECODE_LIB

TOOL_NET_DEPS :=						\
	$(call uniq,$(foreach x,$(TOOL_NET_DIRECTDEPS),$($(x))))

o/$(MODE)/tool/net/net.pkg:					\
		$(TOOL_NET_OBJS)				\
		$(foreach x,$(TOOL_NET_DIRECTDEPS),$($(x)_A).pkg)

o/$(MODE)/tool/net/%.com.dbg:					\
		$(TOOL_NET_DEPS)				\
		o/$(MODE)/tool/net/%.o				\
		o/$(MODE)/tool/net/net.pkg			\
		$(CRT)						\
		$(APE)
	@$(APELINK)

o/$(MODE)/tool/net/redbean.com.dbg:				\
		$(TOOL_NET_DEPS)				\
		o/$(MODE)/tool/net/redbean.o			\
		o/$(MODE)/tool/net/redbean.ico.zip.o		\
		o/$(MODE)/tool/net/redbean.png.zip.o		\
		o/$(MODE)/tool/net/redbean.css.zip.o		\
		o/$(MODE)/tool/net/redbean.html.zip.o		\
		o/$(MODE)/tool/net/net.pkg			\
		$(CRT)						\
		$(APE)
	@$(APELINK)

o/$(MODE)/tool/net/greenbean.com.dbg:				\
		$(TOOL_NET_DEPS)				\
		o/$(MODE)/tool/net/greenbean.o			\
		o/$(MODE)/tool/net/redbean.ico.zip.o		\
		o/$(MODE)/tool/net/redbean.png.zip.o		\
		o/$(MODE)/tool/net/redbean.css.zip.o		\
		o/$(MODE)/tool/net/redbean.html.zip.o		\
		o/$(MODE)/tool/net/net.pkg			\
		$(CRT)						\
		$(APE)
	@$(APELINK)

# ifeq (,$(MODE))
# $(TOOL_NET_OBJS):						\
# 		OVERRIDE_CFLAGS +=				\
# 			-fsanitize=address
# endif

.PHONY: o/$(MODE)/tool/net
o/$(MODE)/tool/net:						\
		$(TOOL_NET_BINS)				\
		$(TOOL_NET_CHECKS)
