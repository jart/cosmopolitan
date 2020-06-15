#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#───vi: set et ft=make ts=8 tw=8 fenc=utf-8 :vi───────────────────────┘
# Description:
#   lex generates state machines for tokenizing languages.

PKGS += THIRD_PARTY_LEX

THIRD_PARTY_LEX =					\
	o/$(MODE)/third_party/lex/lex.com

THIRD_PARTY_LEX_BINS =					\
	$(THIRD_PARTY_LEX)				\
	$(THIRD_PARTY_LEX).dbg

THIRD_PARTY_LEX_OBJS =					\
	o/$(MODE)/third_party/lex/buf.o			\
	o/$(MODE)/third_party/lex/ccl.o			\
	o/$(MODE)/third_party/lex/dfa.o			\
	o/$(MODE)/third_party/lex/ecs.o			\
	o/$(MODE)/third_party/lex/filter.o		\
	o/$(MODE)/third_party/lex/gen.o			\
	o/$(MODE)/third_party/lex/main.o		\
	o/$(MODE)/third_party/lex/misc.o		\
	o/$(MODE)/third_party/lex/nfa.o			\
	o/$(MODE)/third_party/lex/options.o		\
	o/$(MODE)/third_party/lex/parse.o		\
	o/$(MODE)/third_party/lex/regex.o		\
	o/$(MODE)/third_party/lex/scan.o		\
	o/$(MODE)/third_party/lex/scanflags.o		\
	o/$(MODE)/third_party/lex/scanopt.o		\
	o/$(MODE)/third_party/lex/skel.o		\
	o/$(MODE)/third_party/lex/sym.o			\
	o/$(MODE)/third_party/lex/tables.o		\
	o/$(MODE)/third_party/lex/tables_shared.o	\
	o/$(MODE)/third_party/lex/tblcmp.o		\
	o/$(MODE)/third_party/lex/yylex.o		\
	o/$(MODE)/third_party/lex/buf.c.zip.o		\
	o/$(MODE)/third_party/lex/ccl.c.zip.o		\
	o/$(MODE)/third_party/lex/dfa.c.zip.o		\
	o/$(MODE)/third_party/lex/ecs.c.zip.o		\
	o/$(MODE)/third_party/lex/filter.c.zip.o	\
	o/$(MODE)/third_party/lex/gen.c.zip.o		\
	o/$(MODE)/third_party/lex/main.c.zip.o		\
	o/$(MODE)/third_party/lex/misc.c.zip.o		\
	o/$(MODE)/third_party/lex/nfa.c.zip.o		\
	o/$(MODE)/third_party/lex/options.c.zip.o	\
	o/$(MODE)/third_party/lex/parse.c.zip.o		\
	o/$(MODE)/third_party/lex/regex.c.zip.o		\
	o/$(MODE)/third_party/lex/scan.c.zip.o		\
	o/$(MODE)/third_party/lex/scanflags.c.zip.o	\
	o/$(MODE)/third_party/lex/scanopt.c.zip.o	\
	o/$(MODE)/third_party/lex/skel.c.zip.o		\
	o/$(MODE)/third_party/lex/sym.c.zip.o		\
	o/$(MODE)/third_party/lex/tables.c.zip.o	\
	o/$(MODE)/third_party/lex/tables_shared.c.zip.o	\
	o/$(MODE)/third_party/lex/tblcmp.c.zip.o	\
	o/$(MODE)/third_party/lex/yylex.c.zip.o

THIRD_PARTY_LEX_DIRECTDEPS =				\
	LIBC_ALG					\
	LIBC_CALLS					\
	LIBC_CALLS_HEFTY				\
	LIBC_CONV					\
	LIBC_FMT					\
	LIBC_LOG					\
	LIBC_MATH					\
	LIBC_MEM					\
	LIBC_NEXGEN32E					\
	LIBC_RUNTIME					\
	LIBC_STDIO					\
	LIBC_STR					\
	LIBC_STUBS					\
	LIBC_SYSV					\
	LIBC_SYSV_CALLS					\
	LIBC_UNICODE					\
	LIBC_X						\
	THIRD_PARTY_REGEX

THIRD_PARTY_LEX_DEPS :=					\
	$(call uniq,$(foreach x,$(THIRD_PARTY_LEX_DIRECTDEPS),$($(x))))

$(THIRD_PARTY_LEX).pkg:					\
		$(THIRD_PARTY_LEX_OBJS)			\
		$(foreach x,$(THIRD_PARTY_LEX_DIRECTDEPS),$($(x)_A).pkg)

$(THIRD_PARTY_LEX_OBJS):				\
	DEFAULT_CPPFLAGS +=				\
		-DHAVE_CONFIG_H				\
		-isystem third_party/lex

$(THIRD_PARTY_LEX_OBJS):				\
	OVERRIDE_CFLAGS +=				\
		$(OLD_CODE)				\
		-O0

o/$(MODE)/third_party/lex/parse.o			\
o/$(MODE)/third_party/lex/misc.o			\
o/$(MODE)/third_party/lex/dfa.o:			\
	DEFAULT_CPPFLAGS +=				\
		-DSTACK_FRAME_UNLIMITED

o/$(MODE)/third_party/lex/scan.o:			\
	DEFAULT_CFLAGS +=				\
		-w

$(THIRD_PARTY_LEX).dbg:					\
		$(THIRD_PARTY_LEX_DEPS)			\
		$(THIRD_PARTY_LEX_OBJS)			\
		$(THIRD_PARTY_LEX).pkg			\
		$(CRT)					\
		$(APE)
	@$(APELINK)

$(THIRD_PARTY_LEX_OBJS):				\
		$(BUILD_FILES)				\
		third_party/lex/lex.mk

.PHONY: o/$(MODE)/third_party/lex
o/$(MODE)/third_party/lex: $(THIRD_PARTY_LEX_BINS)
