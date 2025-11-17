// Cosmopolitan Restartable Sequence Memory Allocator
//
// This file provides functions named dlX_rseq() which wrap the threaded
// allocator so small allocations (less than 256 bytes) shall be created
// from a locklessly sharded data structure that's only possible to have
// on Linux 4.18+ using restartable sequences without any locks/atomics.
//
// This design is very simple, but the tradeoff so far is likely that it
// can result in fragmentation in the sense that if malloc() gets called
// several times in a row by the same thread, then each allocation could
// potentially be on its own memory page, which puts pressure on the TLB
//
// Another tradeoff is that memory isn't trimmed automatically. You have
// to call malloc_trim() from an orphaned thread to clean up rseq memory
// although please note that larger allocations will still trim normally
// and M_RSEQ_MAX puts an upper bound on rseq pages, and lastly trimming
// could happen automatically when single-threaded programs oom out rseq

#define CACHELINE 64
#define CHUNK_MAX 256
#define SLAB_SIZE __pagesize
#define TRY_AGAIN ((void *)-1l)
#define ALLOCATED ((struct Chunk *)-23l)
#define HAVE_RSEQ (!IsTiny() && SupportsLinux() && __get_rseq()->cpu_id >= 0)

struct Chunk {
  struct Chunk *next;
  unsigned char flindex;
  alignas(MALLOC_ALIGNMENT) char memory[];
};

struct Slab {
  struct Slab *next;
  alignas(MALLOC_ALIGNMENT) char memory[];
};

static struct {
  alignas(CACHELINE) struct Chunk *freelist[8];
} chunks[CPU_SETSIZE];

alignas(CACHELINE) static struct {
  _Atomic(struct Slab *) slabs;
  _Atomic(size_t) max;
} rseq = {
  .max = 65536, // slabs (pages)
};

static const unsigned kSizes[] = {16, 32, 48, 64, 96, 128, 192, 256};

static const unsigned char kPickSize[CHUNK_MAX + 1] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
    3, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
    4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
    4, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
    5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
    5, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
    6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
    6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
    6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
    6, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
    7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
    7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
    7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
    7,
};

static inline void push_chunk(struct Chunk *chunk, unsigned char flindex) {
#ifdef __x86_64__
  asm volatile(".pushsection .rodata.rseq,\"a\",@progbits\n"
               "	.balign	32\n"
               "300:	.long	0\n"                  // rseq_cs::version
               "	.long	0\n"                  // rseq_cs::flags
               "	.quad	301f\n"               // rseq_cs::start_ip
               "	.quad	302f-301f\n"          // rseq_cs::post_commit_offset
               "	.quad	303f\n"               // rseq_cs::abort_ip
               "	.popsection\n"                //
               "301:	movq	$300b,8(%2)\n"        // rseq->rseq_cs
               "	mov	(%2),%%ecx\n"         // rseq->cpu_id_start
               "	shl	$6,%%ecx\n"           //
               "	mov	(%0,%%rcx),%%rdx\n"   // rdx = freelist
               "	mov	%%rdx,(%1)\n"         // chunk->next = rdx
               "	mov	%1,(%0,%%rcx)\n"      // freelist = chunk
               "302:	.pushsection .text.unlikely,\"ax\",@progbits\n"
               "	.byte	0x0f,0xb9,0x4d\n"
               "	.long	0x53053053\n"
               "303:	jmp	301b\n"               // restart on abort
               "	.popsection"
               : /* no outputs */
               : "r"(&chunks[0].freelist[flindex]), "r"(chunk), "r"(__get_rseq())
               : "rcx", "rdx", "memory");
#elifdef __aarch64__
  asm volatile(".pushsection .rodata.rseq,\"a\",@progbits\n"
               "	.balign	32\n"
               "300:	.long	0\n"                  // rseq_cs::version
               "	.long	0\n"                  // rseq_cs::flags
               "	.quad	301f\n"               // rseq_cs::start_ip
               "	.quad	302f-301f\n"          // rseq_cs::post_commit_offset
               "	.quad	303f\n"               // rseq_cs::abort_ip
               "	.popsection\n"                //
               "301:	adrp	x4, 300b\n"           // load address of rseq_cs
               "	add	x4, x4, :lo12:300b\n" //
               "	str	x4, [%1, #8]\n"       // rseq->rseq_cs
               "	ldr	w4, [%1]\n"           // rseq->cpu_id_start
               "	lsl	w4, w4, #6\n"         //
               "	ldr	x5, [%2, x4]\n"       // x5 = freelist
               "	str	x5, [%0]\n"           // chunk->next = x5
               "	str	%0, [%2, x4]\n"       // freelist = chunk
               "302:	.pushsection .text.unlikely,\"ax\",@progbits\n"
               "	.long	0xd428bc00\n"
               "303:	adrp	x5, 301b\n"           // load page address of 301b
               "	add	x5, x5, :lo12:301b\n" // add low 12 bits
               "	br	x5\n"                 // branch to 301b via register
               "	.popsection"
               : /* no outputs */
               : "r"(chunk), "r"(__get_rseq()), "r"(&chunks[0].freelist[flindex])
               : "x4", "x5", "memory");
#endif
}

