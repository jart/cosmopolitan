#ifndef COSMOPOLITAN_LIBC_SYSV_CONSTS_DT_H_
#define COSMOPOLITAN_LIBC_SYSV_CONSTS_DT_H_
#include "libc/runtime/symbolic.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

extern const long DT_UNKNOWN;
extern const long DT_FIFO;
extern const long DT_CHR;
extern const long DT_DIR;
extern const long DT_BLK;
extern const long DT_REG;
extern const long DT_LNK;
extern const long DT_SOCK;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */

#define DT_UNKNOWN LITERALLY(0)
#define DT_FIFO    LITERALLY(1)
#define DT_CHR     LITERALLY(2)
#define DT_DIR     LITERALLY(4)
#define DT_BLK     LITERALLY(6)
#define DT_REG     LITERALLY(8)
#define DT_LNK     LITERALLY(10)
#define DT_SOCK    LITERALLY(12)

#endif /* COSMOPOLITAN_LIBC_SYSV_CONSTS_DT_H_ */
