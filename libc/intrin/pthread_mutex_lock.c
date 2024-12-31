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
#include "libc/calls/blockcancel.internal.h"
#include "libc/calls/state.internal.h"
#include "libc/cosmo.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/intrin/atomic.h"
#include "libc/intrin/describeflags.h"
#include "libc/intrin/kprintf.h"
#include "libc/intrin/strace.h"
#include "libc/intrin/weaken.h"
#include "libc/macros.h"
#include "libc/runtime/internal.h"
#include "libc/thread/lock.h"
#include "libc/thread/posixthread.internal.h"
#include "libc/thread/thread.h"
#include "libc/thread/tls.h"
#include "third_party/nsync/mu.h"

static errno_t pthread_mutex_lock_normal_success(pthread_mutex_t *mutex,
                                                 uint64_t word) {
  if (IsModeDbg() || MUTEX_TYPE(word) == PTHREAD_MUTEX_ERRORCHECK) {
    __deadlock_track(mutex, MUTEX_TYPE(word) == PTHREAD_MUTEX_ERRORCHECK);
    __deadlock_record(mutex, MUTEX_TYPE(word) == PTHREAD_MUTEX_ERRORCHECK);
  }
  return 0;
}

// see "take 3" algorithm in "futexes are tricky" by ulrich drepper
// slightly improved to attempt acquiring multiple times b4 syscall
static int pthread_mutex_lock_drepper(pthread_mutex_t *mutex, uint64_t word,
                                      bool is_trylock) {
  int val = 0;
  if (atomic_compare_exchange_strong_explicit(
          &mutex->_futex, &val, 1, memory_order_acquire, memory_order_acquire))
    return pthread_mutex_lock_normal_success(mutex, word);
  if (is_trylock)
    return EBUSY;
  LOCKTRACE("acquiring pthread_mutex_lock_drepper(%t)...", mutex);
  if (val == 1)
    val = atomic_exchange_explicit(&mutex->_futex, 2, memory_order_acquire);
  BLOCK_CANCELATION;
  while (val > 0) {
    cosmo_futex_wait(&mutex->_futex, 2, MUTEX_PSHARED(word), 0, 0);
    val = atomic_exchange_explicit(&mutex->_futex, 2, memory_order_acquire);
  }
  ALLOW_CANCELATION;
  return pthread_mutex_lock_normal_success(mutex, word);
}

static errno_t pthread_mutex_lock_recursive(pthread_mutex_t *mutex,
                                            uint64_t word, bool is_trylock) {
  uint64_t lock;
  int backoff = 0;
  int me = atomic_load_explicit(&__get_tls()->tib_ptid, memory_order_relaxed);
  bool once = false;
  for (;;) {
    if (MUTEX_OWNER(word) == me) {
      if (MUTEX_DEPTH(word) < MUTEX_DEPTH_MAX) {
        if (atomic_compare_exchange_weak_explicit(
                &mutex->_word, &word, MUTEX_INC_DEPTH(word),
                memory_order_relaxed, memory_order_relaxed))
          return 0;
        continue;
      } else {
        return EAGAIN;
      }
    }
    if (IsModeDbg())
      __deadlock_check(mutex, 0);
    word = MUTEX_UNLOCK(word);
    lock = MUTEX_LOCK(word);
    lock = MUTEX_SET_OWNER(lock, me);
    if (atomic_compare_exchange_weak_explicit(&mutex->_word, &word, lock,
                                              memory_order_acquire,
                                              memory_order_relaxed)) {
      if (IsModeDbg()) {
        __deadlock_track(mutex, 0);
        __deadlock_record(mutex, 0);
      }
      mutex->_pid = __pid;
      return 0;
    }
    if (is_trylock)
      return EBUSY;
    if (!once) {
      LOCKTRACE("acquiring pthread_mutex_lock_recursive(%t)...", mutex);
      once = true;
    }
    for (;;) {
      word = atomic_load_explicit(&mutex->_word, memory_order_relaxed);
      if (MUTEX_OWNER(word) == me)
        break;
      if (word == MUTEX_UNLOCK(word))
        break;
      backoff = pthread_delay_np(mutex, backoff);
    }
  }
}

