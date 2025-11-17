/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2024 Justine Alexandra Roberts Tunney                              │
│                                                                              │
│ Permission to use, copy, modify, and/or distribute this software for         │
│ any purpose with or without fee is hereby granted, provided that the         │
│ above copyright notice and this permission notice appear in all copies.      │
│                                                                              │
│ THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL                │
│ WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED                │
│ WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE             │
│ AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL         │
│ DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR        │
│ PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER               │
│ TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR             │
│ PERFORMANCE OF THIS SOFTWARE.                                                │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/intrin/maps.h"
#include "ape/sections.internal.h"
#include "libc/assert.h"
#include "libc/calls/calls.h"
#include "libc/calls/syscall-sysv.internal.h"
#include "libc/calls/syscall_support-nt.internal.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/intrin/describebacktrace.h"
#include "libc/intrin/kprintf.h"
#include "libc/intrin/maps.h"
#include "libc/intrin/tree.h"
#include "libc/inttypes.h"
#include "libc/macros.h"
#include "libc/nt/enum/memflags.h"
#include "libc/nt/enum/pageflags.h"
#include "libc/nt/memory.h"
#include "libc/nt/runtime.h"
#include "libc/runtime/runtime.h"
#include "libc/runtime/stack.h"
#include "libc/runtime/syslib.internal.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/prot.h"
#include "libc/thread/posixthread.internal.h"
#include "libc/thread/thread.h"
#include "libc/thread/tls.h"

#define MASQUE    0x00fffffffffffff8
#define PTR(x)    ((uintptr_t)(x) & MASQUE)
#define TAG(x)    ROL((uintptr_t)(x) & ~MASQUE, 8)
#define ABA(p, t) ((uintptr_t)(p) | (ROR((uintptr_t)(t), 8) & ~MASQUE))
#define ROL(x, n) (((x) << (n)) | ((x) >> (64 - (n))))
#define ROR(x, n) (((x) >> (n)) | ((x) << (64 - (n))))

#ifdef __x86_64__
__static_yoink("_init_maps");
#endif

struct Maps __maps;

alignas(64) static pthread_mutex_t __maps_lock_obj =
#if MMAP_DEBUG
    PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP;
#else
    PTHREAD_MUTEX_INITIALIZER;
#endif

textstartup uint64_t __maps_hash(uint64_t k) {
  // MurmurHash3 was written by Austin Appleby, and is placed in the
  // public domain. See fmix64() in MurmurHash3.cpp
  k ^= k >> 33;
  k *= 0xff51afd7ed558ccd;
  k ^= k >> 33;
  k *= 0xc4ceb9fe1a85ec53;
  k ^= k >> 33;
  return k;
}

textstartup void __maps_add(struct Map *map) {
  tree_insert(&__maps.maps, &map->tree, __maps_compare);
  ++__maps.count;
}

int __maps_compare(const struct Tree *ra, const struct Tree *rb) {
  const struct Map *a = (const struct Map *)MAP_TREE_CONTAINER(ra);
  const struct Map *b = (const struct Map *)MAP_TREE_CONTAINER(rb);
  return (a->addr > b->addr) - (a->addr < b->addr);
}

__privileged optimizespeed struct Map *__maps_floor(const char *addr) {
  struct Tree *node;
  if ((node = tree_floor(__maps.maps, addr, __maps_search)))
    return MAP_TREE_CONTAINER(node);
  return 0;
}

textstartup static void __maps_adder(struct Map *map, int pagesz) {
  __maps.pages += ((map->size + pagesz - 1) & -pagesz) / pagesz;
  __maps_add(map);
  if (map->prot & (PROT_READ | PROT_WRITE)) {
    __maps_mark(map->addr, map->size);
  } else {
    __maps_unmark(map->addr, map->size);
  }
}

