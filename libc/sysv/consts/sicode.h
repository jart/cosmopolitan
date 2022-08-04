#ifndef COSMOPOLITAN_LIBC_SYSV_CONSTS_SICODE_H_
#define COSMOPOLITAN_LIBC_SYSV_CONSTS_SICODE_H_
#include "libc/runtime/symbolic.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

extern const int32_t SI_USER;
extern const int32_t SI_QUEUE;
extern const int32_t SI_TIMER;
extern const int32_t SI_MESGQ;
extern const int32_t SI_ASYNCIO;
extern const int32_t SI_TKILL;
extern const int32_t SI_ASYNCNL;
extern const int32_t SI_KERNEL;
extern const int32_t SI_NOINFO;
extern const int32_t CLD_EXITED;
extern const int32_t CLD_KILLED;
extern const int32_t CLD_DUMPED;
extern const int32_t CLD_TRAPPED;
extern const int32_t CLD_STOPPED;
extern const int32_t CLD_CONTINUED;
extern const int32_t TRAP_BRKPT;
extern const int32_t TRAP_TRACE;
extern const int32_t SEGV_MAPERR;
extern const int32_t SEGV_ACCERR;
extern const int32_t SEGV_PKUERR;
extern const int32_t FPE_INTDIV;
extern const int32_t FPE_INTOVF;
extern const int32_t FPE_FLTDIV;
extern const int32_t FPE_FLTOVF;
extern const int32_t FPE_FLTUND;
extern const int32_t FPE_FLTRES;
extern const int32_t FPE_FLTINV;
extern const int32_t FPE_FLTSUB;
extern const int32_t ILL_ILLOPC;
extern const int32_t ILL_ILLOPN;
extern const int32_t ILL_ILLADR;
extern const int32_t ILL_ILLTRP;
extern const int32_t ILL_PRVOPC;
extern const int32_t ILL_PRVREG;
extern const int32_t ILL_COPROC;
extern const int32_t ILL_BADSTK;
extern const int32_t BUS_ADRALN;
extern const int32_t BUS_ADRERR;
extern const int32_t BUS_OBJERR;
extern const int32_t BUS_MCEERR_AR;
extern const int32_t BUS_MCEERR_AO;
extern const int32_t BUS_OOMERR;
extern const int32_t POLL_IN;
extern const int32_t POLL_OUT;
extern const int32_t POLL_MSG;
extern const int32_t POLL_ERR;
extern const int32_t POLL_PRI;
extern const int32_t POLL_HUP;
extern const int32_t SYS_SECCOMP;
extern const int32_t SYS_USER_DISPATCH;

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

#define SI_USER           SYMBOLIC(SI_USER)
#define SI_QUEUE          SYMBOLIC(SI_QUEUE)
#define SI_TIMER          SYMBOLIC(SI_TIMER)
#define SI_MESGQ          SYMBOLIC(SI_MESGQ)
#define SI_ASYNCIO        SYMBOLIC(SI_ASYNCIO)
#define SI_TKILL          SYMBOLIC(SI_TKILL)
#define SI_ASYNCNL        SYMBOLIC(SI_ASYNCNL)
#define SI_KERNEL         SYMBOLIC(SI_KERNEL)
#define SI_NOINFO         SYMBOLIC(SI_NOINFO)
#define SEGV_PKUERR       SYMBOLIC(SEGV_PKUERR)
#define FPE_INTDIV        SYMBOLIC(FPE_INTDIV)
#define FPE_INTOVF        SYMBOLIC(FPE_INTOVF)
#define FPE_FLTDIV        SYMBOLIC(FPE_FLTDIV)
#define FPE_FLTOVF        SYMBOLIC(FPE_FLTOVF)
#define FPE_FLTUND        SYMBOLIC(FPE_FLTUND)
#define FPE_FLTRES        SYMBOLIC(FPE_FLTRES)
#define FPE_FLTINV        SYMBOLIC(FPE_FLTINV)
#define FPE_FLTSUB        SYMBOLIC(FPE_FLTSUB)
#define ILL_ILLOPN        SYMBOLIC(ILL_ILLOPN)
#define ILL_ILLADR        SYMBOLIC(ILL_ILLADR)
#define ILL_ILLTRP        SYMBOLIC(ILL_ILLTRP)
#define ILL_PRVOPC        SYMBOLIC(ILL_PRVOPC)
#define BUS_OOMERR        SYMBOLIC(BUS_OOMERR)
#define BUS_MCEERR_AR     SYMBOLIC(BUS_MCEERR_AR)
#define BUS_MCEERR_AO     SYMBOLIC(BUS_MCEERR_AO)
#define SYS_SECCOMP       SYMBOLIC(SYS_SECCOMP)
#define SYS_USER_DISPATCH SYMBOLIC(SYS_USER_DISPATCH)

#endif /* COSMOPOLITAN_LIBC_SYSV_CONSTS_SICODE_H_ */
