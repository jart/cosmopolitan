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
#include "libc/dce.h"
#include "libc/intrin/atomic.h"
#include "libc/intrin/futex.internal.h"
#include "libc/intrin/pthread.h"
#include "libc/limits.h"

static dontinline int pthread_cond_signal_impl(pthread_cond_t *cond, int n) {
  if (atomic_load_explicit(&cond->waits, memory_order_relaxed)) {
    atomic_fetch_add(&cond->seq, 1);
    if (IsLinux() || IsOpenbsd()) {
      _futex_wake(&cond->seq, n, cond->pshared);
    }
  }
  return 0;
}

/**
 * Wakes at least one thread waiting on condition, e.g.
 *
 *     pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
 *     pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
 *
 *     // thread pool waiters
 *     pthread_mutex_lock(&lock);
 *     pthread_cond_wait(&cond, &lock);
 *     pthread_mutex_unlock(&lock);
 *
 *     // waker upper
 *     pthread_mutex_lock(&lock);
 *     pthread_cond_signal(&cond);
 *     pthread_mutex_unlock(&lock);
 *
 * This function has no effect if there aren't any threads currently
 * waiting on the condition.
 *
 * @return 0 on success, or errno on error
 * @see pthread_cond_broadcast
 * @see pthread_cond_wait
 */
int pthread_cond_signal(pthread_cond_t *cond) {
  return pthread_cond_signal_impl(cond, 1);
}

/**
 * Wakes all threads waiting on condition, e.g.
 *
 *     pthread_mutex_t lock;
 *     pthread_mutexattr_t mattr;
 *     pthread_mutexattr_init(&mattr);
 *     pthread_mutexattr_settype(&mattr, PTHREAD_MUTEX_ERRORCHECK);
 *     pthread_mutex_init(&lock, &mattr);
 *
 *     pthread_cond_t cond;
 *     pthread_condattr_t cattr;
 *     pthread_condattr_init(&cattr);
 *     pthread_condattr_setpshared(&cattr, PTHREAD_PROCESS_SHARED);
 *     pthread_cond_init(&cond, &cattr);
 *
 *     // waiting threads
 *     CHECK_EQ(0, pthread_mutex_lock(&lock));
 *     CHECK_EQ(0, pthread_cond_wait(&cond, &lock));
 *     pthread_mutex_unlock(&lock);
 *
 *     // notifying thread
 *     CHECK_EQ(0, pthread_mutex_lock(&lock));
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
int pthread_cond_broadcast(pthread_cond_t *cond) {
  return pthread_cond_signal_impl(cond, INT_MAX);
}
