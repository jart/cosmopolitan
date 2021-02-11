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
	-O2

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
#   - `make MODE=tiny`
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
#   - `make MODE=tiny`
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
#   - `make MODE=tiny`
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
#   - `make MODE=tiny`
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

ifeq ($(MODE), v1)
CONFIG_CPPFLAGS += -DTINY -DNDEBUG -DTRUSTWORTHY -DSUPPORT_VECTOR=1
CONFIG_CCFLAGS += -Os -fno-align-functions -fno-align-jumps -fno-align-labels -fno-align-loops
TARGET_ARCH ?= -msse3
endif

ifeq ($(MODE), v2)
CONFIG_CPPFLAGS += -DTINY -DNDEBUG -DTRUSTWORTHY -DSUPPORT_VECTOR=2
CONFIG_CCFLAGS += -Os -fno-align-functions -fno-align-jumps -fno-align-labels -fno-align-loops
TARGET_ARCH ?= -msse3
endif

ifeq ($(MODE), v3)
CONFIG_CPPFLAGS += -DTINY -DNDEBUG -DTRUSTWORTHY -DSUPPORT_VECTOR=3
CONFIG_CCFLAGS += -Os -fno-align-functions -fno-align-jumps -fno-align-labels -fno-align-loops
TARGET_ARCH ?= -msse3
endif

ifeq ($(MODE), v4)
CONFIG_CPPFLAGS += -DTINY -DNDEBUG -DTRUSTWORTHY -DSUPPORT_VECTOR=4
CONFIG_CCFLAGS += -Os -fno-align-functions -fno-align-jumps -fno-align-labels -fno-align-loops
TARGET_ARCH ?= -msse3
endif

ifeq ($(MODE), v5)
CONFIG_CPPFLAGS += -DTINY -DNDEBUG -DTRUSTWORTHY -DSUPPORT_VECTOR=5
CONFIG_CCFLAGS += -Os -fno-align-functions -fno-align-jumps -fno-align-labels -fno-align-loops
TARGET_ARCH ?= -msse3
endif

ifeq ($(MODE), v6)
CONFIG_CPPFLAGS += -DTINY -DNDEBUG -DTRUSTWORTHY -DSUPPORT_VECTOR=6
CONFIG_CCFLAGS += -Os -fno-align-functions -fno-align-jumps -fno-align-labels -fno-align-loops
TARGET_ARCH ?= -msse3
endif

ifeq ($(MODE), v7)
CONFIG_CPPFLAGS += -DTINY -DNDEBUG -DTRUSTWORTHY -DSUPPORT_VECTOR=7
CONFIG_CCFLAGS += -Os -fno-align-functions -fno-align-jumps -fno-align-labels -fno-align-loops
TARGET_ARCH ?= -msse3
endif

ifeq ($(MODE), v8)
CONFIG_CPPFLAGS += -DTINY -DNDEBUG -DTRUSTWORTHY -DSUPPORT_VECTOR=8
CONFIG_CCFLAGS += -Os -fno-align-functions -fno-align-jumps -fno-align-labels -fno-align-loops
TARGET_ARCH ?= -msse3
endif

ifeq ($(MODE), v9)
CONFIG_CPPFLAGS += -DTINY -DNDEBUG -DTRUSTWORTHY -DSUPPORT_VECTOR=9
CONFIG_CCFLAGS += -Os -fno-align-functions -fno-align-jumps -fno-align-labels -fno-align-loops
TARGET_ARCH ?= -msse3
endif

ifeq ($(MODE), v10)
CONFIG_CPPFLAGS += -DTINY -DNDEBUG -DTRUSTWORTHY -DSUPPORT_VECTOR=10
CONFIG_CCFLAGS += -Os -fno-align-functions -fno-align-jumps -fno-align-labels -fno-align-loops
TARGET_ARCH ?= -msse3
endif

ifeq ($(MODE), v11)
CONFIG_CPPFLAGS += -DTINY -DNDEBUG -DTRUSTWORTHY -DSUPPORT_VECTOR=11
CONFIG_CCFLAGS += -Os -fno-align-functions -fno-align-jumps -fno-align-labels -fno-align-loops
TARGET_ARCH ?= -msse3
endif

ifeq ($(MODE), v12)
CONFIG_CPPFLAGS += -DTINY -DNDEBUG -DTRUSTWORTHY -DSUPPORT_VECTOR=12
CONFIG_CCFLAGS += -Os -fno-align-functions -fno-align-jumps -fno-align-labels -fno-align-loops
TARGET_ARCH ?= -msse3
endif

ifeq ($(MODE), v13)
CONFIG_CPPFLAGS += -DTINY -DNDEBUG -DTRUSTWORTHY -DSUPPORT_VECTOR=13
CONFIG_CCFLAGS += -Os -fno-align-functions -fno-align-jumps -fno-align-labels -fno-align-loops
TARGET_ARCH ?= -msse3
endif

ifeq ($(MODE), v14)
CONFIG_CPPFLAGS += -DTINY -DNDEBUG -DTRUSTWORTHY -DSUPPORT_VECTOR=14
CONFIG_CCFLAGS += -Os -fno-align-functions -fno-align-jumps -fno-align-labels -fno-align-loops
TARGET_ARCH ?= -msse3
endif

ifeq ($(MODE), v15)
CONFIG_CPPFLAGS += -DTINY -DNDEBUG -DTRUSTWORTHY -DSUPPORT_VECTOR=15
CONFIG_CCFLAGS += -Os -fno-align-functions -fno-align-jumps -fno-align-labels -fno-align-loops
TARGET_ARCH ?= -msse3
endif

