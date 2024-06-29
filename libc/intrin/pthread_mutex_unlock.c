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
#include "libc/errno.h"
#include "libc/intrin/atomic.h"
#include "libc/intrin/strace.internal.h"
#include "libc/intrin/weaken.h"
#include "libc/runtime/internal.h"
#include "libc/thread/lock.h"
#include "libc/thread/thread.h"
#include "third_party/nsync/mu.h"

/**
 * Releases mutex.
 *
 * This function does nothing in vfork() children.
 *
 * @return 0 on success or error number on failure
 * @raises EPERM if in error check mode and not owned by caller
 * @vforksafe
 */
errno_t pthread_mutex_unlock(pthread_mutex_t *mutex) {
  int me;
  uint64_t word, lock;

  LOCKTRACE("pthread_mutex_unlock(%t)", mutex);

  // get current state of lock
  word = atomic_load_explicit(&mutex->_word, memory_order_relaxed);

  // use fancy nsync mutex if possible
  if (MUTEX_TYPE(word) == PTHREAD_MUTEX_NORMAL &&        //
      MUTEX_PSHARED(word) == PTHREAD_PROCESS_PRIVATE &&  //
      _weaken(nsync_mu_unlock)) {
    _weaken(nsync_mu_unlock)((nsync_mu *)mutex);
    return 0;
  }

  // implement barebones normal mutexes
  if (MUTEX_TYPE(word) == PTHREAD_MUTEX_NORMAL) {
    lock = MUTEX_UNLOCK(word);
    atomic_store_explicit(&mutex->_word, lock, memory_order_release);
    return 0;
  }

  // implement recursive mutex unlocking
  me = gettid();
  for (;;) {

    // we allow unlocking an initialized lock that wasn't locked, but we
    // don't allow unlocking a lock held by another thread, or unlocking
    // recursive locks from a forked child, since it should be re-init'd
    if (MUTEX_OWNER(word) && (MUTEX_OWNER(word) != me || mutex->_pid != __pid))
      return EPERM;

    // check if this is a nested lock with signal safety
    if (MUTEX_DEPTH(word)) {
      if (atomic_compare_exchange_weak_explicit(
              &mutex->_word, &word, MUTEX_DEC_DEPTH(word), memory_order_relaxed,
              memory_order_relaxed))
        return 0;
      continue;
    }

    // actually unlock the mutex
    if (atomic_compare_exchange_weak_explicit(
            &mutex->_word, &word, MUTEX_UNLOCK(word), memory_order_release,
            memory_order_relaxed))
      return 0;
  }
}
