#ifndef COSMOPOLITAN_LIBC_SYSV_CONSTS_SHM_H_
#define COSMOPOLITAN_LIBC_SYSV_CONSTS_SHM_H_
#include "libc/runtime/symbolic.h"

#define SHM_DEST SYMBOLIC(SHM_DEST)
#define SHM_EXEC SYMBOLIC(SHM_EXEC)
#define SHM_HUGETLB SYMBOLIC(SHM_HUGETLB)
#define SHM_INFO SYMBOLIC(SHM_INFO)
#define SHM_LOCK SYMBOLIC(SHM_LOCK)
#define SHM_LOCKED SYMBOLIC(SHM_LOCKED)
#define SHM_NORESERVE SYMBOLIC(SHM_NORESERVE)
#define SHM_R SYMBOLIC(SHM_R)
#define SHM_RDONLY SYMBOLIC(SHM_RDONLY)
#define SHM_REMAP SYMBOLIC(SHM_REMAP)
#define SHM_RND SYMBOLIC(SHM_RND)
#define SHM_STAT SYMBOLIC(SHM_STAT)
#define SHM_UNLOCK SYMBOLIC(SHM_UNLOCK)
#define SHM_W SYMBOLIC(SHM_W)

#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

hidden extern const long SHM_DEST;
hidden extern const long SHM_EXEC;
hidden extern const long SHM_HUGETLB;
hidden extern const long SHM_INFO;
hidden extern const long SHM_LOCK;
hidden extern const long SHM_LOCKED;
hidden extern const long SHM_NORESERVE;
hidden extern const long SHM_R;
hidden extern const long SHM_RDONLY;
hidden extern const long SHM_REMAP;
hidden extern const long SHM_RND;
hidden extern const long SHM_STAT;
hidden extern const long SHM_UNLOCK;
hidden extern const long SHM_W;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_SYSV_CONSTS_SHM_H_ */