ifeq ($(MODE), v16)
CONFIG_CPPFLAGS += -DTINY -DNDEBUG -DTRUSTWORTHY -DSUPPORT_VECTOR=16
CONFIG_CCFLAGS += -Os -fno-align-functions -fno-align-jumps -fno-align-labels -fno-align-loops
TARGET_ARCH ?= -msse3
endif

ifeq ($(MODE), v17)
CONFIG_CPPFLAGS += -DTINY -DNDEBUG -DTRUSTWORTHY -DSUPPORT_VECTOR=17
CONFIG_CCFLAGS += -Os -fno-align-functions -fno-align-jumps -fno-align-labels -fno-align-loops
TARGET_ARCH ?= -msse3
endif

ifeq ($(MODE), v18)
CONFIG_CPPFLAGS += -DTINY -DNDEBUG -DTRUSTWORTHY -DSUPPORT_VECTOR=18
CONFIG_CCFLAGS += -Os -fno-align-functions -fno-align-jumps -fno-align-labels -fno-align-loops
TARGET_ARCH ?= -msse3
endif

ifeq ($(MODE), v19)
CONFIG_CPPFLAGS += -DTINY -DNDEBUG -DTRUSTWORTHY -DSUPPORT_VECTOR=19
CONFIG_CCFLAGS += -Os -fno-align-functions -fno-align-jumps -fno-align-labels -fno-align-loops
TARGET_ARCH ?= -msse3
endif

ifeq ($(MODE), v20)
CONFIG_CPPFLAGS += -DTINY -DNDEBUG -DTRUSTWORTHY -DSUPPORT_VECTOR=20
CONFIG_CCFLAGS += -Os -fno-align-functions -fno-align-jumps -fno-align-labels -fno-align-loops
TARGET_ARCH ?= -msse3
endif

ifeq ($(MODE), v21)
CONFIG_CPPFLAGS += -DTINY -DNDEBUG -DTRUSTWORTHY -DSUPPORT_VECTOR=21
CONFIG_CCFLAGS += -Os -fno-align-functions -fno-align-jumps -fno-align-labels -fno-align-loops
TARGET_ARCH ?= -msse3
endif

ifeq ($(MODE), v22)
CONFIG_CPPFLAGS += -DTINY -DNDEBUG -DTRUSTWORTHY -DSUPPORT_VECTOR=22
CONFIG_CCFLAGS += -Os -fno-align-functions -fno-align-jumps -fno-align-labels -fno-align-loops
TARGET_ARCH ?= -msse3
endif

ifeq ($(MODE), v23)
CONFIG_CPPFLAGS += -DTINY -DNDEBUG -DTRUSTWORTHY -DSUPPORT_VECTOR=23
CONFIG_CCFLAGS += -Os -fno-align-functions -fno-align-jumps -fno-align-labels -fno-align-loops
TARGET_ARCH ?= -msse3
endif

ifeq ($(MODE), v24)
CONFIG_CPPFLAGS += -DTINY -DNDEBUG -DTRUSTWORTHY -DSUPPORT_VECTOR=24
CONFIG_CCFLAGS += -Os -fno-align-functions -fno-align-jumps -fno-align-labels -fno-align-loops
TARGET_ARCH ?= -msse3
endif

ifeq ($(MODE), v25)
CONFIG_CPPFLAGS += -DTINY -DNDEBUG -DTRUSTWORTHY -DSUPPORT_VECTOR=25
CONFIG_CCFLAGS += -Os -fno-align-functions -fno-align-jumps -fno-align-labels -fno-align-loops
TARGET_ARCH ?= -msse3
endif

ifeq ($(MODE), v26)
CONFIG_CPPFLAGS += -DTINY -DNDEBUG -DTRUSTWORTHY -DSUPPORT_VECTOR=26
CONFIG_CCFLAGS += -Os -fno-align-functions -fno-align-jumps -fno-align-labels -fno-align-loops
TARGET_ARCH ?= -msse3
endif

ifeq ($(MODE), v27)
CONFIG_CPPFLAGS += -DTINY -DNDEBUG -DTRUSTWORTHY -DSUPPORT_VECTOR=27
CONFIG_CCFLAGS += -Os -fno-align-functions -fno-align-jumps -fno-align-labels -fno-align-loops
TARGET_ARCH ?= -msse3
endif

ifeq ($(MODE), v28)
CONFIG_CPPFLAGS += -DTINY -DNDEBUG -DTRUSTWORTHY -DSUPPORT_VECTOR=28
CONFIG_CCFLAGS += -Os -fno-align-functions -fno-align-jumps -fno-align-labels -fno-align-loops
TARGET_ARCH ?= -msse3
endif

ifeq ($(MODE), v29)
CONFIG_CPPFLAGS += -DTINY -DNDEBUG -DTRUSTWORTHY -DSUPPORT_VECTOR=29
CONFIG_CCFLAGS += -Os -fno-align-functions -fno-align-jumps -fno-align-labels -fno-align-loops
TARGET_ARCH ?= -msse3
endif

ifeq ($(MODE), v30)
CONFIG_CPPFLAGS += -DTINY -DNDEBUG -DTRUSTWORTHY -DSUPPORT_VECTOR=30
CONFIG_CCFLAGS += -Os -fno-align-functions -fno-align-jumps -fno-align-labels -fno-align-loops
TARGET_ARCH ?= -msse3
endif

ifeq ($(MODE), v31)
CONFIG_CPPFLAGS += -DTINY -DNDEBUG -DTRUSTWORTHY -DSUPPORT_VECTOR=31
CONFIG_CCFLAGS += -Os -fno-align-functions -fno-align-jumps -fno-align-labels -fno-align-loops
TARGET_ARCH ?= -msse3
endif