textstartup void __maps_stack(char *stackaddr, int pagesz, int guardsize,
                              size_t stacksize, int stackprot) {
  __maps.stack.addr = stackaddr + guardsize;
  __maps.stack.size = stacksize - guardsize;
  __maps.stack.prot = stackprot;
  __maps.stack.hand = MAPS_SUBREGION;
  __maps.stack.flags = MAP_PRIVATE | MAP_ANONYMOUS;
  __maps_adder(&__maps.stack, pagesz);
  if (guardsize) {
    __maps.guard.addr = stackaddr;
    __maps.guard.size = guardsize;
    __maps.guard.prot = PROT_NONE | PROT_GUARD;
    __maps.guard.hand = MAPS_VIRTUAL;
    __maps.guard.flags = MAP_PRIVATE | MAP_ANONYMOUS;
    __maps_adder(&__maps.guard, pagesz);
  } else {
    __maps.stack.hand = MAPS_VIRTUAL;
  }
}

uint64_t __maps_rand(void) {
  MAPS_ASSERT(!__tls_enabled || __maps_held());
  return (__maps.rand *= 15750249268501108917ull) >> 64;
}

void *__maps_randaddr(void) {
  __maps_lock();
  uintptr_t addr;
  if (IsXnuSilicon()) {
    addr = __maps_rand();
    addr &= 0x3fffffffffff;
    addr |= 0x200000000000;
  } else {
    addr = __maps_rand();  // 137gb total for random addresses
    addr &= 0x3fffffffff;  // 38 bit is below 39 bit rpi4 os vaspace
    addr |= 0x2000000000;  // 100gb above aarch64 base
  }
  addr &= -__gransize;
  __maps_unlock();
  return (void *)addr;
}

static void *__maps_valloc(void *addr, size_t size) {
  void *res;
  // MacOS and Windows have bottom-up allocators, so they're unlikely
  // to interfere with our top down allocation style.
  if (IsXnuSilicon()) {
    res = (void *)_sysret(__syslib->__mmap(0, size, PROT_READ | PROT_WRITE,
                                           MAP_PRIVATE | MAP_ANONYMOUS, -1, 0));
  } else if (IsWindows()) {
    res = VirtualAlloc(0, size, kNtMemReserve | kNtMemCommit, kNtPageReadwrite);
  } else {
    res = __sys_mmap(addr, size, PROT_READ | PROT_WRITE,
                     MAP_PRIVATE | MAP_ANONYMOUS, -1, 0, 0);
  }
  if (res == MAP_FAILED)
    res = 0;
  return res;
}

static struct MapSlab *__maps_newslab(void) {
  // we're creating sudden surprise memory. the user might be in the
  // middle of carefully planning a fixed memory structure. we don't
  // want the system allocator to put our surprise memory inside it,
  // and we also want to avoid the chances of accidentally unmapping
  for (;;) {
    void *addr;
    addr = atomic_fetch_sub(&__maps.bolloc, MAPS_SLAB_SIZE) - MAPS_SLAB_SIZE;
    if (!(addr = __maps_valloc(addr, MAPS_SLAB_SIZE)))
      return 0;
    struct MapSlab *slab = addr;
    while (!atomic_compare_exchange_weak(&__maps.slabs, &slab->next, slab)) {
    }
    // here's where bootstrapping becomes interesting
    __maps_mark(addr, MAPS_SLAB_SIZE);
    return slab;
  }
}

void *__maps_balloc(size_t need) {
  struct MapSlab *slab = atomic_load(&__maps.slabs);
  if (!slab)
    if (!(slab = __maps_newslab()))
      return 0;
  for (;;) {
    size_t used = atomic_load(&slab->used);
    if (used + need > MAPS_SLAB_SIZE - sizeof(struct MapSlab)) {
      if (!(slab = __maps_newslab()))
        return 0;
    } else if (atomic_compare_exchange_weak(&slab->used, &used, used + need)) {
      return (char *)slab + sizeof(struct MapSlab) + used;
    }
  }
}

static struct MapPageDirectory *__maps_getpd(
    _Atomic(struct MapPageDirectory *) *pdp) {
  struct MapPageDirectory *pd = atomic_load_explicit(pdp, memory_order_relaxed);
  while (!pd) {
    struct MapPageDirectory *pd2;
    if (!(pd2 = __maps_balloc(sizeof(struct MapPageDirectory))))
      return 0;
    if (atomic_compare_exchange_strong_explicit(
            pdp, &pd, pd2, memory_order_relaxed, memory_order_relaxed))
      pd = pd2;
  }
  return pd;
}

