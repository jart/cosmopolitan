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
#include <cosmo.h>
#include <errno.h>
#include <limits.h>
#include <pthread.h>
#include <stdatomic.h>

enum {
  INIT,
  IS_FINISHED,
  IN_PROGRESS,
  HAS_WAITERS,
};

static void pthread_once_wipe(void *arg) {
  if (atomic_exchange_explicit((atomic_uint *)arg, INIT,
                               memory_order_release) == HAS_WAITERS)
    cosmo_futex_wake((cosmo_futex_t *)arg, INT_MAX, PTHREAD_PROCESS_PRIVATE);
}

dontinline static errno_t pthread_once_impl(pthread_once_t *once,
                                            void init(void)) {
  unsigned word;
  for (;;)
    switch ((word = atomic_load_explicit((atomic_uint *)&once->_lock,
                                         memory_order_acquire))) {
      case IS_FINISHED:
        return 0;
      case INIT:
        if (atomic_compare_exchange_weak_explicit(
                (atomic_uint *)&once->_lock, &word, IN_PROGRESS,
                memory_order_acquire, memory_order_relaxed)) {
          pthread_cleanup_push(pthread_once_wipe, &once->_lock);
          init();
          pthread_cleanup_pop(0);
          if (atomic_exchange_explicit((atomic_uint *)&once->_lock, IS_FINISHED,
                                       memory_order_release) == HAS_WAITERS)
            cosmo_futex_wake((cosmo_futex_t *)&once->_lock, INT_MAX,
                             PTHREAD_PROCESS_PRIVATE);
          return 0;
        }
        continue;
      case IN_PROGRESS:
        if (!atomic_compare_exchange_weak_explicit(
                (atomic_uint *)&once->_lock, &word, HAS_WAITERS,
                memory_order_relaxed, memory_order_relaxed))
          continue;
        // fallthrough
      case HAS_WAITERS:
        cosmo_futex_wait((cosmo_futex_t *)&once->_lock, word,
                         PTHREAD_PROCESS_PRIVATE, CLOCK_REALTIME, 0);
        continue;
      default:
        return EINVAL;
    }
}

/**
 * Ensures initialization function is called exactly once, e.g.
 *
 *     static void *g_factory;
 *
 *     static void InitFactory(void) {
 *       g_factory = expensive();
 *     }
 *
 *     void *GetFactory(void) {
 *       static pthread_once_t once = PTHREAD_ONCE_INIT;
 *       pthread_once(&once, InitFactory);
 *       return g_factory;
 *     }
 *
 * If multiple threads try to initialize at the same time, then a single
 * one will call `init`, then other threads will block, until your first
 * thread has finished initializing.
 *
 * It's safe for your `init` callback to throw a C++ exception. It could
 * also call pthread_exit(). In both cases, the `once` object will reset
 * so that initialization can happen again without deadlocking.
 *
 * This implementation is process private. Consider cosmo_once() when it
 * is necessary to synchronize initialization across multiple processes.
 *
 * @return 0 on success, or errno on error
 */
errno_t pthread_once(pthread_once_t *once, void init(void)) {
  if (atomic_load_explicit((atomic_uint *)&once->_lock, memory_order_acquire) ==
      IS_FINISHED)
    return 0;
  return pthread_once_impl(once, init);
}

__weak_reference(pthread_once, call_once);
