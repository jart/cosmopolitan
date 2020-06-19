#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#───vi: set et ft=make ts=8 tw=8 fenc=utf-8 :vi───────────────────────┘

.PHONY:	o/$(MODE)/third_party
o/$(MODE)/third_party:				\
	o/$(MODE)/third_party/avir		\
	o/$(MODE)/third_party/blas		\
	o/$(MODE)/third_party/bzip2		\
	o/$(MODE)/third_party/compiler_rt	\
	o/$(MODE)/third_party/dlmalloc		\
	o/$(MODE)/third_party/dtoa		\
	o/$(MODE)/third_party/duktape		\
	o/$(MODE)/third_party/editline		\
	o/$(MODE)/third_party/f2c		\
	o/$(MODE)/third_party/getopt		\
	o/$(MODE)/third_party/lex		\
	o/$(MODE)/third_party/linenoise		\
	o/$(MODE)/third_party/lz4cli		\
	o/$(MODE)/third_party/m4		\
	o/$(MODE)/third_party/musl		\
	o/$(MODE)/third_party/regex		\
	o/$(MODE)/third_party/stb		\
	o/$(MODE)/third_party/xed		\
	o/$(MODE)/third_party/zlib
