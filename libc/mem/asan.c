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
#include "libc/intrin/asan.internal.h"
#include "libc/intrin/atomic.h"
#include "libc/intrin/kprintf.h"
#include "libc/intrin/leaky.internal.h"
#include "libc/intrin/likely.h"
#include "libc/intrin/weaken.h"
#include "libc/macros.internal.h"
#include "libc/mem/mem.h"
#include "libc/runtime/symbols.internal.h"
#include "libc/stdckdint.h"
#include "libc/sysv/errfuns.h"
#include "libc/thread/thread.h"
#include "third_party/dlmalloc/dlmalloc.h"
#ifdef __SANITIZE_ADDRESS__

#define RBP __builtin_frame_address(0)

struct AsanExtra {
  uint64_t size;
  struct AsanTrace bt;
};

struct ReportOriginHeap {
  const unsigned char *a;
  int z;
};

static struct AsanMorgue {
  _Atomic(unsigned) i;
  _Atomic(void *) p[ASAN_MORGUE_ITEMS];
} __asan_morgue;

static pthread_spinlock_t __asan_lock;

static int __asan_bsr(uint64_t x) {
  _Static_assert(sizeof(long long) == sizeof(uint64_t), "");
  return __builtin_clzll(x) ^ 63;
}

static uint64_t __asan_roundup2pow(uint64_t x) {
  return 2ull << __asan_bsr(x - 1);
}

static void __asan_write48(uint64_t *value, uint64_t x) {
  uint64_t cookie;
  cookie = 'J' | 'T' << 8;
  cookie ^= x & 0xffff;
  *value = (x & 0xffffffffffff) | cookie << 48;
}

static bool __asan_read48(uint64_t value, uint64_t *x) {
  uint64_t cookie;
  cookie = value >> 48;
  cookie ^= value & 0xffff;
  *x = (int64_t)(value << 16) >> 16;
  return cookie == ('J' | 'T' << 8);
}

static void *__asan_morgue_add(void *p) {
  return atomic_exchange_explicit(
      __asan_morgue.p + (atomic_fetch_add_explicit(&__asan_morgue.i, 1,
                                                   memory_order_acq_rel) &
                         (ARRAYLEN(__asan_morgue.p) - 1)),
      p, memory_order_acq_rel);
}

__attribute__((__destructor__)) static void __asan_morgue_flush(void) {
  unsigned i;
  for (i = 0; i < ARRAYLEN(__asan_morgue.p); ++i)
    if (atomic_load_explicit(__asan_morgue.p + i, memory_order_acquire))
      dlfree(atomic_exchange_explicit(__asan_morgue.p + i, 0,
                                      memory_order_release));
}

static size_t __asan_heap_size(size_t n) {
  if (n < 0x7fffffff0000) {
    n = ROUNDUP(n, _Alignof(struct AsanExtra));
    return __asan_roundup2pow(n + sizeof(struct AsanExtra));
  } else {
    return -1;
  }
}

static void *__asan_allocate(size_t a, size_t n, struct AsanTrace *bt,
                             int underrun, int overrun, int initializer) {
  char *p;
  size_t c;
  struct AsanExtra *e;
  if ((p = dlmemalign(a, __asan_heap_size(n)))) {
    c = dlmalloc_usable_size(p);
    e = (struct AsanExtra *)(p + c - sizeof(*e));
    __asan_unpoison(p, n);
    __asan_poison(p - 16, 16, underrun); /* see dlmalloc design */
    __asan_poison(p + n, c - n, overrun);
    __asan_memset(p, initializer, n);
    __asan_write48(&e->size, n);
    __asan_memcpy(&e->bt, bt, sizeof(*bt));
  }
  return p;
}

static void *__asan_allocate_heap(size_t a, size_t n, struct AsanTrace *bt) {
  return __asan_allocate(a, n, bt, kAsanHeapUnderrun, kAsanHeapOverrun, 0xf9);
}

