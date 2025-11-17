// Cosmopolitan Threaded Memory Allocator
//
// This file provides functions named dlX_threaded() which implement the
// dlmalloc api for multithreaded environments. This implementation goes
// fast for a variety of use cases, and it's fully portable across OSes.
//
// We use two strategies for making multi-threaded malloc() go fast. Our
// first preference is to give each thread its own heap. The thread will
// store a pointer to its mspace in thread-local storage. We recycle the
// heaps we assign. This is very fast because it provides good assurance
// that the mutex on each heap will be uncontended. The only time a lock
// contention should happen, is if a thread shares its malloc'd pointer.
// The downside to assigning each thread its own mspace is if we want to
// have 10,000 threads, it becomes incredibly wasteful. So what we'll do
// is switch to sharding allocations over a fixed number of heaps, based
// on the cpu index, once we have a large number of simultaneous threads

#if !FOOTERS || !MSPACES
#error "threaded dlmalloc needs footers and mspaces"
#endif

static_assert(COSMO_SHARDS == 32);
static_assert(sizeof(unsigned) * 8 == 32);

// bss memory to initialize our heaps which avoids mmapping upon startup
// more critically having this be static ensures pick_heap() never fails
static union {
  alignas(16) char memory[16384];
  struct {
    char foot[16];
    struct malloc_state state;
  };
} g_heaps[COSMO_SHARDS];

static unsigned g_assigned;
static MLOCK_T g_gil;

// tls stores a function pointer to one of these which avoid mispredicts
static void *tmspace_get   (void) { return &g_heaps[cosmo_shard()].state; }
static void *tmspace_get_0 (void) { return &g_heaps[ 0].state; }
static void *tmspace_get_1 (void) { return &g_heaps[ 1].state; }
static void *tmspace_get_2 (void) { return &g_heaps[ 2].state; }
static void *tmspace_get_3 (void) { return &g_heaps[ 3].state; }
static void *tmspace_get_4 (void) { return &g_heaps[ 4].state; }
static void *tmspace_get_5 (void) { return &g_heaps[ 5].state; }
static void *tmspace_get_6 (void) { return &g_heaps[ 6].state; }
static void *tmspace_get_7 (void) { return &g_heaps[ 7].state; }
static void *tmspace_get_8 (void) { return &g_heaps[ 8].state; }
static void *tmspace_get_9 (void) { return &g_heaps[ 9].state; }
static void *tmspace_get_10(void) { return &g_heaps[10].state; }
static void *tmspace_get_11(void) { return &g_heaps[11].state; }
static void *tmspace_get_12(void) { return &g_heaps[12].state; }
static void *tmspace_get_13(void) { return &g_heaps[13].state; }
static void *tmspace_get_14(void) { return &g_heaps[14].state; }
static void *tmspace_get_15(void) { return &g_heaps[15].state; }
static void *tmspace_get_16(void) { return &g_heaps[16].state; }
static void *tmspace_get_17(void) { return &g_heaps[17].state; }
static void *tmspace_get_18(void) { return &g_heaps[18].state; }
static void *tmspace_get_19(void) { return &g_heaps[19].state; }
static void *tmspace_get_20(void) { return &g_heaps[20].state; }
static void *tmspace_get_21(void) { return &g_heaps[21].state; }
static void *tmspace_get_22(void) { return &g_heaps[22].state; }
static void *tmspace_get_23(void) { return &g_heaps[23].state; }
static void *tmspace_get_24(void) { return &g_heaps[24].state; }
static void *tmspace_get_25(void) { return &g_heaps[25].state; }
static void *tmspace_get_26(void) { return &g_heaps[26].state; }
static void *tmspace_get_27(void) { return &g_heaps[27].state; }
static void *tmspace_get_28(void) { return &g_heaps[28].state; }
static void *tmspace_get_29(void) { return &g_heaps[29].state; }
static void *tmspace_get_30(void) { return &g_heaps[30].state; }
static void *tmspace_get_31(void) { return &g_heaps[31].state; }

static const tmspace_get_f kTmspaceGetter[COSMO_SHARDS] = {
    tmspace_get_0,
    tmspace_get_1,
    tmspace_get_2,
    tmspace_get_3,
    tmspace_get_4,
    tmspace_get_5,
    tmspace_get_6,
    tmspace_get_7,
    tmspace_get_8,
    tmspace_get_9,
    tmspace_get_10,
    tmspace_get_11,
    tmspace_get_12,
    tmspace_get_13,
    tmspace_get_14,
    tmspace_get_15,
    tmspace_get_16,
    tmspace_get_17,
    tmspace_get_18,
    tmspace_get_19,
    tmspace_get_20,
    tmspace_get_21,
    tmspace_get_22,
    tmspace_get_23,
    tmspace_get_24,
    tmspace_get_25,
    tmspace_get_26,
    tmspace_get_27,
    tmspace_get_28,
    tmspace_get_29,
    tmspace_get_30,
    tmspace_get_31,
};

