#ifndef COSMOPOLITAN_LIBC_SYSV_CONSTS_SIG_H_
#define COSMOPOLITAN_LIBC_SYSV_CONSTS_SIG_H_
#include "libc/runtime/symbolic.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

extern const long SIGABRT;
extern const long SIGALRM;
extern const long SIGBUS;
extern const long SIGCHLD;
extern const long SIGCONT;
extern const long SIGEMT;
extern const long SIGFPE;
extern const long SIGHUP;
extern const long SIGILL;
extern const long SIGINFO;
extern const long SIGINT;
extern const long SIGIO;
extern const long SIGIOT;
extern const long SIGKILL;
extern const long SIGPIPE;
extern const long SIGPOLL;
extern const long SIGPROF;
extern const long SIGPWR;
extern const long SIGQUIT;
extern const long SIGRTMAX;
extern const long SIGRTMIN;
extern const long SIGSEGV;
extern const long SIGSTKFLT;
extern const long SIGSTOP;
extern const long SIGSYS;
extern const long SIGTERM;
extern const long SIGTRAP;
extern const long SIGTSTP;
extern const long SIGTTIN;
extern const long SIGTTOU;
extern const long SIGUNUSED;
extern const long SIGURG;
extern const long SIGUSR1;
extern const long SIGUSR2;
extern const long SIGVTALRM;
extern const long SIGWINCH;
extern const long SIGXCPU;
extern const long SIGXFSZ;

extern const long SIG_ATOMIC_MIN;
extern const long SIG_BLOCK;
extern const long SIG_SETMASK;
extern const long SIG_UNBLOCK;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */

#define SIGABRT   LITERALLY(6)
#define SIGALRM   LITERALLY(14)
#define SIGBUS    SYMBOLIC(SIGBUS)
#define SIGCHLD   SYMBOLIC(SIGCHLD)
#define SIGCONT   SYMBOLIC(SIGCONT)
#define SIGEMT    SYMBOLIC(SIGEMT)
#define SIGFPE    LITERALLY(8)
#define SIGHUP    LITERALLY(1)
#define SIGILL    LITERALLY(4)
#define SIGINFO   SYMBOLIC(SIGINFO)
#define SIGINT    LITERALLY(2)
#define SIGIO     SYMBOLIC(SIGIO)
#define SIGIOT    LITERALLY(6)
#define SIGKILL   LITERALLY(9)
#define SIGPIPE   LITERALLY(13)
#define SIGPOLL   SYMBOLIC(SIGPOLL)
#define SIGPROF   LITERALLY(27)
#define SIGPWR    SYMBOLIC(SIGPWR)
#define SIGQUIT   LITERALLY(3)
#define SIGRTMAX  SYMBOLIC(SIGRTMAX)
#define SIGRTMIN  SYMBOLIC(SIGRTMIN)
#define SIGSEGV   LITERALLY(11)
#define SIGSTKFLT SYMBOLIC(SIGSTKFLT)
#define SIGSTOP   SYMBOLIC(SIGSTOP)
#define SIGSYS    SYMBOLIC(SIGSYS)
#define SIGTERM   LITERALLY(15)
#define SIGTRAP   LITERALLY(5)
#define SIGTSTP   SYMBOLIC(SIGTSTP)
#define SIGTTIN   LITERALLY(21)
#define SIGTTOU   LITERALLY(22)
#define SIGUNUSED SYMBOLIC(SIGUNUSED)
#define SIGURG    SYMBOLIC(SIGURG)
#define SIGUSR1   SYMBOLIC(SIGUSR1)
#define SIGUSR2   SYMBOLIC(SIGUSR2)
#define SIGVTALRM LITERALLY(26)
#define SIGWINCH  LITERALLY(28)
#define SIGXCPU   LITERALLY(24)
#define SIGXFSZ   LITERALLY(25)

#define SIG_ATOMIC_MIN SYMBOLIC(SIG_ATOMIC_MIN)
#define SIG_BLOCK      SYMBOLIC(SIG_BLOCK)
#define SIG_SETMASK    SYMBOLIC(SIG_SETMASK)
#define SIG_UNBLOCK    SYMBOLIC(SIG_UNBLOCK)

#endif /* COSMOPOLITAN_LIBC_SYSV_CONSTS_SIG_H_ */
