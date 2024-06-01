#ifndef COSMOPOLITAN_LIBC_THREAD_THREAD_H_
#define COSMOPOLITAN_LIBC_THREAD_THREAD_H_

#define PTHREAD_KEYS_MAX              128
#define PTHREAD_STACK_MIN             65536
#define PTHREAD_DESTRUCTOR_ITERATIONS 4

#define PTHREAD_BARRIER_SERIAL_THREAD 31337

#define PTHREAD_MUTEX_DEFAULT    0
#define PTHREAD_MUTEX_NORMAL     0
#define PTHREAD_MUTEX_RECURSIVE  1
#define PTHREAD_MUTEX_ERRORCHECK 2
#define PTHREAD_MUTEX_STALLED    0
#define PTHREAD_MUTEX_ROBUST     1

#define PTHREAD_PROCESS_PRIVATE 0
#define PTHREAD_PROCESS_SHARED  1

#define PTHREAD_CREATE_JOINABLE 0
#define PTHREAD_CREATE_DETACHED 1

#define PTHREAD_INHERIT_SCHED  0
#define PTHREAD_EXPLICIT_SCHED 1

#define PTHREAD_CANCELED ((void *)-1)

#define PTHREAD_CANCEL_ENABLE  0
#define PTHREAD_CANCEL_DISABLE 1
#define PTHREAD_CANCEL_MASKED  2

#define PTHREAD_CANCEL_DEFERRED     0
#define PTHREAD_CANCEL_ASYNCHRONOUS 1

#define PTHREAD_SCOPE_SYSTEM  0
#define PTHREAD_SCOPE_PROCESS 1

#define PTHREAD_ATTR_NO_SIGMASK_NP -1

#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

#define PTHREAD_ONCE_INIT          _PTHREAD_INIT
#define PTHREAD_COND_INITIALIZER   _PTHREAD_INIT
#define PTHREAD_RWLOCK_INITIALIZER _PTHREAD_INIT
#define PTHREAD_MUTEX_INITIALIZER  _PTHREAD_INIT
#define _PTHREAD_INIT \
  { 0 }

typedef uintptr_t pthread_t;
typedef int pthread_id_np_t;
typedef char pthread_condattr_t;
typedef char pthread_rwlockattr_t;
typedef char pthread_barrierattr_t;
typedef unsigned pthread_key_t;
typedef void (*pthread_key_dtor)(void *);

typedef struct pthread_once_s {
  _Atomic(uint32_t) _lock;
} pthread_once_t;

typedef struct pthread_spinlock_s {
  _Atomic(int) _lock;
} pthread_spinlock_t;

typedef struct pthread_mutex_s {
  _Atomic(int32_t) _lock;
  unsigned _type : 2;
  unsigned _pshared : 1;
  unsigned _depth : 6;
  unsigned _owner : 23;
  long _pid;
} pthread_mutex_t;

typedef struct pthread_mutexattr_s {
  char _type;
  char _pshared;
} pthread_mutexattr_t;

typedef struct pthread_cond_s {
  void *_nsync[2];
} pthread_cond_t;

typedef struct pthread_rwlock_s {
  void *_nsync[2];
  char _iswrite;
} pthread_rwlock_t;

typedef struct pthread_barrier_s {
  void *_nsync;
} pthread_barrier_t;

typedef struct pthread_attr_s {
  char __detachstate;
  char __inheritsched;
  char __havesigmask;
  int __schedparam;
  int __schedpolicy;
  int __contentionscope;
  int __guardsize;
  size_t __stacksize;
  uint64_t __sigmask;
  void *__stackaddr;
} pthread_attr_t;

struct _pthread_cleanup_buffer {
  void (*__routine)(void *);
  void *__arg;
  int __canceltype;
  struct _pthread_cleanup_buffer *__prev;
};

/* clang-format off */

