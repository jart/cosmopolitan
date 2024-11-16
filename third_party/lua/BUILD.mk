#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#── vi: set noet ft=make ts=8 sw=8 fenc=utf-8 :vi ────────────────────┘

PKGS += THIRD_PARTY_LUA

THIRD_PARTY_LUA_COMS =						\
	o/$(MODE)/third_party/lua/lua				\
	o/$(MODE)/third_party/lua/luac

THIRD_PARTY_LUA_BINS =						\
	$(THIRD_PARTY_LUA_COMS)					\
	$(THIRD_PARTY_LUA_COMS:%=%.dbg)

THIRD_PARTY_LUA_CHECKS =					\
	$(THIRD_PARTY_LUA_A).pkg				\
	$(THIRD_PARTY_LUA_UNIX).pkg				\
	$(THIRD_PARTY_LUA_HDRS:%=o/$(MODE)/%.ok)		\
	o/$(MODE)/third_party/lua/lua.pkg			\
	o/$(MODE)/third_party/lua/luac.pkg

################################################################################
# lua.a

THIRD_PARTY_LUA =						\
	$(THIRD_PARTY_LUA_A_DEPS)				\
	$(THIRD_PARTY_LUA_A)

THIRD_PARTY_LUA_ARTIFACTS +=					\
	THIRD_PARTY_LUA_A

THIRD_PARTY_LUA_A =						\
	o/$(MODE)/third_party/lua/lua.a

THIRD_PARTY_LUA_A_HDRS =					\
	third_party/lua/cosmo.h					\
	third_party/lua/lapi.h					\
	third_party/lua/lauxlib.h				\
	third_party/lua/lcode.h					\
	third_party/lua/lctype.h				\
	third_party/lua/ldebug.h				\
	third_party/lua/ldo.h					\
	third_party/lua/lfunc.h					\
	third_party/lua/lgc.h					\
	third_party/lua/llex.h					\
	third_party/lua/llimits.h				\
	third_party/lua/lmem.h					\
	third_party/lua/lobject.h				\
	third_party/lua/lopcodes.h				\
	third_party/lua/lparser.h				\
	third_party/lua/lprefix.h				\
	third_party/lua/lrepl.h					\
	third_party/lua/lstate.h				\
	third_party/lua/lstring.h				\
	third_party/lua/ltable.h				\
	third_party/lua/ltests.h				\
	third_party/lua/ltm.h					\
	third_party/lua/lua.h					\
	third_party/lua/luaconf.h				\
	third_party/lua/lualib.h				\
	third_party/lua/lundump.h				\
	third_party/lua/lvm.h					\
	third_party/lua/lzio.h					\
	third_party/lua/tms.h					\
	third_party/lua/visitor.h

THIRD_PARTY_LUA_A_INCS =					\
	third_party/lua/ljumptab.inc				\
	third_party/lua/lopnames.inc

THIRD_PARTY_LUA_A_SRCS =					\
	third_party/lua/lapi.c					\
	third_party/lua/lauxlib.c				\
	third_party/lua/lbaselib.c				\
	third_party/lua/lcode.c					\
	third_party/lua/lcorolib.c				\
	third_party/lua/ldblib.c				\
	third_party/lua/ldebug.c				\
	third_party/lua/ldo.c					\
	third_party/lua/ldump.c					\
	third_party/lua/lfunc.c					\
	third_party/lua/lgc.c					\
	third_party/lua/linit.c					\
	third_party/lua/liolib.c				\
	third_party/lua/llex.c					\
	third_party/lua/llock.c					\
	third_party/lua/lmathlib.c				\
	third_party/lua/lmem.c					\
	third_party/lua/lnotice.c				\
	third_party/lua/loadlib.c				\
	third_party/lua/lobject.c				\
	third_party/lua/lopcodes.c				\
	third_party/lua/loslib.c				\
	third_party/lua/lparser.c				\
	third_party/lua/lrepl.c					\
	third_party/lua/lstate.c				\
	third_party/lua/lstring.c				\
	third_party/lua/lstrlib.c				\
	third_party/lua/ltable.c				\
	third_party/lua/ltablib.c				\
	third_party/lua/ltests.c				\
	third_party/lua/ltm.c					\
	third_party/lua/luacallwithtrace.c			\
	third_party/lua/luaencodejsondata.c			\
	third_party/lua/luaencodeluadata.c			\
	third_party/lua/luaencodeurl.c				\
	third_party/lua/luaformatstack.c			\
	third_party/lua/luaparseurl.c				\
	third_party/lua/luaprintstack.c				\
	third_party/lua/luapushheader.c				\
	third_party/lua/luapushheaders.c			\
	third_party/lua/luapushlatin1.c				\
	third_party/lua/luapushurlparams.c			\
	third_party/lua/lundump.c				\
	third_party/lua/lutf8lib.c				\
	third_party/lua/lvm.c					\
	third_party/lua/lzio.c					\
	third_party/lua/serialize.c				\
	third_party/lua/visitor.c

