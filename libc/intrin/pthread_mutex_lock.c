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
#include "libc/thread/lock.h"
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
  int me;
  uint64_t word, lock;

  LOCKTRACE("pthread_mutex_lock(%t)", mutex);

  if (__vforked)
    return 0;

  // get current state of lock
  word = atomic_load_explicit(&mutex->_word, memory_order_relaxed);

  // use fancy nsync mutex if possible
  if (MUTEX_TYPE(word) == PTHREAD_MUTEX_NORMAL &&        //
      MUTEX_PSHARED(word) == PTHREAD_PROCESS_PRIVATE &&  //
      _weaken(nsync_mu_lock)) {
    _weaken(nsync_mu_lock)((nsync_mu *)mutex);
    return 0;
  }

  // implement barebones normal mutexes
  if (MUTEX_TYPE(word) == PTHREAD_MUTEX_NORMAL) {
    for (;;) {
      word = MUTEX_UNLOCK(word);
      lock = MUTEX_LOCK(word);
      if (atomic_compare_exchange_weak_explicit(&mutex->_word, &word, lock,
                                                memory_order_acquire,
                                                memory_order_relaxed))
        return 0;
      pthread_pause_np();
    }
  }

  // implement recursive mutexes
  me = gettid();
  for (;;) {
    if (MUTEX_OWNER(word) == me) {
      if (MUTEX_TYPE(word) != PTHREAD_MUTEX_ERRORCHECK) {
        if (MUTEX_DEPTH(word) < MUTEX_DEPTH_MAX) {
          if (atomic_compare_exchange_weak_explicit(
                  &mutex->_word, &word, MUTEX_INC_DEPTH(word),
                  memory_order_relaxed, memory_order_relaxed))
            return 0;
          continue;
        } else {
          return EAGAIN;
        }
      } else {
        return EDEADLK;
      }
    }
    word = MUTEX_UNLOCK(word);
    lock = MUTEX_LOCK(word);
    lock = MUTEX_SET_OWNER(lock, me);
    if (atomic_compare_exchange_weak_explicit(&mutex->_word, &word, lock,
                                              memory_order_acquire,
                                              memory_order_relaxed)) {
      mutex->_pid = __pid;
      return 0;
    }
    pthread_pause_np();
  }
}
