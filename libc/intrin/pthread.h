#ifndef COSMOPOLITAN_LIBC_RUNTIME_PTHREAD_H_
#define COSMOPOLITAN_LIBC_RUNTIME_PTHREAD_H_

#define PTHREAD_ONCE_INIT 0

#define PTHREAD_KEYS_MAX 64

#define PTHREAD_BARRIER_SERIAL_THREAD 31337

#define PTHREAD_MUTEX_DEFAULT    PTHREAD_MUTEX_NORMAL
#define PTHREAD_MUTEX_NORMAL     0
#define PTHREAD_MUTEX_RECURSIVE  1
#define PTHREAD_MUTEX_ERRORCHECK 2
#define PTHREAD_MUTEX_STALLED    0
#define PTHREAD_MUTEX_ROBUST     1

#define PTHREAD_PROCESS_DEFAULT PTHREAD_PROCESS_PRIVATE
#define PTHREAD_PROCESS_PRIVATE 0
#define PTHREAD_PROCESS_SHARED  1

#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

/* clang-format off */
#define PTHREAD_MUTEX_INITIALIZER {PTHREAD_MUTEX_DEFAULT}
#define PTHREAD_COND_INITIALIZER {PTHREAD_PROCESS_DEFAULT}
#define PTHREAD_BARRIER_INITIALIZER {PTHREAD_PROCESS_DEFAULT}
#define PTHREAD_RWLOCK_INITIALIZER {PTHREAD_PROCESS_DEFAULT}
/* clang-format on */

typedef void *pthread_t;
typedef int pthread_id_np_t;
typedef int pthread_condattr_t;
typedef int pthread_mutexattr_t;
typedef int pthread_rwlockattr_t;
typedef int pthread_barrierattr_t;
typedef unsigned pthread_key_t;
typedef _Atomic(char) pthread_once_t;
typedef _Atomic(char) pthread_spinlock_t;
typedef void (*pthread_key_dtor)(void *);

typedef struct {
  int attr;
  int reent;
  _Atomic(int) lock;
  _Atomic(int) waits;
} pthread_mutex_t;

typedef struct {
  int attr;
  _Atomic(int) waits;
  _Atomic(unsigned) seq;
} pthread_cond_t;

typedef struct {
  int attr;
  int count;
  _Atomic(int) waits;
  _Atomic(int) popped;
} pthread_barrier_t;

typedef struct {
  int attr;
  _Atomic(int) lock;
  _Atomic(int) waits;
} pthread_rwlock_t;

typedef struct {
  int scope;
  int schedpolicy;
  int detachstate;
  int inheritsched;
  size_t guardsize;
  size_t stacksize;
} pthread_attr_t;

int pthread_yield(void);
void pthread_exit(void *) wontreturn;
pthread_t pthread_self(void) pureconst;
pthread_id_np_t pthread_getthreadid_np(void);
int64_t pthread_getunique_np(pthread_t);
int pthread_attr_init(pthread_attr_t *);
int pthread_attr_destroy(pthread_attr_t *);
int pthread_attr_getdetachstate(const pthread_attr_t *, int *);
int pthread_attr_setdetachstate(pthread_attr_t *, int);
int pthread_attr_getguardsize(const pthread_attr_t *, size_t *);
int pthread_attr_setguardsize(pthread_attr_t *, size_t);
int pthread_attr_getinheritsched(const pthread_attr_t *, int *);
int pthread_attr_setinheritsched(pthread_attr_t *, int);
int pthread_attr_getschedpolicy(const pthread_attr_t *, int *);
int pthread_attr_setschedpolicy(pthread_attr_t *, int);
int pthread_attr_getscope(const pthread_attr_t *, int *);
int pthread_attr_setscope(pthread_attr_t *, int);
int pthread_attr_getstack(const pthread_attr_t *, void **, size_t *);
int pthread_attr_setstack(pthread_attr_t *, void *, size_t);
int pthread_attr_getstacksize(const pthread_attr_t *, size_t *);
int pthread_attr_setstacksize(pthread_attr_t *, size_t);
int pthread_create(pthread_t *, const pthread_attr_t *, void *(*)(void *),
                   void *);
