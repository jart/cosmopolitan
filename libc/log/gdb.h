#ifndef COSMOPOLITAN_LIBC_LOG_GDB_H_
#define COSMOPOLITAN_LIBC_LOG_GDB_H_
#include "libc/calls/calls.h"
#include "libc/runtime/missioncritical.h"
#include "libc/sysv/consts/nr.h"
#include "libc/sysv/consts/w.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

extern volatile int g_gdbsync;

int gdbexec(const char *);
int attachdebugger(intptr_t);

#define attachdebugger(CONTINUE_TO_ADDR) /* shorten backtraces */ \
  SYNCHRONIZE_DEBUGGER((attachdebugger)(CONTINUE_TO_ADDR))

#define SYNCHRONIZE_DEBUGGER(PID)                           \
  ({                                                        \
    int Rc, Pid = (PID);                                    \
    if (Pid != -1) {                                        \
      while ((Rc = WAIT4(Pid, NULL, WNOHANG, NULL)) == 0) { \
        if (g_gdbsync) {                                    \
          g_gdbsync = 0;                                    \
          if (Rc > 0) Pid = 0;                              \
          break;                                            \
        } else {                                            \
          SCHED_YIELD();                                    \
        }                                                   \
      }                                                     \
    }                                                       \
    Pid;                                                    \
  })

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_LOG_GDB_H_ */
