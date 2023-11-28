#ifndef COSMOPOLITAN_LIBC_LOG_GDB_H_
#define COSMOPOLITAN_LIBC_LOG_GDB_H_
#include "libc/calls/calls.h"
#include "libc/calls/struct/rusage.h"
#include "libc/dce.h"
#include "libc/proc/proc.internal.h"
#include "libc/sysv/consts/nr.h"
#include "libc/sysv/consts/w.h"
COSMOPOLITAN_C_START_

/**
 * @fileoverview GDB Attach Support Code.
 *
 * The goal of these macros is to make the backtrace into the failing
 * code as short as possible. It also helps avoid GDB getting confused
 * about how we don't use its readability destroying unwind directives.
 */

extern volatile int g_gdbsync;

int gdbexec(const char *);
int AttachDebugger(intptr_t);

#define AttachDebugger(CONTINUE_TO_ADDR) /* shorten backtraces */ \
  SYNCHRONIZE_DEBUGGER((AttachDebugger)(CONTINUE_TO_ADDR))

#define SYNCHRONIZE_DEBUGGER(PID)                                    \
  ({                                                                 \
    int Rc, Pid = (PID);                                             \
    if (Pid != -1) {                                                 \
      while ((Rc = __inline_wait4(Pid, NULL, WNOHANG, NULL)) == 0) { \
        if (g_gdbsync) {                                             \
          g_gdbsync = 0;                                             \
          if (Rc > 0) Pid = 0;                                       \
          break;                                                     \
        } else {                                                     \
          sched_yield();                                             \
        }                                                            \
      }                                                              \
    }                                                                \
    Pid;                                                             \
  })

#ifdef __x86_64__
#define __inline_wait4(PID, OPT_OUT_WSTATUS, OPTIONS, OPT_OUT_RUSAGE)     \
  ({                                                                      \
    int64_t WaAx;                                                         \
    if (!IsWindows()) {                                                   \
      asm volatile("mov\t%5,%%r10\n\t"                                    \
                   "syscall"                                              \
                   : "=a"(WaAx)                                           \
                   : "0"(__NR_wait4), "D"(PID), "S"(OPT_OUT_WSTATUS),     \
                     "d"(OPTIONS), "g"(OPT_OUT_RUSAGE)                    \
                   : "rcx", "r8", "r9", "r10", "r11", "memory", "cc");    \
    } else {                                                              \
      WaAx = sys_wait4_nt(PID, OPT_OUT_WSTATUS, OPTIONS, OPT_OUT_RUSAGE); \
    }                                                                     \
    WaAx;                                                                 \
  })
#else
#define __inline_wait4 wait4
#endif

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_LOG_GDB_H_ */
