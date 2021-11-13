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
#   # extremely tiny binaries
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
#   make -j8 -O V=1 o//examples/hello.com
#   make o//examples/life.elf -pn |& less
#   etc.
#
# SEE ALSO
#
#   build/config.mk

SHELL   = /bin/sh
HOSTS  ?= freebsd openbsd netbsd rhel7 rhel5 xnu win7 win10
SANITY := $(shell build/sanitycheck $$PPID)

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

-include ~/.cosmo.mk
include build/functions.mk			#─┐
include build/definitions.mk			# ├──META
include build/config.mk				# │  You can build
include build/rules.mk				# │  You can topologically order
include build/online.mk				# │
include libc/stubs/stubs.mk			#─┘
include libc/nexgen32e/nexgen32e.mk		#─┐
include libc/sysv/sysv.mk			# ├──SYSTEM SUPPORT
include libc/nt/nt.mk				# │  You can do math
include libc/intrin/intrin.mk			# │  You can use the stack
include libc/linux/linux.mk			# │  You can manipulate arrays
include libc/tinymath/tinymath.mk		# │  You can issue raw system calls
include third_party/compiler_rt/compiler_rt.mk	# │
include libc/bits/bits.mk			# │
include libc/str/str.mk				# │
include third_party/xed/xed.mk			# │
include third_party/zlib/zlib.mk		# │
include libc/elf/elf.mk				# │
include ape/ape.mk				# │
include libc/fmt/fmt.mk				#─┘
include libc/calls/calls.mk			#─┐
include libc/runtime/runtime.mk			# ├──SYSTEMS RUNTIME
include libc/crt/crt.mk				# │  You can issue system calls
include libc/rand/rand.mk			# │
include libc/unicode/unicode.mk			# │
include third_party/dlmalloc/dlmalloc.mk	#─┘
include libc/mem/mem.mk				#─┐
include libc/ohmyplus/ohmyplus.mk		# ├──DYNAMIC RUNTIME
include libc/zipos/zipos.mk			# │  You can now use stdio
include third_party/gdtoa/gdtoa.mk		# │  You can finally call malloc()
include libc/time/time.mk			# │
include libc/alg/alg.mk				# │
include libc/stdio/stdio.mk			# │
include libc/thread/thread.mk			# │
include net/net.mk				# │
include libc/log/log.mk				# │
include third_party/bzip2/bzip2.mk		# │
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
include dsp/tty/tty.mk				# ├──ONLINE RUNTIME
include libc/dns/dns.mk				# │  You can communicate with the network
include net/http/http.mk			# │
include third_party/mbedtls/mbedtls.mk		# │
include net/https/https.mk			# │
include third_party/regex/regex.mk		#─┘
include third_party/third_party.mk
include libc/testlib/testlib.mk
include tool/viz/lib/vizlib.mk
include third_party/linenoise/linenoise.mk
include third_party/lua/lua.mk
include third_party/make/make.mk
include third_party/argon2/argon2.mk
include third_party/sqlite3/sqlite3.mk
include third_party/mbedtls/test/test.mk
include third_party/quickjs/quickjs.mk
include third_party/lz4cli/lz4cli.mk
include third_party/infozip/infozip.mk
include tool/build/lib/buildlib.mk
include third_party/chibicc/chibicc.mk
include third_party/chibicc/test/test.mk
include third_party/python/python.mk
include tool/build/emucrt/emucrt.mk
include tool/build/emubin/emubin.mk
include tool/build/build.mk
include examples/examples.mk
include examples/pyapp/pyapp.mk
include tool/decode/lib/decodelib.mk
include tool/decode/decode.mk
include tool/hash/hash.mk
include tool/net/net.mk
include tool/viz/viz.mk
include tool/tool.mk
include test/libc/alg/test.mk
include test/libc/tinymath/test.mk
include test/libc/intrin/test.mk
include test/libc/mem/test.mk
include test/libc/nexgen32e/test.mk
include test/libc/runtime/test.mk
include test/libc/sock/test.mk
include test/libc/bits/test.mk
include test/libc/str/test.mk
include test/libc/log/test.mk
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
include test/net/http/test.mk
include test/net/https/test.mk
include test/net/test.mk
include test/tool/build/lib/test.mk
include test/tool/build/test.mk
include test/tool/viz/lib/test.mk
include test/tool/viz/test.mk
include test/tool/net/test.mk
include test/tool/test.mk
include test/dsp/core/test.mk
include test/dsp/scale/test.mk
include test/dsp/tty/test.mk
include test/dsp/test.mk
include examples/package/lib/build.mk
include examples/package/build.mk
#-φ-examples/package/new.sh
include test/test.mk

OBJS	 = $(foreach x,$(PKGS),$($(x)_OBJS))
SRCS	:= $(foreach x,$(PKGS),$($(x)_SRCS))
HDRS	:= $(foreach x,$(PKGS),$($(x)_HDRS))
INCS	 = $(foreach x,$(PKGS),$($(x)_INCS))
BINS	 = $(foreach x,$(PKGS),$($(x)_BINS))
TESTS	 = $(foreach x,$(PKGS),$($(x)_TESTS))
CHECKS	 = $(foreach x,$(PKGS),$($(x)_CHECKS))

bins:	$(BINS)
check:	$(CHECKS)
test:	$(TESTS) all
depend:	o/$(MODE)/depend
tags:	TAGS HTAGS

o/$(MODE)/.x:
	@mkdir -p $(@D) && touch $@

o/$(MODE)/srcs.txt: o/$(MODE)/.x $(MAKEFILES) $(call uniq,$(foreach x,$(SRCS),$(dir $(x))))
	$(file >$@,$(SRCS))
