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
	THIRD_PARTY_SQLITE3					\
	THIRD_PARTY_REGEX					\
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
		o/$(MODE)/tool/net/lsqlite3.o			\
		o/$(MODE)/tool/net/net.pkg			\
		$(CRT)						\
		$(APE)
	@$(APELINK)

o/$(MODE)/tool/net/redbean.com:					\
		o/$(MODE)/tool/net/redbean.com.dbg		\
		tool/net/net.mk					\
		tool/net/.help.txt				\
		tool/net/.init.lua				\
		tool/net/favicon.ico				\
		tool/net/redbean.png
	@$(COMPILE) -AOBJCOPY -T$@ $(OBJCOPY) -S -O binary $< $@
	@$(COMPILE) -ADD -T$@ dd if=$@ of=o/$(MODE)/tool/net/.ape bs=64 count=11 conv=notrunc 2>/dev/null
	@$(COMPILE) -AZIP -T$@ zip -qj $@ o/$(MODE)/tool/net/.ape tool/net/.help.txt tool/net/.init.lua tool/net/favicon.ico tool/net/redbean.png

o/$(MODE)/tool/net/redbean-demo.com.dbg:			\
		o/$(MODE)/tool/net/redbean.com.dbg
	@$(COMPILE) -ACP -T$@ cp $< $@

o/$(MODE)/tool/net/redbean-demo.com:				\
		o/$(MODE)/tool/net/redbean-demo.com.dbg		\
		tool/net/net.mk					\
		tool/net/favicon.ico				\
		tool/net/redbean.png				\
		tool/net/.help.txt				\
		tool/net/demo/.init.lua				\
		tool/net/demo/.reload.lua			\
		tool/net/demo/.lua/mymodule.lua			\
		tool/net/demo/sql.lua				\
		tool/net/demo/404.html				\
		tool/net/demo/hello.lua				\
		tool/net/demo/index.html			\
		tool/net/demo/redbean.css			\
		tool/net/demo/redbean.lua			\
		tool/net/demo/redbean-form.lua			\
		tool/net/demo/redbean-xhr.lua			\
		tool/net/demo/seekable.txt			\
		tool/net/demo/virtualbean.html			\
		tool/net/redbean.c				\
		net/http/parsehttprequest.c			\
		net/http/parseurl.c				\
		net/http/encodeurl.c				\
		test/net/http/parsehttprequest_test.c		\
		test/net/http/parseurl_test.c
	@$(COMPILE) -AOBJCOPY -T$@ $(OBJCOPY) -S -O binary $< $@
	@$(COMPILE) -AMKDIR -T$@ mkdir -p o/$(MODE)/tool/net/.redbean-demo
	@$(COMPILE) -ADD -T$@ dd if=$@ of=o/$(MODE)/tool/net/.redbean-demo/.ape bs=64 count=11 conv=notrunc 2>/dev/null
	@$(COMPILE) -AZIP -T$@ zip -qj $@ o/$(MODE)/tool/net/.redbean-demo/.ape tool/net/.help.txt tool/net/demo/.init.lua tool/net/demo/.reload.lua
	@$(COMPILE) -ARM -T$@ rm -rf o/$(MODE)/tool/net/.lua
	@$(COMPILE) -ACP -T$@ cp -R tool/net/demo/.lua o/$(MODE)/tool/net/
	@(cd o/$(MODE)/tool/net && zip -qr redbean-demo.com .lua)
	@$(COMPILE) -AZIP -T$@ zip -qj $@ tool/net/demo/hello.lua tool/net/demo/sql.lua
	@echo "&lt;-- check out this lua server page" | $(COMPILE) -AZIP -T$@ zip -cqj $@ tool/net/demo/redbean.lua
	@$(COMPILE) -AZIP -T$@ zip -qj $@ tool/net/demo/404.html tool/net/favicon.ico tool/net/redbean.png tool/net/demo/redbean-form.lua tool/net/demo/redbean-xhr.lua
	@echo Uncompressed for HTTP Range requests | $(COMPILE) -AZIP -T$@ zip -cqj0 $@ tool/net/demo/seekable.txt
	@$(COMPILE) -AZIP -T$@ zip -q $@ tool/net/ tool/net/demo/ tool/net/demo/index.html tool/net/demo/redbean.css tool/net/redbean.c net/http/parsehttprequest.c net/http/parseurl.c net/http/encodeurl.c test/net/http/parsehttprequest_test.c test/net/http/parseurl_test.c
	@printf "<p>This is a live instance of <a href=https://justine.lol/redbean/>redbean</a>: a tiny multiplatform webserver that <a href=https://news.ycombinator.com/item?id=26271117>went viral</a> on hacker news a few months ago.\r\nSince then, we've added Lua dynamic serving, which also goes as fast as 1,000,000 requests per second on a core i9 (rather than a cheap virtual machine like this). the text you're reading now is a PKZIP End Of Central Directory comment.\r\n<p>redbean aims to be production worthy across six operating systems, using a single executable file (this demo is hosted on FreeBSD 13). redbean has been enhanced to restore the APE header after startup.\r\nIt automatically generates this listing page based on your ZIP contents. If you use redbean as an application server / web development environment,\r\nthen you'll find other new and useful features like function call logging so you can get that sweet sweet microsecond scale latency." | $(COMPILE) -AZIP -T$@ zip -z $@
	@$(COMPILE) -AMKDIR -T$@ mkdir -p o/$(MODE)/tool/net/virtualbean.justine.lol/
	@$(COMPILE) -ACP -T$@ cp tool/net/redbean.png o/$(MODE)/tool/net/virtualbean.justine.lol/redbean.png
	@$(COMPILE) -ACP -T$@ cp tool/net/demo/virtualbean.html o/$(MODE)/tool/net/virtualbean.justine.lol/index.html
	@(cd o/$(MODE)/tool/net && zip -q redbean-demo.com virtualbean.justine.lol/)
	@(cd o/$(MODE)/tool/net && echo 'Go to <a href=http://virtualbean.justine.lol>http://virtualbean.justine.lol</a>' | zip -cq redbean-demo.com virtualbean.justine.lol/index.html)
	@(cd o/$(MODE)/tool/net && zip -q redbean-demo.com virtualbean.justine.lol/redbean.png)

