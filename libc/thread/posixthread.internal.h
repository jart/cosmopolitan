#ifndef COSMOPOLITAN_LIBC_THREAD_POSIXTHREAD_INTERNAL_H_
#define COSMOPOLITAN_LIBC_THREAD_POSIXTHREAD_INTERNAL_H_
#include "libc/runtime/runtime.h"
#include "libc/thread/spawn.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

/**
 * @fileoverview Cosmopolitan POSIX Thread Internals
 */

enum PosixThreadStatus {
  kPosixThreadStarted,
  kPosixThreadDetached,
  kPosixThreadTerminated,
  kPosixThreadZombie,
};

struct PosixThread {
  struct spawn spawn;
  void *(*start_routine)(void *);
  void *arg;
  void *rc;
  int tid;
  _Atomic(enum PosixThreadStatus) status;
  jmp_buf exiter;
};

void pthread_zombies_add(struct PosixThread *);
void pthread_zombies_decimate(void);
void pthread_zombies_harvest(void);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_THREAD_POSIXTHREAD_INTERNAL_H_ */
