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
#   o/examples/crashreport.com
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

SHELL      = build/bootstrap/cocmd.com
MAKEFLAGS += --no-builtin-rules

.SUFFIXES:
.DELETE_ON_ERROR:
.FEATURES: output-sync
.PHONY: all o bins check test depend tags aarch64 clean

ifneq ($(m),)
ifeq ($(MODE),)
MODE := $(m)
endif
endif

all:	o
o:	o/$(MODE)

o/$(MODE):			\
	o/$(MODE)/ape		\
	o/$(MODE)/dsp		\
	o/$(MODE)/net		\
	o/$(MODE)/libc		\
	o/$(MODE)/test		\
	o/$(MODE)/tool		\
	o/$(MODE)/examples	\
	o/$(MODE)/third_party

ifneq ($(LANDLOCKMAKE_VERSION),)
ifeq ($(wildcard /usr/bin/ape),)
$(warning please run ape/apeinstall.sh if you intend to use landlock make)
$(shell sleep .5)
endif
ifeq ($(USE_SYSTEM_TOOLCHAIN),)
.STRICT = 1
endif
endif

.PLEDGE = stdio rpath wpath cpath fattr proc
.UNVEIL =					\
	libc/integral				\
	libc/stdbool.h				\
	libc/disclaimer.inc			\
	rwc:/dev/shm				\
	rx:build/bootstrap			\
	rx:o/third_party/gcc			\
	r:build/portcosmo.h			\
	/proc/stat				\
	rw:/dev/null				\
	rw:/dev/full				\
	w:o/stack.log				\
	/etc/hosts				\
	~/.runit.psk				\
	/proc/self/status			\
	rx:/usr/bin/qemu-aarch64		\
	rx:o/third_party/qemu/qemu-aarch64	\
	/sys/devices/system/cpu/cpu0/cpufreq/scaling_governor

PKGS =

