#ifndef COSMOPOLITAN_LIBC_SYSV_CONSTS_SIG_H_
#define COSMOPOLITAN_LIBC_SYSV_CONSTS_SIG_H_

/*
 * - No macro is defined for SIGIO and SIGPOLL in order to persuade
 *   ./configure scripts to favor using poll() or select() instead of
 *   interrupt-based i/o.
 *
 * - No macros are defined for SIGRTMIN and SIGRTMAX because the project
 *   hasn't fleshed them out yet.
 *
 * - SIGSTKFLT is Linux-only, intended for the x87 coprocessor stack,
 *   and Linux doesn't actually use it for that so no macro is defined.
 */

#define SIGHUP    1  /* hangup */
#define SIGINT    2  /* interrupt */
#define SIGQUIT   3  /* quit */
#define SIGILL    4  /* illegal instruction */
#define SIGTRAP   5  /* trap trace */
#define SIGABRT   6  /* abort */
#define SIGBUS    7  /* bus error */
#define SIGFPE    8  /* floating point exception */
#define SIGKILL   9  /* kill */
#define SIGUSR1   10 /* user defined signal 1 */
#define SIGSEGV   11 /* segmentation fault */
#define SIGUSR2   12 /* user defined signal 2 */
#define SIGPIPE   13 /* write on a pipe with no one to read it */
#define SIGALRM   14 /* alarm clock */
#define SIGTERM   15 /* software termination signal from kill */
#define SIGCHLD   17 /* to parent on child stop or exit */
#define SIGCONT   18 /* continue a stopped process */
#define SIGSTOP   19 /* sendable stop signal not from tty */
#define SIGTSTP   20 /* stop signal from tty */
#define SIGTTIN   21 /* to readers pgrp upon background tty read */
#define SIGTTOU   22 /* like TTIN for output if (tp->t_local&LTOSTOP) */
#define SIGURG    23 /* urgent condition on IO channel */
#define SIGXCPU   24 /* exceeded CPU time limit */
#define SIGXFSZ   25 /* exceeded file size limit */
#define SIGVTALRM 26 /* virtual time alarm */
#define SIGPROF   27 /* profiling time alarm */
#define SIGWINCH  28 /* window size changes */
#define SIGSYS    31 /* bad argument to system call */
#define SIGTHR    32 /* internal to pthreads */

#define SIG_BLOCK   SIG_BLOCK
#define SIG_SETMASK SIG_SETMASK
#define SIG_UNBLOCK SIG_UNBLOCK

#ifndef __ASSEMBLER__
COSMOPOLITAN_C_START_

extern const int SIG_BLOCK;
extern const int SIG_SETMASK;
extern const int SIG_UNBLOCK;

COSMOPOLITAN_C_END_
#endif /* __ASSEMBLER__ */
#endif /* COSMOPOLITAN_LIBC_SYSV_CONSTS_SIG_H_ */
