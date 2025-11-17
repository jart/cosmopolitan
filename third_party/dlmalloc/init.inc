
void dlmalloc_pre_fork(void) {
  if (__isthreaded < 2)
    return;
  ACQUIRE_LOCK(&g_gil);
  for (long i = 0; i < COSMO_SHARDS; ++i)
    if (g_heaps[i].state.magic)
      ACQUIRE_LOCK(&g_heaps[i].state.mutex);
}

void dlmalloc_post_fork_parent(void) {
  if (__isthreaded < 2)
    return;
  for (long i = COSMO_SHARDS; i--;)
    if (g_heaps[i].state.magic)
      RELEASE_LOCK(&g_heaps[i].state.mutex);
  RELEASE_LOCK(&g_gil);
}

void dlmalloc_post_fork_child(void) {
  for (long i = 0; i < COSMO_SHARDS; ++i)
    if (g_heaps[i].state.magic)
      REFRESH_LOCK(&g_heaps[i].state.mutex);
  REFRESH_LOCK(&g_gil);
}

/* Initialize mparams */
__attribute__((__constructor__(49))) void init_mparams(void) {
  size_t magic, psize, gsize;

  psize = __pagesize;
  gsize = DEFAULT_GRANULARITY ? DEFAULT_GRANULARITY : __gransize;

  /* Sanity-check configuration:
     size_t must be unsigned and as wide as pointer type.
     ints must be at least 4 bytes.
     alignment must be at least 8.
     Alignment, min chunk size, and page size must all be powers of 2.
  */
  static_assert(sizeof(size_t) == sizeof(char*));
  static_assert(MAX_SIZE_T >= MIN_CHUNK_SIZE);
  static_assert(MALLOC_ALIGNMENT >= (size_t)8U);
  static_assert(MALLOC_ALIGNMENT >= (size_t)8U);
  static_assert((MALLOC_ALIGNMENT & (MALLOC_ALIGNMENT-SIZE_T_ONE)) == 0);
  static_assert((MCHUNK_SIZE      & (MCHUNK_SIZE-SIZE_T_ONE))      == 0);
  unassert(     (gsize            & (gsize-SIZE_T_ONE))            == 0 &&
                (psize            & (psize-SIZE_T_ONE))            == 0);
  mparams.granularity = gsize;
  mparams.page_size = psize;
  mparams.mmap_threshold = DEFAULT_MMAP_THRESHOLD;
  mparams.trim_threshold = DEFAULT_TRIM_THRESHOLD;
  mparams.default_mflags = USE_LOCK_BIT|USE_MMAP_BIT|USE_NONCONTIGUOUS_BIT;

  magic = (size_t)(rdtsc() ^ (size_t)0x55555555U);
  magic |= (size_t)8U;    /* ensure nonzero */
  magic &= ~(size_t)7U;   /* improve chances of fault for bad values */
  *(volatile size_t *)&mparams.magic = magic;

  // Initialize threaded allocator for main thread.
  __get_tls()->tib_malloc = tmspace_acquire();

  // Bolt on rseq allocator if Linux 4.18+ (c. 2018)
  use_threaded_allocator();
  if (HAVE_RSEQ) {
    const char *s;
    if ((s = getenv("COSMOPOLITAN_M_RSEQ_MAX")))
      atomic_init(&rseq.max, strtol(s, 0, 0));
    if (rseq.max)
      use_rseq_allocator();
  }

  __runlevel = RUNLEVEL_MALLOC;
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
    } else {
      return 0;
    }
  case M_MMAP_THRESHOLD:
    mparams.mmap_threshold = val;
    return 1;
  case M_RSEQ_MAX:
    if (HAVE_RSEQ) {
      atomic_store_explicit(&rseq.max, val, memory_order_relaxed);
      if (val) {
        use_rseq_allocator();
      } else if (never_used_rseq()) {
        use_threaded_allocator();
      }
      return 1;
    } else {
      return 0;
    }
  default:
    return 0;
  }
}
