#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#───vi: set et ft=make ts=8 tw=8 fenc=utf-8 :vi───────────────────────┘
#
# SYNOPSIS
#
#   Cosmopolitan Core Build Definitions
#
# DESCRIPTION
#
#   Nearly all compiler flag tuning is done within this one file. Flags
#   may be customized with environment variables. We also use this file
#   to compute expensive values once per build.
#
#   When tuning the variables below, please note they're interpreted in
#   the strictest sense. For example, we don't pass CFLAGS to gcc if we
#   know it's compiling a .S file. This allows our `make SILENT=0` logs
#   to be succinct and informative, at the cost of being less forgiving.
#
#   Further note that link order is equally unforgiving in repositories
#   of this scale. We approach that by over-specifying dependencies, in
#   packages that're broken down usually on a per-directory basis. This
#   is aided by the checkdeps and functional programming tools which in
#   most cases should be able to deduce correct ordering automatically.
#
#   Concerning our approach to flag tuning, most of it is non-essential
#   and largely serves to turn features off. Particularly features that
#   would otherwise lock us in to a particular platform or legal terms.
#   Other flags will usually be the ones that provide us marginal gains
#   in terms of performance and code size, but the world won't end when
#   they aren't used. Flags that play a critical role in source working
#   correctly will usually be specified on a object-by-object basis, in
#   their respective packages.
#
# VARIABLES
#
#     CCFLAGS      gcc frontend flags (.i, .c, .cc, .f, .S, .lds, etc.)
#     CPPFLAGS     preprocessor flags (.h, .c, .cc, .S, .inc, .lds, etc.)
#     CFLAGS       c flags (.c only)
#     CXXFLAGS     c++ flags (.cc only)
#     COPTS        c/c++ flags (.c, .cc)
#     LDFLAGS      linker flags (don't use -Wl, frontend prefix)
#     ASFLAGS      assembler flags (don't use -Wa, frontend prefix)
#     TARGET_ARCH  microarchitecture flags (e.g. -march=native)

DD ?= /bin/dd
CP ?= /bin/cp -f
RM ?= /bin/rm -f
SED ?= /bin/sed
MKDIR ?= /bin/mkdir -p
TAGS ?= /usr/bin/ctags  # emacs source builds or something breaks it
ARFLAGS = rcsD
SILENT ?= 1
ZFLAGS ?=
XARGS ?= xargs -P4 -rs8000
NICE ?= build/actuallynice
RAGEL ?= ragel
DOT ?= dot
GZ ?= gzip
CLANG = clang-10
FC = gfortran  #/opt/cross9f/bin/x86_64-linux-musl-gfortran

# see build/compile, etc. which run third_party/gcc/unbundle.sh
AS = o/third_party/gcc/bin/x86_64-linux-musl-as
CC = o/third_party/gcc/bin/x86_64-linux-musl-gcc
CXX = o/third_party/gcc/bin/x86_64-linux-musl-g++
CXXFILT = o/third_party/gcc/bin/x86_64-linux-musl-c++filt
LD = o/third_party/gcc/bin/x86_64-linux-musl-ld.bfd
AR = build/archive
NM = o/third_party/gcc/bin/x86_64-linux-musl-nm
GCC = o/third_party/gcc/bin/x86_64-linux-musl-gcc
STRIP = o/third_party/gcc/bin/x86_64-linux-musl-strip
OBJCOPY = o/third_party/gcc/bin/x86_64-linux-musl-objcopy
OBJDUMP = o/third_party/gcc/bin/x86_64-linux-musl-objdump
ADDR2LINE = o/third_party/gcc/bin/x86_64-linux-musl-addr2line

COMMA := ,
PWD := $(shell pwd)
IMAGE_BASE_VIRTUAL ?= 0x400000
TMPDIR := $(shell build/findtmp)
LOGFMT := $(shell build/getlogfmt)
CCNAME := $(shell build/getccname $(CC))
CCVERSION := $(shell build/getccversion $(CC))
BLAH1 := $(shell build/zipobj 2>/dev/null)
BLAH2 := $(shell build/package 2>/dev/null)

