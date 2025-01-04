#ifndef COSMOPOLITAN_LIBC_THREAD_POSIXTHREAD_INTERNAL_H_
#define COSMOPOLITAN_LIBC_THREAD_POSIXTHREAD_INTERNAL_H_
#include "libc/calls/struct/sched_param.h"
#include "libc/calls/struct/sigaltstack.h"
#include "libc/intrin/atomic.h"
#include "libc/intrin/dll.h"
#include "libc/runtime/runtime.h"
#include "libc/thread/pt.internal.h"
#include "libc/thread/thread.h"
#include "libc/thread/tls.h"

#define PT_BLOCKER_EVENT ((atomic_int *)-1)

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

typedef struct __locale_struct *locale_t;

struct PosixThread {
  int pt_flags;            // 0x00: see PT_* constants
  atomic_int pt_canceled;  // 0x04: thread has bad beliefs
  _Atomic(enum PosixThreadStatus) pt_status;
  _Atomic(atomic_int *) pt_blocker;
  atomic_int pt_refs;         // prevents decimation
  void *(*pt_start)(void *);  // creation callback
  void *pt_val;               // start param / return val
  char *pt_tls;               // bottom of tls allocation
  struct CosmoTib *tib;       // middle of tls allocation
  struct Dll list;            // list of threads
  struct _pthread_cleanup_buffer *pt_cleanup;
  uint64_t pt_blkmask;
  int64_t pt_event;
  locale_t pt_locale;
  intptr_t pt_exiter[5];
  pthread_attr_t pt_attr;
  atomic_bool pt_intoff;
};

typedef void (*atfork_f)(void);

extern struct Dll *_pthread_list;
extern atomic_uint _pthread_count;
extern struct PosixThread _pthread_static;
extern _Atomic(pthread_key_dtor) _pthread_key_dtor[PTHREAD_KEYS_MAX];

int _pthread_cond_signal(pthread_cond_t *) dontthrow paramsnonnull();
int _pthread_mutex_lock(pthread_mutex_t *) dontthrow paramsnonnull();
int _pthread_mutex_trylock(pthread_mutex_t *) dontthrow paramsnonnull();
int _pthread_mutex_unlock(pthread_mutex_t *) dontthrow paramsnonnull();
int _pthread_mutex_wipe_np(pthread_mutex_t *) libcesque paramsnonnull();
int _pthread_reschedule(struct PosixThread *) libcesque;
int _pthread_setschedparam_freebsd(int, int, const struct sched_param *);
int _pthread_tid(struct PosixThread *) libcesque;
intptr_t _pthread_syshand(struct PosixThread *) libcesque;
long _pthread_cancel_ack(void) libcesque;
void _pthread_decimate(enum PosixThreadStatus) dontthrow;
void _pthread_free(struct PosixThread *) libcesque paramsnonnull();
void _pthread_lock(void) dontthrow;
void _pthread_onfork_child(void) dontthrow;
void _pthread_onfork_parent(void) dontthrow;
void _pthread_onfork_prepare(void) dontthrow;
void _pthread_unlock(void) dontthrow;
void _pthread_zombify(struct PosixThread *) dontthrow;

int _pthread_cond_wait(pthread_cond_t *, pthread_mutex_t *) dontthrow
    paramsnonnull();

int _pthread_cond_timedwait(pthread_cond_t *, pthread_mutex_t *,
                            const struct timespec *) dontthrow
    paramsnonnull((1, 2));

forceinline pureconst struct PosixThread *_pthread_self(void) {
  return (struct PosixThread *)__get_tls()->tib_pthread;
}

forceinline void _pthread_ref(struct PosixThread *pt) {
  atomic_fetch_add_explicit(&pt->pt_refs, 1, memory_order_relaxed);
}

forceinline void _pthread_unref(struct PosixThread *pt) {
  atomic_fetch_sub_explicit(&pt->pt_refs, 1, memory_order_acq_rel);
}

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_THREAD_POSIXTHREAD_INTERNAL_H_ */
