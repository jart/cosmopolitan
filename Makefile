#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#── vi: set et ft=make ts=8 sw=8 fenc=utf-8 :vi ──────────────────────┘
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
#   make -j8 -O o//examples/hello
#   o//examples/hello
#
#   # view source
#   less examples/hello.c
#
#   # view binary
#   o//tool/viz/bing o//examples/hello |
#     o//tool/viz/fold
#
#   # view transitive closure of legalese
#   o//tool/viz/bing -n o//examples/hello |
#     o//tool/viz/fold
#
#   # basic debugging
#   make -j8 -O MODE=dbg o/dbg/examples/crashreport
#   o/examples/crashreport
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
#   make -j8 -O V=1 o//examples/hello
#   make o//examples/life.elf -pn |& less
#   etc.
#
# SEE ALSO
#
#   build/config.mk

SHELL      = build/bootstrap/cocmd
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

COMMA := ,
PWD := $(shell pwd)

# detect wsl2 running cosmopolitan binaries on the host by checking whether:
# - user ran build/bootstrap/make, in which case make's working directory is in wsl
# - user ran make, in which case cocmd's working directory is in wsl
ifneq ($(findstring //wsl.localhost/,$(CURDIR) $(PWD)),)
$(warning wsl2 interop is enabled)
$(error you need to run sudo sh -c 'echo -1 > /proc/sys/fs/binfmt_misc/WSLInterop')
endif

UNAME_M := $(shell uname -m)
UNAME_S := $(shell uname -s)

# apple still distributes a 17 year old version of gnu make
ifeq ($(MAKE_VERSION), 3.81)
$(error please use build/bootstrap/make)
endif

LC_ALL = C
SOURCE_DATE_EPOCH = 0

ARFLAGS = rcsD
ZFLAGS ?=
XARGS ?= xargs -P4 -rs8000
DOT ?= dot
CLANG = clang
TMPDIR = o/tmp
AR = $(BOOTSTRAP)/ar.ape
CP = $(BOOTSTRAP)/cp.ape
RM = $(BOOTSTRAP)/rm.ape -f
GZIP = $(BOOTSTRAP)/gzip.ape
ECHO = $(BOOTSTRAP)/echo.ape
CHMOD = $(BOOTSTRAP)/chmod.ape
TOUCH = $(BOOTSTRAP)/touch.ape
PKG = $(BOOTSTRAP)/package.ape
MKDEPS = $(BOOTSTRAP)/mkdeps
ZIPOBJ = $(BOOTSTRAP)/zipobj
ZIPCOPY = $(BOOTSTRAP)/zipcopy
PECHECK = $(BOOTSTRAP)/pecheck
FIXUPOBJ = $(BOOTSTRAP)/fixupobj
OBJBINCOPY = $(BOOTSTRAP)/objbincopy
MKDIR = $(BOOTSTRAP)/mkdir.ape -p
COMPILE = $(BOOTSTRAP)/compile.ape -V9 -M2048m -P8192 $(QUOTA)

# the default build modes is empty string
# on x86_64 hosts, MODE= is the same as MODE=x86_64
# on aarch64 hosts, MODE= is changed to MODE=aarch64
ifeq ($(MODE),)
ifeq ($(UNAME_M),arm64)
MODE := aarch64
endif
ifeq ($(UNAME_M),aarch64)
MODE := aarch64
endif
endif

ifneq ($(findstring aarch64,$(MODE)),)
ARCH = aarch64
HOSTS ?= pi pi5 studio freebsdarm
else
ARCH = x86_64
HOSTS ?= freebsd rhel7 xnu openbsd netbsd win10
endif

ZIPOBJ_FLAGS += -a$(ARCH)

export ADDR2LINE
export LC_ALL
export MKDIR
export MODE
export SOURCE_DATE_EPOCH
export TMPDIR

COSMOCC = .cosmocc/3.8.0
BOOTSTRAP = $(COSMOCC)/bin
TOOLCHAIN = $(COSMOCC)/bin/$(ARCH)-linux-cosmo-
DOWNLOAD := $(shell build/download-cosmocc.sh $(COSMOCC) 3.8.0 813c6b2f95062d2e0a845307a79505424cb98cb038e8013334f8a22e3b92a474)

IGNORE := $(shell $(MKDIR) $(TMPDIR))

AS = $(TOOLCHAIN)as
CC = $(TOOLCHAIN)gcc
CXX = $(TOOLCHAIN)g++
CXXFILT = $(TOOLCHAIN)c++filt
LD = $(TOOLCHAIN)ld.bfd
NM = $(TOOLCHAIN)nm
GCC = $(TOOLCHAIN)gcc
STRIP = $(TOOLCHAIN)strip
OBJCOPY = $(TOOLCHAIN)objcopy
OBJDUMP = $(TOOLCHAIN)objdump
ifneq ($(wildcard $(PWD)/$(TOOLCHAIN)addr2line),)
ADDR2LINE = $(PWD)/$(TOOLCHAIN)addr2line
else
ADDR2LINE = $(TOOLCHAIN)addr2line
endif

# primary build rules
all:	o
o:	o/$(MODE)
o/$(MODE):			\
	o/$(MODE)/ape		\
	o/$(MODE)/ctl		\
	o/$(MODE)/dsp		\
	o/$(MODE)/net		\
	o/$(MODE)/libc		\
	o/$(MODE)/test		\
	o/$(MODE)/tool		\
	o/$(MODE)/examples	\
	o/$(MODE)/third_party

# TODO(jart): Make Emacs `C-c C-c` shortcut not need this.
.PHONY: o/$(MODE)/ o/$(MODE)/.
o/$(MODE)/: o/$(MODE)
o/$(MODE)/.: o/$(MODE)

# check if we're using o//third_party/make/make
# we added sandboxing to guarantee cosmo's makefile is hermetic
# it also shaves away 200ms of startup latency with native $(uniq)
ifneq ($(LANDLOCKMAKE_VERSION),)
ifeq ($(UNAME_S),Linux)
ifeq ($(wildcard /usr/bin/ape),)
$(warning please run ape/apeinstall.sh if you intend to use landlock make)
$(shell sleep .5)
endif
endif
ifneq ($(TOOLCHAIN),)
.STRICT = 1
endif
endif

.PLEDGE += stdio rpath wpath cpath fattr proc
.UNVEIL +=					\
	libc/integral				\
	libc/stdbool.h				\
	rwc:/dev/shm				\
	rx:.cosmocc				\
	rx:build/bootstrap			\
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
include libc/nexgen32e/BUILD.mk			#─┐
include libc/sysv/BUILD.mk			# ├──SYSTEM SUPPORT
include libc/nt/BUILD.mk			# │  You can do math
include libc/intrin/BUILD.mk			# │  You can use the stack
include third_party/compiler_rt/BUILD.mk	# │  You can manipulate arrays
include libc/tinymath/BUILD.mk			# │  You can issue raw system calls
include libc/str/BUILD.mk			# │
include third_party/xed/BUILD.mk		# │
include third_party/puff/BUILD.mk		# │
include libc/elf/BUILD.mk			# │
include ape/BUILD.mk				# │
include libc/fmt/BUILD.mk			# │
include libc/vga/BUILD.mk			# │
include libc/irq/BUILD.mk			#─┘
include libc/calls/BUILD.mk			#─┐
include libc/irq/BUILD.mk			# ├──SYSTEMS RUNTIME
include third_party/nsync/BUILD.mk		# │  You can issue system calls
include libc/runtime/BUILD.mk			# │
include libc/crt/BUILD.mk			# │
include third_party/dlmalloc/BUILD.mk		#─┘
include libc/mem/BUILD.mk			#─┐
include third_party/gdtoa/BUILD.mk		# ├──DYNAMIC RUNTIME
include third_party/nsync/mem/BUILD.mk		# │  You can now use stdio
include libc/proc/BUILD.mk			# │  You can now use threads
include libc/dlopen/BUILD.mk			# │  You can now use processes
include libc/thread/BUILD.mk			# │  You can finally call malloc()
include third_party/zlib/BUILD.mk		# │
include libc/stdio/BUILD.mk			# │
include tool/hello/BUILD.mk			# │
include third_party/tz/BUILD.mk			# │
include net/BUILD.mk				# │
include third_party/vqsort/BUILD.mk		# │
include libc/log/BUILD.mk			# │
include third_party/getopt/BUILD.mk		# │
include third_party/bzip2/BUILD.mk		# │
include dsp/core/BUILD.mk			# │
include third_party/zlib/gz/BUILD.mk		# │
include third_party/intel/BUILD.mk		# │
include third_party/aarch64/BUILD.mk		# │
include libc/BUILD.mk				#─┘
include libc/sock/BUILD.mk			#─┐
include net/http/BUILD.mk			# ├──ONLINE RUNTIME
include third_party/musl/BUILD.mk		# │  You can communicate with the network
include libc/x/BUILD.mk				# │
include dsp/scale/BUILD.mk			# │
include dsp/mpeg/BUILD.mk			# │
include dsp/tty/BUILD.mk			# │
include dsp/BUILD.mk				# │
include third_party/stb/BUILD.mk		# │
include third_party/mbedtls/BUILD.mk		# │
include third_party/ncurses/BUILD.mk		# │
include third_party/readline/BUILD.mk		# │
include third_party/libunwind/BUILD.mk		# |
include third_party/libcxxabi/BUILD.mk		# |
include third_party/double-conversion/BUILD.mk	# │
include ctl/BUILD.mk				# │
include third_party/libcxx/BUILD.mk		# │
include third_party/openmp/BUILD.mk		# │
include third_party/pcre/BUILD.mk		# │
include third_party/less/BUILD.mk		# │
include net/https/BUILD.mk			# │
include third_party/regex/BUILD.mk		#─┘
include third_party/tidy/BUILD.mk
include third_party/BUILD.mk
include third_party/nsync/testing/BUILD.mk
include libc/testlib/BUILD.mk
include tool/viz/lib/BUILD.mk
include tool/args/BUILD.mk
include test/math/BUILD.mk
include test/posix/BUILD.mk
include test/ctl/BUILD.mk
include test/libcxx/BUILD.mk
include test/tool/args/BUILD.mk
include third_party/linenoise/BUILD.mk
include third_party/maxmind/BUILD.mk
include net/finger/BUILD.mk
include third_party/double-conversion/test/BUILD.mk
include third_party/lua/BUILD.mk
include third_party/tree/BUILD.mk
include third_party/zstd/BUILD.mk
include third_party/tr/BUILD.mk
include third_party/sed/BUILD.mk
include third_party/awk/BUILD.mk
include third_party/hiredis/BUILD.mk
include third_party/make/BUILD.mk
include third_party/ctags/BUILD.mk
include third_party/finger/BUILD.mk
include third_party/argon2/BUILD.mk
include third_party/smallz4/BUILD.mk
include third_party/sqlite3/BUILD.mk
include third_party/mbedtls/test/BUILD.mk
include third_party/lz4cli/BUILD.mk
include third_party/zip/BUILD.mk
include third_party/xxhash/BUILD.mk
include third_party/unzip/BUILD.mk
include tool/build/lib/BUILD.mk
include third_party/chibicc/BUILD.mk
include third_party/chibicc/test/BUILD.mk
include third_party/python/BUILD.mk
include tool/build/BUILD.mk
include tool/curl/BUILD.mk
include third_party/qemu/BUILD.mk
include third_party/libcxxabi/test/BUILD.mk
include examples/BUILD.mk
include examples/pyapp/BUILD.mk
include examples/pylife/BUILD.mk
include tool/decode/lib/BUILD.mk
include tool/decode/BUILD.mk
include tool/lambda/lib/BUILD.mk
include tool/lambda/BUILD.mk
include tool/plinko/lib/BUILD.mk
include tool/plinko/BUILD.mk
include test/tool/plinko/BUILD.mk
include tool/net/BUILD.mk
include tool/viz/BUILD.mk
include tool/BUILD.mk
include net/turfwar/BUILD.mk
include test/libc/tinymath/BUILD.mk
include test/libc/intrin/BUILD.mk
include test/libc/mem/BUILD.mk
include test/libc/nexgen32e/BUILD.mk
include test/libc/runtime/BUILD.mk
include test/libc/thread/BUILD.mk
include test/libc/sock/BUILD.mk
include test/libc/str/BUILD.mk
include test/libc/log/BUILD.mk
include test/libc/str/BUILD.mk
include test/libc/calls/BUILD.mk
include test/libc/x/BUILD.mk
include test/libc/xed/BUILD.mk
include test/libc/fmt/BUILD.mk
include test/libc/time/BUILD.mk
include test/libc/proc/BUILD.mk
include test/libc/stdio/BUILD.mk
include test/libc/BUILD.mk
include test/net/http/BUILD.mk
include test/net/https/BUILD.mk
include test/net/finger/BUILD.mk
include test/net/BUILD.mk
include test/tool/build/lib/BUILD.mk
include test/tool/build/BUILD.mk
include test/tool/viz/lib/BUILD.mk
include test/tool/viz/BUILD.mk
include test/tool/net/BUILD.mk
include test/tool/BUILD.mk
include test/dsp/core/BUILD.mk
include test/dsp/scale/BUILD.mk
include test/dsp/tty/BUILD.mk
include test/dsp/BUILD.mk
include examples/package/lib/BUILD.mk
include examples/package/BUILD.mk
#-φ-examples/package/new.sh
include test/BUILD.mk

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
	$(COMPILE) -AMKDEPS -L320 $(MKDEPS) -o $@ -s -r o/$(MODE)/ @o/$(MODE)/srcs.txt @o/$(MODE)/hdrs.txt @o/$(MODE)/incs.txt

o/$(MODE)/srcs-old.txt: o/$(MODE)/.x $(MAKEFILES) $(call uniq,$(foreach x,$(SRCS),$(dir $(x))))
	$(file >$@) $(foreach x,$(SRCS),$(file >>$@,$(x)))
o/$(MODE)/hdrs-old.txt: o/$(MODE)/.x $(MAKEFILES) $(call uniq,$(foreach x,$(HDRS) $(INCS),$(dir $(x))))
	$(file >$@) $(foreach x,$(HDRS) $(INCS),$(file >>$@,$(x)))

TAGS: private .UNSANDBOXED = 1
TAGS:	o/$(MODE)/srcs-old.txt $(SRCS) #o/$(MODE)/third_party/ctags/ctags
	@$(RM) $@
	@o/$(MODE)/third_party/ctags/ctags $(TAGSFLAGS) -L $< -o $@

HTAGS: private .UNSANDBOXED = 1
HTAGS:	o/$(MODE)/hdrs-old.txt $(filter-out third_party/libcxx/%,$(HDRS)) #o/$(MODE)/third_party/ctags/ctags
	@$(RM) $@
	@build/htags o/$(MODE)/third_party/ctags/ctags -L $< -o $@

loc: private .UNSANDBOXED = 1
loc: o/$(MODE)/tool/build/summy
	find -name \*.h -or -name \*.c -or -name \*.S | \
	$(XARGS) wc -l | grep total | awk '{print $$1}' | $<

# PLEASE: MAINTAIN TOPOLOGICAL ORDER
# FROM HIGHEST LEVEL TO LOWEST LEVEL
COSMOPOLITAN_OBJECTS =			\
	CTL				\
	THIRD_PARTY_DOUBLECONVERSION	\
	THIRD_PARTY_OPENMP		\
	TOOL_ARGS			\
	NET_HTTP			\
	LIBC_SOCK			\
	LIBC_NT_WS2_32			\
	LIBC_NT_IPHLPAPI		\
	LIBC_X				\
	THIRD_PARTY_GETOPT		\
	LIBC_LOG			\
	THIRD_PARTY_TZ			\
	THIRD_PARTY_MUSL		\
	THIRD_PARTY_ZLIB_GZ		\
	THIRD_PARTY_LIBCXXABI		\
	THIRD_PARTY_LIBUNWIND		\
	LIBC_STDIO			\
	THIRD_PARTY_GDTOA		\
	THIRD_PARTY_REGEX		\
	LIBC_THREAD			\
	LIBC_PROC			\
	THIRD_PARTY_NSYNC_MEM		\
	LIBC_MEM			\
	THIRD_PARTY_DLMALLOC		\
	LIBC_DLOPEN			\
	LIBC_RUNTIME			\
	THIRD_PARTY_NSYNC		\
	LIBC_ELF			\
	LIBC_IRQ			\
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
	LIBC_NT_BCRYPTPRIMITIVES	\
	LIBC_NT_KERNEL32		\
	LIBC_NEXGEN32E

COSMOPOLITAN_H_PKGS =			\
	APE				\
	LIBC				\
	LIBC_CALLS			\
	LIBC_ELF			\
	LIBC_FMT			\
	LIBC_DLOPEN			\
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
	LIBC_TINYMATH			\
	LIBC_X				\
	LIBC_VGA			\
	NET_HTTP			\
	TOOL_ARGS			\
	THIRD_PARTY_DLMALLOC		\
	THIRD_PARTY_GDTOA		\
	THIRD_PARTY_GETOPT		\
	THIRD_PARTY_MUSL		\
	THIRD_PARTY_ZLIB		\
	THIRD_PARTY_ZLIB_GZ		\
	THIRD_PARTY_REGEX

COSMOCC_PKGS =				\
	$(COSMOPOLITAN_H_PKGS)		\
	CTL				\
	THIRD_PARTY_AARCH64		\
	THIRD_PARTY_LIBCXX		\
	THIRD_PARTY_LIBCXXABI		\
	THIRD_PARTY_LIBUNWIND		\
	THIRD_PARTY_OPENMP		\
	THIRD_PARTY_INTEL

o/$(MODE)/cosmopolitan.a:		\
		$(foreach x,$(COSMOPOLITAN_OBJECTS),$($(x)_A_OBJS))

COSMOCC_HDRS =								\
	$(wildcard libc/integral/*)					\
	$(foreach x,$(COSMOCC_PKGS),$($(x)_HDRS))			\
	$(foreach x,$(COSMOCC_PKGS),$($(x)_INCS))

o/cosmocc.h.txt: Makefile libc $(MAKEFILES) $(call uniq,$(foreach x,$(HDRS) $(INCS),$(dir $(x)))) $(HDRS) $(INCS)
	$(file >$@, $(call uniq,$(COSMOCC_HDRS)))

COSMOPOLITAN_H_ROOT_HDRS =						\
	libc/integral/normalize.inc					\
	$(foreach x,$(COSMOPOLITAN_H_PKGS),$($(x)_HDRS))

o/cosmopolitan.html: private .UNSANDBOXED = 1
o/cosmopolitan.html:							\
		o/$(MODE)/third_party/chibicc/chibicc.dbg		\
		$(filter-out %.s,$(foreach x,$(COSMOPOLITAN_OBJECTS),$($(x)_SRCS)))	\
		$(filter-out %.cpp,$(filter-out %.cc,$(SRCS)))				\
		$(HDRS)
	$(file >$(TMPDIR)/$(subst /,_,$@),$(filter-out %.cpp,$(filter-out %.cc,$(filter-out %.s,$(foreach x,$(COSMOPOLITAN_OBJECTS),$($(x)_SRCS))))))
	o/$(MODE)/third_party/chibicc/chibicc.dbg -J		\
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
	o/$(MODE)/ape/ape.lds			\
	o/$(MODE)/libc/crt/crt.o		\
	o/$(MODE)/ape/ape.elf			\
	o/$(MODE)/ape/ape.o			\
	o/$(MODE)/ape/ape-copy-self.o		\
	o/$(MODE)/ape/ape-no-modify-self.o	\
	o/$(MODE)/cosmopolitan.a		\
	o/$(MODE)/third_party/libcxx/libcxx.a	\
	o/$(MODE)/tool/build/march-native	\
	o/$(MODE)/tool/build/ar			\
	o/$(MODE)/tool/build/mktemper		\
	o/$(MODE)/tool/build/fixupobj		\
	o/$(MODE)/tool/build/zipcopy		\
	o/$(MODE)/tool/build/apelink		\
	o/$(MODE)/tool/build/pecheck
else
TOOLCHAIN_ARTIFACTS =				\
	o/$(MODE)/ape/ape.elf			\
	o/$(MODE)/ape/aarch64.lds		\
	o/$(MODE)/libc/crt/crt.o		\
	o/$(MODE)/cosmopolitan.a		\
	o/$(MODE)/third_party/libcxx/libcxx.a	\
	o/$(MODE)/tool/build/march-native	\
	o/$(MODE)/tool/build/fixupobj		\
	o/$(MODE)/tool/build/zipcopy
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