static inline struct Chunk *pop_chunk(unsigned char flindex) {
  struct Chunk *chunk;
#ifdef __x86_64__
  asm volatile(".pushsection .rodata.rseq,\"a\",@progbits\n"
               "	.balign	32\n"
               "300:	.long	0\n"                  // rseq_cs::version
               "	.long	0\n"                  // rseq_cs::flags
               "	.quad	301f\n"               // rseq_cs::start_ip
               "	.quad	302f-301f\n"          // rseq_cs::post_commit_offset
               "	.quad	303f\n"               // rseq_cs::abort_ip
               "	.popsection\n"                //
               "301:	movq	$300b,8(%1)\n"        // rseq->rseq_cs
               "	mov	(%1),%%ecx\n"         // rseq->cpu_id_start
               "	shl	$6,%%ecx\n"           //
               "	mov	(%2,%%rcx),%0\n"      // chunk = chunks[cpu].freelist
               "	test	%0,%0\n"              //
               "	jz	302f\n"               //
               "	mov	(%0),%%rdx\n"         // rdx = chunk->next
               "	mov	%%rdx,(%2,%%rcx)\n"   // freelist = rdx
               "302:	.pushsection .text.unlikely,\"ax\",@progbits\n"
               "	.byte	0x0f,0xb9,0x4d\n"
               "	.long	0x53053053\n"
               "303:	jmp	301b\n"               // restart on abort
               "	.popsection"
               : "=&r"(chunk)
               : "r"(__get_rseq()), "r"(&chunks[0].freelist[flindex])
               : "rcx", "rdx", "memory");
#elifdef __aarch64__
  asm volatile(".pushsection .rodata.rseq,\"a\",@progbits\n"
               "	.balign	32\n"
               "300:	.long	0\n"                  // rseq_cs::version
               "	.long	0\n"                  // rseq_cs::flags
               "	.quad	301f\n"               // rseq_cs::start_ip
               "	.quad	302f-301f\n"          // rseq_cs::post_commit_offset
               "	.quad	303f\n"               // rseq_cs::abort_ip
               "	.popsection\n"                //
               "301:	adrp	x4, 300b\n"           // load address of rseq_cs
               "	add	x4, x4, :lo12:300b\n" //
               "	str	x4, [%1, #8]\n"       // rseq->rseq_cs
               "	ldr	w4, [%1]\n"           // rseq->cpu_id_start
               "	lsl	w4, w4, #6\n"         //
               "	ldr	%0, [%2, x4]\n"       // chunk = chunks[cpu].freelist
               "	cbz	%0, 302f\n"           //
               "	ldr	x5, [%0]\n"           // x5 = chunk->next
               "	str	x5, [%2, x4]\n"       // freelist = x5
               "302:	.pushsection .text.unlikely,\"ax\",@progbits\n"
               "	.long	0xd428bc00\n"
               "303:	adrp	x5, 301b\n"           // load page address of 301b
               "	add	x5, x5, :lo12:301b\n" // add low 12 bits
               "	br	x5\n"                 // branch to 301b via register
               "	.popsection"
               : "=&r"(chunk)
               : "r"(__get_rseq()), "r"(&chunks[0].freelist[flindex])
               : "x4", "x5", "memory");
#endif
  return chunk;
}