ifeq ($(MODE), v32)
CONFIG_CPPFLAGS += -DTINY -DNDEBUG -DTRUSTWORTHY -DSUPPORT_VECTOR=32
CONFIG_CCFLAGS += -Os -fno-align-functions -fno-align-jumps -fno-align-labels -fno-align-loops
TARGET_ARCH ?= -msse3
endif

ifeq ($(MODE), v33)
CONFIG_CPPFLAGS += -DTINY -DNDEBUG -DTRUSTWORTHY -DSUPPORT_VECTOR=33
CONFIG_CCFLAGS += -Os -fno-align-functions -fno-align-jumps -fno-align-labels -fno-align-loops
TARGET_ARCH ?= -msse3
endif

ifeq ($(MODE), v34)
CONFIG_CPPFLAGS += -DTINY -DNDEBUG -DTRUSTWORTHY -DSUPPORT_VECTOR=34
CONFIG_CCFLAGS += -Os -fno-align-functions -fno-align-jumps -fno-align-labels -fno-align-loops
TARGET_ARCH ?= -msse3
endif

ifeq ($(MODE), v35)
CONFIG_CPPFLAGS += -DTINY -DNDEBUG -DTRUSTWORTHY -DSUPPORT_VECTOR=35
CONFIG_CCFLAGS += -Os -fno-align-functions -fno-align-jumps -fno-align-labels -fno-align-loops
TARGET_ARCH ?= -msse3
endif

ifeq ($(MODE), v36)
CONFIG_CPPFLAGS += -DTINY -DNDEBUG -DTRUSTWORTHY -DSUPPORT_VECTOR=36
CONFIG_CCFLAGS += -Os -fno-align-functions -fno-align-jumps -fno-align-labels -fno-align-loops
TARGET_ARCH ?= -msse3
endif

ifeq ($(MODE), v37)
CONFIG_CPPFLAGS += -DTINY -DNDEBUG -DTRUSTWORTHY -DSUPPORT_VECTOR=37
CONFIG_CCFLAGS += -Os -fno-align-functions -fno-align-jumps -fno-align-labels -fno-align-loops
TARGET_ARCH ?= -msse3
endif

ifeq ($(MODE), v38)
CONFIG_CPPFLAGS += -DTINY -DNDEBUG -DTRUSTWORTHY -DSUPPORT_VECTOR=38
CONFIG_CCFLAGS += -Os -fno-align-functions -fno-align-jumps -fno-align-labels -fno-align-loops
TARGET_ARCH ?= -msse3
endif

ifeq ($(MODE), v39)
CONFIG_CPPFLAGS += -DTINY -DNDEBUG -DTRUSTWORTHY -DSUPPORT_VECTOR=39
CONFIG_CCFLAGS += -Os -fno-align-functions -fno-align-jumps -fno-align-labels -fno-align-loops
TARGET_ARCH ?= -msse3
endif

ifeq ($(MODE), v40)
CONFIG_CPPFLAGS += -DTINY -DNDEBUG -DTRUSTWORTHY -DSUPPORT_VECTOR=40
CONFIG_CCFLAGS += -Os -fno-align-functions -fno-align-jumps -fno-align-labels -fno-align-loops
TARGET_ARCH ?= -msse3
endif

ifeq ($(MODE), v41)
CONFIG_CPPFLAGS += -DTINY -DNDEBUG -DTRUSTWORTHY -DSUPPORT_VECTOR=41
CONFIG_CCFLAGS += -Os -fno-align-functions -fno-align-jumps -fno-align-labels -fno-align-loops
TARGET_ARCH ?= -msse3
endif

ifeq ($(MODE), v42)
CONFIG_CPPFLAGS += -DTINY -DNDEBUG -DTRUSTWORTHY -DSUPPORT_VECTOR=42
CONFIG_CCFLAGS += -Os -fno-align-functions -fno-align-jumps -fno-align-labels -fno-align-loops
TARGET_ARCH ?= -msse3
endif

ifeq ($(MODE), v43)
CONFIG_CPPFLAGS += -DTINY -DNDEBUG -DTRUSTWORTHY -DSUPPORT_VECTOR=43
CONFIG_CCFLAGS += -Os -fno-align-functions -fno-align-jumps -fno-align-labels -fno-align-loops
TARGET_ARCH ?= -msse3
endif

ifeq ($(MODE), v44)
CONFIG_CPPFLAGS += -DTINY -DNDEBUG -DTRUSTWORTHY -DSUPPORT_VECTOR=44
CONFIG_CCFLAGS += -Os -fno-align-functions -fno-align-jumps -fno-align-labels -fno-align-loops
TARGET_ARCH ?= -msse3
endif

ifeq ($(MODE), v45)
CONFIG_CPPFLAGS += -DTINY -DNDEBUG -DTRUSTWORTHY -DSUPPORT_VECTOR=45
CONFIG_CCFLAGS += -Os -fno-align-functions -fno-align-jumps -fno-align-labels -fno-align-loops
TARGET_ARCH ?= -msse3
endif

ifeq ($(MODE), v46)
CONFIG_CPPFLAGS += -DTINY -DNDEBUG -DTRUSTWORTHY -DSUPPORT_VECTOR=46
CONFIG_CCFLAGS += -Os -fno-align-functions -fno-align-jumps -fno-align-labels -fno-align-loops
TARGET_ARCH ?= -msse3
endif

ifeq ($(MODE), v47)
CONFIG_CPPFLAGS += -DTINY -DNDEBUG -DTRUSTWORTHY -DSUPPORT_VECTOR=47
CONFIG_CCFLAGS += -Os -fno-align-functions -fno-align-jumps -fno-align-labels -fno-align-loops
TARGET_ARCH ?= -msse3
endif

