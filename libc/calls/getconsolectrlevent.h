#ifndef COSMOPOLITAN_LIBC_CALLS_GETCONSOLECTRLEVENT_H_
#define COSMOPOLITAN_LIBC_CALLS_GETCONSOLECTRLEVENT_H_
#include "libc/nt/enum/ctrlevent.h"
#include "libc/sysv/consts/sig.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

static inline int GetConsoleCtrlEvent(int sig) {
  switch (sig) {
    case SIGINT:
      return kNtCtrlCEvent;
    case SIGQUIT:
      return kNtCtrlBreakEvent;
    default:
      return -1;
  }
}

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_CALLS_GETCONSOLECTRLEVENT_H_ */