static bool is_slab_used(struct Slab *slab) {
  char *p = slab->memory;
  char *e = slab->memory + SLAB_SIZE - sizeof(struct Slab);
  while (e - p > sizeof(struct Chunk)) {
    struct Chunk *chunk = (struct Chunk *)p;
    size_t bytes = sizeof(struct Chunk) + kSizes[chunk->flindex];
    if (e - p < bytes)
      break;
    if (chunk->next == ALLOCATED)
      return true;
    p += bytes;
  }
  return false;
}

static int trim_slabs(void) {

  // pluck unused slabs
  struct Slab *freed = 0;
  struct Slab **prev = (struct Slab **)&rseq.slabs;
  for (struct Slab *slab = *prev; slab; slab = *prev) {
    if (is_slab_used(slab)) {
      prev = &slab->next;
    } else {
      *prev = slab->next;
      slab->next = freed;
      freed = slab;
    }
  }
  if (!freed)
    return 0;

  // make freed slabs dangerous
  for (struct Slab *slab = freed; slab; slab = slab->next)
    __maps_unmark(slab, SLAB_SIZE);

  // remove chunks that are dangerous
  for (int i = 0; i < CPU_SETSIZE; ++i) {
    for (int j = 0; j < 8; ++j) {
      struct Chunk **prev = &chunks[i].freelist[j];
      for (struct Chunk *chunk = *prev; chunk; chunk = *prev) {
        if (kisdangerous(chunk)) {
          *prev = chunk->next;
        } else {
          prev = &chunk->next;
        }
      }
    }
  }

  // release slabs to operating system
  size_t count = 0;
  struct Slab *next;
  for (struct Slab *slab = freed; slab; slab = next) {
    next = slab->next;
    __sys_munmap(slab, SLAB_SIZE);
    ++count;
  }

  // adjust accounting to recover from oom
  if (ckd_add(&count, count,
              atomic_load_explicit(&rseq.max, memory_order_relaxed)))
    count = MAX_SIZE_T;
  atomic_store_explicit(&rseq.max, count, memory_order_relaxed);

  return 1;
}

static void *dlmalloc_rseq_populate(size_t n) {

  // makes it possible to limit the growth of untrimmable rseq memory
  //
  // this may be tuned using `export COSMOPOLITAN_M_RSEQ_MAX=x` since
  // that environment variable is read once very early in the process
  // initialization, it's also possible to tune this value at runtime
  // instead, using `mallopt(M_RSEQ_MAX, x)`. the value of `x` is the
  // number of slabs which are allowed to be made, or in other words,
  // the number of pages of memory rseq can use. if the value is zero
  // then rseq is disabled, and a negative value means it's unlimited
  //
  // when a single-threaded program transitions to the OOM state this
  // code performs a full trim of rseq memory, which is a Θ(n) chore.
  // if either threads existed or the trim failed to reclaim at least
  // one slab, then rseq goes into the oom state permanently and will
  // stop obtaining OS memory, and delegate to the threaded allocator
  size_t max = atomic_load_explicit(&rseq.max, memory_order_relaxed);
  for (;;) {
    if (!max)
      return dlmalloc_threaded(n);
    if (atomic_compare_exchange_weak_explicit(&rseq.max, &max, max - 1,
                                              memory_order_relaxed,
                                              memory_order_relaxed)) {
      if (max == 1)
        if (pthread_orphan_np())
          trim_slabs();
      break;
    }
  }

  // get memory from system
  // take the fast path since we're linux only
  int oe = errno;
  struct Slab *slab;
  slab = __sys_mmap(0, SLAB_SIZE, PROT_READ | PROT_WRITE,
                    MAP_PRIVATE | MAP_ANONYMOUS, -1, 0, 0);
  if (slab == MAP_FAILED) {
    errno = oe;
    return dlmalloc_threaded(n);
  }
  __maps_mark(slab, SLAB_SIZE);

  // divide slab into chunks added to free lists
  char *p = slab->memory;
  char *e = slab->memory + SLAB_SIZE - sizeof(struct Slab);
  signed char flindex = kPickSize[n];
  while (flindex >= 0) {
    size_t size = kSizes[flindex];
    size_t bytes = sizeof(struct Chunk) + size;
    if (bytes > e - p) {
      --flindex;
      continue;
    }
    struct Chunk *chunk = (struct Chunk *)p;
    chunk->flindex = flindex;
    push_chunk(chunk, flindex);
    p += bytes;
  }

  // track new slab
  slab->next = atomic_load_explicit(&rseq.slabs, memory_order_relaxed);
  for (;;)
    if (atomic_compare_exchange_weak_explicit(&rseq.slabs, &slab->next, slab,
                                              memory_order_release,
                                              memory_order_relaxed))
      break;

  return TRY_AGAIN;
}

