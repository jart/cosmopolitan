#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#── vi: set noet ft=make ts=8 sw=8 fenc=utf-8 :vi ────────────────────┘

PKGS += NET_TURFWAR

NET_TURFWAR_SRCS := $(wildcard net/turfwar/*.c)

NET_TURFWAR_OBJS =					\
	$(NET_TURFWAR_SRCS:%.c=o/$(MODE)/%.o)

NET_TURFWAR_COMS =					\
	$(NET_TURFWAR_SRCS:%.c=o/$(MODE)/%.com)		\
	o/$(MODE)/net/turfwar/turfbean.com

NET_TURFWAR_BINS =					\
	$(NET_TURFWAR_COMS)				\
	$(NET_TURFWAR_COMS:%=%.dbg)

NET_TURFWAR_DIRECTDEPS =				\
	LIBC_CALLS					\
	LIBC_FMT					\
	LIBC_INTRIN					\
	LIBC_LOG					\
	LIBC_MEM					\
	LIBC_NEXGEN32E					\
	LIBC_PROC					\
	LIBC_RUNTIME					\
	LIBC_SOCK					\
	LIBC_STDIO					\
	LIBC_STR					\
	LIBC_SYSV					\
	LIBC_THREAD					\
	LIBC_TIME					\
	LIBC_X						\
	NET_HTTP					\
	THIRD_PARTY_GETOPT				\
	THIRD_PARTY_MUSL				\
	THIRD_PARTY_NSYNC				\
	THIRD_PARTY_NSYNC_MEM				\
	THIRD_PARTY_SQLITE3				\
	THIRD_PARTY_STB					\
	THIRD_PARTY_ZLIB

NET_TURFWAR_DEPS :=					\
	$(call uniq,$(foreach x,$(NET_TURFWAR_DIRECTDEPS),$($(x))))

o/$(MODE)/net/turfwar/turfwar.pkg:			\
		$(NET_TURFWAR_OBJS)			\
		$(foreach x,$(NET_TURFWAR_DIRECTDEPS),$($(x)_A).pkg)

o/$(MODE)/net/turfwar/%.com.dbg:			\
		$(NET_TURFWAR_DEPS)			\
		o/$(MODE)/net/turfwar/%.o		\
		o/$(MODE)/net/turfwar/turfwar.pkg	\
		$(CRT)					\
		$(APE_NO_MODIFY_SELF)
	@$(APELINK)

o/$(MODE)/net/turfwar/turfbean.com.dbg:			\
		$(TOOL_NET_DEPS)			\
		o/$(MODE)/tool/net/redbean.o		\
		$(TOOL_NET_REDBEAN_LUA_MODULES)		\
		o/$(MODE)/tool/net/net.pkg		\
		o/$(MODE)/net/turfwar/.init.lua.zip.o	\
		o/$(MODE)/tool/net/redbean.png.zip.o	\
		o/$(MODE)/tool/net/favicon.ico.zip.o	\
		$(CRT)					\
		$(APE_NO_MODIFY_SELF)
	@$(APELINK)

o/$(MODE)/net/turfwar/turfbean.com:			\
		o/$(MODE)/net/turfwar/turfbean.com.dbg	\
		o/$(MODE)/third_party/zip/zip.com	\
		o/$(MODE)/tool/build/symtab.com
	@$(MAKE_OBJCOPY)
	@$(MAKE_SYMTAB_CREATE)
	@$(MAKE_SYMTAB_ZIP)

o/$(MODE)/net/turfwar/.init.lua.zip.o: private		\
		ZIPOBJ_FLAGS +=				\
			-B

$(NET_TURFWAR_OBJS):					\
		$(BUILD_FILES)				\
		net/turfwar/BUILD.mk

.PHONY: o/$(MODE)/net/turfwar
o/$(MODE)/net/turfwar: $(NET_TURFWAR_BINS) $(NET_TURFWAR_CHECKS)
