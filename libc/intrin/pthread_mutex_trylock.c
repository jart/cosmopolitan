/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2023 Justine Alexandra Roberts Tunney                              │
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
#include "libc/calls/calls.h"
#include "libc/errno.h"
#include "libc/intrin/atomic.h"
#include "libc/intrin/weaken.h"
#include "libc/runtime/internal.h"
#include "libc/thread/thread.h"
#include "third_party/nsync/mu.h"

/**
 * Attempts acquiring lock.
 *
 * Unlike pthread_mutex_lock() this function won't block and instead
 * returns an error immediately if the lock couldn't be acquired.
 *
 * @return 0 if lock was acquired, otherwise an errno
 * @raise EAGAIN if maximum number of recursive locks is held
 * @raise EBUSY if lock is currently held in read or write mode
 * @raise EINVAL if `mutex` doesn't refer to an initialized lock
 * @raise EDEADLK if `mutex` is `PTHREAD_MUTEX_ERRORCHECK` and the
 *     current thread already holds this mutex
 */
errno_t pthread_mutex_trylock(pthread_mutex_t *mutex) {
  int t;

  // delegate to *NSYNC if possible
  if (mutex->_type == PTHREAD_MUTEX_NORMAL &&
      mutex->_pshared == PTHREAD_PROCESS_PRIVATE &&  //
      _weaken(nsync_mu_trylock)) {
    if (_weaken(nsync_mu_trylock)((nsync_mu *)mutex)) {
      return 0;
    } else {
      return EBUSY;
    }
  }

  // handle normal mutexes
  if (mutex->_type == PTHREAD_MUTEX_NORMAL) {
    if (!atomic_exchange_explicit(&mutex->_lock, 1, memory_order_acquire)) {
      return 0;
    } else {
      return EBUSY;
    }
  }

  // handle recursive and error check mutexes
  t = gettid();
  if (mutex->_owner == t) {
    if (mutex->_type != PTHREAD_MUTEX_ERRORCHECK) {
      if (mutex->_depth < 63) {
        ++mutex->_depth;
        return 0;
      } else {
        return EAGAIN;
      }
    } else {
      return EDEADLK;
    }
  }

  if (atomic_exchange_explicit(&mutex->_lock, 1, memory_order_acquire)) {
    return EBUSY;
  }

  mutex->_depth = 0;
  mutex->_owner = t;
  mutex->_pid = __pid;

  return 0;
}
