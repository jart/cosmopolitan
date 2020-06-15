#ifndef COSMOPOLITAN_LIBC_SYSV_CONSTS_DT_H_
#define COSMOPOLITAN_LIBC_SYSV_CONSTS_DT_H_
#include "libc/runtime/symbolic.h"

#define DT_BLK SYMBOLIC(DT_BLK)
#define DT_CHR SYMBOLIC(DT_CHR)
#define DT_DIR SYMBOLIC(DT_DIR)
#define DT_FIFO SYMBOLIC(DT_FIFO)
#define DT_LNK SYMBOLIC(DT_LNK)
#define DT_REG SYMBOLIC(DT_REG)
#define DT_SOCK SYMBOLIC(DT_SOCK)
#define DT_UNKNOWN SYMBOLIC(DT_UNKNOWN)

#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

hidden extern const long DT_BLK;
hidden extern const long DT_CHR;
hidden extern const long DT_DIR;
hidden extern const long DT_FIFO;
hidden extern const long DT_LNK;
hidden extern const long DT_REG;
hidden extern const long DT_SOCK;
hidden extern const long DT_UNKNOWN;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_SYSV_CONSTS_DT_H_ */