-include ~/.cosmo.mk
include build/functions.mk			#─┐
include build/definitions.mk			# ├──META
include build/config.mk				# │  You can build
include build/rules.mk				# │  You can topologically order
include build/online.mk				#─┘
include libc/nexgen32e/nexgen32e.mk		#─┐
include libc/sysv/sysv.mk			# ├──SYSTEM SUPPORT
include libc/nt/nt.mk				# │  You can do math
include libc/intrin/intrin.mk			# │  You can use the stack
include third_party/compiler_rt/compiler_rt.mk	# │  You can manipulate arrays
include libc/tinymath/tinymath.mk		# │  You can issue raw system calls
include libc/str/str.mk				# │
include third_party/xed/xed.mk			# │
include third_party/puff/puff.mk		# │
include libc/elf/elf.mk				# │
include ape/ape.mk				# │
include libc/fmt/fmt.mk				# │
include libc/vga/vga.mk				#─┘
include libc/calls/calls.mk			#─┐
include third_party/nsync/nsync.mk		# │
include libc/runtime/runtime.mk			# ├──SYSTEMS RUNTIME
include third_party/double-conversion/dc.mk	# │  You can issue system calls
include libc/crt/crt.mk				# │
include third_party/dlmalloc/dlmalloc.mk	#─┘
include libc/mem/mem.mk				#─┐
include third_party/gdtoa/gdtoa.mk		# ├──DYNAMIC RUNTIME
include third_party/nsync/mem/mem.mk		# │  You can now use stdio
include libc/proc/proc.mk			# │  You can now use threads
include libc/thread/thread.mk			# │  You can now use processes
include tool/hello/hello.mk			# │  You can finally call malloc()
include third_party/zlib/zlib.mk		# │
include libc/stdio/stdio.mk			# │
include libc/time/time.mk			# │
include net/net.mk				# │
include third_party/vqsort/vqsort.mk		# │
include libc/log/log.mk				# │
include third_party/getopt/getopt.mk		# │
include third_party/bzip2/bzip2.mk		# │
include dsp/core/core.mk			# │
include third_party/musl/musl.mk		# │
include libc/x/x.mk				# │
include third_party/stb/stb.mk			# │
include dsp/scale/scale.mk			# │
include dsp/mpeg/mpeg.mk			# │
include dsp/dsp.mk				# │
include third_party/zlib/gz/gz.mk		# │
include third_party/intel/intel.mk		# │
include third_party/aarch64/aarch64.mk		# │
include libc/libc.mk				#─┘
include libc/sock/sock.mk			#─┐
include dsp/tty/tty.mk				# ├──ONLINE RUNTIME
include libc/dns/dns.mk				# │  You can communicate with the network
include net/http/http.mk			# │
include third_party/mbedtls/mbedtls.mk		# │
include third_party/libcxx/libcxx.mk		# │
include third_party/ggml/ggml.mk		# │
include third_party/radpajama/radpajama.mk	# │
include net/https/https.mk			# │
include third_party/regex/regex.mk		#─┘
include third_party/tidy/tidy.mk
include third_party/third_party.mk
include third_party/nsync/testing/testing.mk
include libc/testlib/testlib.mk
include tool/viz/lib/vizlib.mk
include tool/args/args.mk
include test/tool/args/test.mk
include third_party/linenoise/linenoise.mk
include third_party/maxmind/maxmind.mk
include net/finger/finger.mk
include third_party/double-conversion/test/test.mk
include third_party/lua/lua.mk
include third_party/zstd/zstd.mk
include third_party/tr/tr.mk
include third_party/sed/sed.mk
include third_party/awk/awk.mk
include third_party/hiredis/hiredis.mk
include third_party/make/make.mk
include third_party/ctags/ctags.mk
include third_party/finger/finger.mk
include third_party/argon2/argon2.mk
include third_party/smallz4/smallz4.mk
include third_party/sqlite3/sqlite3.mk
include third_party/mbedtls/test/test.mk
include third_party/quickjs/quickjs.mk
include third_party/lz4cli/lz4cli.mk
include third_party/zip/zip.mk
include third_party/xxhash/xxhash.mk
include third_party/unzip/unzip.mk
include tool/build/lib/buildlib.mk
include third_party/chibicc/chibicc.mk
include third_party/chibicc/test/test.mk
include third_party/python/python.mk
include tool/build/build.mk
include tool/curl/curl.mk
include third_party/qemu/qemu.mk
include examples/examples.mk
include examples/pyapp/pyapp.mk
include examples/pylife/pylife.mk
include tool/decode/lib/decodelib.mk
include tool/decode/decode.mk
include tool/lambda/lib/lib.mk
include tool/lambda/lambda.mk
include tool/plinko/lib/lib.mk
include tool/plinko/plinko.mk
include test/tool/plinko/test.mk
include tool/net/net.mk
include tool/viz/viz.mk
include tool/tool.mk
include net/turfwar/turfwar.mk
include test/libc/tinymath/test.mk
include test/libc/intrin/test.mk
include test/libc/mem/test.mk
include test/libc/nexgen32e/test.mk
include test/libc/runtime/test.mk
include test/libc/thread/test.mk
include test/libc/sock/test.mk
include test/libc/str/test.mk
include test/libc/log/test.mk
include test/libc/str/test.mk
include test/libc/calls/test.mk
include test/libc/x/test.mk
include test/libc/xed/test.mk
include test/libc/fmt/test.mk
include test/libc/dns/test.mk
include test/libc/time/test.mk
include test/libc/proc/test.mk
include test/libc/stdio/test.mk
include test/libc/release/test.mk
include test/libc/test.mk
include test/net/http/test.mk
include test/net/https/test.mk
include test/net/finger/test.mk
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
test:	$(TESTS)
depend:	o/$(MODE)/depend
tags:	TAGS HTAGS

o/$(MODE)/.x:
	@$(COMPILE) -AMKDIR -tT$@ $(MKDIR) $(@D)

o/$(MODE)/srcs.txt: o/$(MODE)/.x $(MAKEFILES) $(call uniq,$(foreach x,$(SRCS),$(dir $(x)))) $(SRCS)
	$(file >$@,$(SRCS))
o/$(MODE)/hdrs.txt: o/$(MODE)/.x $(MAKEFILES) $(call uniq,$(foreach x,$(HDRS) $(INCS),$(dir $(x)))) $(HDRS) $(INCS)
	$(file >$@,$(HDRS) $(INCS))
o/$(MODE)/incs.txt: o/$(MODE)/.x $(MAKEFILES) $(call uniq,$(foreach x,$(INCS) $(INCS),$(dir $(x)))) $(INCS) $(INCS)
	$(file >$@,$(INCS))
o/$(MODE)/depend: o/$(MODE)/.x o/$(MODE)/srcs.txt o/$(MODE)/hdrs.txt o/$(MODE)/incs.txt $(SRCS) $(HDRS) $(INCS)
	$(COMPILE) -AMKDEPS -L320 $(MKDEPS) -o $@ -r o/$(MODE)/ @o/$(MODE)/srcs.txt @o/$(MODE)/hdrs.txt @o/$(MODE)/incs.txt