int pthread_detach(pthread_t);
int pthread_cancel(pthread_t);
int pthread_join(pthread_t, void **);
int pthread_equal(pthread_t, pthread_t);
int pthread_once(pthread_once_t *, void (*)(void));
int pthread_spin_init(pthread_spinlock_t *, int);
int pthread_spin_destroy(pthread_spinlock_t *);
int pthread_spin_lock(pthread_spinlock_t *);
int pthread_spin_unlock(pthread_spinlock_t *);
int pthread_spin_trylock(pthread_spinlock_t *);
int pthread_mutexattr_init(pthread_mutexattr_t *);
int pthread_mutexattr_destroy(pthread_mutexattr_t *);
int pthread_mutexattr_gettype(const pthread_mutexattr_t *, int *);
int pthread_mutexattr_settype(pthread_mutexattr_t *, int);
int pthread_mutex_init(pthread_mutex_t *, const pthread_mutexattr_t *);
int pthread_mutex_lock(pthread_mutex_t *);
int pthread_mutex_unlock(pthread_mutex_t *);
int pthread_mutex_trylock(pthread_mutex_t *);
int pthread_mutex_destroy(pthread_mutex_t *);
int pthread_mutex_consistent(pthread_mutex_t *);
int pthread_condattr_init(pthread_condattr_t *);
int pthread_condattr_destroy(pthread_condattr_t *);
int pthread_condattr_setpshared(pthread_condattr_t *, int);
int pthread_condattr_getpshared(const pthread_condattr_t *, int *);
int pthread_cond_init(pthread_cond_t *, const pthread_condattr_t *);
int pthread_cond_destroy(pthread_cond_t *);
int pthread_cond_wait(pthread_cond_t *, pthread_mutex_t *);
int pthread_cond_broadcast(pthread_cond_t *);
int pthread_cond_signal(pthread_cond_t *);
int pthread_rwlockattr_init(pthread_rwlockattr_t *);
int pthread_rwlockattr_destroy(pthread_rwlockattr_t *);
int pthread_rwlockattr_setpshared(pthread_rwlockattr_t *, int);
int pthread_rwlockattr_getpshared(const pthread_rwlockattr_t *, int *);
int pthread_rwlock_init(pthread_rwlock_t *, const pthread_rwlockattr_t *);
int pthread_rwlock_destroy(pthread_rwlock_t *);
int pthread_rwlock_rdlock(pthread_rwlock_t *);
int pthread_rwlock_tryrdlock(pthread_rwlock_t *);
int pthread_rwlock_wrlock(pthread_rwlock_t *);
int pthread_rwlock_trywrlock(pthread_rwlock_t *);
int pthread_rwlock_unlock(pthread_rwlock_t *);
int pthread_key_create(pthread_key_t *, pthread_key_dtor);
int pthread_key_delete(pthread_key_t);
int pthread_setspecific(pthread_key_t, void *);
void *pthread_getspecific(pthread_key_t);
int pthread_barrierattr_init(pthread_barrierattr_t *);
int pthread_barrierattr_destroy(pthread_barrierattr_t *);
int pthread_barrierattr_getpshared(const pthread_barrierattr_t *, int *);
int pthread_barrierattr_setpshared(pthread_barrierattr_t *, int);
int pthread_barrier_wait(pthread_barrier_t *);
int pthread_barrier_destroy(pthread_barrier_t *);
int pthread_barrier_init(pthread_barrier_t *, const pthread_barrierattr_t *,
                         unsigned);

#define pthread_spin_init(pSpin, multiprocess) (*(pSpin) = 0)
#define pthread_spin_destroy(pSpin)            (*(pSpin) = 0)

#if (__GNUC__ + 0) * 100 + (__GNUC_MINOR__ + 0) >= 407
extern const errno_t EBUSY;
#define pthread_spin_unlock(pSpin) \
  (__atomic_store_n(pSpin, 0, __ATOMIC_RELAXED), 0)
#define pthread_spin_trylock(pSpin) \
  (__atomic_test_and_set(pSpin, __ATOMIC_SEQ_CST) ? EBUSY : 0)
#ifdef TINY
#define pthread_spin_lock(pSpin) __pthread_spin_lock_tiny(pSpin)
#else
#define pthread_spin_lock(pSpin) __pthread_spin_lock_cooperative(pSpin)
#endif
#define __pthread_spin_lock_tiny(pSpin)                      \
  ({                                                         \
    while (__atomic_test_and_set(pSpin, __ATOMIC_SEQ_CST)) { \
      __builtin_ia32_pause();                                \
    }                                                        \
    0;                                                       \
  })
