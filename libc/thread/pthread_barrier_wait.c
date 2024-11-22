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
#include "libc/errno.h"
#include "libc/intrin/atomic.h"
#include "libc/limits.h"
#include "libc/thread/thread.h"

/**
 * Waits for all threads to arrive at barrier.
 *
 * When the barrier is broken, the state becomes reset to what it was
 * when pthread_barrier_init() was called, so that the barrior may be
 * used again in the same way.
 *
 * Unlike pthread_cond_timedwait() this function is not a cancelation
 * point. It is not needed to have cleanup handlers on block cancels.
 *
 * @return 0 on success, `PTHREAD_BARRIER_SERIAL_THREAD` to one lucky
 *     thread which was the last arrival, or an errno on error
 * @raise EINVAL if barrier is used incorrectly
 */
errno_t pthread_barrier_wait(pthread_barrier_t *barrier) {
  int n;

  // enter barrier
  atomic_fetch_add_explicit(&barrier->_waiters, 1, memory_order_acq_rel);
  n = atomic_fetch_sub_explicit(&barrier->_counter, 1, memory_order_acq_rel);
  n = n - 1;

  // this can only happen on invalid usage
  if (n < 0)
    return EINVAL;

  // reset count and wake waiters if we're last at barrier
  if (!n) {
    atomic_store_explicit(&barrier->_counter, barrier->_count,
                          memory_order_release);
    atomic_store_explicit(&barrier->_waiters, 0, memory_order_release);
    cosmo_futex_wake(&barrier->_waiters, INT_MAX, barrier->_pshared);
    return PTHREAD_BARRIER_SERIAL_THREAD;
  }

  // wait for everyone else to arrive at barrier
  BLOCK_CANCELATION;
  while ((n = atomic_load_explicit(&barrier->_waiters, memory_order_acquire)))
    cosmo_futex_wait(&barrier->_waiters, n, barrier->_pshared, 0, 0);
  ALLOW_CANCELATION;

  return 0;
}