static inline void *dlmalloc_rseq_actual(size_t n) {
  for (;;) {
    struct Chunk *chunk;
    if (LIKELY((chunk = pop_chunk(kPickSize[n])))) {
      chunk->next = ALLOCATED;
      return chunk->memory;
    }
    void *res = dlmalloc_rseq_populate(n);
    if (res != TRY_AGAIN)
      return res;
  }
}

static void *dlmalloc_rseq(size_t n) {
  if (LIKELY(n <= CHUNK_MAX))
    return dlmalloc_rseq_actual(n);
  return dlmalloc_threaded(n);
}

static inline struct Chunk *get_chunk(char *ptr) {
  return (struct Chunk *)(ptr - sizeof(struct Chunk));
}

static void dlfree_rseq(void *ptr) {
  if (LIKELY(ptr)) {
    struct Chunk *chunk = get_chunk(ptr);
    if (LIKELY(chunk->next == ALLOCATED)) {
      push_chunk(chunk, chunk->flindex);
    } else {
      dlfree_threaded(ptr);
    }
  }
}

static size_t dlmalloc_usable_size_rseq(void* ptr) {
  if (UNLIKELY(!ptr))
    return 0;
  struct Chunk *chunk = get_chunk(ptr);
  if (LIKELY(chunk->next == ALLOCATED))
    return kSizes[chunk->flindex];
  return dlmalloc_usable_size_threaded(ptr);
}

static void *dlcalloc_rseq(size_t n_elements, size_t elem_size) {
  void *p;
  size_t n;
  if (UNLIKELY(ckd_mul(&n, n_elements, elem_size)))
    n = MAX_SIZE_T;
  if (UNLIKELY(n > CHUNK_MAX))
    return dlcalloc_threaded(n_elements, elem_size);
  if (UNLIKELY(!(p = dlmalloc_rseq_actual(n))))
    return 0;
  if (LIKELY(n))
    memset(p, 0, n);
  return p;
}

static void *dlmemalign_rseq(size_t a, size_t n) {
  if (a <= MALLOC_ALIGNMENT)
    return dlmalloc(n);
  return dlmemalign_threaded(a, n);
}

static void *dlrealloc_rseq(void *ptr, size_t n) {
  if (UNLIKELY(!ptr))
    return dlmalloc(n);
  struct Chunk *chunk = get_chunk(ptr);
  if (chunk->next != ALLOCATED)
    return dlrealloc_threaded(ptr, n);
  if (n <= kSizes[chunk->flindex])
    return ptr;
  void *ptr2;
  if (UNLIKELY(!(ptr2 = dlmalloc(n))))
    return 0;
  memcpy(ptr2, ptr, kSizes[chunk->flindex]);
  push_chunk(chunk, chunk->flindex);
  return ptr2;
}

static void *dlrealloc_in_place_rseq(void *ptr, size_t n) {
  if (!ptr)
    return 0;
  struct Chunk *chunk = get_chunk(ptr);
  if (chunk->next != ALLOCATED)
    return dlrealloc_in_place_threaded(ptr, n);
  if (n > kSizes[chunk->flindex])
    return 0;
  return ptr;
}