ifeq ($(MODE), v48)
CONFIG_CPPFLAGS += -DTINY -DNDEBUG -DTRUSTWORTHY -DSUPPORT_VECTOR=48
CONFIG_CCFLAGS += -Os -fno-align-functions -fno-align-jumps -fno-align-labels -fno-align-loops
TARGET_ARCH ?= -msse3
endif

ifeq ($(MODE), v49)
CONFIG_CPPFLAGS += -DTINY -DNDEBUG -DTRUSTWORTHY -DSUPPORT_VECTOR=49
CONFIG_CCFLAGS += -Os -fno-align-functions -fno-align-jumps -fno-align-labels -fno-align-loops
TARGET_ARCH ?= -msse3
endif

ifeq ($(MODE), v50)
CONFIG_CPPFLAGS += -DTINY -DNDEBUG -DTRUSTWORTHY -DSUPPORT_VECTOR=50
CONFIG_CCFLAGS += -Os -fno-align-functions -fno-align-jumps -fno-align-labels -fno-align-loops
TARGET_ARCH ?= -msse3
endif

ifeq ($(MODE), v51)
CONFIG_CPPFLAGS += -DTINY -DNDEBUG -DTRUSTWORTHY -DSUPPORT_VECTOR=51
CONFIG_CCFLAGS += -Os -fno-align-functions -fno-align-jumps -fno-align-labels -fno-align-loops
TARGET_ARCH ?= -msse3
endif

ifeq ($(MODE), v52)
CONFIG_CPPFLAGS += -DTINY -DNDEBUG -DTRUSTWORTHY -DSUPPORT_VECTOR=52
CONFIG_CCFLAGS += -Os -fno-align-functions -fno-align-jumps -fno-align-labels -fno-align-loops
TARGET_ARCH ?= -msse3
endif

ifeq ($(MODE), v53)
CONFIG_CPPFLAGS += -DTINY -DNDEBUG -DTRUSTWORTHY -DSUPPORT_VECTOR=53
CONFIG_CCFLAGS += -Os -fno-align-functions -fno-align-jumps -fno-align-labels -fno-align-loops
TARGET_ARCH ?= -msse3
endif

ifeq ($(MODE), v54)
CONFIG_CPPFLAGS += -DTINY -DNDEBUG -DTRUSTWORTHY -DSUPPORT_VECTOR=54
CONFIG_CCFLAGS += -Os -fno-align-functions -fno-align-jumps -fno-align-labels -fno-align-loops
TARGET_ARCH ?= -msse3
endif

ifeq ($(MODE), v55)
CONFIG_CPPFLAGS += -DTINY -DNDEBUG -DTRUSTWORTHY -DSUPPORT_VECTOR=55
CONFIG_CCFLAGS += -Os -fno-align-functions -fno-align-jumps -fno-align-labels -fno-align-loops
TARGET_ARCH ?= -msse3
endif

ifeq ($(MODE), v56)
CONFIG_CPPFLAGS += -DTINY -DNDEBUG -DTRUSTWORTHY -DSUPPORT_VECTOR=56
CONFIG_CCFLAGS += -Os -fno-align-functions -fno-align-jumps -fno-align-labels -fno-align-loops
TARGET_ARCH ?= -msse3
endif

ifeq ($(MODE), v57)
CONFIG_CPPFLAGS += -DTINY -DNDEBUG -DTRUSTWORTHY -DSUPPORT_VECTOR=57
CONFIG_CCFLAGS += -Os -fno-align-functions -fno-align-jumps -fno-align-labels -fno-align-loops
TARGET_ARCH ?= -msse3
endif

ifeq ($(MODE), v58)
CONFIG_CPPFLAGS += -DTINY -DNDEBUG -DTRUSTWORTHY -DSUPPORT_VECTOR=58
CONFIG_CCFLAGS += -Os -fno-align-functions -fno-align-jumps -fno-align-labels -fno-align-loops
TARGET_ARCH ?= -msse3
endif

ifeq ($(MODE), v59)
CONFIG_CPPFLAGS += -DTINY -DNDEBUG -DTRUSTWORTHY -DSUPPORT_VECTOR=59
CONFIG_CCFLAGS += -Os -fno-align-functions -fno-align-jumps -fno-align-labels -fno-align-loops
TARGET_ARCH ?= -msse3
endif

ifeq ($(MODE), v60)
CONFIG_CPPFLAGS += -DTINY -DNDEBUG -DTRUSTWORTHY -DSUPPORT_VECTOR=60
CONFIG_CCFLAGS += -Os -fno-align-functions -fno-align-jumps -fno-align-labels -fno-align-loops
TARGET_ARCH ?= -msse3
endif

ifeq ($(MODE), v61)
CONFIG_CPPFLAGS += -DTINY -DNDEBUG -DTRUSTWORTHY -DSUPPORT_VECTOR=61
CONFIG_CCFLAGS += -Os -fno-align-functions -fno-align-jumps -fno-align-labels -fno-align-loops
TARGET_ARCH ?= -msse3
endif

ifeq ($(MODE), v62)
CONFIG_CPPFLAGS += -DTINY -DNDEBUG -DTRUSTWORTHY -DSUPPORT_VECTOR=62
CONFIG_CCFLAGS += -Os -fno-align-functions -fno-align-jumps -fno-align-labels -fno-align-loops
TARGET_ARCH ?= -msse3
endif

ifeq ($(MODE), v63)
CONFIG_CPPFLAGS += -DTINY -DNDEBUG -DTRUSTWORTHY -DSUPPORT_VECTOR=63
CONFIG_CCFLAGS += -Os -fno-align-functions -fno-align-jumps -fno-align-labels -fno-align-loops
TARGET_ARCH ?= -msse3
endif

