#include "libc/sysv/consts/auxv.h"
#include "libc/runtime/runtime.h"
#include "libc/nexgen32e/rdtsc.h"
#include "libc/runtime/runtime.h"

void dlmalloc_pre_fork(void) {
#if ONLY_MSPACES
  mstate h;
  for (unsigned i = ARRAYLEN(g_heaps); i--;)
    if ((h = atomic_load_explicit(&g_heaps[i], memory_order_acquire)))
      ACQUIRE_LOCK(&h->mutex);
#else
  ACQUIRE_LOCK(&(gm)->mutex);
#endif
}

void dlmalloc_post_fork_parent(void) {
#if ONLY_MSPACES
  mstate h;
  for (unsigned i = 0; i < ARRAYLEN(g_heaps); ++i)
    if ((h = atomic_load_explicit(&g_heaps[i], memory_order_acquire)))
      RELEASE_LOCK(&h->mutex);
#else
  RELEASE_LOCK(&(gm)->mutex);
#endif
}

void dlmalloc_post_fork_child(void) {
#if ONLY_MSPACES
  mstate h;
  for (unsigned i = 0; i < ARRAYLEN(g_heaps); ++i)
    if ((h = atomic_load_explicit(&g_heaps[i], memory_order_acquire)))
      REFRESH_LOCK(&h->mutex);
#else
  REFRESH_LOCK(&(gm)->mutex);
#endif
}

/* Initialize mparams */
__attribute__((__constructor__(49))) int init_mparams(void) {

  if (mparams.magic == 0) {
    size_t magic;
    size_t psize;
    size_t gsize;

    psize = __pagesize;
    gsize = DEFAULT_GRANULARITY ? DEFAULT_GRANULARITY : psize;

    /* Sanity-check configuration:
       size_t must be unsigned and as wide as pointer type.
       ints must be at least 4 bytes.
       alignment must be at least 8.
       Alignment, min chunk size, and page size must all be powers of 2.
    */
    if ((sizeof(size_t) != sizeof(char*)) ||
        (MAX_SIZE_T < MIN_CHUNK_SIZE)  ||
        (sizeof(int) < 4)  ||
        (MALLOC_ALIGNMENT < (size_t)8U) ||
        ((MALLOC_ALIGNMENT & (MALLOC_ALIGNMENT-SIZE_T_ONE)) != 0) ||
        ((MCHUNK_SIZE      & (MCHUNK_SIZE-SIZE_T_ONE))      != 0) ||
        ((gsize            & (gsize-SIZE_T_ONE))            != 0) ||
        ((psize            & (psize-SIZE_T_ONE))            != 0))
      ABORT;
    mparams.granularity = gsize;
    mparams.page_size = psize;
    mparams.mmap_threshold = DEFAULT_MMAP_THRESHOLD;
    mparams.trim_threshold = DEFAULT_TRIM_THRESHOLD;
    mparams.default_mflags = USE_LOCK_BIT|USE_MMAP_BIT|USE_NONCONTIGUOUS_BIT;

#if !ONLY_MSPACES
    /* Set up lock for main malloc area */
    gm->mflags = mparams.default_mflags;
    (void)INITIAL_LOCK(&gm->mutex);
#endif

    {
#if USE_DEV_RANDOM
      int fd;
      unsigned char buf[sizeof(size_t)];
      /* Try to use /dev/urandom, else fall back on using time */
      if ((fd = open("/dev/urandom", O_RDONLY)) >= 0 &&
          read(fd, buf, sizeof(buf)) == sizeof(buf)) {
        magic = *((size_t *) buf);
        close(fd);
      }
      else
#endif /* USE_DEV_RANDOM */
      magic = (size_t)(rdtsc() ^ (size_t)0x55555555U);
      magic |= (size_t)8U;    /* ensure nonzero */
      magic &= ~(size_t)7U;   /* improve chances of fault for bad values */
      /* Until memory modes commonly available, use volatile-write */
      (*(volatile size_t *)(&(mparams.magic))) = magic;
    }
  }

#if ONLY_MSPACES
  threaded_dlmalloc();
#endif

  __runlevel = RUNLEVEL_MALLOC;
  return 1;
}

/* support for mallopt */
static int change_mparam(int param_number, int value) {
  size_t val;
  ensure_initialization();
  val = (value == -1)? MAX_SIZE_T : (size_t)value;
  switch(param_number) {
  case M_TRIM_THRESHOLD:
    mparams.trim_threshold = val;
    return 1;
  case M_GRANULARITY:
    if (val >= mparams.page_size && ((val & (val-1)) == 0)) {
      mparams.granularity = val;
      return 1;
    }
    else
      return 0;
  case M_MMAP_THRESHOLD:
    mparams.mmap_threshold = val;
    return 1;
  default:
    return 0;
  }
}