int pthread_atfork(void (*)(void), void (*)(void), void (*)(void)) dontthrow;
int pthread_attr_destroy(pthread_attr_t *) libcesque paramsnonnull();
int pthread_attr_getdetachstate(const pthread_attr_t *, int *) libcesque paramsnonnull();
int pthread_attr_getguardsize(const pthread_attr_t *, size_t *) libcesque paramsnonnull();
int pthread_attr_getinheritsched(const pthread_attr_t *, int *) libcesque paramsnonnull();
int pthread_attr_getschedpolicy(const pthread_attr_t *, int *) libcesque paramsnonnull();
int pthread_attr_getscope(const pthread_attr_t *, int *) libcesque paramsnonnull();
int pthread_attr_getstack(const pthread_attr_t *, void **, size_t *) libcesque paramsnonnull();
int pthread_attr_getstacksize(const pthread_attr_t *, size_t *) libcesque paramsnonnull();
int pthread_attr_init(pthread_attr_t *) libcesque paramsnonnull();
int pthread_attr_setdetachstate(pthread_attr_t *, int) libcesque paramsnonnull();
int pthread_attr_setguardsize(pthread_attr_t *, size_t) libcesque paramsnonnull();
int pthread_attr_setinheritsched(pthread_attr_t *, int) libcesque paramsnonnull();
int pthread_attr_setschedpolicy(pthread_attr_t *, int) libcesque paramsnonnull();
int pthread_attr_setscope(pthread_attr_t *, int) libcesque paramsnonnull();
int pthread_attr_setstack(pthread_attr_t *, void *, size_t) libcesque paramsnonnull((1));
int pthread_attr_setstacksize(pthread_attr_t *, size_t) libcesque paramsnonnull();
int pthread_barrier_destroy(pthread_barrier_t *) libcesque paramsnonnull();
int pthread_barrier_init(pthread_barrier_t *, const pthread_barrierattr_t *, unsigned) libcesque paramsnonnull((1));
int pthread_barrier_wait(pthread_barrier_t *) libcesque paramsnonnull();
int pthread_barrierattr_destroy(pthread_barrierattr_t *) libcesque paramsnonnull();
int pthread_barrierattr_getpshared(const pthread_barrierattr_t *, int *) libcesque paramsnonnull();
int pthread_barrierattr_init(pthread_barrierattr_t *) libcesque paramsnonnull();
int pthread_barrierattr_setpshared(pthread_barrierattr_t *, int) libcesque paramsnonnull();
int pthread_cancel(pthread_t) libcesque;
int pthread_cond_broadcast(pthread_cond_t *) libcesque paramsnonnull();
int pthread_cond_destroy(pthread_cond_t *) libcesque paramsnonnull();
int pthread_cond_init(pthread_cond_t *, const pthread_condattr_t *) libcesque paramsnonnull((1));
int pthread_cond_signal(pthread_cond_t *) libcesque paramsnonnull();
int pthread_cond_wait(pthread_cond_t *, pthread_mutex_t *) libcesque paramsnonnull();
int pthread_condattr_destroy(pthread_condattr_t *) libcesque paramsnonnull();
int pthread_condattr_getpshared(const pthread_condattr_t *, int *) libcesque paramsnonnull();
int pthread_condattr_init(pthread_condattr_t *) libcesque paramsnonnull();
int pthread_condattr_setpshared(pthread_condattr_t *, int) libcesque paramsnonnull();
int pthread_create(pthread_t *, const pthread_attr_t *, void *(*)(void *), void *) dontthrow paramsnonnull((1));
int pthread_detach(pthread_t) libcesque;
int pthread_equal(pthread_t, pthread_t) libcesque;
int pthread_getattr_np(pthread_t, pthread_attr_t *) libcesque paramsnonnull();
int pthread_getname_np(pthread_t, char *, size_t) libcesque paramsnonnull();
int pthread_getunique_np(pthread_t, pthread_id_np_t *) libcesque paramsnonnull();
int pthread_join(pthread_t, void **) libcesque;
int pthread_key_create(pthread_key_t *, pthread_key_dtor) libcesque paramsnonnull((1));
int pthread_key_delete(pthread_key_t) libcesque;
int pthread_kill(pthread_t, int) libcesque;
int pthread_mutex_consistent(pthread_mutex_t *) libcesque paramsnonnull();
int pthread_mutex_destroy(pthread_mutex_t *) libcesque paramsnonnull();
int pthread_mutex_init(pthread_mutex_t *, const pthread_mutexattr_t *) libcesque paramsnonnull((1));
int pthread_mutex_lock(pthread_mutex_t *) libcesque paramsnonnull();
int pthread_mutex_trylock(pthread_mutex_t *) libcesque paramsnonnull();
int pthread_mutex_unlock(pthread_mutex_t *) libcesque paramsnonnull();
int pthread_mutexattr_destroy(pthread_mutexattr_t *) libcesque paramsnonnull();
int pthread_mutexattr_getpshared(const pthread_mutexattr_t *, int *) libcesque paramsnonnull();
int pthread_mutexattr_gettype(const pthread_mutexattr_t *, int *) libcesque paramsnonnull();
int pthread_mutexattr_init(pthread_mutexattr_t *) libcesque paramsnonnull();
int pthread_mutexattr_setpshared(pthread_mutexattr_t *, int) libcesque paramsnonnull();
int pthread_mutexattr_settype(pthread_mutexattr_t *, int) libcesque paramsnonnull();
int pthread_once(pthread_once_t *, void (*)(void)) paramsnonnull();
int pthread_orphan_np(void) libcesque;
int pthread_rwlock_destroy(pthread_rwlock_t *) libcesque paramsnonnull();
int pthread_rwlock_init(pthread_rwlock_t *, const pthread_rwlockattr_t *) libcesque paramsnonnull((1));
int pthread_rwlock_rdlock(pthread_rwlock_t *) libcesque paramsnonnull();
int pthread_rwlock_tryrdlock(pthread_rwlock_t *) libcesque paramsnonnull();
int pthread_rwlock_trywrlock(pthread_rwlock_t *) libcesque paramsnonnull();
int pthread_rwlock_unlock(pthread_rwlock_t *) libcesque paramsnonnull();
int pthread_rwlock_wrlock(pthread_rwlock_t *) libcesque paramsnonnull();
int pthread_rwlockattr_destroy(pthread_rwlockattr_t *) libcesque paramsnonnull();
int pthread_rwlockattr_getpshared(const pthread_rwlockattr_t *, int *) libcesque paramsnonnull();
int pthread_rwlockattr_init(pthread_rwlockattr_t *) libcesque paramsnonnull();
int pthread_rwlockattr_setpshared(pthread_rwlockattr_t *, int) libcesque paramsnonnull();
int pthread_setcancelstate(int, int *) libcesque;
int pthread_setcanceltype(int, int *) libcesque;
int pthread_setname_np(pthread_t, const char *) libcesque paramsnonnull();
int pthread_setschedprio(pthread_t, int) libcesque;
int pthread_setspecific(pthread_key_t, const void *) libcesque;
int pthread_spin_destroy(pthread_spinlock_t *) libcesque paramsnonnull();
int pthread_spin_init(pthread_spinlock_t *, int) libcesque paramsnonnull();
int pthread_spin_lock(pthread_spinlock_t *) libcesque paramsnonnull();
int pthread_spin_trylock(pthread_spinlock_t *) libcesque paramsnonnull();
int pthread_spin_unlock(pthread_spinlock_t *) libcesque paramsnonnull();
int pthread_testcancel_np(void) libcesque;
int pthread_tryjoin_np(pthread_t, void **) libcesque;
int pthread_yield_np(void) libcesque;
int pthread_yield(void) libcesque;
pthread_id_np_t pthread_getthreadid_np(void) libcesque;
pthread_t pthread_self(void) libcesque pureconst;
void *pthread_getspecific(pthread_key_t) libcesque;
void pthread_cleanup_pop(struct _pthread_cleanup_buffer *, int) libcesque paramsnonnull();
void pthread_cleanup_push(struct _pthread_cleanup_buffer *, void (*)(void *), void *) libcesque paramsnonnull((1));
void pthread_exit(void *) libcesque wontreturn;
void pthread_testcancel(void) libcesque;
void pthread_pause_np(void) libcesque;

/* clang-format on */

#define pthread_cleanup_push(routine, arg)  \
  {                                         \
    struct _pthread_cleanup_buffer _buffer; \
    pthread_cleanup_push(&_buffer, (routine), (arg));

#define pthread_cleanup_pop(execute)        \
  pthread_cleanup_pop(&_buffer, (execute)); \
  }

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_THREAD_THREAD_H_ */