THIRD_PARTY_LUA_A_OBJS =					\
	$(THIRD_PARTY_LUA_A_SRCS:%.c=o/$(MODE)/%.o)

THIRD_PARTY_LUA_A_DIRECTDEPS =					\
	LIBC_CALLS						\
	LIBC_FMT						\
	LIBC_INTRIN						\
	LIBC_LOG						\
	LIBC_MEM						\
	LIBC_NEXGEN32E						\
	LIBC_PROC						\
	LIBC_RUNTIME						\
	LIBC_STDIO						\
	LIBC_STR						\
	LIBC_SYSTEM						\
	LIBC_SYSV						\
	LIBC_THREAD						\
	LIBC_TINYMATH						\
	LIBC_X							\
	NET_HTTP						\
	THIRD_PARTY_DOUBLECONVERSION				\
	THIRD_PARTY_GDTOA					\
	THIRD_PARTY_LINENOISE					\
	THIRD_PARTY_MUSL					\
	THIRD_PARTY_TZ						\

THIRD_PARTY_LUA_A_DEPS :=					\
	$(call uniq,$(foreach x,$(THIRD_PARTY_LUA_A_DIRECTDEPS),$($(x))))

$(THIRD_PARTY_LUA_A):						\
		third_party/lua/				\
		$(THIRD_PARTY_LUA_A).pkg			\
		$(THIRD_PARTY_LUA_A_OBJS)

$(THIRD_PARTY_LUA_A).pkg:					\
		$(THIRD_PARTY_LUA_A_OBJS)			\
		$(foreach x,$(THIRD_PARTY_LUA_A_DIRECTDEPS),$($(x)_A).pkg)

o/$(MODE)/third_party/lua/lmathlib.o				\
o//third_party/lua/lgc.o: private				\
		CFLAGS +=					\
			-O2

o/$(MODE)/third_party/lua/lvm.o: private			\
		CFLAGS +=					\
			-fno-gcse

$(THIRD_PARTY_LUA_A_OBJS): private				\
		CFLAGS +=					\
			-ffunction-sections			\
			-fdata-sections

################################################################################
# lunix.a

THIRD_PARTY_LUA_UNIX =						\
	$(THIRD_PARTY_LUA_A_DEPS)				\
	$(THIRD_PARTY_LUA_A)

THIRD_PARTY_LUA_ARTIFACTS +=					\
	THIRD_PARTY_LUA_UNIX

THIRD_PARTY_LUA_UNIX_A =					\
	o/$(MODE)/third_party/lua/lunix.a

THIRD_PARTY_LUA_UNIX_HDRS =					\
	third_party/lua/lunix.h

THIRD_PARTY_LUA_UNIX_SRCS =					\
	third_party/lua/lunix.c

THIRD_PARTY_LUA_UNIX_OBJS =					\
	$(THIRD_PARTY_LUA_UNIX_SRCS:%.c=o/$(MODE)/%.o)

THIRD_PARTY_LUA_UNIX_DIRECTDEPS =				\
	LIBC_CALLS						\
	LIBC_FMT						\
	LIBC_INTRIN						\
	LIBC_LOG						\
	LIBC_MEM						\
	LIBC_NEXGEN32E						\
	LIBC_NT_KERNEL32					\
	LIBC_PROC						\
	LIBC_RUNTIME						\
	LIBC_SOCK						\
	LIBC_STDIO						\
	LIBC_STR						\
	LIBC_SYSV						\
	LIBC_THREAD						\
	LIBC_X							\
	THIRD_PARTY_LUA						\
	THIRD_PARTY_NSYNC					\
	THIRD_PARTY_TZ

THIRD_PARTY_LUA_UNIX_DEPS :=					\
	$(call uniq,$(foreach x,$(THIRD_PARTY_LUA_UNIX_DIRECTDEPS),$($(x))))