#if PTHREAD_USE_NSYNC
static errno_t pthread_mutex_lock_recursive_nsync(pthread_mutex_t *mutex,
                                                  uint64_t word,
                                                  bool is_trylock) {
  int me = atomic_load_explicit(&__get_tls()->tib_ptid, memory_order_relaxed);
  for (;;) {
    if (MUTEX_OWNER(word) == me) {
      if (MUTEX_DEPTH(word) < MUTEX_DEPTH_MAX) {
        if (atomic_compare_exchange_weak_explicit(
                &mutex->_word, &word, MUTEX_INC_DEPTH(word),
                memory_order_relaxed, memory_order_relaxed))
          return 0;
        continue;
      } else {
        return EAGAIN;
      }
    }
    if (IsModeDbg())
      __deadlock_check(mutex, 0);
    if (!is_trylock) {
      _weaken(nsync_mu_lock)((nsync_mu *)mutex->_nsync);
    } else {
      if (!_weaken(nsync_mu_trylock)((nsync_mu *)mutex->_nsync))
        return EBUSY;
    }
    if (IsModeDbg()) {
      __deadlock_track(mutex, 0);
      __deadlock_record(mutex, 0);
    }
    word = MUTEX_UNLOCK(word);
    word = MUTEX_LOCK(word);
    word = MUTEX_SET_OWNER(word, me);
    mutex->_word = word;
    mutex->_pid = __pid;
    return 0;
  }
}
#endif

static errno_t pthread_mutex_lock_impl(pthread_mutex_t *mutex,
                                       bool is_trylock) {
  uint64_t word = atomic_load_explicit(&mutex->_word, memory_order_relaxed);

  // handle recursive mutexes
  if (MUTEX_TYPE(word) == PTHREAD_MUTEX_RECURSIVE) {
#if PTHREAD_USE_NSYNC
    if (_weaken(nsync_mu_lock) &&
        MUTEX_PSHARED(word) == PTHREAD_PROCESS_PRIVATE) {
      return pthread_mutex_lock_recursive_nsync(mutex, word, is_trylock);
    } else {
      return pthread_mutex_lock_recursive(mutex, word, is_trylock);
    }
#else
    return pthread_mutex_lock_recursive(mutex, word, is_trylock);
#endif
  }

  // check if normal mutex is already owned by calling thread
  if (!is_trylock &&
      (MUTEX_TYPE(word) == PTHREAD_MUTEX_ERRORCHECK ||
       (IsModeDbg() && MUTEX_TYPE(word) == PTHREAD_MUTEX_DEFAULT))) {
    if (__deadlock_tracked(mutex) == 1) {
      if (IsModeDbg() && MUTEX_TYPE(word) != PTHREAD_MUTEX_ERRORCHECK) {
        kprintf("error: attempted to lock non-recursive mutex that's already "
                "held by the calling thread: %t\n",
                mutex);
        DebugBreak();
      }
      return EDEADLK;
    }
  }

  // check if locking will create cycle in lock graph
  if (IsModeDbg() || MUTEX_TYPE(word) == PTHREAD_MUTEX_ERRORCHECK)
    if (__deadlock_check(mutex, MUTEX_TYPE(word) == PTHREAD_MUTEX_ERRORCHECK))
      return EDEADLK;

#if PTHREAD_USE_NSYNC
  // use superior mutexes if possible
  if (MUTEX_PSHARED(word) == PTHREAD_PROCESS_PRIVATE &&
      _weaken(nsync_mu_lock)) {
    // on apple silicon we should just put our faith in ulock
    // otherwise *nsync gets struck down by the eye of sauron
    if (!IsXnuSilicon()) {
      if (!is_trylock) {
        _weaken(nsync_mu_lock)((nsync_mu *)mutex->_nsync);
        return pthread_mutex_lock_normal_success(mutex, word);
      } else {
        if (_weaken(nsync_mu_trylock)((nsync_mu *)mutex->_nsync))
          return pthread_mutex_lock_normal_success(mutex, word);
        return EBUSY;
      }
    }
  }
#endif

  // isc licensed non-recursive mutex implementation
  return pthread_mutex_lock_drepper(mutex, word, is_trylock);
}

