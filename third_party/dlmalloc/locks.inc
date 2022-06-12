// clang-format off
#include "libc/nexgen32e/threaded.h"

/* --------------------------- Lock preliminaries ------------------------ */

/*
  When locks are defined, there is one global lock, plus
  one per-mspace lock.

  The global lock_ensures that mparams.magic and other unique
  mparams values are initialized only once. It also protects
  sequences of calls to MORECORE.  In many cases sys_alloc requires
  two calls, that should not be interleaved with calls by other
  threads.  This does not protect against direct calls to MORECORE
  by other threads not using this lock, so there is still code to
  cope the best we can on interference.

  Per-mspace locks surround calls to malloc, free, etc.
  By default, locks are simple non-reentrant mutexes.

  Because lock-protected regions generally have bounded times, it is
  OK to use the supplied simple spinlocks. Spinlocks are likely to
  improve performance for lightly contended applications, but worsen
  performance under heavy contention.

  If USE_LOCKS is > 1, the definitions of lock routines here are
  bypassed, in which case you will need to define the type MLOCK_T,
  and at least INITIAL_LOCK, DESTROY_LOCK, ACQUIRE_LOCK, RELEASE_LOCK
  and TRY_LOCK.  You must also declare a
    static MLOCK_T malloc_global_mutex = { initialization values };.

*/

#if !USE_LOCKS
#define USE_LOCK_BIT               (0U)
#define INITIAL_LOCK(l)            (0)
#define DESTROY_LOCK(l)            (0)
#define ACQUIRE_MALLOC_GLOBAL_LOCK()
#define RELEASE_MALLOC_GLOBAL_LOCK()

#else
#if USE_LOCKS > 1
/* -----------------------  User-defined locks ------------------------ */
/* Define your own lock implementation here */
/* #define INITIAL_LOCK(lk)  ... */
/* #define DESTROY_LOCK(lk)  ... */
/* #define ACQUIRE_LOCK(lk)  ... */
/* #define RELEASE_LOCK(lk)  ... */
/* #define TRY_LOCK(lk) ... */
/* static MLOCK_T malloc_global_mutex = ... */

#elif USE_SPIN_LOCKS

/* First, define CAS_LOCK and CLEAR_LOCK on ints */
/* Note CAS_LOCK defined to return 0 on success */

#if defined(__GNUC__)&& (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 1))
#define CAS_LOCK(sl)     __sync_lock_test_and_set(sl, 1)
#define CLEAR_LOCK(sl)   __sync_lock_release(sl)

#elif (defined(__GNUC__) && (defined(__i386__) || defined(__x86_64__)))
/* Custom spin locks for older gcc on x86 */
FORCEINLINE int x86_cas_lock(int *sl) {
  int ret;
  int val = 1;
  int cmp = 0;
  __asm__ __volatile__  ("lock; cmpxchgl %1, %2"
                         : "=a" (ret)
                         : "r" (val), "m" (*(sl)), "0"(cmp)
                         : "memory", "cc");
  return ret;
}

FORCEINLINE void x86_clear_lock(int* sl) {
  assert(*sl != 0);
  int prev = 0;
  int ret;
  __asm__ __volatile__ ("lock; xchgl %0, %1"
                        : "=r" (ret)
                        : "m" (*(sl)), "0"(prev)
                        : "memory");
}

#define CAS_LOCK(sl)     x86_cas_lock(sl)
#define CLEAR_LOCK(sl)   x86_clear_lock(sl)

#else /* Win32 MSC */
#define CAS_LOCK(sl)     interlockedexchange(sl, (LONG)1)
#define CLEAR_LOCK(sl)   interlockedexchange (sl, (LONG)0)

#endif /* ... gcc spins locks ... */

