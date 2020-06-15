#ifndef COSMOPOLITAN_LIBC_SYSV_CONSTS_BLK_H_
#define COSMOPOLITAN_LIBC_SYSV_CONSTS_BLK_H_
#include "libc/runtime/symbolic.h"

#define BLK_BYTECOUNT SYMBOLIC(BLK_BYTECOUNT)
#define BLK_EOF SYMBOLIC(BLK_EOF)
#define BLK_EOR SYMBOLIC(BLK_EOR)
#define BLK_ERRORS SYMBOLIC(BLK_ERRORS)
#define BLK_RESTART SYMBOLIC(BLK_RESTART)

#define BLKBSZGET SYMBOLIC(BLKBSZGET)
#define BLKBSZSET SYMBOLIC(BLKBSZSET)
#define BLKFLSBUF SYMBOLIC(BLKFLSBUF)
#define BLKFRAGET SYMBOLIC(BLKFRAGET)
#define BLKFRASET SYMBOLIC(BLKFRASET)
#define BLKGETSIZE SYMBOLIC(BLKGETSIZE)
#define BLKGETSIZE64 SYMBOLIC(BLKGETSIZE64)
#define BLKRAGET SYMBOLIC(BLKRAGET)
#define BLKRASET SYMBOLIC(BLKRASET)
#define BLKROGET SYMBOLIC(BLKROGET)
#define BLKROSET SYMBOLIC(BLKROSET)
#define BLKRRPART SYMBOLIC(BLKRRPART)
#define BLKSECTGET SYMBOLIC(BLKSECTGET)
#define BLKSECTSET SYMBOLIC(BLKSECTSET)
#define BLKSSZGET SYMBOLIC(BLKSSZGET)
#define BLKTYPE SYMBOLIC(BLKTYPE)

#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

hidden extern const long BLK_BYTECOUNT;
hidden extern const long BLK_EOF;
hidden extern const long BLK_EOR;
hidden extern const long BLK_ERRORS;
hidden extern const long BLK_RESTART;

hidden extern const long BLKBSZGET;
hidden extern const long BLKBSZSET;
hidden extern const long BLKFLSBUF;
hidden extern const long BLKFRAGET;
hidden extern const long BLKFRASET;
hidden extern const long BLKGETSIZE64;
hidden extern const long BLKGETSIZE;
hidden extern const long BLKRAGET;
hidden extern const long BLKRASET;
hidden extern const long BLKROGET;
hidden extern const long BLKROSET;
hidden extern const long BLKRRPART;
hidden extern const long BLKSECTGET;
hidden extern const long BLKSECTSET;
hidden extern const long BLKSSZGET;
hidden extern const long BLKTYPE;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_SYSV_CONSTS_BLK_H_ */