/**
 * Locks mutex, e.g.
 *
 *     pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
 *     pthread_mutex_lock(&lock);
 *     // do work...
 *     pthread_mutex_unlock(&lock);
 *
 * The long way to do that is:
 *
 *     pthread_mutex_t lock;
 *     pthread_mutexattr_t attr;
 *     pthread_mutexattr_init(&attr);
 *     pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_DEFAULT);
 *     pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_PRIVATE);
 *     pthread_mutex_init(&lock, &attr);
 *     pthread_mutexattr_destroy(&attr);
 *     pthread_mutex_lock(&lock);
 *     // do work...
 *     pthread_mutex_unlock(&lock);
 *     pthread_mutex_destroy(&lock);
 *
 * The following non-POSIX initializers are also provided by cosmo libc:
 *
 * - `PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP`
 * - `PTHREAD_ERRORCHECK_MUTEX_INITIALIZER_NP`
 * - `PTHREAD_NORMAL_MUTEX_INITIALIZER_NP`
 *
 * Locking a mutex that's already locked by the calling thread will make
 * the thread hang indefinitely, i.e. it's a deadlock condition. You can
 * use `PTHREAD_MUTEX_RECURSIVE` to allow recursive locking, which could
 * result in somewhat less performance. An alternative solution is using
 * the `PTHREAD_MUTEX_ERRORCHECK` mode, which raises `EDEADLK` for that.
 *
 * If a thread locks a mutex while other mutexes are already locked then
 * you need to observe a consistent global ordering, otherwise deadlocks
 * might occur. The Cosmopolitan runtime can detect these cycles quickly
 * so you can fix your code before it becomes an issue. With error check
 * mode, an EPERM will be returned. If your app is using `cosmocc -mdbg`
 * then an error message will be printed including the demangled symbols
 * of the mutexes in the strongly connected component that was detected.
 * Please note that, even for debug builds mutexes set to explicitly use
 * the `PTHREAD_MUTEX_ERRORCHECK` mode will return an error code instead
 * which means the cosmo debug mode only influences undefined behaviors.
 *
 * Cosmopolitan only supports error checking on mutexes stored in static
 * memory, i.e. your `mutex` pointer must point inside the .data or .bss
 * sections of your executable. When compiling your programs using -mdbg
 * all your locks will gain error checking automatically. When deadlocks
 * are detected an error message will be printed and a SIGTRAP signal is
 * raised, which may be ignored to force EDEADLK and EPERM to be raised.
 *
 * Using `cosmocc -mdbg` also enhances `--strace` with information about
 * mutexes. First, locks and unlocks will be logged. Since the lock line
 * only appears after the lock is acquired, that might mean you'll never
 * get an indication about a lock that takes a very long time to acquire
 * so, whenever a lock can't immediately be acquired, a second line gets
 * printed *before* the lock is acquired to let you know that the thread
 * is waiting for a particular lock. If your mutex object resides within
 * static memory, then its demangled symbol name will be printed. If you
 * call ShowCrashReports() at the beginning of your main() function then
 * you'll also see a backtrace when a locking violation occurs. When the
 * symbols in the violation error messages show up as numbers, and it is
 * desirable to see demangled symbols without enabling full crash report
 * functionality the GetSymbolTable() function may be called for effect.
 *
 * If you use `PTHREAD_MUTEX_NORMAL`, instead of `PTHREAD_MUTEX_DEFAULT`
 * then deadlocking is actually defined behavior according to POSIX.1 so
 * the helpfulness of `cosmocc -mdbg` will be somewhat weakened.
 *
 * If your `mutex` object resides in `MAP_SHARED` memory, then undefined
 * behavior will happen unless you use `PTHREAD_PROCESS_SHARED` mode, if
 * the lock is used by multiple processes.
 *
 * This function does nothing when the process is in vfork() mode.
 *
 * @return 0 on success, or error number on failure
 * @raise EDEADLK if mutex is recursive and locked by another thread
 * @raise EDEADLK if mutex is non-recursive and locked by current thread
 * @raise EDEADLK if cycle is detected in global nested lock graph
 * @raise EAGAIN if maximum recursive locks is exceeded
 * @see pthread_spin_lock()
 * @vforksafe
 */
errno_t _pthread_mutex_lock(pthread_mutex_t *mutex) {
  if (__tls_enabled && !__vforked) {
    errno_t err = pthread_mutex_lock_impl(mutex, false);
    LOCKTRACE("pthread_mutex_lock(%t) → %s", mutex, DescribeErrno(err));
    return err;
  } else {
    LOCKTRACE("skipping pthread_mutex_lock(%t) due to runtime state", mutex);
    return 0;
  }
}

/**
 * Attempts acquiring lock.
 *
 * Unlike pthread_mutex_lock() this function won't block and instead
 * returns an error immediately if the lock couldn't be acquired.
 *
 * @return 0 if lock was acquired, otherwise an errno
 * @raise EBUSY if lock is currently held by another thread
 * @raise EAGAIN if maximum number of recursive locks is held
 * @raise EDEADLK if `mutex` is `PTHREAD_MUTEX_ERRORCHECK` and the
 *     current thread already holds this mutex
 */
errno_t _pthread_mutex_trylock(pthread_mutex_t *mutex) {
  if (__tls_enabled && !__vforked) {
    errno_t err = pthread_mutex_lock_impl(mutex, true);
    LOCKTRACE("pthread_mutex_trylock(%t) → %s", mutex, DescribeErrno(err));
    return err;
  } else {
    LOCKTRACE("skipping pthread_mutex_trylock(%t) due to runtime state", mutex);
    return 0;
  }
}

__weak_reference(_pthread_mutex_lock, pthread_mutex_lock);
__weak_reference(_pthread_mutex_trylock, pthread_mutex_trylock);