#if !defined(USE_RECURSIVE_LOCKS) || USE_RECURSIVE_LOCKS == 0
/* Plain spin locks use single word (embedded in malloc_states) */
static dontinline int spin_acquire_lock(int *sl) {
  while (*(volatile int *)sl != 0 || CAS_LOCK(sl)) {
    __builtin_ia32_pause();
  }
  return 0;
}

#define MLOCK_T               int
#define TRY_LOCK(sl)          !CAS_LOCK(sl)
#define RELEASE_LOCK(sl)      if (__threaded) CLEAR_LOCK(sl)
#define ACQUIRE_LOCK(sl)      (__threaded && CAS_LOCK(sl) ? spin_acquire_lock(sl) : 0)
#define INITIAL_LOCK(sl)      (*sl = 0)
#define DESTROY_LOCK(sl)      (0)
static MLOCK_T malloc_global_mutex = 0;

#else /* USE_RECURSIVE_LOCKS */
/* types for lock owners */
#ifdef WIN32
#define THREAD_ID_T           DWORD
#define CURRENT_THREAD        GetCurrentThreadId()
#define EQ_OWNER(X,Y)         ((X) == (Y))
#else
/*
  Note: the following assume that pthread_t is a type that can be
  initialized to (casted) zero. If this is not the case, you will need to
  somehow redefine these or not use spin locks.
*/
#define THREAD_ID_T           pthread_t
#define CURRENT_THREAD        pthread_self()
#define EQ_OWNER(X,Y)         pthread_equal(X, Y)
#endif

struct malloc_recursive_lock {
  int sl;
  unsigned int c;
  THREAD_ID_T threadid;
};

#define MLOCK_T  struct malloc_recursive_lock
static MLOCK_T malloc_global_mutex = { 0, 0, (THREAD_ID_T)0};

FORCEINLINE void recursive_release_lock(MLOCK_T *lk) {
  assert(lk->sl != 0);
  if (--lk->c == 0) {
    CLEAR_LOCK(&lk->sl);
  }
}

FORCEINLINE int recursive_acquire_lock(MLOCK_T *lk) {
  THREAD_ID_T mythreadid = CURRENT_THREAD;
  for (;;) {
    if (*((volatile int *)(&lk->sl)) == 0) {
      if (!CAS_LOCK(&lk->sl)) {
        lk->threadid = mythreadid;
        lk->c = 1;
        return 0;
      }
    }
    else if (EQ_OWNER(lk->threadid, mythreadid)) {
      ++lk->c;
      return 0;
    }
    __builtin_ia32_pause();
  }
}

FORCEINLINE int recursive_try_lock(MLOCK_T *lk) {
  THREAD_ID_T mythreadid = CURRENT_THREAD;
  if (*((volatile int *)(&lk->sl)) == 0) {
    if (!CAS_LOCK(&lk->sl)) {
      lk->threadid = mythreadid;
      lk->c = 1;
      return 1;
    }
  }
  else if (EQ_OWNER(lk->threadid, mythreadid)) {
    ++lk->c;
    return 1;
  }
  return 0;
}

#define RELEASE_LOCK(lk)      recursive_release_lock(lk)
#define TRY_LOCK(lk)          recursive_try_lock(lk)
#define ACQUIRE_LOCK(lk)      recursive_acquire_lock(lk)
#define INITIAL_LOCK(lk)      ((lk)->threadid = (THREAD_ID_T)0, (lk)->sl = 0, (lk)->c = 0)
#define DESTROY_LOCK(lk)      (0)
#endif /* USE_RECURSIVE_LOCKS */

#elif defined(WIN32) /* Win32 critical sections */
#define MLOCK_T               CRITICAL_SECTION
#define ACQUIRE_LOCK(lk)      (EnterCriticalSection(lk), 0)
#define RELEASE_LOCK(lk)      LeaveCriticalSection(lk)
#define TRY_LOCK(lk)          TryEnterCriticalSection(lk)
#define INITIAL_LOCK(lk)      (!InitializeCriticalSectionAndSpinCount((lk), 0x80000000|4000))
#define DESTROY_LOCK(lk)      (DeleteCriticalSection(lk), 0)
#define NEED_GLOBAL_LOCK_INIT

