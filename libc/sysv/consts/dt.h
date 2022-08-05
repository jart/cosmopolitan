#ifndef COSMOPOLITAN_LIBC_SYSV_CONSTS_DT_H_
#define COSMOPOLITAN_LIBC_SYSV_CONSTS_DT_H_
#include "libc/runtime/symbolic.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

extern const uint8_t DT_UNKNOWN;
extern const uint8_t DT_FIFO;
extern const uint8_t DT_CHR;
extern const uint8_t DT_DIR;
extern const uint8_t DT_BLK;
extern const uint8_t DT_REG;
extern const uint8_t DT_LNK;
extern const uint8_t DT_SOCK;

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
