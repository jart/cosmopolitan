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
#include "libc/thread/thread.h"

/**
 * Acquires spin lock.
 *
 * If the lock is already held, this function will wait for it to become
 * available. No genuine error conditions are currently defined. This is
 * similar to pthread_mutex_lock() except spin locks are much simpler so
 * this API is able to offer a performance advantage in situations where
 * scalable contention handling isn't necessary. Spinlocks are also very
 * small especially in MODE=tiny where a lock needs 16 bytes of code and
 * unlocking needs just 5 bytes. The lock object also only takes 1 byte.
 *
 * The posixly correct way to use this API is as follows:
 *
 *     pthread_spinlock_t lock;
 *     pthread_spin_init(&lock, PTHREAD_PROCESS_PRIVATE);
 *     pthread_spin_lock(&lock);
 *     // do work...
 *     pthread_spin_unlock(&lock);
 *     pthread_spin_destroy(&lock);
 *
 * Cosmopolitan permits succinct notation for spin locks:
 *
 *     pthread_spinlock_t lock = {0};
 *     pthread_spin_lock(&lock);
 *     // do work...
 *     pthread_spin_unlock(&lock);
 *
 * @return 0 on success, or errno on error
 * @see pthread_spin_trylock
 * @see pthread_spin_unlock
 * @see pthread_spin_init
 */
errno_t(pthread_spin_lock)(pthread_spinlock_t *spin) {
  return pthread_spin_lock(spin);
}