export ADDR2LINE
export OBJDUMP
export CCNAME
export CCVERSION
export CP
export DD
export GZ
export IMAGE_BASE_VIRTUAL
export LOGFMT
export MKDIR
export MODE
export RM
export SED
export SILENT
export TMPDIR
export ZFLAGS

FTRACE =								\
	-pg

SANITIZER =								\
	-fsanitize=leak							\
	-fsanitize=undefined						\
	-fsanitize=implicit-signed-integer-truncation			\
	-fsanitize=implicit-integer-sign-change

NO_MAGIC =								\
	-mno-fentry							\
	-fno-stack-protector						\
	-fno-sanitize=all

OLD_CODE =								\
	-fno-strict-aliasing						\
	-fno-strict-overflow

TRADITIONAL =								\
	-Wno-implicit-int						\
	-Wno-return-type						\
	-Wno-pointer-sign

DEFAULT_CCFLAGS =							\
	-Wall								\
	-Werror								\
	-fdebug-prefix-map="$(PWD)"=					\
	-frecord-gcc-switches

DEFAULT_OFLAGS =							\
	-g								\
	-gdescribe-dies

DEFAULT_COPTS =								\
	-mno-red-zone							\
	-fno-math-errno							\
	-fno-trapping-math						\
	-fno-fp-int-builtin-inexact					\
	-fno-ident							\
	-fno-common							\
	-fno-gnu-unique							\
	-fstrict-aliasing						\
	-fstrict-overflow						\
	-fno-omit-frame-pointer						\
	-fno-semantic-interposition					\
	-mno-omit-leaf-frame-pointer

MATHEMATICAL =								\
	-O3								\
	-fwrapv

DEFAULT_CPPFLAGS =							\
	-DIMAGE_BASE_VIRTUAL=$(IMAGE_BASE_VIRTUAL)			\
	-nostdinc							\
	-iquote .

DEFAULT_CFLAGS =							\
	-std=gnu2x

DEFAULT_CXXFLAGS =							\
	-std=gnu++11							\
	-fno-rtti							\
	-fno-exceptions							\
	-fuse-cxa-atexit						\
	-fno-threadsafe-statics						\
	-Wno-int-in-bool-context					\
	-Wno-narrowing

DEFAULT_ASFLAGS =							\
	-W								\
	-I.								\
	--noexecstack

DEFAULT_LDFLAGS =							\
	-static								\
	-nostdlib							\
	-m elf_x86_64							\
	--gc-sections							\
	--build-id=none							\
	--cref -Map=$@.map						\
	--no-dynamic-linker						\
	-z max-page-size=0x1000						\
	-Ttext-segment=$(IMAGE_BASE_VIRTUAL)

ASONLYFLAGS =								\
	-g								\
	--debug-prefix-map="$(PWD)"=

DEFAULT_LDLIBS =

MCA =	llvm-mca-10							\
	-mtriple=x86_64-pc-linux-gnu					\
	-iterations=3							\
	-instruction-info						\
	-iterations=3							\
	-all-stats							\
	-all-views							\
	-timeline

cc.flags =								\
	$(DEFAULT_CCFLAGS)						\
	$(CONFIG_CCFLAGS)						\
	$(CCFLAGS)							\
	$(OVERRIDE_CCFLAGS)

o.flags =								\
	$(DEFAULT_OFLAGS)						\
	$(CONFIG_OFLAGS)						\
	$(OFLAGS)							\
	$(OVERRIDE_OFLAGS)

cpp.flags =								\
	$(DEFAULT_CPPFLAGS)						\
	$(CONFIG_CPPFLAGS)						\
	$(CPPFLAGS)							\
	$(OVERRIDE_CPPFLAGS)						\
	-include libc/integral/normalize.inc

copt.flags =								\
	$(TARGET_ARCH)							\
	$(DEFAULT_COPTS)						\
	$(CONFIG_COPTS)							\
	$(COPTS)							\
	$(OVERRIDE_COPTS)

