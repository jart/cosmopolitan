/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
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
#include "libc/dce.h"
#include "libc/intrin/asan.internal.h"
#include "libc/intrin/atomic.h"
#include "libc/intrin/extend.internal.h"
#include "libc/macros.internal.h"
#include "libc/runtime/memtrack.internal.h"
#include "libc/sysv/consts/map.h"
#include "libc/thread/thread.h"
#include "libc/thread/tls.h"

#define KMALLOC_ALIGN sizeof(intptr_t)

static struct {
  char *endptr;
  size_t total;
  pthread_spinlock_t lock;
} g_kmalloc;

void __kmalloc_lock(void) {
  pthread_spin_lock(&g_kmalloc.lock);
}

void __kmalloc_unlock(void) {
  pthread_spin_unlock(&g_kmalloc.lock);
}

#ifdef _NOPL0
#define __kmalloc_lock()   _NOPL0("__threadcalls", __kmalloc_lock)
#define __kmalloc_unlock() _NOPL0("__threadcalls", __kmalloc_unlock)
#endif

/**
 * Allocates permanent memory.
 *
 * The code malloc() depends upon uses this function to allocate memory.
 * The returned memory can't be freed, and leak detection is impossible.
 * This function panics when memory isn't available.
 *
 * Memory returned by this function is aligned on the word size, and as
 * such, kmalloc() shouldn't be used for vector operations.
 *
 * @return zero-initialized memory on success, or null w/ errno
 * @raise ENOMEM if we require more vespene gas
 */
void *kmalloc(size_t size) {
  char *p, *e;
  size_t i, n, t;
  n = ROUNDUP(size + (IsAsan() * 8), KMALLOC_ALIGN);
  __kmalloc_lock();
  t = g_kmalloc.total;
  e = g_kmalloc.endptr;
  i = t;
  t += n;
  p = (char *)kMemtrackKmallocStart;
  if (!e) e = p;
  if ((e = _extend(p, t, e, MAP_PRIVATE,
                   kMemtrackKmallocStart + kMemtrackKmallocSize))) {
    g_kmalloc.endptr = e;
    g_kmalloc.total = t;
  } else {
    p = 0;
  }
  __kmalloc_unlock();
  if (p) {
    _unassert(!((intptr_t)(p + i) & (KMALLOC_ALIGN - 1)));
    if (IsAsan()) __asan_poison(p + i + size, n - size, kAsanHeapOverrun);
    return p + i;
  } else {
    return 0;
  }
}
