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
#include "libc/assert.h"
#include "libc/calls/calls.h"
#include "libc/errno.h"
#include "libc/intrin/atomic.h"
#include "libc/intrin/futex.internal.h"
#include "libc/thread/thread.h"

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
 * @return 0 on success, or error number on failure
 * @see pthread_spin_lock()
 */
int pthread_mutex_lock(pthread_mutex_t *mutex) {
  int c, d, t;

  if (mutex->type == PTHREAD_MUTEX_NORMAL) {
    // From Futexes Are Tricky Version 1.1 § Mutex, Take 3;
    // Ulrich Drepper, Red Hat Incorporated, June 27, 2004.
    c = 0;
    if (!atomic_compare_exchange_strong_explicit(
            &mutex->lock, &c, 1, memory_order_acquire, memory_order_relaxed)) {
      if (c != 2) {
        c = atomic_exchange_explicit(&mutex->lock, 2, memory_order_acquire);
      }
      while (c) {
        _futex_wait(&mutex->lock, 2, mutex->pshared, 0);
        c = atomic_exchange_explicit(&mutex->lock, 2, memory_order_acquire);
      }
    }
    return 0;
  }

  t = gettid();
  if (mutex->type == PTHREAD_MUTEX_ERRORCHECK) {
    c = atomic_load_explicit(&mutex->lock, memory_order_relaxed);
    if ((c & 0x000fffff) == t) {
      assert(!"deadlock");
      return EDEADLK;
    }
  }

  for (;;) {
    c = 0;
    d = 0x10100000 | t;
    if (atomic_compare_exchange_weak_explicit(
            &mutex->lock, &c, d, memory_order_acquire, memory_order_relaxed)) {
      break;
    } else {
      if ((c & 0x000fffff) == t) {
        if ((c & 0x0ff00000) < 0x0ff00000) {
          c = atomic_fetch_add_explicit(&mutex->lock, 0x00100000,
                                        memory_order_relaxed);
          break;
        } else {
          assert(!"recurse");
          return EAGAIN;
        }
      }
      if ((c & 0xf0000000) == 0x10000000) {
        d = 0x20000000 | c;
        if (atomic_compare_exchange_weak_explicit(&mutex->lock, &c, d,
                                                  memory_order_acquire,
                                                  memory_order_relaxed)) {
          c = d;
        }
      }
      if ((c & 0xf0000000) == 0x20000000) {
        _futex_wait(&mutex->lock, c, mutex->pshared, 0);
      }
    }
  }

  return 0;
}
