/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2022 Justine Alexandra Roberts Tunney                              │
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
#include "libc/assert.h"
#include "libc/atomic.h"
#include "libc/intrin/atomic.h"
#include "libc/intrin/kprintf.h"
#include "libc/log/backtrace.internal.h"
#include "libc/log/log.h"
#include "libc/macros.internal.h"
#include "libc/mem/hook.internal.h"
#include "libc/mem/mem.h"
#include "libc/runtime/symbols.internal.h"
#include "libc/sysv/consts/o.h"
#include "libc/thread/thread.h"
#include "third_party/dlmalloc/dlmalloc.h"

/**
 * @fileoverview Malloc Logging
 *
 * If you put the following in your main file:
 *
 *     __static_yoink("enable_memory_log");
 *
 * Then memory allocations with constant backtraces will be logged to
 * standard error. The columns printed are
 *
 *     MEM TID OP USAGE PTR OLD SIZE CALLER1 CALLER2 CALLER3 CALLER4
 *
 * delimited by spaces. For example, to see peak malloc usage:
 *
 *     ./myprog.com 2>log
 *     grep ^MEM log | sort -nk4 | tail -n10
 *
 * To see the largest allocations:
 *
 *     ./myprog.com 2>log
 *     grep ^MEM log | grep -v free | sort -nk7 | tail -n10
 */

static struct Memlog {
  void (*free)(void *);
  void *(*malloc)(size_t);
  void *(*calloc)(size_t, size_t);
  void *(*memalign)(size_t, size_t);
  void *(*realloc)(void *, size_t);
  void *(*realloc_in_place)(void *, size_t);
  size_t (*bulk_free)(void *[], size_t);
  struct Allocs {
    long i, n, f;
    struct Alloc {
      void *addr;
      long size;
    } * p;
  } allocs;
  atomic_long usage;
} __memlog;

static pthread_mutex_t __memlog_lock_obj;

static void __memlog_lock(void) {
  pthread_mutex_lock(&__memlog_lock_obj);
}

static void __memlog_unlock(void) {
  pthread_mutex_unlock(&__memlog_lock_obj);
}

static long __memlog_size(void *p) {
  return malloc_usable_size(p) + 16;
}

static void __memlog_backtrace(struct StackFrame *frame, intptr_t *a,
                               intptr_t *b, intptr_t *c, intptr_t *d) {
  *a = *b = *c = *d = 0;
  if (!frame) return;
  *a = frame->addr;
  if (!(frame = frame->next)) return;
  *b = frame->addr;
  if (!(frame = frame->next)) return;
  *c = frame->addr;
  if (!(frame = frame->next)) return;
  *d = frame->addr;
}

static long __memlog_find(void *p) {
  long i;
  for (i = 0; i < __memlog.allocs.i; ++i) {
    if (__memlog.allocs.p[i].addr == p) {
      return i;
    }
  }
  return -1;
}

static void __memlog_insert(void *p) {
  long i, n, n2;
  struct Alloc *p2;
  n = __memlog_size(p);
  for (i = __memlog.allocs.f; i < __memlog.allocs.i; ++i) {
    if (!__memlog.allocs.p[i].addr) {
      __memlog.allocs.p[i].addr = p;
      __memlog.allocs.p[i].size = n;
      __memlog.usage += n;
      return;
    }
  }
  if (i == __memlog.allocs.n) {
    p2 = __memlog.allocs.p;
    n2 = __memlog.allocs.n;
    n2 += 1;
    n2 += n2 >> 1;
    if ((p2 = dlrealloc(p2, n2 * sizeof(*p2)))) {
      __memlog.allocs.p = p2;
      __memlog.allocs.n = n2;
    } else {
      return;
    }
  }
  __memlog.allocs.p[i].addr = p;
  __memlog.allocs.p[i].size = n;
  __memlog.allocs.i++;
  __memlog.usage += n;
}

static void __memlog_update(void *p2, void *p) {
  long i, n;
  n = __memlog_size(p2);
  for (i = 0; i < __memlog.allocs.i; ++i) {
    if (__memlog.allocs.p[i].addr == p) {
      __memlog.usage += n - __memlog.allocs.p[i].size;
      __memlog.allocs.p[i].addr = p2;
      __memlog.allocs.p[i].size = n;
      unassert(__memlog.usage >= 0);
      return;
    }
  }
  __builtin_unreachable();
}

