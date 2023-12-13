#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#── vi: set et ft=make ts=8 sw=8 fenc=utf-8 :vi ──────────────────────┘

# Default Mode
#
#   - `make`
#   - Optimized
#   - Backtraces
#   - Syscall tracing
#   - Function tracing
#
ifeq ($(MODE),)
ENABLE_FTRACE = 1
CONFIG_OFLAGS ?= -g
CONFIG_CCFLAGS += -O2 $(BACKTRACES)
CONFIG_CPPFLAGS += -DSYSDEBUG
TARGET_ARCH ?= -msse3
endif
ifeq ($(MODE), x86_64)
ENABLE_FTRACE = 1
CONFIG_OFLAGS ?= -g
CONFIG_CCFLAGS += -O2 $(BACKTRACES)
CONFIG_CPPFLAGS += -DSYSDEBUG
endif
ifeq ($(MODE), aarch64)
ENABLE_FTRACE = 1
CONFIG_OFLAGS ?= -g
CONFIG_CCFLAGS += -O2 $(BACKTRACES)
CONFIG_CPPFLAGS += -DSYSDEBUG
endif

# Zero Optimization Mode
#
#   - Goes 2x slower
#   - Supports --strace
#   - Unsupports --ftrace
#   - Better GDB debugging
#
ifeq ($(MODE), zero)
ENABLE_FTRACE = 1
CONFIG_OFLAGS ?= -g
OVERRIDE_CFLAGS += -O0
OVERRIDE_CXXFLAGS += -O0
CONFIG_CPPFLAGS += -DSYSDEBUG
endif
ifeq ($(MODE), aarch64-zero)
CONFIG_OFLAGS ?= -g
OVERRIDE_CFLAGS += -O0 -fdce
OVERRIDE_CXXFLAGS += -O0 -fdce
CONFIG_CPPFLAGS += -DSYSDEBUG
endif

# Fast Build Mode
#
#   - `make MODE=fastbuild`
#   - No debugging
#   - Syscall tracing
#   - Function tracing
#   - Some optimizations
#   - Limited Backtraces
#
ifeq ($(MODE), fastbuild)
ENABLE_FTRACE = 1
CONFIG_CCFLAGS += $(BACKTRACES) -O
CONFIG_CPPFLAGS += -DSYSDEBUG -DDWARFLESS
CONFIG_LDFLAGS += -S
TARGET_ARCH ?= -msse3
endif

# Optimized Mode
#
#   - `make MODE=opt`
#   - Backtraces
#   - More optimized
#   - Syscall tracing
#   - Function tracing
#   - Reasonably small
#   - No memory corruption detection
#   - assert() / CHECK_xx() may leak code into binary for debuggability
#   - GCC 8+ hoists check fails into .text.cold, thus minimizing impact
#
ifeq ($(MODE), opt)
ENABLE_FTRACE = 1
CONFIG_OFLAGS ?= -g
CONFIG_CPPFLAGS += -DNDEBUG -DSYSDEBUG
CONFIG_CCFLAGS += $(BACKTRACES) -O3 -fmerge-all-constants
TARGET_ARCH ?= -march=native
endif

# Optimized Linux Mode
# The Fastest Mode of All
#
#   - `make MODE=optlinux`
#   - Turns on red zone
#   - Turns off backtraces
#   - Turns off function tracing
#   - Turns off support for older cpu models
#   - Turns off support for other operating systems
#
ifeq ($(MODE), optlinux)
CONFIG_OFLAGS ?= -g
CONFIG_CPPFLAGS += -DNDEBUG -DSYSDEBUG -DSUPPORT_VECTOR=1
CONFIG_CCFLAGS += -O3 -fmerge-all-constants
CONFIG_COPTS += -mred-zone
TARGET_ARCH ?= -march=native
endif

# Release Mode
#
# Follows traditional closed source release binary norms.
#
#   - `make MODE=rel`
#   - More optimized
#   - Reasonably small
#   - Numeric backtraces
#   - No DWARF data bloat
#   - Toilsome debuggability
#   - assert() statements removed
#   - DCHECK_xx() statements removed
#   - No memory corruption detection
#   - CHECK_xx() won't leak strings into binary
#
ifeq ($(MODE), rel)
CONFIG_CPPFLAGS += -DNDEBUG -DDWARFLESS
CONFIG_CCFLAGS += $(BACKTRACES) -O2
TARGET_ARCH ?= -msse3
PYFLAGS += -O1
endif

