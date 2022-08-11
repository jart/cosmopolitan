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
#include "libc/bits/atomic.h"
#include "libc/calls/calls.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/intrin/kprintf.h"
#include "libc/intrin/pthread.h"

/**
 * Releases mutex.
 * @return 0 on success or error number on failure
 * @raises EPERM if in error check mode and not owned by caller
 */
int(pthread_mutex_unlock)(pthread_mutex_t *mutex) {
  int me, owner;
  switch (mutex->attr) {
    case PTHREAD_MUTEX_ERRORCHECK:
      me = gettid();
      owner = atomic_load_explicit(&mutex->lock, memory_order_relaxed);
      if (owner != me) {
        assert(!"perm lock");
        return EPERM;
      }
      // fallthrough
    case PTHREAD_MUTEX_RECURSIVE:
      if (--mutex->reent) return 0;
      // fallthrough
    case PTHREAD_MUTEX_NORMAL:
      atomic_store_explicit(&mutex->lock, 0, memory_order_relaxed);
      if (atomic_load_explicit(&mutex->waits, memory_order_relaxed) > 0) {
        _pthread_mutex_wake(mutex);
      }
      return 0;
    default:
      assert(!"inva lock");
      return EINVAL;
  }
}