f.flags =								\
	$(DEFAULT_FFLAGS)						\
	$(CONFIG_FFLAGS)						\
	$(FFLAGS)							\
	$(OVERRIDE_FFLAGS)

c.flags =								\
	$(DEFAULT_CFLAGS)						\
	$(CONFIG_CFLAGS)						\
	$(CFLAGS)							\
	$(OVERRIDE_CFLAGS)

cxx.flags =								\
	$(DEFAULT_CXXFLAGS)						\
	$(CONFIG_CXXFLAGS)						\
	$(CXXFLAGS)							\
	$(OVERRIDE_CXXFLAGS)

s.flags =								\
	$(DEFAULT_ASFLAGS)						\
	$(CONFIG_ASFLAGS)						\
	$(ASFLAGS)							\
	$(OVERRIDE_ASFLAGS)

S.flags = $(addprefix -Wa$(COMMA),$(s.flags))

LD.libs =								\
	$(LDLIBS)							\
	$(LOADLIBES)							\
	$(DEFAULT_LDLIBS)						\
	$(CONFIG_LDLIBS)						\
	$(LDLIBS)							\
	$(DEFAULT_LIBS)							\
	$(CONFIG_LIBS)							\
	$(LIBS)

COMPILE.c.flags = $(cc.flags) $(cpp.flags) $(copt.flags) $(c.flags)
COMPILE.cxx.flags = $(cc.flags) $(cpp.flags) $(copt.flags) $(cxx.flags)
COMPILE.f.flags = $(cc.flags) $(copt.flags) $(f.flags)
COMPILE.F.flags = $(cc.flags) $(cpp.flags) $(copt.flags) $(f.flags)
COMPILE.i.flags = $(cc.flags) $(copt.flags) $(c.flags)
COMPILE.ii.flags = $(cc.flags) $(copt.flags) $(cxx.flags)
LINK.flags = $(DEFAULT_LDFLAGS) $(CONFIG_LDFLAGS) $(LDFLAGS)
OBJECTIFY.c.flags = $(OBJECTIFY.S.flags) $(c.flags)
OBJECTIFY.cxx.flags = $(OBJECTIFY.S.flags) $(cxx.flags)
OBJECTIFY.s.flags = $(ASONLYFLAGS) $(s.flags)
OBJECTIFY.S.flags = $(copt.flags) $(cc.flags) $(o.flags) $(cpp.flags) $(S.flags)
OBJECTIFY.f.flags = $(copt.flags) $(cc.flags) $(o.flags) $(copt.flags) $(S.flags) $(f.flags)
OBJECTIFY.F.flags = $(OBJECTIFY.f.flags) $(cpp.flags)
PREPROCESS.flags = -E $(copt.flags) $(cc.flags) $(cpp.flags)
PREPROCESS.lds.flags = -D__LINKER__ $(filter-out -g%,$(PREPROCESS.flags)) -P -xc

COMPILE.c = $(CC) -S $(COMPILE.c.flags)
COMPILE.cxx = $(CXX) -S $(COMPILE.cxx.flags)
COMPILE.i = $(CC) -S $(COMPILE.i.flags)
COMPILE.f = $(FC) -S $(COMPILE.f.flags)
COMPILE.F = $(FC) -S $(COMPILE.F.flags)
OBJECTIFY.s = $(AS) $(OBJECTIFY.s.flags)
OBJECTIFY.S = $(CC) $(OBJECTIFY.S.flags) -c
OBJECTIFY.f = $(FC) $(OBJECTIFY.f.flags) -c
OBJECTIFY.F = $(FC) $(OBJECTIFY.F.flags) -c
OBJECTIFY.c = $(CC) $(OBJECTIFY.c.flags) -c
OBJECTIFY.cxx = $(CXX) $(OBJECTIFY.cxx.flags) -c
PREPROCESS = $(CC) $(PREPROCESS.flags)
PREPROCESS.lds = $(CC) $(PREPROCESS.lds.flags)
LINK = build/link $(LD) $(LINK.flags)
ELF = o/libc/elf/elf.lds
ELFLINK = ACTION=LINK.elf $(LINK) $(LINKARGS) $(OUTPUT_OPTION)
ARCHIVE = $(AR) $(ARFLAGS)
LINKARGS = $(patsubst %.lds,-T %.lds,$(call uniqr,$(LD.libs) $(filter-out %.pkg,$^)))
LOLSAN = build/lolsan -b $(IMAGE_BASE_VIRTUAL)