static MLOCK_T malloc_global_mutex;
static volatile LONG malloc_global_mutex_status;

/* Use spin loop to initialize global lock */
static void init_malloc_global_mutex() {
  for (;;) {
    long stat = malloc_global_mutex_status;
    if (stat > 0)
      return;
    /* transition to < 0 while initializing, then to > 0) */
    if (stat == 0 &&
        interlockedcompareexchange(&malloc_global_mutex_status, (LONG)-1, (LONG)0) == 0) {
      InitializeCriticalSection(&malloc_global_mutex);
      interlockedexchange(&malloc_global_mutex_status, (LONG)1);
      return;
    }
    SleepEx(0, FALSE);
  }
}

#else /* pthreads-based locks */
#define MLOCK_T               pthread_mutex_t
#define ACQUIRE_LOCK(lk)      pthread_mutex_lock(lk)
#define RELEASE_LOCK(lk)      pthread_mutex_unlock(lk)
#define TRY_LOCK(lk)          (!pthread_mutex_trylock(lk))
#define INITIAL_LOCK(lk)      pthread_init_lock(lk)
#define DESTROY_LOCK(lk)      pthread_mutex_destroy(lk)

#if defined(USE_RECURSIVE_LOCKS) && USE_RECURSIVE_LOCKS != 0 && defined(linux) && !defined(PTHREAD_MUTEX_RECURSIVE)
/* Cope with old-style linux recursive lock initialization by adding */
/* skipped internal declaration from pthread.h */
extern int pthread_mutexattr_setkind_np __P ((pthread_mutexattr_t *__attr,
                                              int __kind));
#define PTHREAD_MUTEX_RECURSIVE PTHREAD_MUTEX_RECURSIVE_NP
#define pthread_mutexattr_settype(x,y) pthread_mutexattr_setkind_np(x,y)
#endif /* USE_RECURSIVE_LOCKS ... */

static MLOCK_T malloc_global_mutex = PTHREAD_MUTEX_INITIALIZER;

static int pthread_init_lock (MLOCK_T *lk) {
  pthread_mutexattr_t attr;
  if (pthread_mutexattr_init(&attr)) return 1;
#if defined(USE_RECURSIVE_LOCKS) && USE_RECURSIVE_LOCKS != 0
  if (pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE)) return 1;
#endif
  if (pthread_mutex_init(lk, &attr)) return 1;
  if (pthread_mutexattr_destroy(&attr)) return 1;
  return 0;
}

#endif /* ... lock types ... */

/* Common code for all lock types */
#define USE_LOCK_BIT               (2U)

#ifndef ACQUIRE_MALLOC_GLOBAL_LOCK
#define ACQUIRE_MALLOC_GLOBAL_LOCK()  ACQUIRE_LOCK(&malloc_global_mutex);
#endif

#ifndef RELEASE_MALLOC_GLOBAL_LOCK
#define RELEASE_MALLOC_GLOBAL_LOCK()  RELEASE_LOCK(&malloc_global_mutex);
#endif

#endif /* USE_LOCKS */

struct malloc_chunk {
  size_t               prev_foot;  /* Size of previous chunk (if free).  */
  size_t               head;       /* Size and inuse bits. */
  struct malloc_chunk* fd;         /* double links -- used only if free. */
  struct malloc_chunk* bk;
};

typedef struct malloc_chunk  mchunk;
typedef struct malloc_chunk* mchunkptr;
typedef struct malloc_chunk* sbinptr;  /* The type of bins of chunks */
typedef unsigned int bindex_t;         /* Described below */
typedef unsigned int binmap_t;         /* Described below */
typedef unsigned int flag_t;           /* The type of various bit flag sets */
