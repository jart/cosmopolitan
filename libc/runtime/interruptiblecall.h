#ifndef COSMOPOLITAN_LIBC_RUNTIME_INTERRUPTIBLECALL_H_
#define COSMOPOLITAN_LIBC_RUNTIME_INTERRUPTIBLECALL_H_
#include "libc/runtime/runtime.h"
#include "libc/calls/struct/sigaction.h"
#include "libc/calls/calls.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

struct InterruptibleCall {
  struct InterruptibleCall *prev;
  intptr_t returnval;
  int sig;
  int flags;
  jmp_buf jb;
  struct sigaction sa_new;
  struct sigaction sa_old;
};

intptr_t interruptiblecall(struct InterruptibleCall *state,
                           intptr_t callback(intptr_t p1, intptr_t p2,
                                             intptr_t p3, intptr_t p4),
                           intptr_t p1, intptr_t p2, intptr_t p3, intptr_t p4);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_RUNTIME_INTERRUPTIBLECALL_H_ */
