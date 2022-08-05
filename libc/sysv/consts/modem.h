#ifndef COSMOPOLITAN_LIBC_SYSV_CONSTS_MODEM_H_
#define COSMOPOLITAN_LIBC_SYSV_CONSTS_MODEM_H_
#include "libc/runtime/symbolic.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

extern const uint64_t TIOCMGET;
extern const uint64_t TIOCMSET;
extern const uint64_t TIOCMBIC;
extern const uint64_t TIOCMBIS;

extern const int TIOCM_CAR;
extern const int TIOCM_CD;
extern const int TIOCM_CTS;
extern const int TIOCM_DSR;
extern const int TIOCM_DTR;
extern const int TIOCM_LE;
extern const int TIOCM_RI;
extern const int TIOCM_RNG;
extern const int TIOCM_RTS;
extern const int TIOCM_SR;
extern const int TIOCM_ST;

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
