#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#───vi: set et ft=make ts=8 tw=8 fenc=utf-8 :vi───────────────────────┘
#
# SYNOPSIS
#
#   Freestanding Hermetically-Sealed Monolithic Repository
#
# REQUIREMENTS
#
#   You can run your programs on any operating system, but you have
#   to build them on Linux 2.6+ (or WSL) using GNU Make. A modern C
#   compiler that's statically-linked comes included as a courtesy.
#
# EXAMPLES
#
#   # build and run everything
#   make -j8 -O
#   make -j8 -O MODE=dbg
#   make -j8 -O MODE=opt
#   make -j8 -O MODE=rel
#   make -j8 -O MODE=tiny
#
#   # build individual target
#   make -j8 -O o//examples/hello.com
#   o//examples/hello.com
#
#   # view source
#   less examples/hello.c
#
#   # view binary
#   o//tool/viz/bing.com o//examples/hello.com |
#     o//tool/viz/fold.com
#
#   # view transitive closure of legalese
#   o//tool/viz/bing.com -n o//examples/hello.com |
#     o//tool/viz/fold.com
#
#   # basic debugging
#   make -j8 -O MODE=dbg o/dbg/examples/crashreport.com
#   o/dbg/examples/crashreport.com
#   less examples/crashreport.c
#
#   # extremely tiny binaries (scout's honor escape hatch)
#   # please pay the $1k for commercial license if you can
#   make -j8 -O MODE=tiny				\
#               LDFLAGS+=-s				\
#               CPPFLAGS+=-DIM_FEELING_NAUGHTY	        \
#               CPPFLAGS+=-DSUPPORT_VECTOR=0b00000001	\
#               o/tiny/examples/hello4.elf
#   ls -hal o/tiny/examples/hello4.elf
#   o/tiny/examples/hello4.elf
#
# TROUBLESHOOTING
#
#   make -j8 -O SILENT=0 o//examples/hello.com
#   make o//examples/life.elf -pn |& less
#   etc.
#
# SEE ALSO
#
#   build/config.mk

SHELL         = /bin/sh
HOSTS        ?= freebsd openbsd alpine
SANITY       := $(shell build/sanitycheck $$PPID)
GNUMAKEFLAGS += --output-sync

.SUFFIXES:
.DELETE_ON_ERROR:
.FEATURES: output-sync
.PHONY: all o bins check test depend tags

all:	o
o:	o/$(MODE)/ape		\
	o/$(MODE)/dsp		\
	o/$(MODE)/net		\
	o/$(MODE)/libc		\
	o/$(MODE)/test		\
	o/$(MODE)/tool		\
	o/$(MODE)/examples	\
	o/$(MODE)/third_party

PKGS =