static struct AsanExtra *__asan_get_extra(const void *p, size_t *c) {
  int f;
  long x, n;
  struct AsanExtra *e;
  f = (intptr_t)p >> 16;
  if (!kisdangerous(p) && (n = dlmalloc_usable_size((void *)p)) > sizeof(*e) &&
      !ckd_add(&x, (intptr_t)p, n) && x <= 0x800000000000 &&
      (LIKELY(f == (int)((x - 1) >> 16)) || !kisdangerous((void *)(x - 1))) &&
      (LIKELY(f == (int)((x = x - sizeof(*e)) >> 16)) ||
       __asan_is_mapped(x >> 16)) &&
      !(x & (_Alignof(struct AsanExtra) - 1))) {
    *c = n;
    return (struct AsanExtra *)x;
  } else {
    return 0;
  }
}

// Returns true if `p` was allocated by an IGNORE_LEAKS(function).
int __asan_is_leaky(void *p) {
  int sym;
  size_t c, i, n;
  intptr_t f, *l;
  struct AsanExtra *e;
  struct SymbolTable *st;
  if (!_weaken(GetSymbolTable))
    notpossible;
  if (!(e = __asan_get_extra(p, &c)))
    return 0;
  if (!__asan_read48(e->size, &n))
    return 0;
  if (!__asan_is_mapped((((intptr_t)p >> 3) + 0x7fff8000) >> 16))
    return 0;
  if (!(st = GetSymbolTable()))
    return 0;
  for (i = 0; i < ARRAYLEN(e->bt.p) && e->bt.p[i]; ++i) {
    if ((sym = _weaken(__get_symbol)(st, e->bt.p[i])) == -1)
      continue;
    f = st->addr_base + st->symbols[sym].x;
    for (l = _leaky_start; l < _leaky_end; ++l)
      if (f == *l)
        return 1;
  }
  return 0;
}

#define __asan_trace __asan_rawtrace

int __asan_print_trace(void *p) {
  size_t c, i, n;
  struct AsanExtra *e;
  if (!(e = __asan_get_extra(p, &c))) {
    kprintf(" bad pointer");
    return einval();
  }
  if (!__asan_read48(e->size, &n)) {
    kprintf(" bad cookie");
    return -1;
  }
  kprintf("\n%p %,lu bytes [asan]", (char *)p, n);
  if (!__asan_is_mapped((((intptr_t)p >> 3) + 0x7fff8000) >> 16))
    kprintf(" (shadow not mapped?!)");
  for (i = 0; i < ARRAYLEN(e->bt.p) && e->bt.p[i]; ++i)
    kprintf("\n%*lx %t", 12, e->bt.p[i], e->bt.p[i]);
  return 0;
}

static void __asan_onmemory(void *x, void *y, size_t n, void *a) {
  const unsigned char *p = x;
  struct ReportOriginHeap *t = a;
  if ((p <= t->a && t->a < p + n) ||
      (p <= t->a + t->z && t->a + t->z < p + n) ||
      (t->a < p && p + n <= t->a + t->z)) {
    kprintf("%p %,lu bytes [dlmalloc]", x, n);
    __asan_print_trace(x);
    kprintf("\n");
  }
}

void __asan_report_memory_origin_heap(const unsigned char *a, int z) {
  struct ReportOriginHeap t;
  kprintf("\nthe memory was allocated by\n");
  t.a = a;
  t.z = z;
  dlmalloc_inspect_all(__asan_onmemory, &t);
}

size_t __asan_get_heap_size(const void *p) {
  size_t n, c;
  struct AsanExtra *e;
  if ((e = __asan_get_extra(p, &c)) && __asan_read48(e->size, &n))
    return n;
  return 0;
}

static __wur __asan_die_f *__asan_report_invalid_pointer(const void *addr) {
  pthread_spin_lock(&__asan_lock);
  kprintf("\n\e[J\e[1;31masan error\e[0m: this corruption at %p shadow %p\n",
          addr, SHADOW(addr));
  return __asan_die();
}

size_t malloc_usable_size(void *p) {
  size_t n, c;
  struct AsanExtra *e;
  if ((e = __asan_get_extra(p, &c)) && __asan_read48(e->size, &n))
    return n;
  __asan_report_invalid_pointer(p)();
  __asan_unreachable();
}

