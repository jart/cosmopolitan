#ifndef COSMOPOLITAN_LIBC_SYSV_CONSTS_MODEM_H_
#define COSMOPOLITAN_LIBC_SYSV_CONSTS_MODEM_H_
#include "libc/runtime/symbolic.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

hidden extern const long TIOCMGET;
hidden extern const long TIOCMSET;
hidden extern const long TIOCMBIC;
hidden extern const long TIOCMBIS;

hidden extern const long TIOCM_CAR;
hidden extern const long TIOCM_CD;
hidden extern const long TIOCM_CTS;
hidden extern const long TIOCM_DSR;
hidden extern const long TIOCM_DTR;
hidden extern const long TIOCM_LE;
hidden extern const long TIOCM_RI;
hidden extern const long TIOCM_RNG;
hidden extern const long TIOCM_RTS;
hidden extern const long TIOCM_SR;
hidden extern const long TIOCM_ST;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */

#define TIOCMGET SYMBOLIC(TIOCMGET)
#define TIOCMSET SYMBOLIC(TIOCMSET)
#define TIOCMBIC SYMBOLIC(TIOCMBIC)
#define TIOCMBIS SYMBOLIC(TIOCMBIS)

#define TIOCM_LE  LITERALLY(0b0000000000000001)
#define TIOCM_DTR LITERALLY(0b0000000000000010)
#define TIOCM_RTS LITERALLY(0b0000000000000100)
#define TIOCM_ST  LITERALLY(0b0000000000001000)
#define TIOCM_SR  LITERALLY(0b0000000000010000)
#define TIOCM_CTS LITERALLY(0b0000000000100000)
#define TIOCM_CAR LITERALLY(0b0000000001000000)
#define TIOCM_CD  LITERALLY(0b0000000001000000)
#define TIOCM_RI  LITERALLY(0b0000000010000000)
#define TIOCM_RNG LITERALLY(0b0000000010000000)
#define TIOCM_DSR LITERALLY(0b0000000100000000)

#endif /* COSMOPOLITAN_LIBC_SYSV_CONSTS_MODEM_H_ */
