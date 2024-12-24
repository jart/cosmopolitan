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
#include "libc/assert.h"
#include "libc/intrin/atomic.h"
#include "libc/intrin/fds.h"
#include "libc/runtime/runtime.h"
#include "libc/thread/posixthread.internal.h"

struct Cursor *__cursor_new(void) {
  struct Cursor *c;
  if ((c = _mapanon(sizeof(struct Cursor)))) {
    if ((c->shared = _mapshared(sizeof(struct CursorShared)))) {
      c->shared->lock = (pthread_mutex_t)PTHREAD_SHARED_MUTEX_INITIALIZER_NP;
    } else {
      munmap(c, sizeof(struct Cursor));
      c = 0;
    }
  }
  return c;
}

void __cursor_ref(struct Cursor *c) {
  if (!c)
    return;
  unassert(atomic_fetch_add_explicit(&c->refs, 1, memory_order_relaxed) >= 0);
}

int __cursor_unref(struct Cursor *c) {
  if (!c)
    return 0;
  if (atomic_fetch_sub_explicit(&c->refs, 1, memory_order_release))
    return 0;
  atomic_thread_fence(memory_order_acquire);
  int rc = munmap(c->shared, sizeof(struct CursorShared));
  rc |= munmap(c, sizeof(struct Cursor));
  return rc;
}

void __cursor_lock(struct Cursor *c) {
  _pthread_mutex_lock(&c->shared->lock);
}

void __cursor_unlock(struct Cursor *c) {
  _pthread_mutex_unlock(&c->shared->lock);
}