#define __pthread_spin_lock_cooperative(pSpin)                        \
  ({                                                                  \
    char __x;                                                         \
    volatile int __i;                                                 \
    unsigned __tries = 0;                                             \
    pthread_spinlock_t *__lock = pSpin;                               \
    for (;;) {                                                        \
      __atomic_load(__lock, &__x, __ATOMIC_RELAXED);                  \
      if (!__x && !__atomic_test_and_set(__lock, __ATOMIC_SEQ_CST)) { \
        break;                                                        \
      } else if (__tries < 7) {                                       \
        for (__i = 0; __i != 1 << __tries; __i++) {                   \
        }                                                             \
        __tries++;                                                    \
      } else {                                                        \
        pthread_yield();                                              \
      }                                                               \
    }                                                                 \
    0;                                                                \
  })
#endif /* GCC 4.7+ */

#define pthread_mutexattr_init(pAttr)           (*(pAttr) = PTHREAD_MUTEX_DEFAULT, 0)
#define pthread_mutexattr_destroy(pAttr)        (*(pAttr) = 0)
#define pthread_mutexattr_gettype(pAttr, pType) (*(pType) = *(pAttr), 0)

#ifdef __GNUC__
#define pthread_mutex_init(mutex, pAttr)          \
  ({                                              \
    pthread_mutexattr_t *_pAttr = (pAttr);        \
    *(mutex) = (pthread_mutex_t){                 \
        _pAttr ? *_pAttr : PTHREAD_MUTEX_DEFAULT, \
    };                                            \
    0;                                            \
  })
#endif

#if (__GNUC__ + 0) * 100 + (__GNUC_MINOR__ + 0) >= 407
#define pthread_mutex_lock(mutex)                              \
  (((mutex)->attr == PTHREAD_MUTEX_NORMAL &&                   \
    !__atomic_load_n(&(mutex)->lock, __ATOMIC_RELAXED) &&      \
    !__atomic_exchange_n(&(mutex)->lock, 1, __ATOMIC_SEQ_CST)) \
       ? 0                                                     \
       : pthread_mutex_lock(mutex))
#define pthread_mutex_unlock(mutex)                              \
  ((mutex)->attr == PTHREAD_MUTEX_NORMAL                         \
       ? (__atomic_store_n(&(mutex)->lock, 0, __ATOMIC_RELAXED), \
          (__atomic_load_n(&(mutex)->waits, __ATOMIC_RELAXED) && \
           _pthread_mutex_wake(mutex)),                          \
          0)                                                     \
       : pthread_mutex_unlock(mutex))
#endif

#define pthread_condattr_init(pAttr)                 (*(pAttr) = PTHREAD_PROCESS_DEFAULT, 0)
#define pthread_condattr_destroy(pAttr)              (*(pAttr) = 0)
#define pthread_condattr_getpshared(pAttr, pPshared) (*(pPshared) = *(pAttr), 0)

#ifdef __GNUC__
#define pthread_cond_init(cond, pAttr)              \
  ({                                                \
    pthread_condattr_t *_pAttr = (pAttr);           \
    *(cond) = (pthread_cond_t){                     \
        _pAttr ? *_pAttr : PTHREAD_PROCESS_DEFAULT, \
    };                                              \
    0;                                              \
  })
#endif

#define pthread_barrierattr_init(pAttr)    (*(pAttr) = PTHREAD_PROCESS_DEFAULT, 0)
#define pthread_barrierattr_destroy(pAttr) (*(pAttr) = 0)
#define pthread_barrierattr_getpshared(pAttr, pPshared) \
  (*(pPshared) = *(pAttr), 0)

#define pthread_rwlockattr_init(pAttr)    (*(pAttr) = PTHREAD_PROCESS_DEFAULT, 0)
#define pthread_rwlockattr_destroy(pAttr) (*(pAttr) = 0)
#define pthread_rwlockattr_getpshared(pAttr, pPshared) \
  (*(pPshared) = *(pAttr), 0)

#ifdef __GNUC__
#define pthread_rwlock_init(rwlock, pAttr)          \
  ({                                                \
    pthread_rwlockattr_t *_pAttr = (pAttr);         \
    *(rwlock) = (pthread_rwlock_t){                 \
        _pAttr ? *_pAttr : PTHREAD_PROCESS_DEFAULT, \
    };                                              \
    0;                                              \
  })
#endif

int _pthread_mutex_wake(pthread_mutex_t *) hidden;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_RUNTIME_PTHREAD_H_ */
