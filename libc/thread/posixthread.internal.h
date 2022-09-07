#ifndef COSMOPOLITAN_LIBC_THREAD_POSIXTHREAD_INTERNAL_H_
#define COSMOPOLITAN_LIBC_THREAD_POSIXTHREAD_INTERNAL_H_
#include "libc/intrin/pthread.h"
#include "libc/runtime/runtime.h"
#include "libc/thread/spawn.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

/**
 * @fileoverview Cosmopolitan POSIX Thread Internals
 */

enum PosixThreadStatus {
  kPosixThreadJoinable,
  kPosixThreadDetached,
  kPosixThreadTerminated,
  kPosixThreadZombie,
};

struct PosixThread {
  struct spawn spawn;
  void *(*start_routine)(void *);
  void *arg;  // start_routine's parameter
  void *rc;   // start_routine's return value
  _Atomic(enum PosixThreadStatus) status;
  jmp_buf exiter;
  size_t stacksize;
  pthread_attr_t attr;
};

void pthread_free(struct PosixThread *) hidden;
void pthread_wait(struct PosixThread *) hidden;
void pthread_zombies_add(struct PosixThread *) hidden;
void pthread_zombies_decimate(void) hidden;
void pthread_zombies_harvest(void) hidden;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_THREAD_POSIXTHREAD_INTERNAL_H_ */
