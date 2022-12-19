#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#───vi: set et ft=make ts=8 tw=8 fenc=utf-8 :vi───────────────────────┘

PKGS += THIRD_PARTY_QUICKJS

THIRD_PARTY_QUICKJS_ARTIFACTS += THIRD_PARTY_QUICKJS_A
THIRD_PARTY_QUICKJS_BINS = $(THIRD_PARTY_QUICKJS_COMS) $(THIRD_PARTY_QUICKJS_COMS:%=%.dbg)
THIRD_PARTY_QUICKJS = $(THIRD_PARTY_QUICKJS_A_DEPS) $(THIRD_PARTY_QUICKJS_A)
THIRD_PARTY_QUICKJS_A = o/$(MODE)/third_party/quickjs/quickjs.a
THIRD_PARTY_QUICKJS_HDRS = $(foreach x,$(THIRD_PARTY_QUICKJS_ARTIFACTS),$($(x)_HDRS))
THIRD_PARTY_QUICKJS_INCS = $(foreach x,$(THIRD_PARTY_QUICKJS_ARTIFACTS),$($(x)_INCS))

THIRD_PARTY_QUICKJS_A_SRCS =							\
	third_party/quickjs/array.c						\
	third_party/quickjs/atof.c						\
	third_party/quickjs/atom.c						\
	third_party/quickjs/atomics.c						\
	third_party/quickjs/bigdecimal.c					\
	third_party/quickjs/bigint.c						\
	third_party/quickjs/byte.c						\
	third_party/quickjs/call.c						\
	third_party/quickjs/cutils.c						\
	third_party/quickjs/date.c						\
	third_party/quickjs/dbuf.c						\
	third_party/quickjs/dbuf.c						\
	third_party/quickjs/diglet.c						\
	third_party/quickjs/eq.c						\
	third_party/quickjs/err.c						\
	third_party/quickjs/float.c						\
	third_party/quickjs/gc.c						\
	third_party/quickjs/gen.c						\
	third_party/quickjs/iter.c						\
	third_party/quickjs/json.c						\
	third_party/quickjs/leb128.c						\
	third_party/quickjs/libbf.c						\
	third_party/quickjs/libregexp.c						\
	third_party/quickjs/libunicode.c					\
	third_party/quickjs/map.c						\
	third_party/quickjs/math.c						\
	third_party/quickjs/mem.c						\
	third_party/quickjs/object.c						\
	third_party/quickjs/parse.c						\
	third_party/quickjs/prim.c						\
	third_party/quickjs/promise.c						\
	third_party/quickjs/proxy.c						\
	third_party/quickjs/quickjs-libc.c					\
	third_party/quickjs/quickjs.c						\
	third_party/quickjs/reflect.c						\
	third_party/quickjs/regexp.c						\
	third_party/quickjs/shape.c						\
	third_party/quickjs/str.c						\
	third_party/quickjs/strbuf.c						\
	third_party/quickjs/tok.c						\
	third_party/quickjs/typedarray.c					\
	third_party/quickjs/uri.c						\
	third_party/quickjs/usage.c

THIRD_PARTY_QUICKJS_A_HDRS =							\
	third_party/quickjs/cutils.h						\
	third_party/quickjs/diglet.h						\
	third_party/quickjs/internal.h						\
	third_party/quickjs/leb128.h						\
	third_party/quickjs/libbf.h						\
	third_party/quickjs/libregexp.h						\
	third_party/quickjs/libunicode.h					\
	third_party/quickjs/list.h						\
	third_party/quickjs/quickjs-libc.h					\
	third_party/quickjs/quickjs.h

THIRD_PARTY_QUICKJS_A_INCS =							\
	third_party/quickjs/libregexp-opcode.inc				\
	third_party/quickjs/libunicode-table.inc				\
	third_party/quickjs/quickjs-atom.inc					\
	third_party/quickjs/quickjs-opcode.inc					\
	third_party/quickjs/unicode_gen_def.inc

THIRD_PARTY_QUICKJS_A_OBJS =							\
	$(THIRD_PARTY_QUICKJS_A_SRCS:%.c=o/$(MODE)/%.o)

THIRD_PARTY_QUICKJS_A_DIRECTDEPS =						\
	LIBC_CALLS								\
	LIBC_FMT								\
	LIBC_INTRIN								\
	LIBC_LOG								\
	LIBC_MEM								\
	LIBC_NEXGEN32E								\
	LIBC_NT_KERNEL32							\
	LIBC_RUNTIME								\
	LIBC_SOCK								\
	LIBC_STDIO								\
	LIBC_STR								\
	LIBC_SYSV								\
	LIBC_SYSV_CALLS								\
	LIBC_TIME								\
	LIBC_TINYMATH								\
	LIBC_X									\
	THIRD_PARTY_COMPILER_RT							\
	THIRD_PARTY_GDTOA							\
	THIRD_PARTY_GETOPT							\
	THIRD_PARTY_MUSL							\
	TOOL_ARGS

THIRD_PARTY_QUICKJS_A_DEPS :=							\
	$(call uniq,$(foreach x,$(THIRD_PARTY_QUICKJS_A_DIRECTDEPS),$($(x))))

$(THIRD_PARTY_QUICKJS_A):							\
		third_party/quickjs/						\
		$(THIRD_PARTY_QUICKJS_A).pkg					\
		$(THIRD_PARTY_QUICKJS_A_OBJS)

$(THIRD_PARTY_QUICKJS_A).pkg:							\
		$(THIRD_PARTY_QUICKJS_A_OBJS)					\
		$(foreach x,$(THIRD_PARTY_QUICKJS_A_DIRECTDEPS),$($(x)_A).pkg)

