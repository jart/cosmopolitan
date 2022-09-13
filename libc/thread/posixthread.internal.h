#ifndef COSMOPOLITAN_LIBC_THREAD_POSIXTHREAD_INTERNAL_H_
#define COSMOPOLITAN_LIBC_THREAD_POSIXTHREAD_INTERNAL_H_
#include "libc/calls/struct/sched_param.h"
#include "libc/runtime/runtime.h"
#include "libc/thread/thread.h"
#include "libc/thread/tls.h"
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
  void *(*start_routine)(void *);
  void *arg;  // start_routine's parameter
  void *rc;   // start_routine's return value
  bool ownstack;
  int tid;
  int *ctid;
  char *tls;
  char *altstack;
  struct CosmoTib *tib;
  _Atomic(enum PosixThreadStatus) status;
  jmp_buf exiter;
  pthread_attr_t attr;
};

hidden extern pthread_spinlock_t _pthread_keys_lock;
hidden extern uint64_t _pthread_key_usage[(PTHREAD_KEYS_MAX + 63) / 64];
hidden extern pthread_key_dtor _pthread_key_dtor[PTHREAD_KEYS_MAX];
hidden extern _Thread_local void *_pthread_keys[PTHREAD_KEYS_MAX];

int _pthread_reschedule(struct PosixThread *) hidden;
int _pthread_setschedparam_freebsd(int, int, const struct sched_param *) hidden;
void _pthread_free(struct PosixThread *) hidden;
void _pthread_ungarbage(void) hidden;
void _pthread_wait(struct PosixThread *) hidden;
void _pthread_zombies_add(struct PosixThread *) hidden;
void _pthread_zombies_decimate(void) hidden;
void _pthread_zombies_harvest(void) hidden;
void _pthread_key_destruct(void *[PTHREAD_KEYS_MAX]);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_THREAD_POSIXTHREAD_INTERNAL_H_ */