ifeq ($(MODE), v64)
CONFIG_CPPFLAGS += -DTINY -DNDEBUG -DTRUSTWORTHY -DSUPPORT_VECTOR=64
CONFIG_CCFLAGS += -Os -fno-align-functions -fno-align-jumps -fno-align-labels -fno-align-loops
TARGET_ARCH ?= -msse3
endif

ifeq ($(MODE), v65)
CONFIG_CPPFLAGS += -DTINY -DNDEBUG -DTRUSTWORTHY -DSUPPORT_VECTOR=65
CONFIG_CCFLAGS += -Os -fno-align-functions -fno-align-jumps -fno-align-labels -fno-align-loops
TARGET_ARCH ?= -msse3
endif

ifeq ($(MODE), v66)
CONFIG_CPPFLAGS += -DTINY -DNDEBUG -DTRUSTWORTHY -DSUPPORT_VECTOR=66
CONFIG_CCFLAGS += -Os -fno-align-functions -fno-align-jumps -fno-align-labels -fno-align-loops
TARGET_ARCH ?= -msse3
endif

ifeq ($(MODE), v67)
CONFIG_CPPFLAGS += -DTINY -DNDEBUG -DTRUSTWORTHY -DSUPPORT_VECTOR=67
CONFIG_CCFLAGS += -Os -fno-align-functions -fno-align-jumps -fno-align-labels -fno-align-loops
TARGET_ARCH ?= -msse3
endif

ifeq ($(MODE), v68)
CONFIG_CPPFLAGS += -DTINY -DNDEBUG -DTRUSTWORTHY -DSUPPORT_VECTOR=68
CONFIG_CCFLAGS += -Os -fno-align-functions -fno-align-jumps -fno-align-labels -fno-align-loops
TARGET_ARCH ?= -msse3
endif

ifeq ($(MODE), v69)
CONFIG_CPPFLAGS += -DTINY -DNDEBUG -DTRUSTWORTHY -DSUPPORT_VECTOR=69
CONFIG_CCFLAGS += -Os -fno-align-functions -fno-align-jumps -fno-align-labels -fno-align-loops
TARGET_ARCH ?= -msse3
endif

ifeq ($(MODE), v70)
CONFIG_CPPFLAGS += -DTINY -DNDEBUG -DTRUSTWORTHY -DSUPPORT_VECTOR=70
CONFIG_CCFLAGS += -Os -fno-align-functions -fno-align-jumps -fno-align-labels -fno-align-loops
TARGET_ARCH ?= -msse3
endif

ifeq ($(MODE), v71)
CONFIG_CPPFLAGS += -DTINY -DNDEBUG -DTRUSTWORTHY -DSUPPORT_VECTOR=71
CONFIG_CCFLAGS += -Os -fno-align-functions -fno-align-jumps -fno-align-labels -fno-align-loops
TARGET_ARCH ?= -msse3
endif

ifeq ($(MODE), v72)
CONFIG_CPPFLAGS += -DTINY -DNDEBUG -DTRUSTWORTHY -DSUPPORT_VECTOR=72
CONFIG_CCFLAGS += -Os -fno-align-functions -fno-align-jumps -fno-align-labels -fno-align-loops
TARGET_ARCH ?= -msse3
endif

ifeq ($(MODE), v73)
CONFIG_CPPFLAGS += -DTINY -DNDEBUG -DTRUSTWORTHY -DSUPPORT_VECTOR=73
CONFIG_CCFLAGS += -Os -fno-align-functions -fno-align-jumps -fno-align-labels -fno-align-loops
TARGET_ARCH ?= -msse3
endif

ifeq ($(MODE), v74)
CONFIG_CPPFLAGS += -DTINY -DNDEBUG -DTRUSTWORTHY -DSUPPORT_VECTOR=74
CONFIG_CCFLAGS += -Os -fno-align-functions -fno-align-jumps -fno-align-labels -fno-align-loops
TARGET_ARCH ?= -msse3
endif

ifeq ($(MODE), v75)
CONFIG_CPPFLAGS += -DTINY -DNDEBUG -DTRUSTWORTHY -DSUPPORT_VECTOR=75
CONFIG_CCFLAGS += -Os -fno-align-functions -fno-align-jumps -fno-align-labels -fno-align-loops
TARGET_ARCH ?= -msse3
endif

ifeq ($(MODE), v76)
CONFIG_CPPFLAGS += -DTINY -DNDEBUG -DTRUSTWORTHY -DSUPPORT_VECTOR=76
CONFIG_CCFLAGS += -Os -fno-align-functions -fno-align-jumps -fno-align-labels -fno-align-loops
TARGET_ARCH ?= -msse3
endif

ifeq ($(MODE), v77)
CONFIG_CPPFLAGS += -DTINY -DNDEBUG -DTRUSTWORTHY -DSUPPORT_VECTOR=77
CONFIG_CCFLAGS += -Os -fno-align-functions -fno-align-jumps -fno-align-labels -fno-align-loops
TARGET_ARCH ?= -msse3
endif

ifeq ($(MODE), v78)
CONFIG_CPPFLAGS += -DTINY -DNDEBUG -DTRUSTWORTHY -DSUPPORT_VECTOR=78
CONFIG_CCFLAGS += -Os -fno-align-functions -fno-align-jumps -fno-align-labels -fno-align-loops
TARGET_ARCH ?= -msse3
endif

ifeq ($(MODE), v79)
CONFIG_CPPFLAGS += -DTINY -DNDEBUG -DTRUSTWORTHY -DSUPPORT_VECTOR=79
CONFIG_CCFLAGS += -Os -fno-align-functions -fno-align-jumps -fno-align-labels -fno-align-loops
TARGET_ARCH ?= -msse3
endif

