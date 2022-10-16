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
#include "libc/intrin/kprintf.h"
#include "libc/macros.internal.h"
#include "libc/runtime/memtrack.internal.h"
#include "libc/sysv/consts/map.h"
#include "libc/thread/thread.h"
#include "libc/thread/tls.h"

#define KMALLOC_ALIGN __BIGGEST_ALIGNMENT__

static struct {
  char *endptr;
  size_t total;
  pthread_spinlock_t lock;
} g_kmalloc;

static void kmalloc_lock(void) {
  if (__threaded) pthread_spin_lock(&g_kmalloc.lock);
}

static void kmalloc_unlock(void) {
  pthread_spin_unlock(&g_kmalloc.lock);
}

__attribute__((__constructor__)) static void kmalloc_init(void) {
  pthread_atfork(kmalloc_lock, kmalloc_unlock, kmalloc_unlock);
}

/**
 * Allocates permanent memory.
 *
 * The code malloc() depends upon uses this function to allocate memory.
 * The returned memory can't be freed, and leak detection is impossible.
 * This function panics when memory isn't available.
 */
void *kmalloc(size_t size) {
  char *start;
  size_t i, n;
  n = ROUNDUP(size + (IsAsan() * 8), KMALLOC_ALIGN);
  kmalloc_lock();
  i = g_kmalloc.total;
  g_kmalloc.total += n;
  start = (char *)kMemtrackKmallocStart;
  if (!g_kmalloc.endptr) g_kmalloc.endptr = start;
  g_kmalloc.endptr =
      _extend(start, g_kmalloc.total, g_kmalloc.endptr, MAP_PRIVATE,
              kMemtrackKmallocStart + kMemtrackKmallocSize);
  kmalloc_unlock();
  _unassert(!((intptr_t)(start + i) & (KMALLOC_ALIGN - 1)));
  if (IsAsan()) __asan_poison(start + i + size, n - size, kAsanHeapOverrun);
  return start + i;
}
