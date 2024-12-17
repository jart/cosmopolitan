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
#include "libc/cosmo.h"
#include "libc/dce.h"
#include "libc/intrin/atomic.h"
#include "libc/limits.h"
#include "libc/thread/thread.h"
#include "third_party/nsync/cv.h"

__static_yoink("nsync_mu_lock");
__static_yoink("nsync_mu_unlock");
__static_yoink("nsync_mu_trylock");

/**
 * Wakes all threads waiting on condition, e.g.
 *
 *     pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
 *     pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
 *     // ...
 *     pthread_mutex_lock(&lock);
 *     pthread_cond_broadcast(&cond);
 *     pthread_mutex_unlock(&lock);
 *
 * This function has no effect if there aren't any threads currently
 * waiting on the condition.
 *
 * @return 0 on success, or errno on error
 * @see pthread_cond_signal
 * @see pthread_cond_wait
 */
errno_t pthread_cond_broadcast(pthread_cond_t *cond) {

#if PTHREAD_USE_NSYNC
  // do nothing if pthread_cond_timedwait() hasn't been called yet
  // this is because we dont know for certain if nsync use is safe
  if (!atomic_load_explicit(&cond->_waited, memory_order_acquire))
    return 0;

  // favor *NSYNC if this is a process private condition variable
  // if using Mike Burrows' code isn't possible, use a naive impl
  if (!cond->_footek) {
    nsync_cv_broadcast((nsync_cv *)cond->_nsync);
    return 0;
  }
#endif

  // roll forward the monotonic sequence
  atomic_fetch_add_explicit(&cond->_sequence, 1, memory_order_acq_rel);
  if (atomic_load_explicit(&cond->_waiters, memory_order_acquire))
    cosmo_futex_wake((atomic_int *)&cond->_sequence, INT_MAX, cond->_pshared);
  return 0;
}
