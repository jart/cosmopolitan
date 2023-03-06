#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#───vi: set et ft=make ts=8 tw=8 fenc=utf-8 :vi───────────────────────┘

.PHONY:	o/$(MODE)/third_party
o/$(MODE)/third_party:				\
	o/$(MODE)/third_party/argon2		\
	o/$(MODE)/third_party/awk		\
	o/$(MODE)/third_party/bzip2		\
	o/$(MODE)/third_party/chibicc		\
	o/$(MODE)/third_party/compiler_rt	\
	o/$(MODE)/third_party/ctags		\
	o/$(MODE)/third_party/dlmalloc		\
	o/$(MODE)/third_party/finger		\
	o/$(MODE)/third_party/gdtoa		\
	o/$(MODE)/third_party/getopt		\
	o/$(MODE)/third_party/hiredis		\
	o/$(MODE)/third_party/libcxx		\
	o/$(MODE)/third_party/linenoise		\
	o/$(MODE)/third_party/lua		\
	o/$(MODE)/third_party/lz4cli		\
	o/$(MODE)/third_party/make		\
	o/$(MODE)/third_party/maxmind		\
	o/$(MODE)/third_party/mbedtls		\
	o/$(MODE)/third_party/musl		\
	o/$(MODE)/third_party/nsync		\
	o/$(MODE)/third_party/puff		\
	o/$(MODE)/third_party/python		\
	o/$(MODE)/third_party/quickjs		\
	o/$(MODE)/third_party/regex		\
	o/$(MODE)/third_party/sed		\
	o/$(MODE)/third_party/smallz4		\
	o/$(MODE)/third_party/sqlite3		\
	o/$(MODE)/third_party/stb		\
	o/$(MODE)/third_party/tidy		\
	o/$(MODE)/third_party/tr		\
	o/$(MODE)/third_party/unzip		\
	o/$(MODE)/third_party/xed		\
	o/$(MODE)/third_party/zip		\
	o/$(MODE)/third_party/zlib
