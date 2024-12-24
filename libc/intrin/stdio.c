/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2021 Justine Alexandra Roberts Tunney                              │
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
#include "libc/intrin/atomic.h"
#include "libc/intrin/kprintf.h"
#include "libc/intrin/weaken.h"
#include "libc/mem/mem.h"
#include "libc/stdio/internal.h"
#include "libc/thread/posixthread.internal.h"

#define STDIO_FILE_USE_AFTER_FREE 1
#define CORRUPT_STDIO_FILE_OBJECT 1

struct Stdio __stdio = {
    .lock = PTHREAD_MUTEX_INITIALIZER,
};

void __stdio_lock(void) {
  _pthread_mutex_lock(&__stdio.lock);
}

void __stdio_unlock(void) {
  _pthread_mutex_unlock(&__stdio.lock);
}

static int refchk(int refs) {
  unassert(refs != STDIO_FILE_USE_AFTER_FREE);
  unassert(refs < CORRUPT_STDIO_FILE_OBJECT);
  return refs;
}

void __stdio_ref(FILE *f) {
  refchk(atomic_fetch_sub_explicit(&f->refs, 1, memory_order_relaxed));
}

static void __stdio_unref_impl(FILE *f, bool should_lock) {
  int refs = atomic_load_explicit(&f->refs, memory_order_relaxed);
  for (;;) {
    refchk(refs);
    if (refs) {
      if (atomic_compare_exchange_strong_explicit(&f->refs, &refs, refs + 1,
                                                  memory_order_acq_rel,
                                                  memory_order_relaxed))
        return;
      continue;
    }
    if (should_lock) {
      __stdio_lock();
      if ((refs = atomic_load_explicit(&f->refs, memory_order_relaxed))) {
        __stdio_unlock();
        continue;
      }
    }
    if (!atomic_compare_exchange_strong_explicit(
            &f->refs, &refs, 1, memory_order_acq_rel, memory_order_relaxed)) {
      if (should_lock)
        __stdio_unlock();
      continue;
    }
    dll_remove(&__stdio.files, &f->elem);
    if (should_lock)
      __stdio_unlock();
    break;
  }
  if (_weaken(free)) {
    _weaken(free)(f->getln);
    if (f->freebuf)
      _weaken(free)(f->buf);
    if (f->freethis)
      _weaken(free)(f);
  }
}

void __stdio_unref(FILE *f) {
  __stdio_unref_impl(f, true);
}

void __stdio_unref_unlocked(FILE *f) {
  __stdio_unref_impl(f, false);
}