# Asan Mode
#
# Safer binaries good for backend production serving.
#
#   - `make MODE=asan`
#   - Memory safety
#   - Production worthy
#   - Backtraces
#   - Debuggability
#   - Larger binaries
#
ifeq ($(MODE), asan)
ENABLE_FTRACE = 1
CONFIG_OFLAGS ?= -g
CONFIG_CPPFLAGS += -D__SANITIZE_ADDRESS__
CONFIG_CCFLAGS += $(BACKTRACES) -O2 -DSYSDEBUG
CONFIG_COPTS += -fsanitize=address
TARGET_ARCH ?= -msse3
QUOTA ?= -C64 -L300
endif

# Debug Mode
#
#   - `make MODE=dbg`
#   - Backtraces
#   - Enables asan
#   - Enables ubsan
#   - Stack canaries
#   - No optimization
#   - Enormous binaries
#
ifeq ($(MODE), dbg)
ENABLE_FTRACE = 1
CONFIG_OFLAGS ?= -g
CONFIG_CPPFLAGS += -DMODE_DBG -D__SANITIZE_ADDRESS__ -D__SANITIZE_UNDEFINED__
CONFIG_CCFLAGS += $(BACKTRACES) -DSYSDEBUG -O0 -fno-inline
CONFIG_COPTS += -fsanitize=address -fsanitize=undefined
TARGET_ARCH ?= -msse3
OVERRIDE_CCFLAGS += -fno-pie
QUOTA ?= -C64 -L300
endif
ifeq ($(MODE), aarch64-dbg)
ENABLE_FTRACE = 1
CONFIG_OFLAGS ?= -g
CONFIG_CPPFLAGS += -DMODE_DBG -D__SANITIZE_UNDEFINED__
CONFIG_CCFLAGS += $(BACKTRACES) -DSYSDEBUG -O0 -fno-inline -fdce
CONFIG_COPTS += -fsanitize=undefined
QUOTA ?= -C64 -L300
endif

# System Five Mode
#
#   - `make MODE=sysv`
#   - Optimized
#   - Backtraces
#   - Debuggable
#   - Syscall tracing
#   - Function tracing
#   - No Windows bloat!
#
ifeq ($(MODE), sysv)
ENABLE_FTRACE = 1
CONFIG_OFLAGS ?= -g
CONFIG_CCFLAGS += $(BACKTRACES) -O2
CONFIG_CPPFLAGS += -DSYSDEBUG -DSUPPORT_VECTOR=121
TARGET_ARCH ?= -msse3
endif

# Tiny Mode
#
#   - `make MODE=tiny`
#   - No checks
#   - No asserts
#   - No canaries
#   - No paranoia
#   - No avx hooks
#   - No backtraces
#   - No algorithmics
#   - YOLO
#
ifeq ($(MODE), tiny)
CONFIG_CPPFLAGS +=			\
	-DTINY				\
	-DNDEBUG			\
	-DTRUSTWORTHY
CONFIG_CCFLAGS +=			\
	-Os				\
	-fno-align-functions		\
	-fno-align-jumps		\
	-fno-align-labels		\
	-fno-align-loops		\
	-fschedule-insns2		\
	-momit-leaf-frame-pointer	\
	-foptimize-sibling-calls	\
	-DDWARFLESS
TARGET_ARCH ?=				\
	-msse3
PYFLAGS +=				\
	-O2				\
	-B
endif
ifeq ($(MODE), x86_64-tiny)
CONFIG_CPPFLAGS +=			\
	-DTINY				\
	-DNDEBUG			\
	-DTRUSTWORTHY
CONFIG_CCFLAGS +=			\
	-Os				\
	-fno-align-functions		\
	-fno-align-jumps		\
	-fno-align-labels		\
	-fno-align-loops		\
	-fschedule-insns2		\
	-momit-leaf-frame-pointer	\
	-foptimize-sibling-calls	\
	-DDWARFLESS