o/$(MODE)/tool/net/redbean-static.com:				\
		o/$(MODE)/tool/net/redbean-static.com.dbg	\
		tool/net/favicon.ico				\
		tool/net/redbean.png
	@$(COMPILE) -AOBJCOPY -T$@ $(OBJCOPY) -S -O binary $< $@
	@$(COMPILE) -AMKDIR -T$@ mkdir -p o/$(MODE)/tool/net/.redbean-static
	@$(COMPILE) -ADD -T$@ dd if=$@ of=o/$(MODE)/tool/net/.redbean-static/.ape bs=64 count=11 conv=notrunc 2>/dev/null
	@$(COMPILE) -AZIP -T$@ zip -qj $@ o/$(MODE)/tool/net/.redbean-static/.ape tool/net/favicon.ico tool/net/redbean.png

o/$(MODE)/tool/net/redbean-static.com.dbg:			\
		$(TOOL_NET_DEPS)				\
		o/$(MODE)/tool/net/redbean-static.o		\
		o/$(MODE)/tool/net/net.pkg			\
		$(CRT)						\
		$(APE)
	@$(APELINK)

o/$(MODE)/tool/net/redbean-static.o: tool/net/redbean.c
	@$(COMPILE) -AOBJECTIFY.c $(OBJECTIFY.c) -DSTATIC $(OUTPUT_OPTION) $<

.PHONY: o/$(MODE)/tool/net
o/$(MODE)/tool/net:						\
		$(TOOL_NET_BINS)				\
		$(TOOL_NET_CHECKS)