-include ~/.cosmo.mk				#──No.1
include build/functions.mk			#─┐
include build/definitions.mk			# ├──meta
include build/config.mk				# │
include build/rules.mk				# │
include build/online.mk				# │
include libc/stubs/stubs.mk			#─┘
include libc/nexgen32e/nexgen32e.mk		#─┐
include libc/intrin/intrin.mk			# │
include libc/linux/linux.mk			# │
include libc/math/math.mk			# ├──metal
include libc/tinymath/tinymath.mk		# │
include third_party/compiler_rt/compiler_rt.mk	# │
include libc/bits/bits.mk			# │
include libc/str/str.mk				# │
include third_party/xed/xed.mk			# │
include third_party/zlib/zlib.mk		# │
include libc/elf/elf.mk				# │
include ape/lib/apelib.mk			# │
include ape/ape.mk				#─┘
include libc/sysv/sysv.mk			#─┐
include libc/nt/nt.mk				# ├──system
include libc/fmt/fmt.mk				# │
include libc/rand/rand.mk			#─┘
include libc/calls/calls.mk			#─┐
include libc/runtime/runtime.mk			# ├──systems
include libc/crt/crt.mk				# │
include libc/unicode/unicode.mk			# │
include third_party/dlmalloc/dlmalloc.mk	# │
include libc/mem/mem.mk				# │
include libc/ohmyplus/ohmyplus.mk		# │
include libc/zipos/zipos.mk			# │
include third_party/gdtoa/gdtoa.mk		# │
include libc/time/time.mk			# │
include libc/alg/alg.mk				# │
include libc/calls/hefty/hefty.mk		# │
include libc/stdio/stdio.mk			# │
include third_party/f2c/f2c.mk			# │
include third_party/blas/blas.mk		# │
include net/net.mk				# │
include libc/log/log.mk				# │
include dsp/core/core.mk			# │
include libc/x/x.mk				# │
include third_party/stb/stb.mk			# │
include dsp/scale/scale.mk			# │
include dsp/mpeg/mpeg.mk			# │
include dsp/dsp.mk				# │
include third_party/musl/musl.mk		# │
include third_party/getopt/getopt.mk		# │
include libc/libc.mk				#─┘
include libc/sock/sock.mk			#─┐
include dsp/tty/tty.mk				# ├──online
include libc/dns/dns.mk				# │
include libc/crypto/crypto.mk			# │
include net/http/http.mk			#─┘
include third_party/lemon/lemon.mk
include third_party/duktape/duktape.mk
include third_party/regex/regex.mk
include third_party/third_party.mk
include libc/testlib/testlib.mk
include tool/viz/lib/vizlib.mk
include examples/examples.mk
include third_party/lz4cli/lz4cli.mk
include tool/build/lib/buildlib.mk
include third_party/chibicc/chibicc.mk
include third_party/chibicc/test/test.mk
include tool/build/emucrt/emucrt.mk
include tool/build/emubin/emubin.mk
include tool/build/build.mk
include tool/calc/calc.mk
include tool/decode/lib/decodelib.mk
include tool/decode/decode.mk
include tool/hash/hash.mk
include tool/net/net.mk
include tool/viz/viz.mk
include tool/tool.mk
include test/libc/alg/test.mk
include test/libc/tinymath/test.mk
include test/libc/math/test.mk
include test/libc/intrin/test.mk
include test/libc/mem/test.mk
include test/libc/nexgen32e/test.mk
include test/libc/runtime/test.mk
include test/libc/sock/test.mk
include test/libc/bits/test.mk
include test/libc/crypto/test.mk
include test/libc/str/test.mk
include test/libc/unicode/test.mk
include test/libc/calls/test.mk
include test/libc/x/test.mk
include test/libc/xed/test.mk
include test/libc/fmt/test.mk
include test/libc/dns/test.mk
include test/libc/rand/test.mk
include test/libc/time/test.mk
include test/libc/stdio/test.mk
include test/libc/release/test.mk
include test/libc/test.mk
include test/ape/lib/test.mk
include test/ape/test.mk
include test/net/http/test.mk
include test/net/test.mk
include test/tool/build/lib/test.mk
include test/tool/build/test.mk
include test/tool/viz/lib/test.mk
include test/tool/viz/test.mk
include test/tool/test.mk
include test/dsp/core/test.mk
include test/dsp/scale/test.mk
include test/dsp/tty/test.mk
include test/dsp/test.mk
include examples/package/lib/build.mk
include examples/package/build.mk
#-φ-examples/package/new.sh
include test/test.mk

OBJS	= $(foreach x,$(PKGS),$($(x)_OBJS))
SRCS	= $(foreach x,$(PKGS),$($(x)_SRCS))
HDRS	= $(foreach x,$(PKGS),$($(x)_HDRS))
BINS	= $(foreach x,$(PKGS),$($(x)_BINS))
TESTS	= $(foreach x,$(PKGS),$($(x)_TESTS))
CHECKS	= $(foreach x,$(PKGS),$($(x)_CHECKS))

bins:	$(BINS)
check:	$(CHECKS)
test:	$(TESTS)
depend:	o/$(MODE)/depend
tags:	TAGS HTAGS

o/$(MODE)/.x:
	@$(MKDIR) $(@D) && touch $@

o/$(MODE)/srcs.txt: o/$(MODE)/.x $(MAKEFILES) $(call uniq,$(foreach x,$(SRCS),$(dir $(x))))
	$(file >$@) $(foreach x,$(SRCS),$(file >>$@,$(x)))

o/$(MODE)/hdrs.txt: o/$(MODE)/.x $(MAKEFILES) $(call uniq,$(foreach x,$(HDRS),$(dir $(x))))
	$(file >$@) $(foreach x,$(HDRS),$(file >>$@,$(x)))

o/$(MODE)/depend: o/$(MODE)/.x o/$(MODE)/srcs.txt o/$(MODE)/hdrs.txt $(SRCS) $(HDRS)
	@build/mkdeps -o $@ -r o/$(MODE)/ o/$(MODE)/srcs.txt o/$(MODE)/hdrs.txt

TAGS:	o/$(MODE)/srcs.txt $(SRCS)
	@rm -f $@
	@ACTION=TAGS TARGET=$@ build/do $(TAGS) $(TAGSFLAGS) -L $< -o $@

HTAGS:	o/$(MODE)/hdrs.txt $(HDRS)
	@rm -f $@
	@ACTION=TAGS TARGET=$@ build/do build/htags -L $< -o $@

