#ifndef COSMOPOLITAN_LIBC_SYSV_CONSTS_MAP_H_
#define COSMOPOLITAN_LIBC_SYSV_CONSTS_MAP_H_
#include "libc/runtime/symbolic.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

hidden extern const long MAP_32BIT;
hidden extern const long MAP_ANON;
hidden extern const long MAP_ANONYMOUS;
hidden extern const long MAP_DENYWRITE;
hidden extern const long MAP_EXECUTABLE;
hidden extern const long MAP_FILE;
hidden extern const long MAP_FIXED;
hidden extern const long MAP_GROWSDOWN;
hidden extern const long MAP_HUGETLB;
hidden extern const long MAP_HUGE_MASK;
hidden extern const long MAP_HUGE_SHIFT;
hidden extern const long MAP_LOCKED;
hidden extern const long MAP_NONBLOCK;
hidden extern const long MAP_NORESERVE;
hidden extern const long MAP_POPULATE;
hidden extern const long MAP_PRIVATE;
hidden extern const long MAP_SHARED;
hidden extern const long MAP_STACK;
hidden extern const long MAP_TYPE;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */

#define MAP_FILE    0
#define MAP_SHARED  1
#define MAP_PRIVATE 2
#define MAP_FIXED   16

#define MAP_32BIT      SYMBOLIC(MAP_32BIT)
#define MAP_ANONYMOUS  SYMBOLIC(MAP_ANONYMOUS)
#define MAP_DENYWRITE  SYMBOLIC(MAP_DENYWRITE)
#define MAP_EXECUTABLE SYMBOLIC(MAP_EXECUTABLE)
#define MAP_GROWSDOWN  SYMBOLIC(MAP_GROWSDOWN)
#define MAP_HUGETLB    SYMBOLIC(MAP_HUGETLB)
#define MAP_HUGE_MASK  SYMBOLIC(MAP_HUGE_MASK)
#define MAP_HUGE_SHIFT SYMBOLIC(MAP_HUGE_SHIFT)
#define MAP_LOCKED     SYMBOLIC(MAP_LOCKED)
#define MAP_NONBLOCK   SYMBOLIC(MAP_NONBLOCK)
#define MAP_NORESERVE  SYMBOLIC(MAP_NORESERVE)
#define MAP_POPULATE   SYMBOLIC(MAP_POPULATE)
#define MAP_TYPE       SYMBOLIC(MAP_TYPE)

#define MAP_ANON  MAP_ANONYMOUS
#define MAP_STACK MAP_GROWSDOWN

#endif /* COSMOPOLITAN_LIBC_SYSV_CONSTS_MAP_H_ */
