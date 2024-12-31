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
#include "libc/cosmo.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/intrin/atomic.h"
#include "libc/intrin/describeflags.h"
#include "libc/intrin/kprintf.h"
#include "libc/intrin/strace.h"
#include "libc/intrin/weaken.h"
#include "libc/runtime/internal.h"
#include "libc/thread/lock.h"
#include "libc/thread/posixthread.internal.h"
#include "libc/thread/thread.h"
#include "libc/thread/tls.h"
#include "third_party/nsync/mu.h"

// see "take 3" algorithm in "futexes are tricky" by ulrich drepper
static void pthread_mutex_unlock_drepper(atomic_int *futex, char pshare) {
  int word = atomic_fetch_sub_explicit(futex, 1, memory_order_release);
  if (word == 2) {
    atomic_store_explicit(futex, 0, memory_order_release);
    cosmo_futex_wake(futex, 1, pshare);
  }
}

static errno_t pthread_mutex_unlock_recursive(pthread_mutex_t *mutex,
                                              uint64_t word) {
  int me = atomic_load_explicit(&__get_tls()->tib_ptid, memory_order_relaxed);
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
            memory_order_relaxed)) {
      if (IsModeDbg())
        __deadlock_untrack(mutex);
      return 0;
    }
  }
}

#if PTHREAD_USE_NSYNC
static errno_t pthread_mutex_unlock_recursive_nsync(pthread_mutex_t *mutex,
                                                    uint64_t word) {
  int me = atomic_load_explicit(&__get_tls()->tib_ptid, memory_order_relaxed);
  for (;;) {

    // we allow unlocking an initialized lock that wasn't locked, but we
    // don't allow unlocking a lock held by another thread, or unlocking
    // recursive locks from a forked child, since it should be re-init'd
    if (MUTEX_OWNER(word) && (MUTEX_OWNER(word) != me || mutex->_pid != __pid))
      return EPERM;

    // check if this is a nested lock with signal safety
    if (MUTEX_DEPTH(word)) {
      if (atomic_compare_exchange_strong_explicit(
              &mutex->_word, &word, MUTEX_DEC_DEPTH(word), memory_order_relaxed,
              memory_order_relaxed))
        return 0;
      continue;
    }

    // actually unlock the mutex
    mutex->_word = MUTEX_UNLOCK(word);
    _weaken(nsync_mu_unlock)((nsync_mu *)mutex->_nsync);
    if (IsModeDbg())
      __deadlock_untrack(mutex);
    return 0;
  }
}
#endif

static errno_t pthread_mutex_unlock_impl(pthread_mutex_t *mutex) {
  uint64_t word = atomic_load_explicit(&mutex->_word, memory_order_relaxed);

  // check if mutex isn't held by calling thread
  if (MUTEX_TYPE(word) == PTHREAD_MUTEX_ERRORCHECK || IsModeDbg()) {
    if (__deadlock_tracked(mutex) == 0) {
      if (IsModeDbg() && MUTEX_TYPE(word) != PTHREAD_MUTEX_ERRORCHECK) {
        kprintf("error: unlock mutex not owned by calling thread: %t\n", mutex);
        DebugBreak();
      }
      return EPERM;
    }
  }

  // handle recursive mutexes
  if (MUTEX_TYPE(word) == PTHREAD_MUTEX_RECURSIVE) {
#if PTHREAD_USE_NSYNC
    if (_weaken(nsync_mu_unlock) &&
        MUTEX_PSHARED(word) == PTHREAD_PROCESS_PRIVATE) {
      return pthread_mutex_unlock_recursive_nsync(mutex, word);
    } else {
      return pthread_mutex_unlock_recursive(mutex, word);
    }
#else
    return pthread_mutex_unlock_recursive(mutex, word);
#endif
  }

#if PTHREAD_USE_NSYNC
  // use superior mutexes if possible
  if (MUTEX_PSHARED(word) == PTHREAD_PROCESS_PRIVATE &&  //
      _weaken(nsync_mu_unlock)) {
    // on apple silicon we should just put our faith in ulock
    // otherwise *nsync gets struck down by the eye of sauron
    if (!IsXnuSilicon()) {
      _weaken(nsync_mu_unlock)((nsync_mu *)mutex->_nsync);
      if (MUTEX_TYPE(word) == PTHREAD_MUTEX_ERRORCHECK || IsModeDbg())
        __deadlock_untrack(mutex);
      return 0;
    }
  }
#endif

  // implement barebones normal mutexes
  pthread_mutex_unlock_drepper(&mutex->_futex, MUTEX_PSHARED(word));
  if (MUTEX_TYPE(word) == PTHREAD_MUTEX_ERRORCHECK || IsModeDbg())
    __deadlock_untrack(mutex);
  return 0;
}

/**
 * Releases mutex.
 *
 * POSIX.1 says it's undefined behavior to unlock a mutex that wasn't
 * locked by the calling thread. Therefore, if `mutex` isn't locked, or
 * it is locked and the thing that locked it was a different thread or
 * process, then you should expect your program to deadlock or crash.
 *
 * This function does nothing in vfork() children.
 *
 * @return 0 on success or error number on failure
 * @raises EPERM if mutex ownership isn't acceptable
 * @vforksafe
 */
errno_t _pthread_mutex_unlock(pthread_mutex_t *mutex) {
  if (__tls_enabled && !__vforked) {
    errno_t err = pthread_mutex_unlock_impl(mutex);
    LOCKTRACE("pthread_mutex_unlock(%t) → %s", mutex, DescribeErrno(err));
    return err;
  } else {
    LOCKTRACE("skipping pthread_mutex_lock(%t) due to runtime state", mutex);
    return 0;
  }
}

__weak_reference(_pthread_mutex_unlock, pthread_mutex_unlock);
