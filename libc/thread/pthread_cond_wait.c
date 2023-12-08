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
#include "libc/thread/thread.h"
#include "libc/thread/thread2.h"

/**
 * Waits for condition, e.g.
 *
 *     pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
 *     pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
 *     // ...
 *     pthread_mutex_lock(&lock);
 *     pthread_cond_wait(&cond, &lock);
 *     pthread_mutex_unlock(&lock);
 *
 * @param mutex needs to be held by thread when calling this function
 * @return 0 on success, or errno on error
 * @raise ECANCELED if calling thread was cancelled in masked mode
 * @raise EPERM if `mutex` is `PTHREAD_MUTEX_ERRORCHECK` and the lock
 *     isn't owned by the current thread
 * @see pthread_cond_timedwait
 * @see pthread_cond_broadcast
 * @see pthread_cond_signal
 * @cancelationpoint
 */
errno_t pthread_cond_wait(pthread_cond_t *cond, pthread_mutex_t *mutex) {
  return pthread_cond_timedwait(cond, mutex, 0);
}