# The compiler won't generate %xmm code for sources extensioned .greg.c,
# which is needed for C modules wanting to run at the executive level or
# during privileged runtime states, e.g. code morphing.
OBJECTIFY.greg.c =							\
	$(CC)								\
	$(filter-out -pg,$(OBJECTIFY.c.flags))				\
	-D__MGENERAL_REGS_ONLY__					\
	-mgeneral-regs-only						\
	-fno-stack-protector						\
	-fno-instrument-functions					\
	-fno-optimize-sibling-calls					\
	-fno-sanitize=all						\
	-c

OBJECTIFY.ansi.c = $(CC) $(OBJECTIFY.c.flags) -ansi -Wextra -Werror -pedantic-errors -c
OBJECTIFY.c99.c = $(CC) $(OBJECTIFY.c.flags) -std=c99 -Wextra -Werror -pedantic-errors -c
OBJECTIFY.c11.c = $(CC) $(OBJECTIFY.c.flags) -std=c11 -Wextra -Werror -pedantic-errors -c
OBJECTIFY.c2x.c = $(CC) $(OBJECTIFY.c.flags) -std=c2x -Wextra -Werror -pedantic-errors -c

OBJECTIFY.real.c =							\
	$(GCC)								\
	-x-no-pg							\
	$(OBJECTIFY.c.flags)						\
	-wrapper build/realify.sh					\
	-D__REAL_MODE__							\
	-ffixed-r8							\
	-ffixed-r9							\
	-ffixed-r10							\
	-ffixed-r11							\
	-ffixed-r12							\
	-ffixed-r13							\
	-ffixed-r14							\
	-ffixed-r15							\
	-mno-red-zone							\
	-fcall-used-rbx							\
	-fno-jump-tables						\
	-fno-shrink-wrap						\
	-fno-schedule-insns2						\
	-flive-range-shrinkage						\
	-fno-omit-frame-pointer						\
	-momit-leaf-frame-pointer					\
	-mpreferred-stack-boundary=3					\
	-fno-delete-null-pointer-checks					\
	-c

OBJECTIFY.ncabi.c =							\
	$(GCC)								\
	$(OBJECTIFY.c.flags)						\
	-mno-sse							\
	-mfpmath=387							\
	-mno-fentry							\
	-fno-stack-protector						\
	-fno-instrument-functions					\
	-fno-optimize-sibling-calls					\
	-fno-sanitize=all						\
	-fcall-saved-rcx						\
	-fcall-saved-rdx						\
	-fcall-saved-rdi						\
	-fcall-saved-rsi						\
	-fcall-saved-r8							\
	-fcall-saved-r9							\
	-fcall-saved-r10						\
	-fcall-saved-r11						\
	-c								\
	-xc

OBJECTIFY.initabi.c =							\
	$(GCC)								\
	$(OBJECTIFY.c.flags)						\
	-mno-fentry							\
	-fno-stack-protector						\
	-fno-instrument-functions					\
	-fno-optimize-sibling-calls					\
	-fno-sanitize=all						\
	-fcall-saved-rdi						\
	-fcall-saved-rsi						\
	-c

TAGSFLAGS =								\
	-e								\
	-a								\
	--if0=no							\
	--langmap=c:.c.h.i						\
	--line-directives=yes						\
	--exclude=libc/nt/struct/imagefileheader.internal.h		\
	--exclude=libc/nt/struct/filesegmentelement.h
