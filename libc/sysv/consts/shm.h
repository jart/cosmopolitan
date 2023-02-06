#ifndef COSMOPOLITAN_LIBC_SYSV_CONSTS_SHM_H_
#define COSMOPOLITAN_LIBC_SYSV_CONSTS_SHM_H_
#include "libc/runtime/symbolic.h"

#define SHM_ANON      SYMBOLIC(SHM_ANON)
#define SHM_DEST      SYMBOLIC(SHM_DEST)
#define SHM_EXEC      SYMBOLIC(SHM_EXEC)
#define SHM_HUGETLB   SYMBOLIC(SHM_HUGETLB)
#define SHM_INFO      SYMBOLIC(SHM_INFO)
#define SHM_LOCK      SYMBOLIC(SHM_LOCK)
#define SHM_LOCKED    SYMBOLIC(SHM_LOCKED)
#define SHM_NORESERVE SYMBOLIC(SHM_NORESERVE)
#define SHM_R         SYMBOLIC(SHM_R)
#define SHM_RDONLY    SYMBOLIC(SHM_RDONLY)
#define SHM_REMAP     SYMBOLIC(SHM_REMAP)
#define SHM_RND       SYMBOLIC(SHM_RND)
#define SHM_STAT      SYMBOLIC(SHM_STAT)
#define SHM_UNLOCK    SYMBOLIC(SHM_UNLOCK)
#define SHM_W         SYMBOLIC(SHM_W)

#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

extern const char *SHM_ANON;
extern const int SHM_DEST;
extern const int SHM_EXEC;
extern const int SHM_HUGETLB;
extern const int SHM_INFO;
extern const int SHM_LOCK;
extern const int SHM_LOCKED;
extern const int SHM_NORESERVE;
extern const int SHM_R;
extern const int SHM_RDONLY;
extern const int SHM_REMAP;
extern const int SHM_RND;
extern const int SHM_STAT;
extern const int SHM_UNLOCK;
extern const int SHM_W;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_SYSV_CONSTS_SHM_H_ */
