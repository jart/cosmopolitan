#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#───vi: set et ft=make ts=8 tw=8 fenc=utf-8 :vi───────────────────────┘

PKGS += THIRD_PARTY_LUA

THIRD_PARTY_LUA_FILES := $(wildcard third_party/lua/*)
THIRD_PARTY_LUA_SRCS = $(filter %.c,$(THIRD_PARTY_LUA_FILES))
THIRD_PARTY_LUA_HDRS = $(filter %.h,$(THIRD_PARTY_LUA_FILES))
THIRD_PARTY_LUA_BINS = $(THIRD_PARTY_LUA_COMS) $(THIRD_PARTY_LUA_COMS:%=%.dbg)
THIRD_PARTY_LUA = $(THIRD_PARTY_LUA_DEPS) $(THIRD_PARTY_LUA_A)
THIRD_PARTY_LUA_A = o/$(MODE)/third_party/lua/lua.a

THIRD_PARTY_LUA_OBJS =					\
	$(THIRD_PARTY_LUA_SRCS:%.c=o/$(MODE)/%.o)

THIRD_PARTY_LUA_COMS =					\
	o/$(MODE)/third_party/lua/lua.com

THIRD_PARTY_LUA_CHECKS =				\
	$(THIRD_PARTY_LUA_A).pkg			\
	$(THIRD_PARTY_LUA_HDRS:%=o/$(MODE)/%.ok)

THIRD_PARTY_LUA_DIRECTDEPS =				\
	LIBC_CALLS					\
	LIBC_FMT					\
	LIBC_INTRIN					\
	LIBC_MEM					\
	LIBC_NEXGEN32E					\
	LIBC_RUNTIME					\
	LIBC_STDIO					\
	LIBC_STR					\
	LIBC_SYSV					\
	LIBC_TIME					\
	LIBC_TINYMATH					\
	LIBC_UNICODE					\
	THIRD_PARTY_GDTOA

THIRD_PARTY_LUA_DEPS :=					\
	$(call uniq,$(foreach x,$(THIRD_PARTY_LUA_DIRECTDEPS),$($(x))))

$(THIRD_PARTY_LUA_A):					\
		third_party/lua/			\
		$(THIRD_PARTY_LUA_A).pkg		\
		$(filter-out %/lua.o,$(THIRD_PARTY_LUA_OBJS))

$(THIRD_PARTY_LUA_A).pkg:				\
		$(THIRD_PARTY_LUA_OBJS)			\
		$(foreach x,$(THIRD_PARTY_LUA_DIRECTDEPS),$($(x)_A).pkg)

o/$(MODE)/third_party/lua/lua.com.dbg:			\
		$(THIRD_PARTY_LUA_DEPS)			\
		$(THIRD_PARTY_LUA_A)			\
		$(THIRD_PARTY_LUA_A).pkg		\
		o/$(MODE)/third_party/lua/lua.o		\
		$(CRT)					\
		$(APE)
	-@$(APELINK)

o/$(MODE)/third_party/lua/lauxlib.o:			\
		OVERRIDE_CFLAGS +=			\
			-DSTACK_FRAME_UNLIMITED

.PHONY: o/$(MODE)/third_party/lua
o/$(MODE)/third_party/lua:				\
		$(THIRD_PARTY_LUA_BINS)			\
		$(THIRD_PARTY_LUA_CHECKS)