// xxx: dlmalloc_inspect_all() normally acquires the mspace mutex which
//      prevents an allocation from being freed while it's being passed
//      to the handler(). we don't bother doing that here. this isn't a
//      public api. it's mostly used to implement cosmopolitan's memory
//      leak detector. since slabs is an append-only data structure, we
//      can guarantee this is thread safe, for that particular use case
static void dlmalloc_inspect_all_rseq(void handler(void *start, void *end,
                                                   size_t used_bytes, void *arg),
                                      void *arg) {
  for (struct Slab *slab = atomic_load(&rseq.slabs); slab; slab = slab->next) {
    char *p = slab->memory;
    char *e = slab->memory + SLAB_SIZE - sizeof(struct Slab);
    while (e - p > sizeof(struct Chunk)) {
      struct Chunk *chunk = (struct Chunk *)p;
      size_t size = kSizes[chunk->flindex];
      size_t bytes = sizeof(struct Chunk) + size;
      if (e - p < bytes)
        break;
      size_t used_bytes = chunk->next == ALLOCATED ? size : 0;
      handler(chunk->memory, chunk->memory + size, used_bytes, arg);
      p += bytes;
    }
  }
  dlmalloc_inspect_all_threaded(handler, arg);
}

static struct mallinfo dlmallinfo_rseq(void) {
  size_t arena = 0;
  size_t smblks = 0;
  size_t fsmblks = 0;
  struct mallinfo mi = dlmallinfo_threaded();
  for (struct Slab *slab = atomic_load(&rseq.slabs); slab; slab = slab->next) {
    arena += SLAB_SIZE;
    char *p = slab->memory;
    char *e = slab->memory + SLAB_SIZE - sizeof(struct Slab);
    while (e - p > sizeof(struct Chunk)) {
      struct Chunk *chunk = (struct Chunk *)p;
      size_t bytes = sizeof(struct Chunk) + kSizes[chunk->flindex];
      if (e - p < bytes)
        break;
      if (chunk->next != ALLOCATED) {
        fsmblks += bytes;
        ++smblks;
      }
      p += bytes;
    }
  }
  mi.arena += arena;
  mi.smblks += smblks;
  mi.fsmblks += fsmblks;
  mi.uordblks += arena;
  mi.usmblks += arena;
  mi.fordblks += fsmblks;
  return mi;
}

// rseq trimming is supported when a single posix thread exists
static int dlmalloc_trim_rseq(size_t pad) {
  int actually_released_memory = 0;
  if (pthread_orphan_np())
    actually_released_memory |= trim_slabs();
  actually_released_memory |= dlmalloc_trim_threaded(pad);
  return actually_released_memory;
}

dontinline static void use_rseq_allocator(void) {
  dlfree = dlfree_rseq;
  dlrealloc = dlrealloc_rseq;
  dlrealloc_in_place = dlrealloc_in_place_rseq;
  dlmalloc_usable_size = dlmalloc_usable_size_rseq;
  dlmallinfo = dlmallinfo_rseq;
  dlmalloc_trim = dlmalloc_trim_rseq;
  dlmalloc_inspect_all = dlmalloc_inspect_all_rseq;
  atomic_thread_fence(memory_order_release);
  dlmalloc = dlmalloc_rseq;
  dlcalloc = dlcalloc_rseq;
  dlmemalign = dlmemalign_rseq;
}

static bool never_used_rseq(void) {
  return !atomic_load(&rseq.slabs) && __isthreaded < 2;
}

#if 0
__attribute__((__destructor__)) static void print_statistics(void) {
  static size_t histo[CPU_SETSIZE];
  for (int j = 0; j < 8; ++j) {
    bzero(histo, sizeof(histo));
    for (int i = 0; i < 192; ++i)
      for (struct Chunk *c = chunks[i].freelist[j]; c; c = c->next)
        ++histo[i];
    kprintf("%d:", kSizes[j]);
    for (int i = 0; i < 180; ++i)
      kprintf(" %zu", histo[i]);
    kprintf("\n");
  }
  size_t count = 0;
  for (struct Slab *s = rseq.slabs; s; s = s->next)
    ++count;
  kprintf("%'zu slabs (%'zu bytes)\n", count, count * SLAB_SIZE);
}
#endif
