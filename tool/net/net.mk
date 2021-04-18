#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#───vi: set et ft=make ts=8 tw=8 fenc=utf-8 :vi───────────────────────┘

PKGS += TOOL_NET

TOOL_NET_FILES := $(wildcard tool/net/*)
TOOL_NET_SRCS = $(filter %.c,$(TOOL_NET_FILES))
TOOL_NET_HDRS = $(filter %.h,$(TOOL_NET_FILES))

TOOL_NET_OBJS =							\
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
	LIBC_DNS						\
	LIBC_FMT						\
	LIBC_INTRIN						\
	LIBC_LOG						\
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
	LIBC_TINYMATH						\
	LIBC_UNICODE						\
	LIBC_X							\
	NET_HTTP						\
	THIRD_PARTY_GETOPT					\
	THIRD_PARTY_LUA						\
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
		o/$(MODE)/tool/net/net.pkg			\
		$(CRT)						\
		$(APE)
	@$(APELINK)

o/$(MODE)/tool/net/redbean.com:					\
		o/$(MODE)/tool/net/redbean.com.dbg		\
		tool/net/favicon.ico				\
		tool/net/redbean.png				\
		tool/net/.init.lua				\
		tool/net/.reload.lua
	@$(COMPILE) -AOBJCOPY -T$@ $(OBJCOPY) -S -O binary $< $@
	@$(COMPILE) -ADD -T$@ dd if=$@ of=o/$(MODE)/tool/net/.ape bs=64 count=11 conv=notrunc 2>/dev/null
	@$(COMPILE) -AZIP -T$@ zip -qj $@ o/$(MODE)/tool/net/.ape tool/net/.init.lua tool/net/.reload.lua tool/net/favicon.ico tool/net/redbean.png

o/$(MODE)/tool/net/redbean-demo.com:				\
		o/$(MODE)/tool/net/redbean.com			\
		tool/net/redbean.mk				\
		tool/net/index.html				\
		tool/net/redbean.css				\
		tool/net/redbean.lua				\
		tool/net/redbean-form.lua			\
		tool/net/redbean-xhr.lua			\
		$(TOOL_NET_HDRS)				\
		$(TOOL_NET_SRCS)
	@$(COMPILE) -ACP -T$@ cp $< $@
	@$(COMPILE) -AZIP -T$@ zip -qj $@ tool/net/redbean.lua tool/net/redbean-form.lua tool/net/redbean-xhr.lua
	@$(COMPILE) -AZIP -T$@ zip -q  $@ tool/net tool/net/index.html tool/net/redbean.css $(TOOL_NET_HDRS) $(TOOL_NET_SRCS)

.PHONY: o/$(MODE)/tool/net
o/$(MODE)/tool/net:						\
		$(TOOL_NET_BINS)				\
		$(TOOL_NET_CHECKS)