THIRD_PARTY_QUICKJS_SRCS =							\
	third_party/quickjs/qjs.c						\
	third_party/quickjs/qjsc.c						\
	third_party/quickjs/run-test262.c					\
	third_party/quickjs/unicode_gen.c					\
	$(foreach x,$(THIRD_PARTY_QUICKJS_ARTIFACTS),$($(x)_SRCS))

THIRD_PARTY_QUICKJS_OBJS =							\
	o/$(MODE)/third_party/quickjs/qjs.o					\
	o/$(MODE)/third_party/quickjs/qjsc.o					\
	o/$(MODE)/third_party/quickjs/run-test262.o				\
	$(THIRD_PARTY_QUICKJS_A_OBJS)

THIRD_PARTY_QUICKJS_COMS =							\
	o/$(MODE)/third_party/quickjs/qjs.com					\
	o/$(MODE)/third_party/quickjs/qjsc.com					\
	o/$(MODE)/third_party/quickjs/run-test262.com				\
	o/$(MODE)/third_party/quickjs/unicode_gen.com

THIRD_PARTY_QUICKJS_CHECKS =							\
	$(THIRD_PARTY_QUICKJS_A).pkg						\
	$(THIRD_PARTY_QUICKJS_A_HDRS:%=o/$(MODE)/%.ok)

o/$(MODE)/third_party/quickjs/qjscalc.c:					\
		third_party/quickjs/qjscalc.js					\
		o/$(MODE)/third_party/quickjs/qjsc.com
	@$(COMPILE) -wAQJSC o/$(MODE)/third_party/quickjs/qjsc.com -fbignum -o $@ -c $<

o/$(MODE)/third_party/quickjs/repl.c:						\
		third_party/quickjs/repl.js					\
		o/$(MODE)/third_party/quickjs/qjsc.com
	@$(COMPILE) -wAQJSC o/$(MODE)/third_party/quickjs/qjsc.com -o $@ -m -c $<

o/$(MODE)/third_party/quickjs/qjs.com.dbg:					\
		$(THIRD_PARTY_QUICKJS)						\
		o/$(MODE)/third_party/quickjs/qjs.o				\
		o/$(MODE)/third_party/quickjs/repl.o				\
		o/$(MODE)/third_party/quickjs/qjscalc.o				\
		$(CRT)								\
		$(APE_NO_MODIFY_SELF)
	@$(APELINK)

o/$(MODE)/third_party/quickjs/qjs.com:						\
		o/$(MODE)/third_party/quickjs/qjs.com.dbg			\
		o/$(MODE)/third_party/zip/zip.com				\
		o/$(MODE)/tool/build/symtab.com
	@$(MAKE_OBJCOPY)
	@$(MAKE_SYMTAB_CREATE)
	@$(MAKE_SYMTAB_ZIP)

o/$(MODE)/third_party/quickjs/qjsc.com.dbg:					\
		$(THIRD_PARTY_QUICKJS)						\
		o/$(MODE)/third_party/quickjs/qjsc.o				\
		$(CRT)								\
		$(APE_NO_MODIFY_SELF)
	@$(APELINK)

# git clone git@github.com:tc39/test262 /opt/test262
# make -j8 MODE=dbg o/dbg/third_party/quickjs/run-test262.com
# o/dbg/third_party/quickjs/run-test262.com -m -c third_party/quickjs/test262.conf -a
o/$(MODE)/third_party/quickjs/run-test262.com.dbg:				\
		$(THIRD_PARTY_QUICKJS_A_DEPS)					\
		$(THIRD_PARTY_QUICKJS_A)					\
		$(THIRD_PARTY_QUICKJS_A).pkg					\
		o/$(MODE)/third_party/quickjs/run-test262.o			\
		$(CRT)								\
		$(APE_NO_MODIFY_SELF)
	@$(APELINK)

o/$(MODE)/third_party/quickjs/unicode_gen.com.dbg:				\
		$(THIRD_PARTY_QUICKJS_A_DEPS)					\
		$(THIRD_PARTY_QUICKJS_A)					\
		$(THIRD_PARTY_QUICKJS_A).pkg					\
		o/$(MODE)/third_party/quickjs/unicode_gen.o			\
		$(CRT)								\
		$(APE_NO_MODIFY_SELF)
	@$(APELINK)

$(THIRD_PARTY_QUICKJS_OBJS): private						\
		OVERRIDE_CPPFLAGS +=						\
			-DCONFIG_BIGNUM						\
			-DCONFIG_VERSION=\"2021-03-27\"

o/tiny/third_party/quickjs/call.o: private					\
		OVERRIDE_CFLAGS +=						\
			-O2

# TODO(jart): Replace alloca() calls with malloc().
o/$(MODE)/third_party/quickjs/libregexp.o					\
o/$(MODE)/third_party/quickjs/quickjs.o: private				\
		OVERRIDE_CPPFLAGS +=						\
			-DSTACK_FRAME_UNLIMITED

o/$(MODE)/third_party/quickjs/call.o: private QUOTA = -M1024m -C32 -L180
o/$(MODE)/third_party/quickjs/quickjs.o: private QUOTA = -M512m -C32 -L180

.PHONY: o/$(MODE)/third_party/quickjs
o/$(MODE)/third_party/quickjs:							\
		$(THIRD_PARTY_QUICKJS_BINS)					\
		$(THIRD_PARTY_QUICKJS_CHECKS)