static void __asan_deallocate(char *p, long kind) {
  size_t c, n;
  struct AsanExtra *e;
  if ((e = __asan_get_extra(p, &c))) {
    if (__asan_read48(e->size, &n)) {
      __asan_poison(p, c, kind);
      if (c <= ASAN_MORGUE_THRESHOLD)
        p = __asan_morgue_add(p);
      dlfree(p);
    } else {
      __asan_report_invalid_pointer(p)();
      __asan_unreachable();
    }
  } else {
    __asan_report_invalid_pointer(p)();
    __asan_unreachable();
  }
}

static void *__asan_realloc_nogrow(void *p, size_t n, size_t m,
                                   struct AsanTrace *bt) {
  return 0;
}

static void *__asan_realloc_grow(void *p, size_t n, size_t m,
                                 struct AsanTrace *bt) {
  char *q;
  if ((q = __asan_allocate_heap(16, n, bt))) {
    __asan_memcpy(q, p, m);
    __asan_deallocate(p, kAsanHeapRelocated);
  }
  return q;
}

static void *__asan_realloc_impl(void *p, size_t n,
                                 void *grow(void *, size_t, size_t,
                                            struct AsanTrace *)) {
  size_t c, m;
  struct AsanExtra *e;
  if ((e = __asan_get_extra(p, &c))) {
    if (__asan_read48(e->size, &m)) {
      if (n <= m) {  // shrink
        __asan_poison((char *)p + n, m - n, kAsanHeapOverrun);
        __asan_write48(&e->size, n);
        return p;
      } else if (n <= c - sizeof(struct AsanExtra)) {  // small growth
        __asan_unpoison((char *)p + m, n - m);
        __asan_write48(&e->size, n);
        return p;
      } else {  // exponential growth
        return grow(p, n, m, &e->bt);
      }
    }
  }
  __asan_report_invalid_pointer(p)();
  __asan_unreachable();
}

void free(void *p) {
  if (!p)
    return;
  __asan_deallocate(p, kAsanHeapFree);
}

void *malloc(size_t size) {
  struct AsanTrace bt;
  __asan_trace(&bt, RBP);
  return __asan_allocate_heap(16, size, &bt);
}

void *memalign(size_t align, size_t size) {
  struct AsanTrace bt;
  __asan_trace(&bt, RBP);
  return __asan_allocate_heap(align, size, &bt);
}

void *calloc(size_t n, size_t m) {
  struct AsanTrace bt;
  __asan_trace(&bt, RBP);
  if (ckd_mul(&n, n, m))
    n = -1;
  return __asan_allocate(16, n, &bt, kAsanHeapUnderrun, kAsanHeapOverrun, 0x00);
}

void *realloc(void *p, size_t n) {
  struct AsanTrace bt;
  if (p) {
    return __asan_realloc_impl(p, n, __asan_realloc_grow);
  } else {
    __asan_trace(&bt, RBP);
    void *res = __asan_allocate_heap(16, n, &bt);
    return res;
  }
}

void *realloc_in_place(void *p, size_t n) {
  return p ? __asan_realloc_impl(p, n, __asan_realloc_nogrow) : 0;
}

int malloc_trim(size_t pad) {
  __asan_morgue_flush();
  return dlmalloc_trim(pad);
}

void *__asan_stack_malloc(size_t size, int classid) {
  struct AsanTrace bt;
  __asan_trace(&bt, RBP);
  return __asan_allocate(16, size, &bt, kAsanStackUnderrun, kAsanStackOverrun,
                         0xf9);
}

void __asan_stack_free(char *p, size_t size, int classid) {
  __asan_deallocate(p, kAsanStackFree);
}

size_t bulk_free(void *p[], size_t n) {
  size_t i;
  for (i = 0; i < n; ++i)
    if (p[i]) {
      __asan_deallocate(p[i], kAsanHeapFree);
      p[i] = 0;
    }
  return 0;
}

#endif /* __SANITIZE_ADDRESS__ */