static void __memlog_log(struct StackFrame *frame, const char *op, void *res,
                         void *old, size_t n) {
  intptr_t a, b, c, d;
  __memlog_backtrace(frame, &a, &b, &c, &d);
  kprintf("MEM %6P %7s %12ld %14p %14p %8zu %t %t %t %t\n", op,
          atomic_load(&__memlog.usage), res, old, n, a, b, c, d);
}

static void __memlog_free(void *p) {
  long i, n;
  if (!p) return;
  __memlog_lock();
  if ((i = __memlog_find(p)) != -1) {
    n = __memlog.allocs.p[i].size;
    __memlog.allocs.p[i].addr = 0;
    __memlog.usage -= __memlog.allocs.p[i].size;
    __memlog.allocs.f = MIN(__memlog.allocs.f, i);
    unassert(__memlog.usage >= 0);
  } else {
    kprintf("memlog could not find %p\n", p);
    notpossible;
  }
  __memlog_unlock();
  unassert(__memlog.free);
  __memlog.free(p);
  __memlog_log(__builtin_frame_address(0), "free", 0, p, n);
}

static void *__memlog_malloc(size_t n) {
  void *res;
  unassert(__memlog.malloc);
  if ((res = __memlog.malloc(n))) {
    __memlog_lock();
    __memlog_insert(res);
    __memlog_unlock();
    __memlog_log(__builtin_frame_address(0), "malloc", res, 0, n);
  }
  return res;
}

static void *__memlog_calloc(size_t n, size_t z) {
  void *res;
  unassert(__memlog.calloc);
  if ((res = __memlog.calloc(n, z))) {
    __memlog_lock();
    __memlog_insert(res);
    __memlog_unlock();
    __memlog_log(__builtin_frame_address(0), "malloc", res, 0, n * z);
  }
  return res;
}

static void *__memlog_memalign(size_t l, size_t n) {
  void *res;
  unassert(__memlog.memalign);
  if ((res = __memlog.memalign(l, n))) {
    __memlog_lock();
    __memlog_insert(res);
    __memlog_unlock();
    __memlog_log(__builtin_frame_address(0), "malloc", res, 0, n);
  }
  return res;
}

static void *__memlog_realloc_impl(void *p, size_t n,
                                   void *(*f)(void *, size_t),
                                   struct StackFrame *frame) {
  void *res;
  unassert(f);
  if ((res = f(p, n))) {
    __memlog_lock();
    if (p) {
      __memlog_update(res, p);
    } else {
      __memlog_insert(res);
    }
    __memlog_unlock();
    __memlog_log(frame, "realloc", res, p, n);
  }
  return res;
}

static void *__memlog_realloc(void *p, size_t n) {
  return __memlog_realloc_impl(p, n, __memlog.realloc,
                               __builtin_frame_address(0));
}

static void *__memlog_realloc_in_place(void *p, size_t n) {
  return __memlog_realloc_impl(p, n, __memlog.realloc_in_place,
                               __builtin_frame_address(0));
}

static size_t __memlog_bulk_free(void *p[], size_t n) {
  size_t i;
  for (i = 0; i < n; ++i) {
    __memlog_free(p[i]);
    p[i] = 0;
  }
  return 0;
}

static textexit void __memlog_destroy(void) {
  __memlog_lock();
  hook_free = __memlog.free;
  hook_malloc = __memlog.malloc;
  hook_calloc = __memlog.calloc;
  hook_realloc = __memlog.realloc;
  hook_memalign = __memlog.memalign;
  hook_bulk_free = __memlog.bulk_free;
  hook_realloc_in_place = __memlog.realloc_in_place;
  dlfree(__memlog.allocs.p);
  __memlog.allocs.p = 0;
  __memlog.allocs.i = 0;
  __memlog.allocs.n = 0;
  __memlog_unlock();
}

static textstartup void __memlog_init(void) {
  GetSymbolTable();
  __memlog_lock();
  __memlog.free = hook_free;
  hook_free = __memlog_free;
  __memlog.malloc = hook_malloc;
  hook_malloc = __memlog_malloc;
  __memlog.calloc = hook_calloc;
  hook_calloc = __memlog_calloc;
  __memlog.realloc = hook_realloc;
  hook_realloc = __memlog_realloc;
  __memlog.memalign = hook_memalign;
  hook_memalign = __memlog_memalign;
  __memlog.bulk_free = hook_bulk_free;
  hook_bulk_free = __memlog_bulk_free;
  __memlog.realloc_in_place = hook_realloc_in_place;
  hook_realloc_in_place = __memlog_realloc_in_place;
  atexit(__memlog_destroy);
  __memlog_unlock();
}

const void *const enable_memory_log[] initarray = {
    __memlog_init,
};
