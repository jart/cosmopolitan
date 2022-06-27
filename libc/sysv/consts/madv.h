#ifndef COSMOPOLITAN_LIBC_SYSV_CONSTS_MADV_H_
#define COSMOPOLITAN_LIBC_SYSV_CONSTS_MADV_H_
#include "libc/runtime/symbolic.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

extern const unsigned MADV_DODUMP;
extern const unsigned MADV_DOFORK;
extern const unsigned MADV_DONTDUMP;
extern const unsigned MADV_DONTFORK;
extern const unsigned MADV_DONTNEED;
extern const unsigned MADV_FREE;
extern const unsigned MADV_HUGEPAGE;
extern const unsigned MADV_HWPOISON;
extern const unsigned MADV_MERGEABLE;
extern const unsigned MADV_NOHUGEPAGE;
extern const unsigned MADV_NORMAL;
extern const unsigned MADV_RANDOM;
extern const unsigned MADV_REMOVE;
extern const unsigned MADV_SEQUENTIAL;
extern const unsigned MADV_UNMERGEABLE;
extern const unsigned MADV_WILLNEED;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */

#define MADV_NORMAL     LITERALLY(0)
#define MADV_RANDOM     LITERALLY(1)
#define MADV_SEQUENTIAL LITERALLY(2)
#define MADV_WILLNEED   LITERALLY(3)

#define MADV_DODUMP      SYMBOLIC(MADV_DODUMP)
#define MADV_DOFORK      SYMBOLIC(MADV_DOFORK)
#define MADV_DONTDUMP    SYMBOLIC(MADV_DONTDUMP)
#define MADV_DONTFORK    SYMBOLIC(MADV_DONTFORK)
#define MADV_DONTNEED    SYMBOLIC(MADV_DONTNEED)
#define MADV_FREE        SYMBOLIC(MADV_FREE)
#define MADV_HUGEPAGE    SYMBOLIC(MADV_HUGEPAGE)
#define MADV_HWPOISON    SYMBOLIC(MADV_HWPOISON)
#define MADV_MERGEABLE   SYMBOLIC(MADV_MERGEABLE)
#define MADV_NOHUGEPAGE  SYMBOLIC(MADV_NOHUGEPAGE)
#define MADV_REMOVE      SYMBOLIC(MADV_REMOVE)
#define MADV_UNMERGEABLE SYMBOLIC(MADV_UNMERGEABLE)

#endif /* COSMOPOLITAN_LIBC_SYSV_CONSTS_MADV_H_ */
