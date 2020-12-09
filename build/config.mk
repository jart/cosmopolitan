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

RAGELFLAGS ?= -G2

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

RAGELFLAGS ?= -G2

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

RAGELFLAGS = -G2

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
	-fno-inline

CONFIG_COPTS +=			\
	$(SECURITY_BLANKETS)	\
	$(SANITIZER)

CONFIG_COPTS +=			\
	-ftrapv

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
#
# Lastly note that in some cases, such as gc(), there simply isn't any
# ANSI workaround available. It's only in cases like that when we'll use
# the __asm__() header workaround, rather than simply removing it. We do
# however try to do that much less often than mainstream C libraries.

ifeq ($(MODE), ansi)

CONFIG_CFLAGS += -std=c11
#CONFIG_CPPFLAGS += -ansi
CONFIG_CXXFLAGS += -std=c++11
TARGET_ARCH ?= -msse3

endif
