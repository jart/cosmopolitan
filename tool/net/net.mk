#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#───vi: set et ft=make ts=8 tw=8 fenc=utf-8 :vi───────────────────────┘

PKGS += TOOL_NET

TOOL_NET_FILES := $(wildcard tool/net/*)
TOOL_NET_SRCS = $(filter %.c,$(TOOL_NET_FILES))
TOOL_NET_HDRS = $(filter %.h,$(TOOL_NET_FILES))

TOOL_NET_OBJS =									\
	$(TOOL_NET_SRCS:%.c=o/$(MODE)/%.o)

TOOL_NET_BINS =									\
	$(TOOL_NET_COMS)							\
	$(TOOL_NET_COMS:%=%.dbg)

TOOL_NET_COMS =									\
	o/$(MODE)/tool/net/dig.com						\
	o/$(MODE)/tool/net/echoserver.com					\
	o/$(MODE)/tool/net/redbean.com						\
	o/$(MODE)/tool/net/redbean-demo.com					\
	o/$(MODE)/tool/net/redbean-static.com					\
	o/$(MODE)/tool/net/redbean-unsecure.com					\
	o/$(MODE)/tool/net/redbean-original.com					\
	o/$(MODE)/tool/net/echoserver.com					\
	o/$(MODE)/tool/net/wb.com

TOOL_NET_DIRECTDEPS =								\
	LIBC_ALG								\
	LIBC_BITS								\
	LIBC_CALLS								\
	LIBC_DNS								\
	LIBC_FMT								\
	LIBC_INTRIN								\
	LIBC_LOG								\
	LIBC_MEM								\
	LIBC_NEXGEN32E								\
	LIBC_NT_IPHLPAPI							\
	LIBC_NT_KERNEL32							\
	LIBC_RAND								\
	LIBC_RUNTIME								\
	LIBC_SOCK								\
	LIBC_STDIO								\
	LIBC_STR								\
	LIBC_STUBS								\
	LIBC_SYSV								\
	LIBC_SYSV_CALLS								\
	LIBC_TIME								\
	LIBC_TINYMATH								\
	LIBC_UNICODE								\
	LIBC_X									\
	LIBC_ZIPOS								\
	NET_HTTP								\
	NET_HTTPS								\
	TOOL_BUILD_LIB								\
	THIRD_PARTY_GDTOA							\
	THIRD_PARTY_GETOPT							\
	THIRD_PARTY_LUA								\
	THIRD_PARTY_MBEDTLS							\
	THIRD_PARTY_REGEX							\
	THIRD_PARTY_SQLITE3							\
	THIRD_PARTY_ZLIB							\
	THIRD_PARTY_ARGON2							\
	TOOL_DECODE_LIB

TOOL_NET_DEPS :=								\
	$(call uniq,$(foreach x,$(TOOL_NET_DIRECTDEPS),$($(x))))

o/$(MODE)/tool/net/net.pkg:							\
		$(TOOL_NET_OBJS)						\
		$(foreach x,$(TOOL_NET_DIRECTDEPS),$($(x)_A).pkg)

o/$(MODE)/tool/net/%.com.dbg:							\
		$(TOOL_NET_DEPS)						\
		o/$(MODE)/tool/net/%.o						\
		o/$(MODE)/tool/net/net.pkg					\
		$(CRT)								\
		$(APE)
	@$(APELINK)

# REDBEAN.COM
#
# The little web server that could!

o/$(MODE)/tool/net/redbean.com.dbg:						\
		$(TOOL_NET_DEPS)						\
		o/$(MODE)/tool/net/redbean.o					\
		o/$(MODE)/tool/net/lsqlite3.o					\
		o/$(MODE)/tool/net/largon2.o					\
		o/$(MODE)/tool/net/net.pkg					\
		$(CRT)								\
		$(APE)
	@$(APELINK)

o/$(MODE)/tool/net/redbean.com:							\
		o/$(MODE)/tool/net/redbean.com.dbg				\
		o/$(MODE)/third_party/infozip/zip.com				\
		tool/net/net.mk							\
		tool/net/help.txt						\
		tool/net/.init.lua						\
		tool/net/favicon.ico						\
		tool/net/redbean.png
	@$(COMPILE) -AOBJCOPY -T$@ $(OBJCOPY) -S -O binary $< $@
	@$(COMPILE) -ADD -T$@ dd if=$@ of=o/$(MODE)/tool/net/.ape bs=64 count=11 conv=notrunc 2>/dev/null
	@$(COMPILE) -AZIP -T$@ o/$(MODE)/third_party/infozip/zip.com -qj $@ o/$(MODE)/tool/net/.ape tool/net/help.txt tool/net/.init.lua tool/net/favicon.ico tool/net/redbean.png

# REDBEAN-DEMO.COM
#
# This redbean-demo.com program is the same as redbean.com except it
# bundles a bunch of example code and there's a live of it available
# online at http://redbean.justine.lol/

o/$(MODE)/tool/net/demo/.init.lua.zip.o						\
o/$(MODE)/tool/net/demo/.reload.lua.zip.o					\
o/$(MODE)/tool/net/demo/sql.lua.zip.o						\
o/$(MODE)/tool/net/demo/fetch.lua.zip.o						\
o/$(MODE)/tool/net/demo/hello.lua.zip.o						\
o/$(MODE)/tool/net/demo/redbean.lua.zip.o					\
o/$(MODE)/tool/net/demo/opensource.lua.zip.o					\
o/$(MODE)/tool/net/demo/closedsource.lua.zip.o					\
o/$(MODE)/tool/net/demo/printpayload.lua.zip.o					\
o/$(MODE)/tool/net/demo/redbean-form.lua.zip.o					\
o/$(MODE)/tool/net/demo/redbean-xhr.lua.zip.o					\
o/$(MODE)/tool/net/redbean.png.zip.o						\
o/$(MODE)/tool/net/favicon.ico.zip.o						\
o/$(MODE)/tool/net/demo/404.html.zip.o:						\
		ZIPOBJ_FLAGS +=							\
			-B

o/$(MODE)/tool/net/demo/.lua/mymodule.lua.zip.o:				\
		ZIPOBJ_FLAGS +=							\
			-C3

o/$(MODE)/tool/net/demo/seekable.txt.zip.o:					\
		ZIPOBJ_FLAGS +=							\
			-B							\
			-0

o/$(MODE)/tool/net/demo/virtualbean.html.zip.o:					\
		ZIPOBJ_FLAGS +=							\
			-Predbean.justine.lol					\
			-B

o/$(MODE)/tool/net/redbean-demo.com.dbg:					\
		$(TOOL_NET_DEPS)						\
		o/$(MODE)/tool/net/redbean.o					\
		o/$(MODE)/tool/net/lsqlite3.o					\
		o/$(MODE)/tool/net/largon2.o					\
		o/$(MODE)/tool/net/net.pkg					\
		o/$(MODE)/tool/net/demo/sql.lua.zip.o				\
		o/$(MODE)/tool/net/demo/fetch.lua.zip.o				\
		o/$(MODE)/tool/net/demo/hello.lua.zip.o				\
		o/$(MODE)/tool/net/demo/redbean.lua.zip.o			\
		o/$(MODE)/tool/net/demo/opensource.lua.zip.o			\
		o/$(MODE)/tool/net/demo/closedsource.lua.zip.o			\
		o/$(MODE)/tool/net/demo/printpayload.lua.zip.o			\
		o/$(MODE)/tool/net/demo/redbean-form.lua.zip.o			\
		o/$(MODE)/tool/net/demo/redbean-xhr.lua.zip.o			\
		o/$(MODE)/tool/net/demo/index.html.zip.o			\
		o/$(MODE)/tool/net/demo/redbean.css.zip.o			\
		o/$(MODE)/tool/net/redbean.png.zip.o				\
		o/$(MODE)/tool/net/favicon.ico.zip.o				\
		o/$(MODE)/tool/net/demo/404.html.zip.o				\
		o/$(MODE)/tool/net/demo/seekable.txt.zip.o			\
		o/$(MODE)/tool/net/demo/virtualbean.html.zip.o			\
		o/$(MODE)/tool/net/demo/.lua/mymodule.lua.zip.o			\
		o/$(MODE)/tool/net/demo/.reload.lua.zip.o			\
		o/$(MODE)/tool/net/demo/.init.lua.zip.o				\
		$(CRT)								\
		$(APE)
	@$(APELINK)

o/$(MODE)/tool/net/redbean-demo.com:						\
		o/$(MODE)/tool/net/redbean-demo.com.dbg
	@$(COMPILE) -AOBJCOPY -T$@ $(OBJCOPY) -S -O binary $< $@
	@$(COMPILE) -AMKDIR -T$@ mkdir -p o/$(MODE)/tool/net/.redbean-demo
	@$(COMPILE) -ADD -T$@ dd if=$@ of=o/$(MODE)/tool/net/.redbean-demo/.ape bs=64 count=11 conv=notrunc 2>/dev/null
	@$(COMPILE) -AZIP -T$@ o/$(MODE)/third_party/infozip/zip.com -qj $@ o/$(MODE)/tool/net/.redbean-demo/.ape

# REDBEAN-STATIC.COM
#
# Passing the -DSTATIC causes Lua and SQLite to be removed. This reduces
# the binary size from roughly 1500 kb to 500 kb. It still supports SSL.

o/$(MODE)/tool/net/redbean-static.com:						\
		o/$(MODE)/tool/net/redbean-static.com.dbg			\
		o/$(MODE)/third_party/infozip/zip.com			\
		tool/net/help.txt						\
		tool/net/favicon.ico						\
		tool/net/redbean.png
	@$(COMPILE) -AOBJCOPY -T$@ $(OBJCOPY) -S -O binary $< $@
	@$(COMPILE) -AMKDIR -T$@ mkdir -p o/$(MODE)/tool/net/.redbean-static
	@$(COMPILE) -ADD -T$@ dd if=$@ of=o/$(MODE)/tool/net/.redbean-static/.ape bs=64 count=11 conv=notrunc 2>/dev/null
	@$(COMPILE) -AZIP -T$@ o/$(MODE)/third_party/infozip/zip.com -qj $@ o/$(MODE)/tool/net/.redbean-static/.ape tool/net/help.txt tool/net/favicon.ico tool/net/redbean.png

o/$(MODE)/tool/net/redbean-static.com.dbg:					\
		$(TOOL_NET_DEPS)						\
		o/$(MODE)/tool/net/redbean-static.o				\
		o/$(MODE)/tool/net/net.pkg					\
		$(CRT)								\
		$(APE)
	@$(APELINK)

o/$(MODE)/tool/net/redbean-static.o: tool/net/redbean.c o/$(MODE)/tool/net/redbean.o
	@$(COMPILE) -AOBJECTIFY.c $(OBJECTIFY.c) -DSTATIC -DREDBEAN=\"redbean-static\" $(OUTPUT_OPTION) $<

# REDBEAN-UNSECURE.COM
#
# Passing the -DUNSECURE will cause the TLS security code to be removed.
# That doesn't mean redbean becomes insecure. It just reduces complexity
# in situations where you'd rather have SSL be handled in an edge proxy.

o/$(MODE)/tool/net/redbean-unsecure.com:					\
		o/$(MODE)/tool/net/redbean-unsecure.com.dbg			\
		o/$(MODE)/third_party/infozip/zip.com			\
		tool/net/help.txt						\
		tool/net/favicon.ico						\
		tool/net/redbean.png
	@$(COMPILE) -AOBJCOPY -T$@ $(OBJCOPY) -S -O binary $< $@
	@$(COMPILE) -AMKDIR -T$@ mkdir -p o/$(MODE)/tool/net/.redbean-unsecure
	@$(COMPILE) -ADD -T$@ dd if=$@ of=o/$(MODE)/tool/net/.redbean-unsecure/.ape bs=64 count=11 conv=notrunc 2>/dev/null
	@$(COMPILE) -AZIP -T$@ o/$(MODE)/third_party/infozip/zip.com -qj $@ o/$(MODE)/tool/net/.redbean-unsecure/.ape tool/net/help.txt tool/net/favicon.ico tool/net/redbean.png

o/$(MODE)/tool/net/redbean-unsecure.com.dbg:					\
		$(TOOL_NET_DEPS)						\
		o/$(MODE)/tool/net/redbean-unsecure.o				\
		o/$(MODE)/tool/net/lsqlite3.o					\
		o/$(MODE)/tool/net/net.pkg					\
		$(CRT)								\
		$(APE)
	@$(APELINK)

o/$(MODE)/tool/net/redbean-unsecure.o: tool/net/redbean.c o/$(MODE)/tool/net/redbean.o
	@$(COMPILE) -AOBJECTIFY.c $(OBJECTIFY.c) -DUNSECURE -DREDBEAN=\"redbean-unsecure\" $(OUTPUT_OPTION) $<

# REDBEAN-ORIGINAL.COM
#
# Passing the -DSTATIC and -DUNSECURE flags together w/ MODE=tiny will
# produce 200kb binary that's very similar to redbean as it existed on
# Hacker News the day it went viral.

o/$(MODE)/tool/net/redbean-original.com:					\
		o/$(MODE)/tool/net/redbean-original.com.dbg			\
		o/$(MODE)/third_party/infozip/zip.com			\
		tool/net/help.txt						\
		tool/net/favicon.ico						\
		tool/net/redbean.png
	@$(COMPILE) -AOBJCOPY -T$@ $(OBJCOPY) -S -O binary $< $@
	@$(COMPILE) -AMKDIR -T$@ mkdir -p o/$(MODE)/tool/net/.redbean-original
	@$(COMPILE) -ADD -T$@ dd if=$@ of=o/$(MODE)/tool/net/.redbean-original/.ape bs=64 count=11 conv=notrunc 2>/dev/null
	@$(COMPILE) -AZIP -T$@ o/$(MODE)/third_party/infozip/zip.com -qj $@ o/$(MODE)/tool/net/.redbean-original/.ape tool/net/help.txt tool/net/favicon.ico tool/net/redbean.png

o/$(MODE)/tool/net/redbean-original.com.dbg:					\
		$(TOOL_NET_DEPS)						\
		o/$(MODE)/tool/net/redbean-original.o				\
		o/$(MODE)/tool/net/lsqlite3.o					\
		o/$(MODE)/tool/net/net.pkg					\
		$(CRT)								\
		$(APE)
	@$(APELINK)

o/$(MODE)/tool/net/redbean-original.o: tool/net/redbean.c o/$(MODE)/tool/net/redbean.o
	@$(COMPILE) -AOBJECTIFY.c $(OBJECTIFY.c) -DSTATIC -DUNSECURE -DREDBEAN=\"redbean-original\" $(OUTPUT_OPTION) $<

.PHONY: o/$(MODE)/tool/net
o/$(MODE)/tool/net:								\
		$(TOOL_NET_BINS)						\
		$(TOOL_NET_CHECKS)