loc: o/$(MODE)/tool/build/summy.com
	find -name \*.h -or -name \*.c -or -name \*.S | \
	$(XARGS) wc -l | grep total | awk '{print $$1}' | $<

COSMOPOLITAN_OBJECTS =		\
	APE_LIB			\
	LIBC			\
	LIBC_ALG		\
	LIBC_BITS		\
	LIBC_CALLS		\
	LIBC_CALLS_HEFTY	\
	LIBC_CRYPTO		\
	LIBC_DNS		\
	LIBC_ELF		\
	LIBC_FMT		\
	LIBC_INTRIN		\
	LIBC_LOG		\
	LIBC_MEM		\
	LIBC_NEXGEN32E		\
	LIBC_NT			\
	LIBC_OHMYPLUS		\
	LIBC_RAND		\
	LIBC_RUNTIME		\
	LIBC_SOCK		\
	LIBC_STDIO		\
	LIBC_STR		\
	LIBC_STUBS		\
	LIBC_SYSV		\
	LIBC_TIME		\
	LIBC_TINYMATH		\
	LIBC_UNICODE		\
	LIBC_X			\
	LIBC_ZIPOS		\
	THIRD_PARTY_COMPILER_RT	\
	THIRD_PARTY_DLMALLOC	\
	THIRD_PARTY_GDTOA	\
	THIRD_PARTY_GETOPT	\
	THIRD_PARTY_MUSL	\
	THIRD_PARTY_REGEX

COSMOPOLITAN_HEADERS =		\
	LIBC			\
	LIBC_ALG		\
	LIBC_BITS		\
	LIBC_CALLS		\
	LIBC_CRYPTO		\
	LIBC_DNS		\
	LIBC_ELF		\
	LIBC_FMT		\
	LIBC_INTRIN		\
	LIBC_LOG		\
	LIBC_MEM		\
	LIBC_NEXGEN32E		\
	LIBC_NT			\
	LIBC_OHMYPLUS		\
	LIBC_RAND		\
	LIBC_RUNTIME		\
	LIBC_SOCK		\
	LIBC_STDIO		\
	LIBC_STR		\
	LIBC_SYSV		\
	LIBC_TIME		\
	LIBC_TINYMATH		\
	LIBC_UNICODE		\
	LIBC_X			\
	LIBC_ZIPOS		\
	THIRD_PARTY_DLMALLOC	\
	THIRD_PARTY_GDTOA	\
	THIRD_PARTY_GETOPT	\
	THIRD_PARTY_MUSL	\
	THIRD_PARTY_ZLIB	\
	THIRD_PARTY_REGEX

o/$(MODE)/cosmopolitan.a: $(filter-out o/libc/stubs/exit11.o,$(foreach x,$(COSMOPOLITAN_OBJECTS),$($(x)_OBJS)))
o/cosmopolitan.h:				\
		o/$(MODE)/tool/build/rollup.com	\
		libc/integral/normalize.inc	\
		$(foreach x,$(COSMOPOLITAN_HEADERS),$($(x)_HDRS))
	@ACTION=ROLLUP TARGET=$@ build/do $^ >$@

o/cosmopolitan.html:							\
		o/$(MODE)/third_party/chibicc/chibicc.com.dbg		\
		$(filter-out %.s,$(foreach x,$(COSMOPOLITAN_OBJECTS),$($(x)_SRCS)))
	o/$(MODE)/third_party/chibicc/chibicc.com.dbg -J		\
		-fno-common -include libc/integral/normalize.inc -o $@	\
		$(filter-out %.s,$(foreach x,$(COSMOPOLITAN_OBJECTS),$($(x)_SRCS)))

# UNSPECIFIED PREREQUISITES TUTORIAL
#
# A build rule must exist for all files that make needs to consider in
# order to build the requested goal. That includes input source files,
# even if the rule is empty and does nothing. Otherwise, the .DEFAULT
# rule gets triggered.
#
# This is a normal and neecssary behavior when source files get deleted.
# The build reacts automatically to this happening, by simply deleting
# and regenerating the dependency graph; so we can safely use wildcard.
#
# This is abnormal if it needs to keep doing that repeatedly. That can
# only mean the build config is broken.
#
# Also note that a suboptimal in-between state may exist, where running
# `make -pn` reveals rules being generated with the .DEFAULT target, but
# never get executed since they're not members of the transitive closure
# of `make all`. In that case the build config could be improved.
%.mk:
~/.cosmo.mk:
$(SRCS):
$(HDRS):
.DEFAULT:
	@echo >&2
	@echo NOTE: deleting o/$(MODE)/depend because of an unspecified prerequisite: $@ >&2
	@echo >&2
	rm -f o/$(MODE)/depend

-include o/$(MODE)/depend
