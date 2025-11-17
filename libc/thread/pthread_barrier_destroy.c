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
#include "libc/calls/blockcancel.internal.h"
#include "libc/cosmo.h"
#include "libc/intrin/atomic.h"
#include "libc/thread/barrier.h"
#include "libc/thread/thread.h"

/**
 * Destroys barrier.
 *
 * Cosmopolitan barriers do not internally allocate memory or resources.
 * So the world is unlikely to end if you don't call this function. When
 * you need to do this is if your barrier object is on the stack and you
 * aren't synchronizing teardown using some function like pthread_join()
 *
 * This implementation does not fail. Rather than returning EBUSY if the
 * barrier has waiters, this will block until completion. This operation
 * can not be canceled in deferred mode. If there is a bug in your code,
 * then this function might deadlock; that is the worst that can happen.
 *
 * @return always zero
 */
errno_t pthread_barrier_destroy(pthread_barrier_t *b) {
  unsigned max = BARRIER_MAX - BARRIER_MAX % b->_count;
  unsigned entered = atomic_load_explicit(&b->_entered, memory_order_relaxed);
  if (atomic_fetch_add_explicit(&b->_exited, max - entered,
                                memory_order_relaxed) < entered) {
    while (entered) {
      BLOCK_CANCELATION;
      cosmo_futex_wait((atomic_int *)&b->_entered, entered, b->_pshared, 0, 0);
      ALLOW_CANCELATION;
      entered = atomic_load_explicit(&b->_entered, memory_order_relaxed);
    }
  }
  atomic_thread_fence(memory_order_acquire);
  return 0;
}