static struct MapPageTable *__maps_getpt(_Atomic(struct MapPageTable *) *ptp) {
  struct MapPageTable *pt = atomic_load_explicit(ptp, memory_order_relaxed);
  if (!pt) {
    struct MapPageTable *pt2;
    if (!(pt2 = __maps_balloc(sizeof(struct MapPageTable))))
      return 0;
    if (atomic_compare_exchange_strong_explicit(
            ptp, &pt, pt2, memory_order_relaxed, memory_order_relaxed))
      pt = pt2;
  }
  return pt;
}

// adds interval to rbtree
bool __maps_track(char *addr, size_t size, int prot, int flags) {
  struct Map *map;
  if (!(map = __maps_alloc()))
    return false;
  map->addr = addr;
  map->size = size;
  map->prot = prot;
  map->flags = flags;
  map->hand = MAPS_VIRTUAL;
  __maps_insert(map);
  __maps_mark(addr, size);
  return true;
}

struct Map *__maps_alloc(void) {
  struct Map *map, *tip;
  tip = atomic_load_explicit(&__maps.freed, memory_order_relaxed);
  while ((map = (struct Map *)PTR(tip)))
    if (atomic_compare_exchange_weak_explicit(
            &__maps.freed, &tip, ABA(map->freed, TAG(tip) + 1),
            memory_order_acquire, memory_order_relaxed))
      return map;
  if (!(map = __maps_balloc(sizeof(struct Map))))
    return 0;
  return map;
}

void __maps_free(struct Map *map) {
  struct Map *tip;
  MAPS_ASSERT(!TAG(map));
  map->size = 0;
  map->addr = MAP_FAILED;
  map->hand = -1;
  for (tip = atomic_load_explicit(&__maps.freed, memory_order_relaxed);;) {
    map->freed = (struct Map *)PTR(tip);
    if (atomic_compare_exchange_weak_explicit(
            &__maps.freed, &tip, ABA(map, TAG(tip) + 1), memory_order_release,
            memory_order_relaxed))
      break;
  }
}

void __maps_mark(void *addr, size_t size) {
  MAPS_ASSERT(!((uintptr_t)addr & (__pagesize - 1)));
  int olderr = errno;
  uintptr_t w = (uintptr_t)addr;
  uintptr_t e = (uintptr_t)addr + size;
  struct MapPageDirectory *pd;
  struct MapPageTable *pt;
  while (w < e) {
    if (!(pd = __maps_getpd(&__maps.alive)))
      goto OutOfMemory;
    size_t i = (w >> 48) & 511;
    if (!(pd = __maps_getpd(&pd->p[i].pd)))
      goto OutOfMemory;
    i = (w >> 39) & 511;
    if (!(pd = __maps_getpd(&pd->p[i].pd)))
      goto OutOfMemory;
    i = (w >> 30) & 511;
    if (!(pd = __maps_getpd(&pd->p[i].pd)))
      goto OutOfMemory;
    i = (w >> 21) & 511;
    if (!(pt = __maps_getpt(&pd->p[i].pt)))
      goto OutOfMemory;
    i = (w >> 12) & 511;
    uint_fast64_t mask = 1ull << (i & 63);
    w += __pagesize;
    while (w < e) {
      int i2 = (w >> 12) & 511;
      if (i2 / 64 != i / 64)
        break;
      i = i2;
      mask |= 1ull << (i & 63);
      w += __pagesize;
    }
    atomic_fetch_or_explicit(&pt->p[i / 64], mask, memory_order_release);
  }
  return;
OutOfMemory:
  errno = olderr;
}