TARGET_ARCH ?=				\
	-msse3
PYFLAGS +=				\
	-O2				\
	-B
endif
ifeq ($(MODE), aarch64-tiny)
# TODO(jart): -mcmodel=tiny
CONFIG_CPPFLAGS +=			\
	-DTINY				\
	-DNDEBUG			\
	-DTRUSTWORTHY
CONFIG_CCFLAGS +=			\
	-Os				\
	-fno-align-functions		\
	-fno-align-jumps		\
	-fno-align-labels		\
	-fno-align-loops		\
	-fschedule-insns2		\
	-momit-leaf-frame-pointer	\
	-foptimize-sibling-calls	\
	-DDWARFLESS
PYFLAGS +=				\
	-O2				\
	-B
endif

# Linux-Only Tiny Mode
#
#   - `make MODE=tinylinux`
#   - No checks
#   - No asserts
#   - No canaries
#   - No paranoia
#   - No avx hooks
#   - No backtraces
#   - No portability
#   - No algorithmics
#   - YOLO
#
ifeq ($(MODE), tinylinux)
CONFIG_CPPFLAGS +=			\
	-DTINY				\
	-DNDEBUG			\
	-DTRUSTWORTHY			\
	-DSUPPORT_VECTOR=1		\
	-DDWARFLESS
CONFIG_CCFLAGS +=			\
	-Os				\
	-fno-align-functions		\
	-fno-align-jumps		\
	-fno-align-labels		\
	-fno-align-loops
TARGET_ARCH ?=				\
	-msse3
endif

# Linux+BSD Tiny Mode
#
#   - `make MODE=tinylinuxbsd`
#   - No apple
#   - No checks
#   - No asserts
#   - No canaries
#   - No paranoia
#   - No microsoft
#   - No avx hooks
#   - No backtraces
#   - No algorithmics
#   - YOLO
#
ifeq ($(MODE), tinylinuxbsd)
CONFIG_CPPFLAGS +=		\
	-DTINY			\
	-DNDEBUG		\
	-DTRUSTWORTHY		\
	-DSUPPORT_VECTOR=113	\
	-DDWARFLESS
CONFIG_CCFLAGS +=		\
	-Os			\
	-fno-align-functions	\
	-fno-align-jumps	\
	-fno-align-labels	\
	-fno-align-loops
TARGET_ARCH ?=			\
	-msse3
endif

# Unix Tiny Mode
#
#   - `make MODE=tinysysv`
#   - No checks
#   - No asserts
#   - No canaries
#   - No paranoia
#   - No microsoft
#   - No avx hooks
#   - No backtraces
#   - No algorithmics
#   - YOLO
#
ifeq ($(MODE), tinysysv)
CONFIG_CPPFLAGS +=		\
	-DTINY			\
	-DNDEBUG		\
	-DTRUSTWORTHY		\
	-DSUPPORT_VECTOR=121	\
	-DDWARFLESS
CONFIG_CCFLAGS +=		\
	-Os			\
	-fno-align-functions	\
	-fno-align-jumps	\
	-fno-align-labels	\
	-fno-align-loops
TARGET_ARCH ?=			\
	-msse3
endif

# Tiny Metallic Unix Mode
#
#   - `make MODE=tinynowin`
#   - No checks
#   - No asserts
#   - No canaries
#   - No paranoia
#   - No microsoft
#   - No avx hooks
#   - No backtraces
#   - No algorithmics
#   - YOLO
#
ifeq ($(MODE), tinynowin)
CONFIG_CPPFLAGS +=		\
	-DTINY			\
	-DNDEBUG		\
	-DTRUSTWORTHY		\
	-DSUPPORT_VECTOR=251	\
	-DDWARFLESS
CONFIG_CCFLAGS +=		\
	-Os			\
	-fno-align-functions	\
	-fno-align-jumps	\
	-fno-align-labels	\
	-fno-align-loops
TARGET_ARCH ?=			\
	-msse3
endif