tmspace_get_f tmspace_acquire(void) {
  tmspace_get_f res = &tmspace_get;
  if (~g_assigned) {
    bool switch_to_cpu_shard = false;
    ACQUIRE_LOCK(&g_gil);
    if (~g_assigned) {
      unsigned index = bsf(~g_assigned);
      g_assigned |= 1u << index;
      if (!g_heaps[index].state.magic)
        create_mspace_with_base(&g_heaps[index], sizeof(g_heaps[index]), true);
      if (~g_assigned) {
        res = kTmspaceGetter[index];
      } else {
        switch_to_cpu_shard = true;
      }
    }
    RELEASE_LOCK(&g_gil);
    if (switch_to_cpu_shard) {
      // permanently transition malloc runtime to always use cpuid
      // sharding once the process reaches 32 simultaneous threads
      _pthread_lock();
      for (struct Dll *e = dll_first(_pthread_list); e; e = dll_next(_pthread_list, e))
        POSIXTHREAD_CONTAINER(e)->tib->tib_malloc = &tmspace_get;
      _pthread_unlock();
    }
  }
  return res;
}

void tmspace_release(tmspace_get_f getter) {
  if (~g_assigned && getter != &tmspace_get) {
    mspace_trim(getter(), 0);
    ACQUIRE_LOCK(&g_gil);
    if (~g_assigned)
      for (unsigned index = 0; index < COSMO_SHARDS; ++index)
        if (getter == kTmspaceGetter[index])
          g_assigned &= ~(1u << index);
    RELEASE_LOCK(&g_gil);
  }
}

static void dlfree_threaded(void *p) {
  // we configured dlmalloc to store the mpsace pointer in the footer to
  // ensure we free on the old heap if the kernel reschedules our thread
  return mspace_free(0, p);
}

static size_t dlmalloc_usable_size_threaded(void *mem) {
  return mspace_usable_size(mem);
}

static void *dlrealloc_in_place_threaded(void *p, size_t n) {
  return mspace_realloc_in_place(0, p, n);
}

int dlmallopt(int param_number, int value) {
  return mspace_mallopt(param_number, value);
}

static int dlmalloc_trim_threaded(size_t pad) {
  int got_some = 0;
  for (long i = 0; i < COSMO_SHARDS && g_heaps[i].state.magic; ++i)
    got_some |= mspace_trim(&g_heaps[i].state, pad);
  return got_some;
}

struct ThreadedMallocVisitor {
  mstate heap;
  void (*handler)(void *start, void *end,
                  size_t used_bytes, void *arg);
  void *arg;
};

static void threaded_malloc_visitor(void *start, void *end,
                                    size_t used_bytes, void *arg) {
  struct ThreadedMallocVisitor *tmv = arg;
  if (start == tmv->heap)
    return;
  tmv->handler(start, end, used_bytes, tmv->arg);
}

static void dlmalloc_inspect_all_threaded(void handler(void *start, void *end,
                                                       size_t used_bytes, void *arg),
                                          void *arg) {
  for (long i = 0; i < COSMO_SHARDS && g_heaps[i].state.magic; ++i) {
    struct ThreadedMallocVisitor tmv = {&g_heaps[i].state, handler, arg};
    mspace_inspect_all(&g_heaps[i].state, threaded_malloc_visitor, &tmv);
  }
}

static inline mspace pick_heap(void) {
  tmspace_get_f getter = __get_tls()->tib_malloc;
  return getter();
}

static void *dlmalloc_threaded(size_t n) {
  return mspace_malloc(pick_heap(), n);
}

static void *dlcalloc_threaded(size_t n, size_t z) {
  return mspace_calloc(pick_heap(), n, z);
}

static void *dlrealloc_threaded(void *p, size_t n) {
  if (p)
    return mspace_realloc(0, p, n);
  return dlmalloc_threaded(n);
}

static void *dlmemalign_threaded(size_t a, size_t n) {
  return mspace_memalign(pick_heap(), a, n);
}

static struct mallinfo dlmallinfo_threaded(void) {
  struct mallinfo res = {0};
  for (long i = 0; i < COSMO_SHARDS && g_heaps[i].state.magic; ++i) {
    struct mallinfo mi = mspace_mallinfo(&g_heaps[i].state);
    res.arena += mi.arena;
    res.ordblks += mi.ordblks;
    res.hblkhd += mi.hblkhd;
    res.usmblks += mi.usmblks;
    res.uordblks += mi.uordblks;
    res.fordblks += mi.fordblks;
    res.keepcost += mi.keepcost;
  }
  return res;
}

// b/c it's possible for malloc() to return memory inside .bss lool
bool __is_g_heaps(void *ptr) {
  char *p = ptr;
  char *a = (char *)g_heaps;
  char *b = (char *)g_heaps + sizeof(g_heaps);
  return a <= p && p < b;
}

dontinline static void use_threaded_allocator(void) {
  dlmalloc = dlmalloc_threaded;
  dlfree = dlfree_threaded;
  dlcalloc = dlcalloc_threaded;
  dlrealloc = dlrealloc_threaded;
  dlrealloc_in_place = dlrealloc_in_place_threaded;
  dlmemalign = dlmemalign_threaded;
  dlmalloc_usable_size = dlmalloc_usable_size_threaded;
  dlmalloc_inspect_all = dlmalloc_inspect_all_threaded;
  dlmallinfo = dlmallinfo_threaded;
  dlmalloc_trim = dlmalloc_trim_threaded;
}
