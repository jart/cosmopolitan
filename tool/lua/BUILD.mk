#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#── vi: set noet ft=make ts=8 sw=8 fenc=utf-8 :vi ────────────────────┘

PKGS += TOOL_LUA

TOOL_LUA_FILES := $(wildcard tool/lua/*)
TOOL_LUA_SRCS = $(filter %.c,$(TOOL_LUA_FILES))
TOOL_LUA_HDRS = $(filter %.h,$(TOOL_LUA_FILES))

TOOL_LUA_OBJS =								\
	$(TOOL_LUA_SRCS:%.c=o/$(MODE)/%.o)

TOOL_LUA_BINS =								\
	$(TOOL_LUA_COMS)						\
	$(TOOL_LUA_COMS:%=%.dbg)

TOOL_LUA_COMS =								\
	o/$(MODE)/tool/lua/lua

TOOL_LUA_CHECKS =							\
	$(TOOL_LUA_HDRS:%=o/$(MODE)/%.ok)

################################################################################
# lua standalone with cosmo module

TOOL_LUA_LUA_MODULES =							\
	o/$(MODE)/tool/lua/lcosmo.o					\
	o/$(MODE)/tool/net/lfuncs.o					\
	o/$(MODE)/tool/net/lpath.o					\
	o/$(MODE)/tool/net/lhttp.o					\
	o/$(MODE)/tool/net/lre.o					\
	o/$(MODE)/tool/net/ljson.o					\
	o/$(MODE)/tool/net/lsqlite3.o					\
	o/$(MODE)/tool/net/largon2.o					\
	o/$(MODE)/tool/net/lfetch.o					\
	o/$(MODE)/tool/net/lgetopt.o					\
	o/$(MODE)/tool/net/lzip.o					\
	o/$(MODE)/tool/net/lgoodsocket.o				\
	o/$(MODE)/third_party/lz4cli/lz4.o

TOOL_LUA_DIRECTDEPS =							\
	DSP_SCALE							\
	LIBC_CALLS							\
	LIBC_FMT							\
	LIBC_INTRIN							\
	LIBC_LOG							\
	LIBC_MEM							\
	LIBC_NEXGEN32E							\
	LIBC_PROC							\
	LIBC_RUNTIME							\
	LIBC_SOCK							\
	LIBC_STDIO							\
	LIBC_STR							\
	LIBC_SYSV							\
	LIBC_SYSV_CALLS							\
	LIBC_THREAD							\
	LIBC_TINYMATH							\
	LIBC_X								\
	NET_HTTP							\
	NET_HTTPS							\
	THIRD_PARTY_ARGON2						\
	THIRD_PARTY_COMPILER_RT						\
	THIRD_PARTY_GDTOA						\
	THIRD_PARTY_GETOPT						\
	THIRD_PARTY_LINENOISE						\
	THIRD_PARTY_LUA							\
	THIRD_PARTY_LUA_UNIX						\
	THIRD_PARTY_MBEDTLS						\
	THIRD_PARTY_MUSL						\
	THIRD_PARTY_REGEX						\
	THIRD_PARTY_SQLITE3						\
	THIRD_PARTY_TZ							\
	THIRD_PARTY_ZLIB						\
	TOOL_ARGS

TOOL_LUA_DEPS :=							\
	$(call uniq,$(foreach x,$(TOOL_LUA_DIRECTDEPS),$($(x))))

o/$(MODE)/tool/lua/lua.main.o: third_party/lua/lua.main.c
	@$(COMPILE) -AOBJECTIFY.c $(OBJECTIFY.c) $(OUTPUT_OPTION) -DLUA_COSMO $<

TOOL_LUA_ASSETS =							\
	o/$(MODE)/tool/lua/definitions.lua.zip.o			\
	o/$(MODE)/tool/lua/cosmo/help/init.lua.zip.o			\
	o/$(MODE)/tool/lua/cosmo/skill/init.lua.zip.o			\
	o/$(MODE)/tool/lua/cosmo/http/init.lua.zip.o

# Strip tool/lua/ prefix and prepend .lua/ so files end up at /zip/.lua/
o/$(MODE)/tool/lua/definitions.lua.zip.o: private ZIPOBJ_FLAGS += -C2 -P.lua
o/$(MODE)/tool/lua/cosmo/%.zip.o: private ZIPOBJ_FLAGS += -C2 -P.lua

# Copy base definitions.lua for embedding
tool/lua/definitions.lua: tool/net/definitions.lua
	@cp $< $@

o/$(MODE)/tool/lua/definitions.lua.zip.o: tool/lua/definitions.lua

o/$(MODE)/tool/lua/lua.dbg:						\
		$(TOOL_LUA_DEPS)					\
		$(TOOL_LUA_LUA_MODULES)					\
		o/$(MODE)/tool/lua/lua.main.o				\
		o/$(MODE)/tool/lua/.args.zip.o				\
		$(TOOL_LUA_ASSETS)					\
		$(CRT)							\
		$(APE_NO_MODIFY_SELF)
	@$(APELINK)

$(TOOL_LUA_OBJS): tool/lua/BUILD.mk

o/$(MODE)/tool/lua/test_cosmo.ok: o/$(MODE)/tool/lua/lua.dbg tool/lua/test_cosmo.lua
	$< tool/lua/test_cosmo.lua
	@touch $@

o/$(MODE)/tool/lua/cosmo/help/test.ok: o/$(MODE)/tool/lua/lua.dbg tool/lua/cosmo/help/test.lua
	$< tool/lua/cosmo/help/test.lua
	@touch $@

o/$(MODE)/tool/lua/cosmo/skill/test.ok: o/$(MODE)/tool/lua/lua.dbg tool/lua/cosmo/skill/test.lua
	$< tool/lua/cosmo/skill/test.lua
	@touch $@

o/$(MODE)/tool/lua/test_docs.ok: o/$(MODE)/tool/lua/lua.dbg tool/lua/test_docs.lua
	$< tool/lua/test_docs.lua
	@touch $@

o/$(MODE)/tool/lua/test_strftime.ok: o/$(MODE)/tool/lua/lua.dbg tool/lua/test_strftime.lua
	$< tool/lua/test_strftime.lua
	@touch $@

o/$(MODE)/tool/lua/test_getopt.ok: o/$(MODE)/tool/lua/lua.dbg tool/lua/test_getopt.lua
	$< tool/lua/test_getopt.lua
	@touch $@

o/$(MODE)/tool/lua/test_lz4.ok: o/$(MODE)/tool/lua/lua.dbg tool/lua/test_lz4.lua
	$< tool/lua/test_lz4.lua
	@touch $@

o/$(MODE)/tool/lua/test_zip.ok: o/$(MODE)/tool/lua/lua.dbg tool/lua/test_zip.lua
	$< tool/lua/test_zip.lua
	@touch $@

o/$(MODE)/tool/lua/cosmo/http/test.ok: o/$(MODE)/tool/lua/lua.dbg tool/lua/cosmo/http/test.lua
	$< tool/lua/cosmo/http/test.lua
	@touch $@

o/$(MODE)/tool/lua/cosmo/http/test_server.ok: o/$(MODE)/tool/lua/lua.dbg tool/lua/cosmo/http/test_server.lua
	$< tool/lua/cosmo/http/test_server.lua
	@touch $@

o/$(MODE)/tool/lua/cosmo/http/test_security.ok: o/$(MODE)/tool/lua/lua.dbg tool/lua/cosmo/http/test_security.lua
	$< tool/lua/cosmo/http/test_security.lua
	@touch $@

o/$(MODE)/tool/lua/cosmo/http/test_server_security.ok: o/$(MODE)/tool/lua/lua.dbg tool/lua/cosmo/http/test_server_security.lua
	$< tool/lua/cosmo/http/test_server_security.lua
	@touch $@

o/$(MODE)/tool/lua/test_goodsocket.ok: o/$(MODE)/tool/lua/lua.dbg tool/lua/test_goodsocket.lua
	$< tool/lua/test_goodsocket.lua
	@touch $@

TOOL_LUA_TESTS =							\
	o/$(MODE)/tool/lua/test_cosmo.ok				\
	o/$(MODE)/tool/lua/cosmo/help/test.ok				\
	o/$(MODE)/tool/lua/cosmo/skill/test.ok				\
	o/$(MODE)/tool/lua/test_docs.ok					\
	o/$(MODE)/tool/lua/test_getopt.ok				\
	o/$(MODE)/tool/lua/test_lz4.ok					\
	o/$(MODE)/tool/lua/test_strftime.ok				\
	o/$(MODE)/tool/lua/test_zip.ok					\
	o/$(MODE)/tool/lua/cosmo/http/test.ok				\
	o/$(MODE)/tool/lua/cosmo/http/test_server.ok			\
	o/$(MODE)/tool/lua/cosmo/http/test_security.ok			\
	o/$(MODE)/tool/lua/cosmo/http/test_server_security.ok		\
	o/$(MODE)/tool/lua/test_goodsocket.ok

.PHONY: o/$(MODE)/tool/lua
o/$(MODE)/tool/lua:							\
		$(TOOL_LUA_BINS)					\
		$(TOOL_LUA_CHECKS)

.PHONY: o/$(MODE)/tool/lua/test
o/$(MODE)/tool/lua/test:						\
		$(TOOL_LUA_TESTS)
