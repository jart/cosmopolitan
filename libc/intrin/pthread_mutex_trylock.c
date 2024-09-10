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
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/intrin/atomic.h"
#include "libc/intrin/weaken.h"
#include "libc/runtime/internal.h"
#include "libc/thread/lock.h"
#include "libc/thread/thread.h"
#include "third_party/nsync/futex.internal.h"
#include "third_party/nsync/mu.h"

static errno_t pthread_mutex_trylock_spin(atomic_int *word) {
  if (!atomic_exchange_explicit(word, 1, memory_order_acquire))
    return 0;
  return EBUSY;
}

static errno_t pthread_mutex_trylock_drepper(atomic_int *futex) {
  int word = 0;
  if (atomic_compare_exchange_strong_explicit(
          futex, &word, 1, memory_order_acquire, memory_order_acquire))
    return 0;
  return EBUSY;
}

static errno_t pthread_mutex_trylock_recursive(pthread_mutex_t *mutex,
                                               uint64_t word) {
  uint64_t lock;
  int me = gettid();
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
    return EBUSY;
  }
}

static errno_t pthread_mutex_trylock_recursive_nsync(pthread_mutex_t *mutex,
                                                     uint64_t word) {
  int me = gettid();
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
    if (_weaken(nsync_mu_trylock)((nsync_mu *)mutex->_nsyncx)) {
      word = MUTEX_UNLOCK(word);
      word = MUTEX_LOCK(word);
      word = MUTEX_SET_OWNER(word, me);
      mutex->_word = word;
      mutex->_pid = __pid;
      return 0;
    } else {
      return EBUSY;
    }
  }
}

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

  // get current state of lock
  uint64_t word = atomic_load_explicit(&mutex->_word, memory_order_relaxed);

#if PTHREAD_USE_NSYNC
  // use superior mutexes if possible
  if (MUTEX_TYPE(word) == PTHREAD_MUTEX_NORMAL &&
      MUTEX_PSHARED(word) == PTHREAD_PROCESS_PRIVATE &&  //
      _weaken(nsync_mu_trylock)) {
    // on apple silicon we should just put our faith in ulock
    // otherwise *nsync gets struck down by the eye of sauron
    if (!IsXnuSilicon()) {
      if (_weaken(nsync_mu_trylock)((nsync_mu *)mutex)) {
        return 0;
      } else {
        return EBUSY;
      }
    }
  }
#endif

  // handle normal mutexes
  if (MUTEX_TYPE(word) == PTHREAD_MUTEX_NORMAL) {
    if (_weaken(nsync_futex_wait_)) {
      return pthread_mutex_trylock_drepper(&mutex->_futex);
    } else {
      return pthread_mutex_trylock_spin(&mutex->_futex);
    }
  }

  // handle recursive and error checking mutexes
#if PTHREAD_USE_NSYNC
  if (_weaken(nsync_mu_trylock) &&
      MUTEX_PSHARED(word) == PTHREAD_PROCESS_PRIVATE) {
    return pthread_mutex_trylock_recursive_nsync(mutex, word);
  } else {
    return pthread_mutex_trylock_recursive(mutex, word);
  }
#else
  return pthread_mutex_trylock_recursive(mutex, word);
#endif
}