o/$(MODE)/srcs-old.txt: o/$(MODE)/.x $(MAKEFILES) $(call uniq,$(foreach x,$(SRCS),$(dir $(x))))
	$(file >$@) $(foreach x,$(SRCS),$(file >>$@,$(x)))
o/$(MODE)/hdrs-old.txt: o/$(MODE)/.x $(MAKEFILES) $(call uniq,$(foreach x,$(HDRS) $(INCS),$(dir $(x))))
	$(file >$@) $(foreach x,$(HDRS) $(INCS),$(file >>$@,$(x)))

TAGS: private .UNSANDBOXED = 1
TAGS:	o/$(MODE)/srcs-old.txt $(SRCS) #o/$(MODE)/third_party/ctags/ctags.com
	@$(RM) $@
	@o/$(MODE)/third_party/ctags/ctags.com $(TAGSFLAGS) -L $< -o $@

HTAGS: private .UNSANDBOXED = 1
HTAGS:	o/$(MODE)/hdrs-old.txt $(filter-out third_party/libcxx/%,$(HDRS)) #o/$(MODE)/third_party/ctags/ctags.com
	@$(RM) $@
	@build/htags o/$(MODE)/third_party/ctags/ctags.com -L $< -o $@

loc: private .UNSANDBOXED = 1
loc: o/$(MODE)/tool/build/summy.com
	find -name \*.h -or -name \*.c -or -name \*.S | \
	$(XARGS) wc -l | grep total | awk '{print $$1}' | $<

# PLEASE: MAINTAIN TOPOLOGICAL ORDER
# FROM HIGHEST LEVEL TO LOWEST LEVEL
COSMOPOLITAN_OBJECTS =			\
	TOOL_ARGS			\
	NET_HTTP			\
	LIBC_DNS			\
	LIBC_SOCK			\
	LIBC_NT_WS2_32			\
	LIBC_NT_IPHLPAPI		\
	LIBC_NT_MSWSOCK			\
	LIBC_X				\
	THIRD_PARTY_GETOPT		\
	LIBC_LOG			\
	LIBC_TIME			\
	THIRD_PARTY_MUSL		\
	THIRD_PARTY_ZLIB_GZ		\
	LIBC_STDIO			\
	THIRD_PARTY_GDTOA		\
	THIRD_PARTY_REGEX		\
	LIBC_THREAD			\
	LIBC_PROC			\
	THIRD_PARTY_NSYNC_MEM		\
	LIBC_MEM			\
	THIRD_PARTY_DLMALLOC		\
	LIBC_RUNTIME			\
	THIRD_PARTY_NSYNC		\
	LIBC_ELF			\
	LIBC_CALLS			\
	LIBC_SYSV_CALLS			\
	LIBC_VGA			\
	LIBC_NT_PSAPI			\
	LIBC_NT_POWRPROF		\
	LIBC_NT_PDH			\
	LIBC_NT_GDI32			\
	LIBC_NT_COMDLG32		\
	LIBC_NT_USER32			\
	LIBC_NT_NTDLL			\
	LIBC_NT_ADVAPI32		\
	LIBC_NT_SYNCHRONIZATION		\
	LIBC_FMT			\
	THIRD_PARTY_ZLIB		\
	THIRD_PARTY_PUFF		\
	THIRD_PARTY_COMPILER_RT		\
	LIBC_TINYMATH			\
	THIRD_PARTY_XED			\
	LIBC_STR			\
	LIBC_SYSV			\
	LIBC_INTRIN			\
	LIBC_NT_KERNEL32		\
	LIBC_NEXGEN32E

COSMOPOLITAN_HEADERS =			\
	APE				\
	LIBC				\
	LIBC_CALLS			\
	LIBC_DNS			\
	LIBC_ELF			\
	LIBC_FMT			\
	LIBC_INTRIN			\
	LIBC_LOG			\
	LIBC_MEM			\
	LIBC_NEXGEN32E			\
	LIBC_NT				\
	LIBC_RUNTIME			\
	LIBC_SOCK			\
	LIBC_STDIO			\
	LIBC_PROC			\
	THIRD_PARTY_NSYNC		\
	THIRD_PARTY_XED			\
	LIBC_STR			\
	LIBC_SYSV			\
	LIBC_THREAD			\
	LIBC_TIME			\
	LIBC_TINYMATH			\
	LIBC_X				\
	LIBC_VGA			\
	NET_HTTP			\
	TOOL_ARGS			\
	THIRD_PARTY_DLMALLOC		\
	THIRD_PARTY_GDTOA		\
	THIRD_PARTY_GETOPT		\
	THIRD_PARTY_MUSL		\
	THIRD_PARTY_REGEX

