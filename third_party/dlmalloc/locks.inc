#include "libc/cosmo.h"
#include "libc/intrin/kprintf.h"
#include "libc/intrin/maps.h"
#include "libc/thread/thread.h"

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

#if USE_SPIN_LOCKS

#define MLOCK_T atomic_uint

static int malloc_inlk(MLOCK_T *lk) {
  atomic_store_explicit(lk, 0, memory_order_relaxed);
  return 0;
}

static int malloc_wipe(MLOCK_T *lk) {
  atomic_store_explicit(lk, 0, memory_order_relaxed);
  return 0;
}

static int malloc_kilk(MLOCK_T *lk) {
  return 0;
}

static int malloc_lock(MLOCK_T *lk) {
  for (;;) {
    if (!atomic_exchange_explicit(lk, 1, memory_order_acquire))
      break;
    for (;;)
      if (!atomic_load_explicit(lk, memory_order_relaxed))
        break;
  }
  return 0;
}

static int malloc_unlk(MLOCK_T *lk) {
  atomic_store_explicit(lk, 0, memory_order_release);
  return 0;
}

#else

#define MLOCK_T struct MallocLock

struct MallocLock {
#if DEBUG
  void *edges;
#endif
  nsync_mu mu;
};

static int malloc_inlk(MLOCK_T *lk) {
  bzero(lk, sizeof(*lk));
  return 0;
}

static int malloc_wipe(MLOCK_T *lk) {
  bzero(&lk->mu, sizeof(lk->mu));
  return 0;
}

static int malloc_kilk(MLOCK_T *lk) {
  return 0;
}

static int malloc_lock(MLOCK_T *lk) {
#if DEBUG
  __deadlock_check(lk, 0);
#endif
  nsync_mu_lock(&lk->mu);
#if DEBUG
  __deadlock_record(lk, 0);
  __deadlock_track(lk, 0);
#endif
  return 0;
}

static int malloc_unlk(MLOCK_T *lk) {
#if DEBUG
  if (__deadlock_tracked(lk) == 0) {
    kprintf("error: unlock malloc mutex not owned by caller: %t\n", lk);
    DebugBreak();
  }
#endif
  nsync_mu_unlock(&lk->mu);
#if DEBUG
  __deadlock_untrack(lk);
#endif
  return 0;
}

#endif

#define ACQUIRE_LOCK(lk) malloc_lock(lk)
#define RELEASE_LOCK(lk) malloc_unlk(lk)
#define INITIAL_LOCK(lk) malloc_inlk(lk)
#define REFRESH_LOCK(lk) malloc_wipe(lk)
#define DESTROY_LOCK(lk) malloc_kilk(lk)

#define USE_LOCK_BIT               (2U)

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
