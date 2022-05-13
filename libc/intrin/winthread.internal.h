#ifndef COSMOPOLITAN_LIBC_RUNTIME_WINTHREAD_INTERNAL_H_
#define COSMOPOLITAN_LIBC_RUNTIME_WINTHREAD_INTERNAL_H_
#include "libc/intrin/tls.h"
#include "libc/runtime/runtime.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

struct WinThread {
  uint32_t tid;
  int flags;
  int *ctid;
  int (*func)(void *);
  void *arg;
};

extern int __winthread;

static inline struct WinThread *GetWinThread(void) {
  return TlsGetValue(__winthread);
}

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_RUNTIME_WINTHREAD_INTERNAL_H_ */
