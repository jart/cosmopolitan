#ifndef COSMOPOLITAN_LIBC_SYSV_CONSTS_SIG_H_
#define COSMOPOLITAN_LIBC_SYSV_CONSTS_SIG_H_
#include "libc/runtime/symbolic.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

extern const int SIGABRT;
extern const int SIGALRM;
extern const int SIGBUS;
extern const int SIGTHR;
extern const int SIGCHLD;
extern const int SIGCONT;
extern const int SIGEMT;
extern const int SIGFPE;
extern const int SIGHUP;
extern const int SIGILL;
extern const int SIGINFO;
extern const int SIGINT;
extern const int SIGIO;
extern const int SIGIOT;
extern const int SIGKILL;
extern const int SIGPIPE;
extern const int SIGPOLL;
extern const int SIGPROF;
extern const int SIGPWR;
extern const int SIGQUIT;
extern const int SIGRTMAX;
extern const int SIGRTMIN;
extern const int SIGSEGV;
extern const int SIGSTKFLT;
extern const int SIGSTOP;
extern const int SIGSYS;
extern const int SIGTERM;
extern const int SIGTRAP;
extern const int SIGTSTP;
extern const int SIGTTIN;
extern const int SIGTTOU;
extern const int SIGUNUSED;
extern const int SIGURG;
extern const int SIGUSR1;
extern const int SIGUSR2;
extern const int SIGVTALRM;
extern const int SIGWINCH;
extern const int SIGXCPU;
extern const int SIGXFSZ;

extern const int SIG_BLOCK;
extern const int SIG_SETMASK;
extern const int SIG_UNBLOCK;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */

#define SIGABRT   LITERALLY(6)
#define SIGALRM   LITERALLY(14)
#define SIGFPE    LITERALLY(8)
#define SIGHUP    LITERALLY(1)
#define SIGILL    LITERALLY(4)
#define SIGINT    LITERALLY(2)
#define SIGIOT    LITERALLY(6)
#define SIGKILL   LITERALLY(9)
#define SIGPIPE   LITERALLY(13)
#define SIGPROF   LITERALLY(27)
#define SIGQUIT   LITERALLY(3)
#define SIGSEGV   LITERALLY(11)
#define SIGTERM   LITERALLY(15)
#define SIGTRAP   LITERALLY(5)
#define SIGTTIN   LITERALLY(21)
#define SIGTTOU   LITERALLY(22)
#define SIGVTALRM LITERALLY(26)
#define SIGWINCH  LITERALLY(28)
#define SIGXCPU   LITERALLY(24)
#define SIGXFSZ   LITERALLY(25)

#define SIGBUS    SYMBOLIC(SIGBUS)
#define SIGTHR    SYMBOLIC(SIGTHR)
#define SIGCHLD   SYMBOLIC(SIGCHLD)
#define SIGCONT   SYMBOLIC(SIGCONT)
#define SIGEMT    SYMBOLIC(SIGEMT)
#define SIGINFO   SYMBOLIC(SIGINFO)
#define SIGIO     SYMBOLIC(SIGIO)
#define SIGPOLL   SYMBOLIC(SIGPOLL)
#define SIGPWR    SYMBOLIC(SIGPWR)
#define SIGRTMAX  SYMBOLIC(SIGRTMAX)
#define SIGRTMIN  SYMBOLIC(SIGRTMIN)
#define SIGSTKFLT SYMBOLIC(SIGSTKFLT)
#define SIGSTOP   SYMBOLIC(SIGSTOP)
#define SIGSYS    SYMBOLIC(SIGSYS)
#define SIGTSTP   SYMBOLIC(SIGTSTP)
#define SIGUNUSED SYMBOLIC(SIGUNUSED)
#define SIGURG    SYMBOLIC(SIGURG)
#define SIGUSR1   SYMBOLIC(SIGUSR1)
#define SIGUSR2   SYMBOLIC(SIGUSR2)

#define SIG_BLOCK   SYMBOLIC(SIG_BLOCK)
#define SIG_SETMASK SYMBOLIC(SIG_SETMASK)
#define SIG_UNBLOCK SYMBOLIC(SIG_UNBLOCK)

#endif /* COSMOPOLITAN_LIBC_SYSV_CONSTS_SIG_H_ */