ifeq ($(MODE), v80)
CONFIG_CPPFLAGS += -DTINY -DNDEBUG -DTRUSTWORTHY -DSUPPORT_VECTOR=80
CONFIG_CCFLAGS += -Os -fno-align-functions -fno-align-jumps -fno-align-labels -fno-align-loops
TARGET_ARCH ?= -msse3
endif

ifeq ($(MODE), v81)
CONFIG_CPPFLAGS += -DTINY -DNDEBUG -DTRUSTWORTHY -DSUPPORT_VECTOR=81
CONFIG_CCFLAGS += -Os -fno-align-functions -fno-align-jumps -fno-align-labels -fno-align-loops
TARGET_ARCH ?= -msse3
endif

ifeq ($(MODE), v82)
CONFIG_CPPFLAGS += -DTINY -DNDEBUG -DTRUSTWORTHY -DSUPPORT_VECTOR=82
CONFIG_CCFLAGS += -Os -fno-align-functions -fno-align-jumps -fno-align-labels -fno-align-loops
TARGET_ARCH ?= -msse3
endif

ifeq ($(MODE), v83)
CONFIG_CPPFLAGS += -DTINY -DNDEBUG -DTRUSTWORTHY -DSUPPORT_VECTOR=83
CONFIG_CCFLAGS += -Os -fno-align-functions -fno-align-jumps -fno-align-labels -fno-align-loops
TARGET_ARCH ?= -msse3
endif

ifeq ($(MODE), v84)
CONFIG_CPPFLAGS += -DTINY -DNDEBUG -DTRUSTWORTHY -DSUPPORT_VECTOR=84
CONFIG_CCFLAGS += -Os -fno-align-functions -fno-align-jumps -fno-align-labels -fno-align-loops
TARGET_ARCH ?= -msse3
endif

ifeq ($(MODE), v85)
CONFIG_CPPFLAGS += -DTINY -DNDEBUG -DTRUSTWORTHY -DSUPPORT_VECTOR=85
CONFIG_CCFLAGS += -Os -fno-align-functions -fno-align-jumps -fno-align-labels -fno-align-loops
TARGET_ARCH ?= -msse3
endif

ifeq ($(MODE), v86)
CONFIG_CPPFLAGS += -DTINY -DNDEBUG -DTRUSTWORTHY -DSUPPORT_VECTOR=86
CONFIG_CCFLAGS += -Os -fno-align-functions -fno-align-jumps -fno-align-labels -fno-align-loops
TARGET_ARCH ?= -msse3
endif

ifeq ($(MODE), v87)
CONFIG_CPPFLAGS += -DTINY -DNDEBUG -DTRUSTWORTHY -DSUPPORT_VECTOR=87
CONFIG_CCFLAGS += -Os -fno-align-functions -fno-align-jumps -fno-align-labels -fno-align-loops
TARGET_ARCH ?= -msse3
endif

ifeq ($(MODE), v88)
CONFIG_CPPFLAGS += -DTINY -DNDEBUG -DTRUSTWORTHY -DSUPPORT_VECTOR=88
CONFIG_CCFLAGS += -Os -fno-align-functions -fno-align-jumps -fno-align-labels -fno-align-loops
TARGET_ARCH ?= -msse3
endif

ifeq ($(MODE), v89)
CONFIG_CPPFLAGS += -DTINY -DNDEBUG -DTRUSTWORTHY -DSUPPORT_VECTOR=89
CONFIG_CCFLAGS += -Os -fno-align-functions -fno-align-jumps -fno-align-labels -fno-align-loops
TARGET_ARCH ?= -msse3
endif

ifeq ($(MODE), v90)
CONFIG_CPPFLAGS += -DTINY -DNDEBUG -DTRUSTWORTHY -DSUPPORT_VECTOR=90
CONFIG_CCFLAGS += -Os -fno-align-functions -fno-align-jumps -fno-align-labels -fno-align-loops
TARGET_ARCH ?= -msse3
endif

ifeq ($(MODE), v91)
CONFIG_CPPFLAGS += -DTINY -DNDEBUG -DTRUSTWORTHY -DSUPPORT_VECTOR=91
CONFIG_CCFLAGS += -Os -fno-align-functions -fno-align-jumps -fno-align-labels -fno-align-loops
TARGET_ARCH ?= -msse3
endif

ifeq ($(MODE), v92)
CONFIG_CPPFLAGS += -DTINY -DNDEBUG -DTRUSTWORTHY -DSUPPORT_VECTOR=92
CONFIG_CCFLAGS += -Os -fno-align-functions -fno-align-jumps -fno-align-labels -fno-align-loops
TARGET_ARCH ?= -msse3
endif

ifeq ($(MODE), v93)
CONFIG_CPPFLAGS += -DTINY -DNDEBUG -DTRUSTWORTHY -DSUPPORT_VECTOR=93
CONFIG_CCFLAGS += -Os -fno-align-functions -fno-align-jumps -fno-align-labels -fno-align-loops
TARGET_ARCH ?= -msse3
endif

ifeq ($(MODE), v94)
CONFIG_CPPFLAGS += -DTINY -DNDEBUG -DTRUSTWORTHY -DSUPPORT_VECTOR=94
CONFIG_CCFLAGS += -Os -fno-align-functions -fno-align-jumps -fno-align-labels -fno-align-loops
TARGET_ARCH ?= -msse3
endif

ifeq ($(MODE), v95)
CONFIG_CPPFLAGS += -DTINY -DNDEBUG -DTRUSTWORTHY -DSUPPORT_VECTOR=95
CONFIG_CCFLAGS += -Os -fno-align-functions -fno-align-jumps -fno-align-labels -fno-align-loops
TARGET_ARCH ?= -msse3
endif

