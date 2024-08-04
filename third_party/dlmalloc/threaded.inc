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
#include "libc/dce.h"
#include "libc/intrin/magicu.h"
#include "libc/intrin/strace.h"
#include "libc/intrin/weaken.h"
#include "libc/macros.h"
#include "libc/nexgen32e/rdtscp.h"
#include "libc/nexgen32e/x86feature.h"
#include "libc/runtime/runtime.h"
#include "libc/thread/thread.h"
#include "libc/runtime/runtime.h"
#include "libc/intrin/weaken.h"
#include "third_party/dlmalloc/dlmalloc.h"

#if !FOOTERS || !MSPACES
#error "threaded dlmalloc needs footers and mspaces"
#endif

static struct magicu magiu;
static unsigned g_heapslen;
static mstate g_heaps[128];

void dlfree(void *p) {
  return mspace_free(0, p);
}

size_t dlmalloc_usable_size(void* mem) {
  return mspace_usable_size(mem);
}

void* dlrealloc_in_place(void *p, size_t n) {
  return mspace_realloc_in_place(0, p, n);
}

int dlmallopt(int param_number, int value) {
  return mspace_mallopt(param_number, value);
}

int dlmalloc_trim(size_t pad) {
  int got_some = 0;
  for (unsigned i = 0; i < g_heapslen; ++i)
    got_some |= mspace_trim(g_heaps[i], pad);
  return got_some;
}

size_t dlbulk_free(void *array[], size_t nelem) {
  for (size_t i = 0; i < nelem; ++i)
    mspace_free(0, array[i]);
  return 0;
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

void dlmalloc_inspect_all(void handler(void *start, void *end,
                                       size_t used_bytes, void *arg),
                          void *arg) {
  for (unsigned i = 0; i < g_heapslen; ++i) {
    struct ThreadedMallocVisitor tmv = {g_heaps[i], handler, arg};
    mspace_inspect_all(g_heaps[i], threaded_malloc_visitor, &tmv);
  }
}

forceinline mstate get_arena(void) {
  unsigned cpu;
#ifdef __x86_64__
  unsigned tsc_aux;
  rdtscp(&tsc_aux);
  cpu = TSC_AUX_CORE(tsc_aux);
#else
  long tpidr_el0;
  asm("mrs\t%0,tpidr_el0" : "=r"(tpidr_el0));
  cpu = tpidr_el0 & 255;
#endif
  return g_heaps[__magicu_div(cpu, magiu) % g_heapslen];
}

static void *dlmalloc_single(size_t n) {
  return mspace_malloc(g_heaps[0], n);
}

static void *dlmalloc_threaded(size_t n) {
  return mspace_malloc(get_arena(), n);
}

static void *dlcalloc_single(size_t n, size_t z) {
  return mspace_calloc(g_heaps[0], n, z);
}

static void *dlcalloc_threaded(size_t n, size_t z) {
  return mspace_calloc(get_arena(), n, z);
}

static void *dlrealloc_single(void *p, size_t n) {
  return mspace_realloc(g_heaps[0], p, n);
}

static void *dlrealloc_threaded(void *p, size_t n) {
  if (p)
    return mspace_realloc(0, p, n);
  else
    return mspace_malloc(get_arena(), n);
}

static void *dlmemalign_single(size_t a, size_t n) {
  return mspace_memalign(g_heaps[0], a, n);
}

static void *dlmemalign_threaded(size_t a, size_t n) {
  return mspace_memalign(get_arena(), a, n);
}

static struct mallinfo dlmallinfo_single(void) {
  return mspace_mallinfo(g_heaps[0]);
}

static struct mallinfo dlmallinfo_threaded(void) {
  return mspace_mallinfo(get_arena());
}

static int dlmalloc_atoi(const char *s) {
  int c, x = 0;
  while ((c = *s++)) {
    x *= 10;
    x += c - '0';
  }
  return x;
}

static void use_single_heap(bool uses_locks) {
  g_heapslen = 1;
  dlmalloc = dlmalloc_single;
  dlcalloc = dlcalloc_single;
  dlrealloc = dlrealloc_single;
  dlmemalign = dlmemalign_single;
  dlmallinfo = dlmallinfo_single;
  if (!(g_heaps[0] = create_mspace(0, uses_locks)))
    __builtin_trap();
}

static void threaded_dlmalloc(void) {
  int heaps, cpus;
  const char *var;

  if (!_weaken(pthread_create))
    return use_single_heap(false);

  if (!IsAarch64() && !X86_HAVE(RDTSCP))
    return use_single_heap(true);

  // determine how many independent heaps we should install
  // by default we do an approximation of one heap per core
  // this code makes the c++ stl go 164x faster on my ryzen
  cpus = __get_cpu_count();
  if (cpus == -1)
    heaps = 1;
  else if ((var = getenv("COSMOPOLITAN_HEAP_COUNT")))
    heaps = dlmalloc_atoi(var);
  else
    heaps = cpus >> 1;
  if (heaps <= 1)
    return use_single_heap(true);
  if (heaps > ARRAYLEN(g_heaps))
    heaps = ARRAYLEN(g_heaps);

  // find 𝑑 such that sched_getcpu() / 𝑑 is within [0,heaps)
  // turn 𝑑 into a fast magic that can divide by multiplying
  magiu = __magicu_get(cpus / heaps);

  // we need this too due to linux's cpu count affinity hack
  g_heapslen = heaps;

  // create the arenas
  for (size_t i = 0; i < g_heapslen; ++i)
    if (!(g_heaps[i] = create_mspace(0, true)))
      __builtin_trap();

  // install function pointers
  dlmalloc = dlmalloc_threaded;
  dlcalloc = dlcalloc_threaded;
  dlrealloc = dlrealloc_threaded;
  dlmemalign = dlmemalign_threaded;
  dlmallinfo = dlmallinfo_threaded;

  STRACE("created %d dlmalloc arenas for %d cpus", heaps, cpus);
}
