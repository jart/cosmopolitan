#ifndef COSMOPOLITAN_LIBC_SYSV_CONSTS_N_H_
#define COSMOPOLITAN_LIBC_SYSV_CONSTS_N_H_
#include "libc/runtime/symbolic.h"

#define N_6PACK SYMBOLIC(N_6PACK)
#define N_AX25 SYMBOLIC(N_AX25)
#define N_HCI SYMBOLIC(N_HCI)
#define N_HDLC SYMBOLIC(N_HDLC)
#define N_IRDA SYMBOLIC(N_IRDA)
#define N_MASC SYMBOLIC(N_MASC)
#define N_MOUSE SYMBOLIC(N_MOUSE)
#define N_PPP SYMBOLIC(N_PPP)
#define N_PROFIBUS_FDL SYMBOLIC(N_PROFIBUS_FDL)
#define N_R3964 SYMBOLIC(N_R3964)
#define N_SLIP SYMBOLIC(N_SLIP)
#define N_SMSBLOCK SYMBOLIC(N_SMSBLOCK)
#define N_STRIP SYMBOLIC(N_STRIP)
#define N_SYNC_PPP SYMBOLIC(N_SYNC_PPP)
#define N_TTY SYMBOLIC(N_TTY)
#define N_X25 SYMBOLIC(N_X25)

#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

hidden extern const long N_6PACK;
hidden extern const long N_AX25;
hidden extern const long N_HCI;
hidden extern const long N_HDLC;
hidden extern const long N_IRDA;
hidden extern const long N_MASC;
hidden extern const long N_MOUSE;
hidden extern const long N_PPP;
hidden extern const long N_PROFIBUS_FDL;
hidden extern const long N_R3964;
hidden extern const long N_SLIP;
hidden extern const long N_SMSBLOCK;
hidden extern const long N_STRIP;
hidden extern const long N_SYNC_PPP;
hidden extern const long N_TTY;
hidden extern const long N_X25;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_SYSV_CONSTS_N_H_ */
