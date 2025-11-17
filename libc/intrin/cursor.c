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
#include "libc/calls/internal.h"
#include "libc/calls/state.internal.h"
#include "libc/intrin/atomic.h"
#include "libc/intrin/fds.h"
#include "libc/runtime/runtime.h"
#include "libc/sysv/pib.h"
#include "libc/thread/posixthread.internal.h"

// did you know that current seek position of a file descriptor can be
// shared by multiple file descriptors across multiple processes? it's
// very expensive to support this, but we're going to do our very best

void __cursor_destroy(struct Cursor *c) {
  munmap(c->shared, sizeof(struct CursorShared));
  munmap(c, sizeof(struct Cursor));
}

struct Cursor *__cursor_new(void) {
  // assume __fds_lock() is held
  struct Cursor *c;
  if ((c = __get_pib()->fds.freed_cursors)) {
    __get_pib()->fds.freed_cursors = c->next_free_cursor;
    atomic_init(&c->refs, 0);
    c->shared->pointer = 0;
    return c;
  }
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

bool __cursor_ref(struct Cursor *c) {
  if (!c)
    return false;
  return atomic_fetch_add_explicit(&c->refs, 1, memory_order_relaxed) >= 0;
}

void __cursor_unref(struct Cursor *c) {
  if (atomic_fetch_sub_explicit(&c->refs, 1, memory_order_release))
    return;
  // we've now established that, within this process, no file descriptor
  // exists that's using this cursor. however while the cursor was being
  // used, fork() or execve() could have enabled it to spread elsewhere!
  __fds_lock();
  if (!c->is_multiprocess) {
    c->next_free_cursor = __get_pib()->fds.freed_cursors;
    __get_pib()->fds.freed_cursors = c;
    __fds_unlock();
  } else {
    __fds_unlock();
    __cursor_destroy(c);
  }
}

void __cursor_lock(struct Cursor *c) {
  pthread_mutex_lock(&c->shared->lock);
}

void __cursor_unlock(struct Cursor *c) {
  pthread_mutex_unlock(&c->shared->lock);
}
