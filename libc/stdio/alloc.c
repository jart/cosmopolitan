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
#include "libc/intrin/atomic.h"
#include "libc/intrin/kmalloc.h"
#include "libc/stdio/internal.h"
#include "libc/str/str.h"
#include "libc/thread/thread.h"

static _Atomic(FILE *) __stdio_freelist;

FILE *__stdio_alloc(void) {
  FILE *f;
  f = atomic_load_explicit(&__stdio_freelist, memory_order_acquire);
  while (f) {
    if (atomic_compare_exchange_weak_explicit(
            &__stdio_freelist, &f,
            atomic_load_explicit(&f->next, memory_order_acquire),
            memory_order_release, memory_order_relaxed)) {
      atomic_store_explicit(&f->next, 0, memory_order_release);
      break;
    }
  }
  if (!f) {
    f = kmalloc(sizeof(FILE));
  }
  if (f) {
    ((pthread_mutex_t *)f->lock)->_type = PTHREAD_MUTEX_RECURSIVE;
  }
  return f;
}

void __stdio_free(FILE *f) {
  FILE *g;
  bzero(f, sizeof(*f));
  g = atomic_load_explicit(&__stdio_freelist, memory_order_acquire);
  for (;;) {
    atomic_store_explicit(&f->next, g, memory_order_release);
    if (atomic_compare_exchange_weak_explicit(&__stdio_freelist, &g, f,
                                              memory_order_release,
                                              memory_order_relaxed)) {
      break;
    }
  }
}
