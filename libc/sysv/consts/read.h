#ifndef COSMOPOLITAN_LIBC_SYSV_CONSTS_READ_H_
#define COSMOPOLITAN_LIBC_SYSV_CONSTS_READ_H_
#include "libc/runtime/symbolic.h"

#define READ_10 SYMBOLIC(READ_10)
#define READ_12 SYMBOLIC(READ_12)
#define READ_6 SYMBOLIC(READ_6)
#define READ_BLOCK_LIMITS SYMBOLIC(READ_BLOCK_LIMITS)
#define READ_BUFFER SYMBOLIC(READ_BUFFER)
#define READ_CAPACITY SYMBOLIC(READ_CAPACITY)
#define READ_DEFECT_DATA SYMBOLIC(READ_DEFECT_DATA)
#define READ_ELEMENT_STATUS SYMBOLIC(READ_ELEMENT_STATUS)
#define READ_LONG SYMBOLIC(READ_LONG)
#define READ_POSITION SYMBOLIC(READ_POSITION)
#define READ_REVERSE SYMBOLIC(READ_REVERSE)
#define READ_TOC SYMBOLIC(READ_TOC)

#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

hidden extern const long READ_10;
hidden extern const long READ_12;
hidden extern const long READ_6;
hidden extern const long READ_BLOCK_LIMITS;
hidden extern const long READ_BUFFER;
hidden extern const long READ_CAPACITY;
hidden extern const long READ_DEFECT_DATA;
hidden extern const long READ_ELEMENT_STATUS;
hidden extern const long READ_LONG;
hidden extern const long READ_POSITION;
hidden extern const long READ_REVERSE;
hidden extern const long READ_TOC;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_SYSV_CONSTS_READ_H_ */
