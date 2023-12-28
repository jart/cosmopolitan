
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

static int malloc_wipe(MLOCK_T *lk) {
  bzero(lk, sizeof(*lk));
  return 0;
}

static int malloc_lock(MLOCK_T *lk) {
  if (!__threaded) return 0;
  while (atomic_exchange_explicit(lk, 1, memory_order_acquire)) {
    pthread_pause_np();
  }
  return 0;
}

static int malloc_unlock(MLOCK_T *lk) {
  if (!__threaded) return 0;
  atomic_store_explicit(lk, 0, memory_order_release);
  return 0;
}

#else

#define MLOCK_T nsync_mu

static int malloc_wipe(MLOCK_T *lk) {
  bzero(lk, sizeof(*lk));
  return 0;
}

static int malloc_lock(MLOCK_T *lk) {
  if (!__threaded) return 0;
  nsync_mu_lock(lk);
  return 0;
}

static int malloc_unlock(MLOCK_T *lk) {
  if (!__threaded) return 0;
  nsync_mu_unlock(lk);
  return 0;
}

#endif

#define ACQUIRE_LOCK(lk) malloc_lock(lk)
#define RELEASE_LOCK(lk) malloc_unlock(lk)
#define INITIAL_LOCK(lk) malloc_wipe(lk)
#define DESTROY_LOCK(lk)
#define ACQUIRE_MALLOC_GLOBAL_LOCK() ACQUIRE_LOCK(&malloc_global_mutex);
#define RELEASE_MALLOC_GLOBAL_LOCK() RELEASE_LOCK(&malloc_global_mutex);

static MLOCK_T malloc_global_mutex;

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
