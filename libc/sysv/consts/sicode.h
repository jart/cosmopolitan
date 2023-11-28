#ifndef COSMOPOLITAN_LIBC_SYSV_CONSTS_SICODE_H_
#define COSMOPOLITAN_LIBC_SYSV_CONSTS_SICODE_H_
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

#define CLD_EXITED    1
#define CLD_KILLED    2
#define CLD_DUMPED    3
#define CLD_TRAPPED   4
#define CLD_STOPPED   5
#define CLD_CONTINUED 6
#define TRAP_BRKPT    1
#define TRAP_TRACE    2
#define SEGV_MAPERR   1
#define SEGV_ACCERR   2
#define ILL_ILLOPC    1
#define ILL_PRVREG    6
#define ILL_COPROC    7
#define ILL_BADSTK    8
#define BUS_ADRALN    1
#define BUS_ADRERR    2
#define BUS_OBJERR    3
#define POLL_IN       1
#define POLL_OUT      2
#define POLL_MSG      3
#define POLL_ERR      4
#define POLL_PRI      5
#define POLL_HUP      6

#define SI_USER           SI_USER
#define SI_QUEUE          SI_QUEUE
#define SI_TIMER          SI_TIMER
#define SI_MESGQ          SI_MESGQ
#define SI_ASYNCIO        SI_ASYNCIO
#define SI_TKILL          SI_TKILL
#define SI_ASYNCNL        SI_ASYNCNL
#define SI_KERNEL         SI_KERNEL
#define SI_NOINFO         SI_NOINFO
#define SEGV_PKUERR       SEGV_PKUERR
#define FPE_INTDIV        FPE_INTDIV
#define FPE_INTOVF        FPE_INTOVF
#define FPE_FLTDIV        FPE_FLTDIV
#define FPE_FLTOVF        FPE_FLTOVF
#define FPE_FLTUND        FPE_FLTUND
#define FPE_FLTRES        FPE_FLTRES
#define FPE_FLTINV        FPE_FLTINV
#define FPE_FLTSUB        FPE_FLTSUB
#define ILL_ILLOPN        ILL_ILLOPN
#define ILL_ILLADR        ILL_ILLADR
#define ILL_ILLTRP        ILL_ILLTRP
#define ILL_PRVOPC        ILL_PRVOPC
#define BUS_OOMERR        BUS_OOMERR
#define BUS_MCEERR_AR     BUS_MCEERR_AR
#define BUS_MCEERR_AO     BUS_MCEERR_AO
#define SYS_SECCOMP       SYS_SECCOMP
#define SYS_USER_DISPATCH SYS_USER_DISPATCH


COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_SYSV_CONSTS_SICODE_H_ */