o/$(MODE)/hdrs.txt: o/$(MODE)/.x $(MAKEFILES) $(call uniq,$(foreach x,$(HDRS) $(INCS),$(dir $(x))))
	$(file >$@,$(HDRS) $(INCS))
o/$(MODE)/incs.txt: o/$(MODE)/.x $(MAKEFILES) $(call uniq,$(foreach x,$(INCS) $(INCS),$(dir $(x))))
	$(file >$@,$(INCS))
o/$(MODE)/depend: o/$(MODE)/.x o/$(MODE)/srcs.txt o/$(MODE)/hdrs.txt o/$(MODE)/incs.txt $(SRCS) $(HDRS) $(INCS)
	@$(COMPILE) -AMKDEPS -L320 $(MKDEPS) -o $@ -r o/$(MODE)/ @o/$(MODE)/srcs.txt @o/$(MODE)/hdrs.txt @o/$(MODE)/incs.txt

o/$(MODE)/srcs-old.txt: o/$(MODE)/.x $(MAKEFILES) $(call uniq,$(foreach x,$(SRCS),$(dir $(x))))
	$(file >$@) $(foreach x,$(SRCS),$(file >>$@,$(x)))
o/$(MODE)/hdrs-old.txt: o/$(MODE)/.x $(MAKEFILES) $(call uniq,$(foreach x,$(HDRS) $(INCS),$(dir $(x))))
	$(file >$@) $(foreach x,$(HDRS) $(INCS),$(file >>$@,$(x)))

TAGS:	o/$(MODE)/srcs-old.txt $(SRCS)
	@rm -f $@
	@$(COMPILE) -ATAGS -T$@ $(TAGS) $(TAGSFLAGS) -L $< -o $@

HTAGS:	o/$(MODE)/hdrs-old.txt $(HDRS)
	@rm -f $@
	@$(COMPILE) -ATAGS -T$@ build/htags -L $< -o $@

loc: o/$(MODE)/tool/build/summy.com
	find -name \*.h -or -name \*.c -or -name \*.S | \
	$(XARGS) wc -l | grep total | awk '{print $$1}' | $<

COSMOPOLITAN_OBJECTS =		\
	LIBC_DNS		\
	LIBC_SOCK		\
	LIBC_NT_WS2_32		\
	LIBC_NT_IPHLPAPI	\
	LIBC_NT_MSWSOCK		\
	LIBC_OHMYPLUS		\
	LIBC_X			\
	THIRD_PARTY_GETOPT	\
	LIBC_LOG		\
	LIBC_UNICODE		\
	LIBC_TIME		\
	LIBC_ZIPOS		\
	THIRD_PARTY_ZLIB	\
	THIRD_PARTY_MUSL	\
	LIBC_STDIO		\
	THIRD_PARTY_GDTOA	\
	THIRD_PARTY_REGEX	\
	LIBC_ALG		\
	LIBC_MEM		\
	THIRD_PARTY_DLMALLOC	\
	LIBC_BITS		\
	LIBC_RUNTIME		\
	LIBC_ELF		\
	LIBC_CALLS		\
	LIBC_RAND		\
	LIBC_SYSV_CALLS		\
	LIBC_NT_KERNELBASE	\
	LIBC_NT_SHELL32		\
	LIBC_NT_GDI32		\
	LIBC_NT_COMDLG32	\
	LIBC_NT_NETAPI32	\
	LIBC_NT_URL		\
	LIBC_NT_USER32		\
	LIBC_NT_NTDLL		\
	LIBC_NT_ADVAPI32	\
	LIBC_FMT		\
	THIRD_PARTY_COMPILER_RT	\
	LIBC_THREAD		\
	LIBC_TINYMATH		\
	LIBC_STR		\
	LIBC_SYSV		\
	LIBC_INTRIN		\
	LIBC_NT_KERNEL32	\
	LIBC_NEXGEN32E

COSMOPOLITAN_HEADERS =		\
	LIBC			\
	LIBC_ALG		\
	LIBC_BITS		\
	LIBC_CALLS		\
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
	LIBC_THREAD		\
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

o/$(MODE)/cosmopolitan.a:	\
		$(foreach x,$(COSMOPOLITAN_OBJECTS),$($(x)_A_OBJS))

o/cosmopolitan.h:				\
		o/$(MODE)/tool/build/rollup.com	\
		libc/integral/normalize.inc	\
		$(foreach x,$(COSMOPOLITAN_HEADERS),$($(x)_HDRS))
	$(file >$@.args,libc/integral/normalize.inc $(foreach x,$(COSMOPOLITAN_HEADERS),$($(x)_HDRS)))
	@$(COMPILE) -AROLLUP -T$@ o/$(MODE)/tool/build/rollup.com @$@.args >$@

o/cosmopolitan.html:							\
		o/$(MODE)/third_party/chibicc/chibicc.com.dbg		\
		$(filter-out %.s,$(foreach x,$(COSMOPOLITAN_OBJECTS),$($(x)_SRCS)))
	o/$(MODE)/third_party/chibicc/chibicc.com.dbg -J		\
		-fno-common -include libc/integral/normalize.inc -o $@	\
		$(filter-out %.s,$(foreach x,$(COSMOPOLITAN_OBJECTS),$($(x)_SRCS)))

$(SRCS):					\
	libc/integral/normalize.inc		\
	libc/integral/c.inc			\
	libc/integral/cxx.inc			\
	libc/integral/cxxtypescompat.inc	\
	libc/integral/lp64arg.inc		\
	libc/integral/lp64.inc

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
$(INCS):
.DEFAULT:
	@echo >&2
	@echo NOTE: deleting o/$(MODE)/depend because of an unspecified prerequisite: $@ >&2
	@echo >&2
	rm -f o/$(MODE)/depend

-include o/$(MODE)/depend