ifeq ($(MODE), v96)
CONFIG_CPPFLAGS += -DTINY -DNDEBUG -DTRUSTWORTHY -DSUPPORT_VECTOR=96
CONFIG_CCFLAGS += -Os -fno-align-functions -fno-align-jumps -fno-align-labels -fno-align-loops
TARGET_ARCH ?= -msse3
endif

ifeq ($(MODE), v97)
CONFIG_CPPFLAGS += -DTINY -DNDEBUG -DTRUSTWORTHY -DSUPPORT_VECTOR=97
CONFIG_CCFLAGS += -Os -fno-align-functions -fno-align-jumps -fno-align-labels -fno-align-loops
TARGET_ARCH ?= -msse3
endif

ifeq ($(MODE), v98)
CONFIG_CPPFLAGS += -DTINY -DNDEBUG -DTRUSTWORTHY -DSUPPORT_VECTOR=98
CONFIG_CCFLAGS += -Os -fno-align-functions -fno-align-jumps -fno-align-labels -fno-align-loops
TARGET_ARCH ?= -msse3
endif

ifeq ($(MODE), v99)
CONFIG_CPPFLAGS += -DTINY -DNDEBUG -DTRUSTWORTHY -DSUPPORT_VECTOR=99
CONFIG_CCFLAGS += -Os -fno-align-functions -fno-align-jumps -fno-align-labels -fno-align-loops
TARGET_ARCH ?= -msse3
endif

ifeq ($(MODE), v100)
CONFIG_CPPFLAGS += -DTINY -DNDEBUG -DTRUSTWORTHY -DSUPPORT_VECTOR=100
CONFIG_CCFLAGS += -Os -fno-align-functions -fno-align-jumps -fno-align-labels -fno-align-loops
TARGET_ARCH ?= -msse3
endif

ifeq ($(MODE), v101)
CONFIG_CPPFLAGS += -DTINY -DNDEBUG -DTRUSTWORTHY -DSUPPORT_VECTOR=101
CONFIG_CCFLAGS += -Os -fno-align-functions -fno-align-jumps -fno-align-labels -fno-align-loops
TARGET_ARCH ?= -msse3
endif

ifeq ($(MODE), v102)
CONFIG_CPPFLAGS += -DTINY -DNDEBUG -DTRUSTWORTHY -DSUPPORT_VECTOR=102
CONFIG_CCFLAGS += -Os -fno-align-functions -fno-align-jumps -fno-align-labels -fno-align-loops
TARGET_ARCH ?= -msse3
endif

ifeq ($(MODE), v103)
CONFIG_CPPFLAGS += -DTINY -DNDEBUG -DTRUSTWORTHY -DSUPPORT_VECTOR=103
CONFIG_CCFLAGS += -Os -fno-align-functions -fno-align-jumps -fno-align-labels -fno-align-loops
TARGET_ARCH ?= -msse3
endif

ifeq ($(MODE), v104)
CONFIG_CPPFLAGS += -DTINY -DNDEBUG -DTRUSTWORTHY -DSUPPORT_VECTOR=104
CONFIG_CCFLAGS += -Os -fno-align-functions -fno-align-jumps -fno-align-labels -fno-align-loops
TARGET_ARCH ?= -msse3
endif

ifeq ($(MODE), v105)
CONFIG_CPPFLAGS += -DTINY -DNDEBUG -DTRUSTWORTHY -DSUPPORT_VECTOR=105
CONFIG_CCFLAGS += -Os -fno-align-functions -fno-align-jumps -fno-align-labels -fno-align-loops
TARGET_ARCH ?= -msse3
endif

ifeq ($(MODE), v106)
CONFIG_CPPFLAGS += -DTINY -DNDEBUG -DTRUSTWORTHY -DSUPPORT_VECTOR=106
CONFIG_CCFLAGS += -Os -fno-align-functions -fno-align-jumps -fno-align-labels -fno-align-loops
TARGET_ARCH ?= -msse3
endif

ifeq ($(MODE), v107)
CONFIG_CPPFLAGS += -DTINY -DNDEBUG -DTRUSTWORTHY -DSUPPORT_VECTOR=107
CONFIG_CCFLAGS += -Os -fno-align-functions -fno-align-jumps -fno-align-labels -fno-align-loops
TARGET_ARCH ?= -msse3
endif

ifeq ($(MODE), v108)
CONFIG_CPPFLAGS += -DTINY -DNDEBUG -DTRUSTWORTHY -DSUPPORT_VECTOR=108
CONFIG_CCFLAGS += -Os -fno-align-functions -fno-align-jumps -fno-align-labels -fno-align-loops
TARGET_ARCH ?= -msse3
endif

ifeq ($(MODE), v109)
CONFIG_CPPFLAGS += -DTINY -DNDEBUG -DTRUSTWORTHY -DSUPPORT_VECTOR=109
CONFIG_CCFLAGS += -Os -fno-align-functions -fno-align-jumps -fno-align-labels -fno-align-loops
TARGET_ARCH ?= -msse3
endif

ifeq ($(MODE), v110)
CONFIG_CPPFLAGS += -DTINY -DNDEBUG -DTRUSTWORTHY -DSUPPORT_VECTOR=110
CONFIG_CCFLAGS += -Os -fno-align-functions -fno-align-jumps -fno-align-labels -fno-align-loops
TARGET_ARCH ?= -msse3
endif

ifeq ($(MODE), v111)
CONFIG_CPPFLAGS += -DTINY -DNDEBUG -DTRUSTWORTHY -DSUPPORT_VECTOR=111
CONFIG_CCFLAGS += -Os -fno-align-functions -fno-align-jumps -fno-align-labels -fno-align-loops
TARGET_ARCH ?= -msse3
endif