# no x87 instructions mode
#
#     export MODE=nox87
#     make -j8 toolchain
#     cosmocc -o /tmp/hello.com hello.c
#
# lets you shave ~23kb off blink
#
#     git clone https://github.com/jart/blink
#     cd blink
#     ./configure --disable-x87
#     make -j8
#     o//blink/blink /tmp/hello.com
#
ifeq ($(MODE), nox87)
ENABLE_FTRACE = 1
CONFIG_COPTS += -mlong-double-64
CONFIG_CCFLAGS += $(BACKTRACES) -O2
CONFIG_CPPFLAGS += -DSYSDEBUG -DNOX87
TARGET_ARCH ?= -msse3
endif

# LLVM Mode
#
# We aim to support:
#
#     make -j8 m=llvm o/llvm/libc
#
# The rest of the monorepo may not work with llvm.
#
ifeq ($(MODE), llvm)
.STRICT = 0
TARGET_ARCH ?= -msse3
CONFIG_CCFLAGS += $(BACKTRACES) -DSYSDEBUG -O2
AS = clang
CC = clang
CXX = clang++
CXXFILT = llvm-c++filt
LD = ld.lld
NM = llvm-nm
GCC = clang
STRIP = llvm-strip
OBJCOPY = llvm-objcopy
OBJDUMP = llvm-objdump
ADDR2LINE = llvm-addr2line
endif

# ANSI Mode
#
# These flags cause GCC to predefine __STRICT_ANSI__. Please be warned
# that Cosmopolitan headers are written to comply with that request if
# it's possible to do so. Consider the following example:
#
#   make -j12 -O o//tool/viz/printvideo.i
#   clang-format-10 -i o//tool/viz/printvideo.i
#   less o//tool/viz/printvideo.i
#
# You'll notice functions like memcpy(), ioctl(), etc. get expanded into
# wild-eyed gnu-style performance hacks. You can turn it off as follows:
#
#   make -j12 -O MODE=ansi o/ansi/tool/viz/printvideo.i
#   clang-format-10 -i o/ansi/tool/viz/printvideo.i
#   less o/ansi/tool/viz/printvideo.i
#
# Here it becomes clear that ANSI mode can help you decouple your source
# from Cosmopolitan, by turning it into plain ordinary textbook C code.
#
# Another potential use case is distributing code to folks using tools
# such as MSVC or XCode. You can run your binary objects through a tool
# like objconv to convert them to COFF or MachO. Then use ANSI mode to
# rollup one header file that'll enable linkage with minimal issues.
ifeq ($(MODE), ansi)
CONFIG_CFLAGS += -std=c11
#CONFIG_CPPFLAGS += -ansi
CONFIG_CXXFLAGS += -std=c++11
TARGET_ARCH ?= -msse3
endif

ifneq ($(ENABLE_FTRACE),)
CONFIG_CPPFLAGS += -DFTRACE
FTRACE_CCFLAGS = -fno-inline-functions-called-once
OVERRIDE_CFLAGS += $(FTRACE_CCFLAGS)
OVERRIDE_CXXFLAGS += $(FTRACE_CCFLAGS)
# function prologue nops for --ftrace
ifeq ($(ARCH), x86_64)
# this flag causes gcc to generate functions like this
#
#       nop nop nop nop nop nop nop nop nop
#     func:
#       nop nop
#       ...
#
# which tool/build/fixupobj.c improves at build time like this
#
#       nop nop nop nop nop nop nop nop nop
#     func:
#       xchg %ax,%ax
#       ...
#
# which --ftrace morphs at runtime like this
#
#       ud2                # 2 bytes
#       call ftrace_hook   # 5 bytes
#       jmp +2             # 2 bytes
#     func:
#       jmp -7             # 2 bytes
#       ...
#
CONFIG_CCFLAGS += -fpatchable-function-entry=18,16
endif
ifeq ($(ARCH), aarch64)
# this flag causes gcc to generate functions like this
#
#       nop nop nop nop nop nop
#     func:
#       nop
#       ...
#
# which --ftrace morphs at runtime like this
#
#       brk #31337
#       stp x29,x30,[sp,#-16]!
#       mov x29,sp
#       bl  ftrace_hook
#       ldp x29,x30,[sp],#16
#       b   +1
#     func:
#       b   -5
#       ...
#
CONFIG_CCFLAGS += -fpatchable-function-entry=7,6
endif
endif
