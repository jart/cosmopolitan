#ifndef COSMOPOLITAN_TOOL_PLINKO_LIB_CONFIG_H_
#define COSMOPOLITAN_TOOL_PLINKO_LIB_CONFIG_H_
#include <limits.h>

#define HISTO_ASSOC     0
#define HISTO_GARBAGE   0
#define DEBUG_TREE      0
#define DEBUG_CLOSURE   0
#define DEBUG_GARBAGE   0
#define DEBUG_MATCHER   0
#define EXPLAIN_GARBAGE 0
#define AVERSIVENESS    1000

#define NEED_GC  1
#define NEED_TMC 2
#define NEED_POP 4

#define STACK 65536
#define TERM  0x20000
#define HEAP  0x10000

#define DWBITS (sizeof(dword) * CHAR_BIT)

#endif /* COSMOPOLITAN_TOOL_PLINKO_LIB_CONFIG_H_ */