$(THIRD_PARTY_LUA_A):						\
		third_party/lua/				\
		$(THIRD_PARTY_LUA_UNIX_A).pkg			\
		$(THIRD_PARTY_LUA_UNIX_OBJS)

$(THIRD_PARTY_LUA_UNIX_A).pkg:					\
		$(THIRD_PARTY_LUA_UNIX_OBJS)			\
		$(foreach x,$(THIRD_PARTY_LUA_UNIX_DIRECTDEPS),$($(x)_A).pkg)

################################################################################
# lua

THIRD_PARTY_LUA_LUA_DIRECTDEPS =				\
	LIBC_CALLS						\
	LIBC_FMT						\
	LIBC_INTRIN						\
	LIBC_NEXGEN32E						\
	LIBC_STDIO						\
	LIBC_LOG						\
	LIBC_MEM						\
	LIBC_STR						\
	LIBC_SYSV						\
	LIBC_THREAD						\
	THIRD_PARTY_LINENOISE					\
	THIRD_PARTY_LUA						\
	THIRD_PARTY_LUA_UNIX					\
	TOOL_ARGS

THIRD_PARTY_LUA_LUA_DEPS :=					\
	$(call uniq,$(foreach x,$(THIRD_PARTY_LUA_LUA_DIRECTDEPS),$($(x))))

o/$(MODE)/third_party/lua/lua.pkg:				\
		o/$(MODE)/third_party/lua/lua.main.o		\
		$(foreach x,$(THIRD_PARTY_LUA_LUA_DIRECTDEPS),$($(x)_A).pkg)

o/$(MODE)/third_party/lua/lua.dbg:				\
		$(THIRD_PARTY_LUA_LUA_DEPS)			\
		o/$(MODE)/third_party/lua/lua.pkg		\
		o/$(MODE)/third_party/lua/lua.main.o		\
		$(CRT)						\
		$(APE_NO_MODIFY_SELF)
	@$(APELINK)

################################################################################
# luac

THIRD_PARTY_LUA_LUAC_DIRECTDEPS =				\
	LIBC_FMT						\
	LIBC_INTRIN						\
	LIBC_NEXGEN32E						\
	LIBC_RUNTIME						\
	LIBC_STDIO						\
	LIBC_MEM						\
	LIBC_STR						\
	LIBC_SYSV						\
	THIRD_PARTY_LUA						\
	TOOL_ARGS

THIRD_PARTY_LUA_LUAC_DEPS :=					\
	$(call uniq,$(foreach x,$(THIRD_PARTY_LUA_LUAC_DIRECTDEPS),$($(x))))

o/$(MODE)/third_party/lua/luac.pkg:				\
		o/$(MODE)/third_party/lua/luac.main.o		\
		$(foreach x,$(THIRD_PARTY_LUA_LUAC_DIRECTDEPS),$($(x)_A).pkg)

o/$(MODE)/third_party/lua/luac.dbg:				\
		$(THIRD_PARTY_LUA_LUAC_DEPS)			\
		o/$(MODE)/third_party/lua/luac.pkg		\
		o/$(MODE)/third_party/lua/luac.main.o		\
		$(CRT)						\
		$(APE_NO_MODIFY_SELF)
	@$(APELINK)

################################################################################

THIRD_PARTY_LUA_SRCS =						\
	$(foreach x,$(THIRD_PARTY_LUA_ARTIFACTS),$($(x)_SRCS))	\
	third_party/lua/lua.main.c				\
	third_party/lua/luac.main.c

THIRD_PARTY_LUA_LIBS = $(foreach x,$(THIRD_PARTY_LUA_ARTIFACTS),$($(x)))
THIRD_PARTY_LUA_HDRS = $(foreach x,$(THIRD_PARTY_LUA_ARTIFACTS),$($(x)_HDRS))
THIRD_PARTY_LUA_INCS = $(foreach x,$(THIRD_PARTY_LUA_ARTIFACTS),$($(x)_INCS))
THIRD_PARTY_LUA_OBJS = $(foreach x,$(THIRD_PARTY_LUA_ARTIFACTS),$($(x)_OBJS))
$(THIRD_PARTY_LUA_OBJS): third_party/lua/BUILD.mk

.PHONY: o/$(MODE)/third_party/lua
o/$(MODE)/third_party/lua:					\
		$(THIRD_PARTY_LUA_LIBS)				\
		$(THIRD_PARTY_LUA_BINS)				\
		$(THIRD_PARTY_LUA_CHECKS)
