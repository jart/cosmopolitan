#ifndef COSMOPOLITAN_TOOL_PLINKO_LIB_CONFIG_H_
#define COSMOPOLITAN_TOOL_PLINKO_LIB_CONFIG_H_
#include "libc/dce.h"

#define HISTO_ASSOC     0
#define HISTO_GARBAGE   0
#define DEBUG_TREE      0
#define DEBUG_CLOSURE   0
#define DEBUG_GARBAGE   0
#define DEBUG_MATCHER   0
#define EXPLAIN_GARBAGE 0
#define AVERSIVENESS    15

#define NEED_GC  1
#define NEED_TMC 2
#define NEED_POP 4

#define STACK 65536

// we want a two power that's large enough for emoji but not
// not so large that we're drowning in virtual memory pages!
#define TERM 0x20000

COSMOPOLITAN_C_START_

#if IsModeDbg()
#define inline dontinline
#undef forceinline
#define forceinline static dontinline
#endif

#define BANE   INT_MIN
#define DWBITS (sizeof(dword) * CHAR_BIT)

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_TOOL_PLINKO_LIB_CONFIG_H_ */
