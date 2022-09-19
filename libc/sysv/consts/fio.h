#ifndef COSMOPOLITAN_LIBC_SYSV_CONSTS_FIO_H_
#define COSMOPOLITAN_LIBC_SYSV_CONSTS_FIO_H_
#include "libc/runtime/symbolic.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

extern const uint32_t FIOASYNC;
extern const uint32_t FIOCLEX;
extern const uint32_t FIONBIO;
extern const uint32_t FIONCLEX;
extern const uint32_t FIONREAD;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */

#define FIOASYNC SYMBOLIC(FIOASYNC)
#define FIOCLEX  SYMBOLIC(FIOCLEX)
#define FIONBIO  SYMBOLIC(FIONBIO)
#define FIONCLEX SYMBOLIC(FIONCLEX)
#define FIONREAD SYMBOLIC(FIONREAD)

#endif /* COSMOPOLITAN_LIBC_SYSV_CONSTS_FIO_H_ */
