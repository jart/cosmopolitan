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
#include "libc/calls/calls.h"
#include "libc/calls/struct/timespec.h"
#include "libc/errno.h"
#include "libc/intrin/atomic.h"
#include "libc/intrin/futex.internal.h"
#include "libc/thread/thread.h"
#include "libc/thread/thread2.h"

/**
 * Waits for condition with optional time limit, e.g.
 *
 *     struct timespec ts;  // one second timeout
 *     ts = _timespec_add(_timespec_mono(), _timespec_frommillis(1000));
 *     if (pthread_cond_timedwait(cond, mutex, &ts) == ETIMEDOUT) {
 *       // handle timeout...
 *     }
 *
 * @param mutex needs to be held by thread when calling this function
 * @param abstime may be null to wait indefinitely and should contain
 *     some arbitrary interval added to a `CLOCK_MONOTONIC` timestamp
 * @return 0 on success, or errno on error
 * @raise ETIMEDOUT if `abstime` was specified and the current time
 *     exceeded its value
 * @raise EPERM if `mutex` is `PTHREAD_MUTEX_ERRORCHECK` and the lock
 *     isn't owned by the current thread
 * @raise EINVAL if `0 ≤ abstime->tv_nsec < 1000000000` wasn't the case
 * @see pthread_cond_broadcast
 * @see pthread_cond_signal
 */
int pthread_cond_timedwait(pthread_cond_t *cond, pthread_mutex_t *mutex,
                           const struct timespec *abstime) {
  int c, rc, err, seq;
  struct timespec now, rel, *tsp;

  if (abstime && !(0 <= abstime->tv_nsec && abstime->tv_nsec < 1000000000)) {
    assert(!"bad abstime");
    return EINVAL;
  }

  if (mutex->type == PTHREAD_MUTEX_ERRORCHECK) {
    c = atomic_load_explicit(&mutex->lock, memory_order_relaxed);
    if ((c & 0x000fffff) != gettid()) {
      assert(!"permlock");
      return EPERM;
    }
  }

  atomic_fetch_add(&cond->waits, 1);
  if (pthread_mutex_unlock(mutex)) notpossible;

  rc = 0;
  seq = atomic_load_explicit(&cond->seq, memory_order_relaxed);
  do {
    if (!abstime) {
      tsp = 0;
    } else {
      now = _timespec_mono();
      if (_timespec_gte(now, *abstime)) {
        rc = ETIMEDOUT;
        break;
      }
      rel = _timespec_sub(*abstime, now);
      tsp = &rel;
    }
    _futex_wait(&cond->seq, seq, cond->pshared, tsp);
  } while (seq == atomic_load_explicit(&cond->seq, memory_order_relaxed));

  if (pthread_mutex_lock(mutex)) notpossible;
  atomic_fetch_sub(&cond->waits, 1);

  return rc;
}
