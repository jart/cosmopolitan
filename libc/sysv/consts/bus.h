#ifndef COSMOPOLITAN_LIBC_SYSV_CONSTS_BUS_H_
#define COSMOPOLITAN_LIBC_SYSV_CONSTS_BUS_H_
#include "libc/runtime/symbolic.h"

#define BUS_ADRALN SYMBOLIC(BUS_ADRALN)
#define BUS_ADRERR SYMBOLIC(BUS_ADRERR)
#define BUS_DEVICE_RESET SYMBOLIC(BUS_DEVICE_RESET)
#define BUS_MCEERR_AO SYMBOLIC(BUS_MCEERR_AO)
#define BUS_MCEERR_AR SYMBOLIC(BUS_MCEERR_AR)
#define BUS_OBJERR SYMBOLIC(BUS_OBJERR)

#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

hidden extern const long BUS_ADRALN;
hidden extern const long BUS_ADRERR;
hidden extern const long BUS_DEVICE_RESET;
hidden extern const long BUS_MCEERR_AO;
hidden extern const long BUS_MCEERR_AR;
hidden extern const long BUS_OBJERR;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_SYSV_CONSTS_BUS_H_ */