void __maps_unmark(void *addr, size_t size) {
  MAPS_ASSERT(!((uintptr_t)addr & (__pagesize - 1)));
  uintptr_t w = (uintptr_t)addr;
  uintptr_t e = (uintptr_t)addr + size;
  struct MapPageDirectory *pd;
  struct MapPageTable *pt;
  while (w < e) {
    if (!(pd = atomic_load_explicit(&__maps.alive, memory_order_relaxed)))
      return;
    size_t i = (w >> 48) & 511;
    if (!(pd = atomic_load_explicit(&pd->p[i].pd, memory_order_relaxed))) {
      w += __pagesize;
      continue;
    }
    i = (w >> 39) & 511;
    if (!(pd = atomic_load_explicit(&pd->p[i].pd, memory_order_relaxed))) {
      w += __pagesize;
      continue;
    }
    i = (w >> 30) & 511;
    if (!(pd = atomic_load_explicit(&pd->p[i].pd, memory_order_relaxed))) {
      w += __pagesize;
      continue;
    }
    i = (w >> 21) & 511;
    if (!(pt = atomic_load_explicit(&pd->p[i].pt, memory_order_relaxed))) {
      w += __pagesize;
      continue;
    }
    i = (w >> 12) & 511;
    uint_fast64_t mask = 1ull << (i & 63);
    w += __pagesize;
    while (w < e) {
      int i2 = (w >> 12) & 511;
      if (i2 / 64 != i / 64)
        break;
      i = i2;
      mask |= 1ull << (i & 63);
      w += __pagesize;
    }
    atomic_fetch_and_explicit(&pt->p[i / 64], ~mask, memory_order_release);
  }
}

textstartup void __maps_init(void) {
  if (__maps.once)
    return;
  __maps.once = true;

  int pagesz = __pagesize;

  // initialize lemur64
  __maps.randlo = 2131259787901769494;
  __maps.randlo ^= __maps_hash((uintptr_t)__builtin_frame_address(0));
  __maps.randlo ^= __maps_hash(kStartTsc);

  // initialize internal memory allocator
  __maps.balloc = __maps_randaddr();
  atomic_init(&__maps.bolloc, __maps.balloc);
  atomic_init(&__maps.slabs, __maps.slab1);

  __maps_lock();

  // record _start() stack mapping
  if (!IsWindows()) {

    // linux v4.12+ reserves 1mb of guard space beneath rlimit_stack
    // https://lwn.net/Articles/725832/. if we guess too small, then
    // slackmap will create a bunch of zombie stacks in __print_maps
    // to coverup the undisclosed memory but no cost if we guess big
    size_t guardsize = 1024 * 1024;
    guardsize += __pagesize - 1;
    guardsize &= -__pagesize;

    // track the main stack region that the os gave to start earlier
    struct AddrSize stack = __get_main_stack();
    __maps_stack(stack.addr - guardsize, pagesz, guardsize,
                 guardsize + stack.size, (uintptr_t)ape_stack_prot);
  }

  // record .text and .data mappings
  if (&_rodata[0] > &__executable_start[0])
    __maps_track((char *)__executable_start, _rodata - __executable_start,
                 PROT_READ | PROT_EXEC, MAP_NOFORK);
  if (&_etext[0] > &_rodata[0])
    __maps_track((char *)_rodata, _etext - _rodata, PROT_READ, MAP_NOFORK);
  uintptr_t ds = ((uintptr_t)_etext + pagesz - 1) & -pagesz;
  if (ds < (uintptr_t)_end)
    __maps_track((char *)ds, (uintptr_t)_end - ds, PROT_READ | PROT_WRITE,
                 MAP_NOFORK);

  __maps_unlock();
}

void __maps_lock(void) {
  if (__tls_enabled) {
    MAPS_ASSERT(!__maps_held());
    pthread_mutex_lock(&__maps_lock_obj);
  }
}

void __maps_unlock(void) {
  if (__tls_enabled) {
    MAPS_ASSERT(__maps_held());
    pthread_mutex_unlock(&__maps_lock_obj);
    MAPS_ASSERT(!__maps_held());
  }
}

void __maps_wipe(void) {
  pthread_mutex_wipe_np(&__maps_lock_obj);
}

#if MMAP_DEBUG
void __maps_assert_fail(const char *expr, const char *file, int line) {
  kprintf("%!s:%d: assertion failed: %!s\n", file, line, expr);
  kprintf("bt %!s\n", DescribeBacktrace(__builtin_frame_address(0)));
  asm("int3");
  _Exit(79);
}
#endif
