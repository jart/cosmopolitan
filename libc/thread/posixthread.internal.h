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

// LEGAL TRANSITIONS             ┌──> TERMINATED
// pthread_create ─┬─> JOINABLE ─┴┬─> DETACHED ──> ZOMBIE
//                 └──────────────┘
enum PosixThreadStatus {

  // this is a running thread that needs pthread_join()
  //
  // the following transitions are possible:
  //
  // - kPosixThreadJoinable -> kPosixThreadTerminated if start_routine()
  //   returns, or is longjmp'd out of by pthread_exit(), and the thread
  //   is waiting to be joined.
  //
  // - kPosixThreadJoinable -> kPosixThreadDetached if pthread_detach()
  //   is called on this thread.
  kPosixThreadJoinable,

  // this is a managed thread that'll be cleaned up by the library.
  //
  // the following transitions are possible:
  //
  // - kPosixThreadDetached -> kPosixThreadZombie if start_routine()
  //   returns, or is longjmp'd out of by pthread_exit(), and the thread
  //   is waiting to be joined.
  kPosixThreadDetached,

  // this is a joinable thread that terminated.
  //
  // the following transitions are possible:
  //
  // - kPosixThreadTerminated -> _pthread_free() will happen when
  //   pthread_join() is called by the user.
  kPosixThreadTerminated,

  // this is a detached thread that terminated.
  //
  // the following transitions are possible:
  //
  // - kPosixThreadZombie -> _pthread_free() will happen whenever
  //   convenient, e.g. pthread_create() entry or atexit handler.
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

void _pthread_free(struct PosixThread *) hidden;
void _pthread_wait(struct PosixThread *) hidden;
void _pthread_zombies_add(struct PosixThread *) hidden;
void _pthread_zombies_decimate(void) hidden;
void _pthread_zombies_harvest(void) hidden;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_THREAD_POSIXTHREAD_INTERNAL_H_ */
