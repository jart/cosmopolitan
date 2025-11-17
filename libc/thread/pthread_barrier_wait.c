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
#include "libc/limits.h"
#include "libc/serialize.h"
#include "libc/thread/barrier.h"
#include "libc/thread/thread.h"

static int pthread_barrier_exit(pthread_barrier_t *b, unsigned entered) {
  unsigned exited =
      atomic_fetch_add_explicit(&b->_exited, 1, memory_order_release) + 1;
  if (exited == BARRIER_MAX - BARRIER_MAX % b->_count) {
    atomic_thread_fence(memory_order_acquire);
    atomic_store_explicit(&b->_round, 0, memory_order_relaxed);
    atomic_store_explicit(&b->_exited, 0, memory_order_relaxed);
    atomic_store_explicit(&b->_entered, 0, memory_order_release);
    cosmo_futex_wake((atomic_int *)&b->_entered, INT_MAX, b->_pshared);
  }
  if (entered % b->_count)
    return 0;
  return PTHREAD_BARRIER_SERIAL_THREAD;
}

/**
 * Waits for all threads to arrive at barrier.
 *
 * Your barrier object needs to be initialized by pthread_barrier_init()
 * which specifies a thread count. Any thread that calls this will block
 * until that number of threads are waiting at the same time. That's the
 * point when the barrier is released and your threads may then proceed.
 *
 * Upon being released a return code of 0 is returned to all threads but
 * one. Whichever thread arrived at the barrier first, will instead have
 * the return code `PTHREAD_BARRIER_SERIAL_THREAD`.
 *
 * Your barrier is automatically reset when it's released so that it may
 * be reused without needing to reinitialize the object.
 *
 * This function offers an acquire release memory barrier to all threads
 * participating in the barrier.
 *
 * This implementation never fails.
 *
 * @return 0 or `PTHREAD_BARRIER_SERIAL_THREAD`
 */
int pthread_barrier_wait(pthread_barrier_t *b) {
  unsigned entered, round, max = BARRIER_MAX - BARRIER_MAX % b->_count;
  for (;;) {
    entered =
        atomic_fetch_add_explicit(&b->_entered, 1, memory_order_acq_rel) + 1;
    round = atomic_load_explicit(&b->_round, memory_order_relaxed);
    if (entered <= max)
      break;
    while (entered > max) {
      BLOCK_CANCELATION;
      cosmo_futex_wait((atomic_int *)&b->_entered, entered, b->_pshared, 0, 0);
      ALLOW_CANCELATION;
      entered = atomic_load_explicit(&b->_entered, memory_order_relaxed);
    }
  }
  while (round + b->_count <= entered) {
    unsigned next = entered - entered % b->_count;
    if (atomic_compare_exchange_weak_explicit(&b->_round, &round, next,
                                              memory_order_release,
                                              memory_order_relaxed)) {
      round = next;
      cosmo_futex_wake((atomic_int *)&b->_round, INT_MAX, b->_pshared);
      if (entered <= round)
        return pthread_barrier_exit(b, entered);
      break;
    }
  }
  while (entered > round) {
    BLOCK_CANCELATION;
    cosmo_futex_wait((atomic_int *)&b->_round, round, b->_pshared, 0, 0);
    ALLOW_CANCELATION;
    round = atomic_load_explicit(&b->_round, memory_order_relaxed);
  }
  atomic_thread_fence(memory_order_acquire);
  return pthread_barrier_exit(b, entered);
}
