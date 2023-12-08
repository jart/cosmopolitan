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
#include "libc/errno.h"
#include "libc/thread/thread.h"
#include "libc/thread/thread2.h"
#include "third_party/nsync/common.internal.h"
#include "third_party/nsync/cv.h"
#include "third_party/nsync/time.h"

/**
 * Waits for condition with optional time limit, e.g.
 *
 *     struct timespec ts;  // one second timeout
 *     ts = timespec_add(timespec_real(), timespec_frommillis(1000));
 *     if (pthread_cond_timedwait(cond, mutex, &ts) == ETIMEDOUT) {
 *       // handle timeout...
 *     }
 *
 * @param mutex needs to be held by thread when calling this function
 * @param abstime may be null to wait indefinitely and should contain
 *     some arbitrary interval added to a `CLOCK_REALTIME` timestamp
 * @return 0 on success, or errno on error
 * @raise ETIMEDOUT if `abstime` was specified and the current time
 *     exceeded its value
 * @raise EPERM if `mutex` is `PTHREAD_MUTEX_ERRORCHECK` and the lock
 *     isn't owned by the current thread
 * @raise EINVAL if `0 ≤ abstime->tv_nsec < 1000000000` wasn't the case
 * @raise ECANCELED if calling thread was cancelled in masked mode
 * @see pthread_cond_broadcast()
 * @see pthread_cond_signal()
 * @cancelationpoint
 */
errno_t pthread_cond_timedwait(pthread_cond_t *cond, pthread_mutex_t *mutex,
                               const struct timespec *abstime) {
  if (abstime && !(0 <= abstime->tv_nsec && abstime->tv_nsec < 1000000000)) {
    return EINVAL;
  }
  if (mutex->_type != PTHREAD_MUTEX_NORMAL) {
    nsync_panic_("pthread cond needs normal mutex\n");
  }
  return nsync_cv_wait_with_deadline(
      (nsync_cv *)cond, (nsync_mu *)mutex,
      abstime ? *abstime : nsync_time_no_deadline, 0);
}
