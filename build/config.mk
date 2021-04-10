#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#───vi: set et ft=make ts=8 tw=8 fenc=utf-8 :vi───────────────────────┘

# Default Mode
#
#   - `make`
#   - Backtraces
#   - Function tracing
#   - Reasonably small
#   - Reasonably optimized
#   - Reasonably debuggable

ifeq ($(MODE),)

CONFIG_CCFLAGS +=		\
	$(BACKTRACES)		\
	$(FTRACE)		\
	-Og

TARGET_ARCH ?=			\
	-msse3

endif

# Optimized Mode
#
#   - `make MODE=opt`
#   - Backtraces
#   - More optimized
#   - Reasonably small
#   - No memory corruption detection
#   - assert() / CHECK_xx() may leak code into binary for debuggability
#   - GCC 8+ hoists check fails into .text.cold, thus minimizing impact

ifeq ($(MODE), opt)

CONFIG_CPPFLAGS +=		\
	-DNDEBUG		\
	-msse2avx		\
	-Wa,-msse2avx

CONFIG_CCFLAGS +=		\
	$(BACKTRACES)		\
	-O3

TARGET_ARCH ?=			\
	-march=native

endif

# Release Mode
#
#   - `make MODE=rel`
#   - More optimized
#   - Reasonably small
#   - Numeric backtraces
#   - Toilsome debuggability
#   - assert() statements removed
#   - DCHECK_xx() statements removed
#   - No memory corruption detection
#   - CHECK_xx() won't leak strings into binary

ifeq ($(MODE), rel)

CONFIG_CPPFLAGS +=		\
	-DNDEBUG

CONFIG_CCFLAGS +=		\
	$(BACKTRACES)		\
	-O2

TARGET_ARCH ?=			\
	-msse3

endif

# Debug Mode
#
#   - `make MODE=dbg`
#   - Backtraces
#   - Zero optimization
#   - Enables sanitization
#   - Enables stack canaries
#   - Enormous binaries (b/c ubsan suboptimalities)

ifeq ($(MODE), dbg)

CONFIG_CPPFLAGS +=		\
	-DMODE_DBG

CONFIG_CCFLAGS +=		\
	$(BACKTRACES)		\
	$(FTRACE)		\
	-Og

CONFIG_COPTS +=			\
	$(SECURITY_BLANKETS)	\
	$(SANITIZER)

TARGET_ARCH ?=			\
	-msse3

OVERRIDE_CCFLAGS +=		\
	-fno-pie

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
ifeq ($(MODE), tiny)
CONFIG_CPPFLAGS +=		\
	-DTINY			\
	-DNDEBUG		\
	-DTRUSTWORTHY
CONFIG_CCFLAGS +=		\
	-Os			\
	-fno-align-functions	\
	-fno-align-jumps	\
	-fno-align-labels	\
	-fno-align-loops
TARGET_ARCH ?=			\
	-msse3
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
ifeq ($(MODE), tinylinux)
CONFIG_CPPFLAGS +=		\
	-DTINY			\
	-DNDEBUG		\
	-DTRUSTWORTHY		\
	-DSUPPORT_VECTOR=1
CONFIG_CCFLAGS +=		\
	-Os			\
	-fno-align-functions	\
	-fno-align-jumps	\
	-fno-align-labels	\
	-fno-align-loops
TARGET_ARCH ?=			\
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
ifeq ($(MODE), tinylinuxbsd)
CONFIG_CPPFLAGS +=		\
	-DTINY			\
	-DNDEBUG		\
	-DTRUSTWORTHY		\
	-DSUPPORT_VECTOR=113
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
ifeq ($(MODE), tinysysv)
CONFIG_CPPFLAGS +=		\
	-DTINY			\
	-DNDEBUG		\
	-DTRUSTWORTHY		\
	-DSUPPORT_VECTOR=121
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
ifeq ($(MODE), tinynowin)
CONFIG_CPPFLAGS +=		\
	-DTINY			\
	-DNDEBUG		\
	-DTRUSTWORTHY		\
	-DSUPPORT_VECTOR=251
CONFIG_CCFLAGS +=		\
	-Os			\
	-fno-align-functions	\
	-fno-align-jumps	\
	-fno-align-labels	\
	-fno-align-loops
TARGET_ARCH ?=			\
	-msse3
endif

# LLVM Mode
ifeq ($(MODE), llvm)
TARGET_ARCH ?= -msse3
CONFIG_CCFLAGS += $(BACKTRACES) $(FTRACE) -O2
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
