#ifndef COSMOPOLITAN_LIBC_THREAD_POSIXTHREAD_INTERNAL_H_
#define COSMOPOLITAN_LIBC_THREAD_POSIXTHREAD_INTERNAL_H_
#include "libc/calls/struct/sched_param.h"
#include "libc/calls/struct/sigaltstack.h"
#include "libc/intrin/dll.h"
#include "libc/runtime/runtime.h"
#include "libc/thread/thread.h"
#include "libc/thread/tls.h"

#define PT_OWNSTACK       1
#define PT_STATIC         2
#define PT_ASYNC          4
#define PT_NOCANCEL       8
#define PT_MASKED         16
#define PT_INCANCEL       32
#define PT_POLLING        64   // windows only
#define PT_INSEMAPHORE    128  // windows only
#define PT_OPENBSD_KLUDGE 128  // openbsd only

#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

// LEGAL TRANSITIONS             ┌──> TERMINATED ─┐
// pthread_create ─┬─> JOINABLE ─┴┬─> DETACHED ───┴─> ZOMBIE
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
  //
  // - kPosixThreadJoinable -> kPosixThreadZombie if another thread
  //   calls fork().
  kPosixThreadJoinable,

  // this is a managed thread that'll be cleaned up by the library.
  //
  // the following transitions are possible:
  //
  // - kPosixThreadDetached -> kPosixThreadZombie if start_routine()
  //   returns, or is longjmp'd out of by pthread_exit(), and the thread
  //   is waiting to be joined.
  //
  // - kPosixThreadDetached -> kPosixThreadZombie if another thread
  //   calls fork().
  kPosixThreadDetached,

  // this is a joinable thread that terminated.
  //
  // the following transitions are possible:
  //
  // - kPosixThreadTerminated -> _pthread_free() will happen when
  //   pthread_join() is called by the user.
  // - kPosixThreadTerminated -> kPosixThreadZombie will happen when
  //   pthread_detach() is called by the user.
  // - kPosixThreadTerminated -> kPosixThreadZombie if another thread
  //   calls fork().
  kPosixThreadTerminated,

  // this is a detached thread that terminated.
  //
  // the following transitions are possible:
  //
  // - kPosixThreadZombie -> _pthread_free() will happen whenever
  //   convenient, e.g. pthread_create() entry or atexit handler.
  kPosixThreadZombie,
};

#define POSIXTHREAD_CONTAINER(e) DLL_CONTAINER(struct PosixThread, list, e)

struct PosixThread {
  int pt_flags;            // 0x00: see PT_* constants
  _Atomic(int) cancelled;  // 0x04: thread has bad beliefs
  _Atomic(enum PosixThreadStatus) status;
  _Atomic(int) ptid;       // transitions 0 → tid
  void *(*start)(void *);  // creation callback
  void *arg;               // start's parameter
  void *rc;                // start's return value
  char *tls;               // bottom of tls allocation
  struct CosmoTib *tib;    // middle of tls allocation
  struct Dll list;         // list of threads
  _Atomic(_Atomic(int) *) pt_futex;
  intptr_t semaphore;
  intptr_t iohandle;
  void *ioverlap;
  jmp_buf exiter;
  pthread_attr_t attr;
  int abort_errno;
  struct _pthread_cleanup_buffer *cleanup;
};

typedef void (*atfork_f)(void);

extern struct Dll *_pthread_list;
extern pthread_spinlock_t _pthread_lock;
extern struct PosixThread _pthread_static;
extern _Atomic(pthread_key_dtor) _pthread_key_dtor[PTHREAD_KEYS_MAX];

void _pthread_decimate(void);
int _pthread_tid(struct PosixThread *);
void _pthread_unkey(struct CosmoTib *);
void _pthread_unwind(struct PosixThread *);
int _pthread_reschedule(struct PosixThread *);
intptr_t _pthread_syshand(struct PosixThread *);
int _pthread_atfork(atfork_f, atfork_f, atfork_f);
int _pthread_setschedparam_freebsd(int, int, const struct sched_param *);
void _pthread_free(struct PosixThread *, bool);
void _pthread_zombify(struct PosixThread *);
void _pthread_onfork_prepare(void);
void _pthread_onfork_parent(void);
void _pthread_onfork_child(void);
long _pthread_cancel_ack(void);
void _pthread_ungarbage(void);

__funline pureconst struct PosixThread *_pthread_self(void) {
  return (struct PosixThread *)__get_tls()->tib_pthread;
}

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_THREAD_POSIXTHREAD_INTERNAL_H_ */
