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
	-O3

#TARGET_ARCH ?=			\
	-msse3

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
# Folks who want it deserve to get it, good and hard.

ifeq ($(MODE), ansi)

CONFIG_CCFLAGS +=		\
	-std=c11		\
	-Og

endif
