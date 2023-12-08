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
#include "libc/calls/calls.h"
#include "libc/calls/state.internal.h"
#include "libc/errno.h"
#include "libc/intrin/atomic.h"
#include "libc/intrin/strace.internal.h"
#include "libc/intrin/weaken.h"
#include "libc/runtime/internal.h"
#include "libc/thread/thread.h"
#include "libc/thread/tls.h"
#include "third_party/nsync/mu.h"

/**
 * Locks mutex.
 *
 * Here's an example of using a normal mutex:
 *
 *     pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
 *     pthread_mutex_lock(&lock);
 *     // do work...
 *     pthread_mutex_unlock(&lock);
 *     pthread_mutex_destroy(&lock);
 *
 * Cosmopolitan permits succinct notation for normal mutexes:
 *
 *     pthread_mutex_t lock = {0};
 *     pthread_mutex_lock(&lock);
 *     // do work...
 *     pthread_mutex_unlock(&lock);
 *
 * Here's an example of the proper way to do recursive mutexes:
 *
 *     pthread_mutex_t lock;
 *     pthread_mutexattr_t attr;
 *     pthread_mutexattr_init(&attr);
 *     pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
 *     pthread_mutex_init(&lock, &attr);
 *     pthread_mutexattr_destroy(&attr);
 *     pthread_mutex_lock(&lock);
 *     // do work...
 *     pthread_mutex_unlock(&lock);
 *     pthread_mutex_destroy(&lock);
 *
 * This function does nothing in vfork() children.
 *
 * @return 0 on success, or error number on failure
 * @see pthread_spin_lock()
 * @vforksafe
 */
errno_t pthread_mutex_lock(pthread_mutex_t *mutex) {
  int t;

  LOCKTRACE("pthread_mutex_lock(%t)", mutex);

  if (__vforked) {
    return 0;
  }

  if (mutex->_type == PTHREAD_MUTEX_NORMAL &&        //
      mutex->_pshared == PTHREAD_PROCESS_PRIVATE &&  //
      _weaken(nsync_mu_lock)) {
    _weaken(nsync_mu_lock)((nsync_mu *)mutex);
    return 0;
  }

  if (mutex->_type == PTHREAD_MUTEX_NORMAL) {
    while (atomic_exchange_explicit(&mutex->_lock, 1, memory_order_acquire)) {
      pthread_pause_np();
    }
    return 0;
  }

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

  while (atomic_exchange_explicit(&mutex->_lock, 1, memory_order_acquire)) {
    pthread_pause_np();
  }

  mutex->_depth = 0;
  mutex->_owner = t;
  mutex->_pid = __pid;

  return 0;
}