ifeq ($(MODE), v112)
CONFIG_CPPFLAGS += -DTINY -DNDEBUG -DTRUSTWORTHY -DSUPPORT_VECTOR=112
CONFIG_CCFLAGS += -Os -fno-align-functions -fno-align-jumps -fno-align-labels -fno-align-loops
TARGET_ARCH ?= -msse3
endif

ifeq ($(MODE), v113)
CONFIG_CPPFLAGS += -DTINY -DNDEBUG -DTRUSTWORTHY -DSUPPORT_VECTOR=113
CONFIG_CCFLAGS += -Os -fno-align-functions -fno-align-jumps -fno-align-labels -fno-align-loops
TARGET_ARCH ?= -msse3
endif

ifeq ($(MODE), v114)
CONFIG_CPPFLAGS += -DTINY -DNDEBUG -DTRUSTWORTHY -DSUPPORT_VECTOR=114
CONFIG_CCFLAGS += -Os -fno-align-functions -fno-align-jumps -fno-align-labels -fno-align-loops
TARGET_ARCH ?= -msse3
endif

ifeq ($(MODE), v115)
CONFIG_CPPFLAGS += -DTINY -DNDEBUG -DTRUSTWORTHY -DSUPPORT_VECTOR=115
CONFIG_CCFLAGS += -Os -fno-align-functions -fno-align-jumps -fno-align-labels -fno-align-loops
TARGET_ARCH ?= -msse3
endif

ifeq ($(MODE), v116)
CONFIG_CPPFLAGS += -DTINY -DNDEBUG -DTRUSTWORTHY -DSUPPORT_VECTOR=116
CONFIG_CCFLAGS += -Os -fno-align-functions -fno-align-jumps -fno-align-labels -fno-align-loops
TARGET_ARCH ?= -msse3
endif

ifeq ($(MODE), v117)
CONFIG_CPPFLAGS += -DTINY -DNDEBUG -DTRUSTWORTHY -DSUPPORT_VECTOR=117
CONFIG_CCFLAGS += -Os -fno-align-functions -fno-align-jumps -fno-align-labels -fno-align-loops
TARGET_ARCH ?= -msse3
endif

ifeq ($(MODE), v118)
CONFIG_CPPFLAGS += -DTINY -DNDEBUG -DTRUSTWORTHY -DSUPPORT_VECTOR=118
CONFIG_CCFLAGS += -Os -fno-align-functions -fno-align-jumps -fno-align-labels -fno-align-loops
TARGET_ARCH ?= -msse3
endif

ifeq ($(MODE), v119)
CONFIG_CPPFLAGS += -DTINY -DNDEBUG -DTRUSTWORTHY -DSUPPORT_VECTOR=119
CONFIG_CCFLAGS += -Os -fno-align-functions -fno-align-jumps -fno-align-labels -fno-align-loops
TARGET_ARCH ?= -msse3
endif

ifeq ($(MODE), v120)
CONFIG_CPPFLAGS += -DTINY -DNDEBUG -DTRUSTWORTHY -DSUPPORT_VECTOR=120
CONFIG_CCFLAGS += -Os -fno-align-functions -fno-align-jumps -fno-align-labels -fno-align-loops
TARGET_ARCH ?= -msse3
endif

ifeq ($(MODE), v121)
CONFIG_CPPFLAGS += -DTINY -DNDEBUG -DTRUSTWORTHY -DSUPPORT_VECTOR=121
CONFIG_CCFLAGS += -Os -fno-align-functions -fno-align-jumps -fno-align-labels -fno-align-loops
TARGET_ARCH ?= -msse3
endif

ifeq ($(MODE), v122)
CONFIG_CPPFLAGS += -DTINY -DNDEBUG -DTRUSTWORTHY -DSUPPORT_VECTOR=122
CONFIG_CCFLAGS += -Os -fno-align-functions -fno-align-jumps -fno-align-labels -fno-align-loops
TARGET_ARCH ?= -msse3
endif

ifeq ($(MODE), v123)
CONFIG_CPPFLAGS += -DTINY -DNDEBUG -DTRUSTWORTHY -DSUPPORT_VECTOR=123
CONFIG_CCFLAGS += -Os -fno-align-functions -fno-align-jumps -fno-align-labels -fno-align-loops
TARGET_ARCH ?= -msse3
endif

ifeq ($(MODE), v124)
CONFIG_CPPFLAGS += -DTINY -DNDEBUG -DTRUSTWORTHY -DSUPPORT_VECTOR=124
CONFIG_CCFLAGS += -Os -fno-align-functions -fno-align-jumps -fno-align-labels -fno-align-loops
TARGET_ARCH ?= -msse3
endif

ifeq ($(MODE), v125)
CONFIG_CPPFLAGS += -DTINY -DNDEBUG -DTRUSTWORTHY -DSUPPORT_VECTOR=125
CONFIG_CCFLAGS += -Os -fno-align-functions -fno-align-jumps -fno-align-labels -fno-align-loops
TARGET_ARCH ?= -msse3
endif

ifeq ($(MODE), v126)
CONFIG_CPPFLAGS += -DTINY -DNDEBUG -DTRUSTWORTHY -DSUPPORT_VECTOR=126
CONFIG_CCFLAGS += -Os -fno-align-functions -fno-align-jumps -fno-align-labels -fno-align-loops
TARGET_ARCH ?= -msse3
endif

ifeq ($(MODE), v127)
CONFIG_CPPFLAGS += -DTINY -DNDEBUG -DTRUSTWORTHY -DSUPPORT_VECTOR=127
CONFIG_CCFLAGS += -Os -fno-align-functions -fno-align-jumps -fno-align-labels -fno-align-loops
TARGET_ARCH ?= -msse3
endif
