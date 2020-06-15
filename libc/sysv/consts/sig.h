#ifndef COSMOPOLITAN_LIBC_SYSV_CONSTS_SIG_H_
#define COSMOPOLITAN_LIBC_SYSV_CONSTS_SIG_H_
#include "libc/runtime/symbolic.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

hidden extern const long SIGABRT;
hidden extern const long SIGALRM;
hidden extern const long SIGBUS;
hidden extern const long SIGCHLD;
hidden extern const long SIGCONT;
hidden extern const long SIGFPE;
hidden extern const long SIGHUP;
hidden extern const long SIGILL;
hidden extern const long SIGINT;
hidden extern const long SIGIO;
hidden extern const long SIGIOT;
hidden extern const long SIGKILL;
hidden extern const long SIGPIPE;
hidden extern const long SIGPOLL;
hidden extern const long SIGPROF;
hidden extern const long SIGPWR;
hidden extern const long SIGQUIT;
hidden extern const long SIGRTMAX;
hidden extern const long SIGRTMIN;
hidden extern const long SIGSEGV;
hidden extern const long SIGSTKFLT;
hidden extern const long SIGSTKSZ;
hidden extern const long SIGSTOP;
hidden extern const long SIGSYS;
hidden extern const long SIGTERM;
hidden extern const long SIGTRAP;
hidden extern const long SIGTSTP;
hidden extern const long SIGTTIN;
hidden extern const long SIGTTOU;
hidden extern const long SIGUNUSED;
hidden extern const long SIGURG;
hidden extern const long SIGUSR1;
hidden extern const long SIGUSR2;
hidden extern const long SIGVTALRM;
hidden extern const long SIGWINCH;
hidden extern const long SIGXCPU;
hidden extern const long SIGXFSZ;

hidden extern const long SIG_ATOMIC_MIN;
hidden extern const long SIG_BLOCK;
hidden extern const long SIG_SETMASK;
hidden extern const long SIG_UNBLOCK;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */

#define SIGABRT LITERALLY(6)
#define SIGALRM LITERALLY(14)
#define SIGBUS SYMBOLIC(SIGBUS)
#define SIGCHLD SYMBOLIC(SIGCHLD)
#define SIGCONT SYMBOLIC(SIGCONT)
#define SIGFPE LITERALLY(8)
#define SIGHUP LITERALLY(1)
#define SIGILL LITERALLY(4)
#define SIGINT LITERALLY(2)
#define SIGIO SYMBOLIC(SIGIO)
#define SIGIOT LITERALLY(6)
#define SIGKILL LITERALLY(9)
#define SIGPIPE LITERALLY(13)
#define SIGPOLL SYMBOLIC(SIGPOLL)
#define SIGPROF LITERALLY(27)
#define SIGPWR SYMBOLIC(SIGPWR)
#define SIGQUIT LITERALLY(3)
#define SIGRTMAX SYMBOLIC(SIGRTMAX)
#define SIGRTMIN SYMBOLIC(SIGRTMIN)
#define SIGSEGV LITERALLY(11)
#define SIGSTKFLT SYMBOLIC(SIGSTKFLT)
#define SIGSTKSZ SYMBOLIC(SIGSTKSZ)
#define SIGSTOP SYMBOLIC(SIGSTOP)
#define SIGSYS SYMBOLIC(SIGSYS)
#define SIGTERM LITERALLY(15)
#define SIGTRAP LITERALLY(5)
#define SIGTSTP SYMBOLIC(SIGTSTP)
#define SIGTTIN LITERALLY(21)
#define SIGTTOU LITERALLY(22)
#define SIGUNUSED SYMBOLIC(SIGUNUSED)
#define SIGURG SYMBOLIC(SIGURG)
#define SIGUSR1 SYMBOLIC(SIGUSR1)
#define SIGUSR2 SYMBOLIC(SIGUSR2)
#define SIGVTALRM LITERALLY(26)
#define SIGWINCH LITERALLY(28)
#define SIGXCPU LITERALLY(24)
#define SIGXFSZ LITERALLY(25)

#define SIG_ATOMIC_MIN SYMBOLIC(SIG_ATOMIC_MIN)
#define SIG_BLOCK SYMBOLIC(SIG_BLOCK)
#define SIG_SETMASK SYMBOLIC(SIG_SETMASK)
#define SIG_UNBLOCK SYMBOLIC(SIG_UNBLOCK)

#endif /* COSMOPOLITAN_LIBC_SYSV_CONSTS_SIG_H_ */
