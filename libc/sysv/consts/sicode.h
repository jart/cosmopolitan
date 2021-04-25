#ifndef COSMOPOLITAN_LIBC_SYSV_CONSTS_SICODE_H_
#define COSMOPOLITAN_LIBC_SYSV_CONSTS_SICODE_H_
#include "libc/runtime/symbolic.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

extern const long SI_USER;
extern const long SI_QUEUE;
extern const long SI_TIMER;
extern const long SI_MESGQ;
extern const long SI_ASYNCIO;
extern const long SI_TKILL;
extern const long SI_ASYNCNL;
extern const long SI_KERNEL;
extern const long SI_NOINFO;
extern const long CLD_EXITED;
extern const long CLD_KILLED;
extern const long CLD_DUMPED;
extern const long CLD_TRAPPED;
extern const long CLD_STOPPED;
extern const long CLD_CONTINUED;
extern const long TRAP_BRKPT;
extern const long TRAP_TRACE;
extern const long SEGV_MAPERR;
extern const long SEGV_ACCERR;
extern const long FPE_INTDIV;
extern const long FPE_INTOVF;
extern const long FPE_FLTDIV;
extern const long FPE_FLTOVF;
extern const long FPE_FLTUND;
extern const long FPE_FLTRES;
extern const long FPE_FLTINV;
extern const long FPE_FLTSUB;
extern const long ILL_ILLOPC;
extern const long ILL_ILLOPN;
extern const long ILL_ILLADR;
extern const long ILL_ILLTRP;
extern const long ILL_PRVOPC;
extern const long ILL_PRVREG;
extern const long ILL_COPROC;
extern const long ILL_BADSTK;
extern const long BUS_ADRALN;
extern const long BUS_ADRERR;
extern const long BUS_OBJERR;
extern const long BUS_MCEERR_AR;
extern const long BUS_MCEERR_AO;
extern const long POLL_IN;
extern const long POLL_OUT;
extern const long POLL_MSG;
extern const long POLL_ERR;
extern const long POLL_PRI;
extern const long POLL_HUP;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */

#define CLD_EXITED    LITERALLY(1)
#define CLD_KILLED    LITERALLY(2)
#define CLD_DUMPED    LITERALLY(3)
#define CLD_TRAPPED   LITERALLY(4)
#define CLD_STOPPED   LITERALLY(5)
#define CLD_CONTINUED LITERALLY(6)
#define TRAP_BRKPT    LITERALLY(1)
#define TRAP_TRACE    LITERALLY(2)
#define SEGV_MAPERR   LITERALLY(1)
#define SEGV_ACCERR   LITERALLY(2)
#define ILL_ILLOPC    LITERALLY(1)
#define ILL_PRVREG    LITERALLY(6)
#define ILL_COPROC    LITERALLY(7)
#define ILL_BADSTK    LITERALLY(8)
#define BUS_ADRALN    LITERALLY(1)
#define BUS_ADRERR    LITERALLY(2)
#define BUS_OBJERR    LITERALLY(3)
#define POLL_IN       LITERALLY(1)
#define POLL_OUT      LITERALLY(2)
#define POLL_MSG      LITERALLY(3)
#define POLL_ERR      LITERALLY(4)
#define POLL_PRI      LITERALLY(5)
#define POLL_HUP      LITERALLY(6)

#define SI_USER       SYMBOLIC(SI_USER)
#define SI_QUEUE      SYMBOLIC(SI_QUEUE)
#define SI_TIMER      SYMBOLIC(SI_TIMER)
#define SI_MESGQ      SYMBOLIC(SI_MESGQ)
#define SI_ASYNCIO    SYMBOLIC(SI_ASYNCIO)
#define SI_TKILL      SYMBOLIC(SI_TKILL)
#define SI_ASYNCNL    SYMBOLIC(SI_ASYNCNL)
#define SI_KERNEL     SYMBOLIC(SI_KERNEL)
#define SI_NOINFO     SYMBOLIC(SI_NOINFO)
#define FPE_INTDIV    SYMBOLIC(FPE_INTDIV)
#define FPE_INTOVF    SYMBOLIC(FPE_INTOVF)
#define FPE_FLTDIV    SYMBOLIC(FPE_FLTDIV)
#define FPE_FLTOVF    SYMBOLIC(FPE_FLTOVF)
#define FPE_FLTUND    SYMBOLIC(FPE_FLTUND)
#define FPE_FLTRES    SYMBOLIC(FPE_FLTRES)
#define FPE_FLTINV    SYMBOLIC(FPE_FLTINV)
#define FPE_FLTSUB    SYMBOLIC(FPE_FLTSUB)
#define ILL_ILLOPN    SYMBOLIC(ILL_ILLOPN)
#define ILL_ILLADR    SYMBOLIC(ILL_ILLADR)
#define ILL_ILLTRP    SYMBOLIC(ILL_ILLTRP)
#define ILL_PRVOPC    SYMBOLIC(ILL_PRVOPC)
#define BUS_MCEERR_AR SYMBOLIC(BUS_MCEERR_AR)
#define BUS_MCEERR_AO SYMBOLIC(BUS_MCEERR_AO)

#endif /* COSMOPOLITAN_LIBC_SYSV_CONSTS_SICODE_H_ */