o/$(MODE)/cosmopolitan.a:		\
		$(foreach x,$(COSMOPOLITAN_OBJECTS),$($(x)_A_OBJS))

o/cosmopolitan.h:							\
		o/$(MODE)/tool/build/rollup.com				\
		libc/integral/normalize.inc				\
		$(foreach x,$(COSMOPOLITAN_HEADERS),$($(x)_HDRS))	\
		$(foreach x,$(COSMOPOLITAN_HEADERS),$($(x)_INCS))
	$(file >$(TMPDIR)/$(subst /,_,$@),libc/integral/normalize.inc $(foreach x,$(COSMOPOLITAN_HEADERS),$($(x)_HDRS)))
	@$(ECHO) '#ifndef __STRICT_ANSI__' >$@
	@$(ECHO) '#define _COSMO_SOURCE' >>$@
	@$(ECHO) '#endif' >>$@
	@$(COMPILE) -AROLLUP -T$@ o/$(MODE)/tool/build/rollup.com @$(TMPDIR)/$(subst /,_,$@) >>$@

o/cosmopolitan.html: private .UNSANDBOXED = 1
o/cosmopolitan.html:							\
		o/$(MODE)/third_party/chibicc/chibicc.com.dbg		\
		$(filter-out %.s,$(foreach x,$(COSMOPOLITAN_OBJECTS),$($(x)_SRCS)))	\
		$(SRCS)							\
		$(HDRS)
	$(file >$(TMPDIR)/$(subst /,_,$@),$(filter-out %.s,$(foreach x,$(COSMOPOLITAN_OBJECTS),$($(x)_SRCS))))
	o/$(MODE)/third_party/chibicc/chibicc.com.dbg -J		\
		-fno-common -include libc/integral/normalize.inc -o $@	\
		-DCOSMO @$(TMPDIR)/$(subst /,_,$@)

$(SRCS):					\
	libc/integral/normalize.inc		\
	libc/integral/c.inc			\
	libc/integral/cxx.inc			\
	libc/integral/cxxtypescompat.inc	\
	libc/integral/lp64arg.inc		\
	libc/integral/lp64.inc

ifeq ($(ARCH), x86_64)
TOOLCHAIN_ARTIFACTS =				\
	o/cosmopolitan.h			\
	o/$(MODE)/ape/ape.lds			\
	o/$(MODE)/libc/crt/crt.o		\
	o/$(MODE)/ape/ape.elf			\
	o/$(MODE)/ape/ape.o			\
	o/$(MODE)/ape/ape-copy-self.o		\
	o/$(MODE)/ape/ape-no-modify-self.o	\
	o/$(MODE)/cosmopolitan.a		\
	o/$(MODE)/third_party/libcxx/libcxx.a	\
	o/$(MODE)/tool/build/march-native.com	\
	o/$(MODE)/tool/build/ar.com		\
	o/$(MODE)/tool/build/mktemper.com	\
	o/$(MODE)/tool/build/fixupobj.com	\
	o/$(MODE)/tool/build/zipcopy.com	\
	o/$(MODE)/tool/build/apelink.com	\
	o/$(MODE)/tool/build/pecheck.com
else
TOOLCHAIN_ARTIFACTS =				\
	o/$(MODE)/ape/ape.elf			\
	o/$(MODE)/ape/aarch64.lds		\
	o/$(MODE)/libc/crt/crt.o		\
	o/$(MODE)/cosmopolitan.a		\
	o/$(MODE)/third_party/libcxx/libcxx.a	\
	o/$(MODE)/tool/build/march-native.com	\
	o/$(MODE)/tool/build/fixupobj.com	\
	o/$(MODE)/tool/build/zipcopy.com
endif

.PHONY: toolchain
toolchain: $(TOOLCHAIN_ARTIFACTS)

.PHONY: clean_toolchain
clean_toolchain:
	$(RM) $(TOOLCHAIN_ARTIFACTS)

aarch64: private .INTERNET = true
aarch64: private .UNSANDBOXED = true
aarch64:
	$(MAKE) m=aarch64

clean:
	$(RM) -r o

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
	@$(ECHO)
	@$(ECHO) NOTE: deleting o/$(MODE)/depend because of an unspecified prerequisite: $@
	@$(ECHO)
	$(RM) o/$(MODE)/depend

-include o/$(MODE)/depend
