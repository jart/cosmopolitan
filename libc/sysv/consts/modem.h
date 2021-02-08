#ifndef COSMOPOLITAN_LIBC_SYSV_CONSTS_MODEM_H_
#define COSMOPOLITAN_LIBC_SYSV_CONSTS_MODEM_H_
#include "libc/runtime/symbolic.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

extern const long TIOCMGET;
extern const long TIOCMSET;
extern const long TIOCMBIC;
extern const long TIOCMBIS;

extern const long TIOCM_CAR;
extern const long TIOCM_CD;
extern const long TIOCM_CTS;
extern const long TIOCM_DSR;
extern const long TIOCM_DTR;
extern const long TIOCM_LE;
extern const long TIOCM_RI;
extern const long TIOCM_RNG;
extern const long TIOCM_RTS;
extern const long TIOCM_SR;
extern const long TIOCM_ST;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */

#define TIOCMGET SYMBOLIC(TIOCMGET)
#define TIOCMSET SYMBOLIC(TIOCMSET)
#define TIOCMBIC SYMBOLIC(TIOCMBIC)
#define TIOCMBIS SYMBOLIC(TIOCMBIS)

#define TIOCM_LE  LITERALLY(0x01)
#define TIOCM_DTR LITERALLY(0x02)
#define TIOCM_RTS LITERALLY(0x04)
#define TIOCM_ST  LITERALLY(0x08)
#define TIOCM_SR  LITERALLY(0x10)
#define TIOCM_CTS LITERALLY(0x20)
#define TIOCM_CAR LITERALLY(0x40)
#define TIOCM_CD  LITERALLY(0x40)
#define TIOCM_RI  LITERALLY(0x80)
#define TIOCM_RNG LITERALLY(0x80)
#define TIOCM_DSR LITERALLY(0x0100)

#endif /* COSMOPOLITAN_LIBC_SYSV_CONSTS_MODEM_H_ */
