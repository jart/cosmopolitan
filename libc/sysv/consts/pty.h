#ifndef COSMOPOLITAN_LIBC_SYSV_CONSTS_PTY_H_
#define COSMOPOLITAN_LIBC_SYSV_CONSTS_PTY_H_
#include "libc/runtime/symbolic.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

extern const long TIOCPKT;
extern const long TIOCPKT_DATA;
extern const long TIOCPKT_DOSTOP;
extern const long TIOCPKT_FLUSHREAD;
extern const long TIOCPKT_FLUSHWRITE;
extern const long TIOCPKT_IOCTL;
extern const long TIOCPKT_NOSTOP;
extern const long TIOCPKT_START;
extern const long TIOCPKT_STOP;
extern const long TIOCSPTLCK;
extern const long PTMGET;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */

#define TIOCPKT            SYMBOLIC(TIOCPKT)
#define TIOCSPTLCK         SYMBOLIC(TIOCSPTLCK)
#define PTMGET             SYMBOLIC(PTMGET)
#define TIOCPKT_DATA       LITERALLY(0x00)
#define TIOCPKT_DOSTOP     LITERALLY(0x01)
#define TIOCPKT_FLUSHREAD  LITERALLY(0x02)
#define TIOCPKT_FLUSHWRITE LITERALLY(0x04)
#define TIOCPKT_IOCTL      LITERALLY(0x08)
#define TIOCPKT_NOSTOP     LITERALLY(0x10)
#define TIOCPKT_START      LITERALLY(0x20)
#define TIOCPKT_STOP       LITERALLY(0x40)

#endif /* COSMOPOLITAN_LIBC_SYSV_CONSTS_PTY_H_ */
