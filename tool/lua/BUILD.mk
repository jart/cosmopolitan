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
	o/$(MODE)/tool/net/lre.o					\
	o/$(MODE)/tool/net/ljson.o					\
	o/$(MODE)/tool/net/lsqlite3.o					\
	o/$(MODE)/tool/net/largon2.o					\
	o/$(MODE)/tool/net/lfetch.o

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

o/$(MODE)/tool/lua/lua.dbg:						\
		$(TOOL_LUA_DEPS)					\
		$(TOOL_LUA_LUA_MODULES)					\
		o/$(MODE)/tool/lua/lua.main.o				\
		o/$(MODE)/tool/lua/.args.zip.o				\
		$(CRT)							\
		$(APE_NO_MODIFY_SELF)
	@$(APELINK)

$(TOOL_LUA_OBJS): tool/lua/BUILD.mk

o/$(MODE)/tool/lua/test_cosmo.ok: o/$(MODE)/tool/lua/lua.dbg tool/lua/test_cosmo.lua
	$< tool/lua/test_cosmo.lua
	@touch $@

TOOL_LUA_TESTS =							\
	o/$(MODE)/tool/lua/test_cosmo.ok

.PHONY: o/$(MODE)/tool/lua
o/$(MODE)/tool/lua:							\
		$(TOOL_LUA_BINS)					\
		$(TOOL_LUA_CHECKS)

.PHONY: o/$(MODE)/tool/lua/test
o/$(MODE)/tool/lua/test:						\
		$(TOOL_LUA_TESTS)
