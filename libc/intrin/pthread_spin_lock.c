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
#include "libc/intrin/atomic.h"
#include "libc/intrin/strace.internal.h"
#include "libc/thread/thread.h"

#ifdef pthread_spin_lock
#undef pthread_spin_lock
#endif

/**
 * Acquires spin lock, e.g.
 *
 *     pthread_spinlock_t lock;
 *     pthread_spin_init(&lock, PTHREAD_PROCESS_PRIVATE);
 *     pthread_spin_lock(&lock);
 *     // do work...
 *     pthread_spin_unlock(&lock);
 *     pthread_spin_destroy(&lock);
 *
 * This function has undefined behavior when `spin` wasn't intialized,
 * was destroyed, or if the lock's already held by the calling thread.
 *
 * @return 0 on success, or errno on error
 * @see pthread_spin_trylock
 * @see pthread_spin_unlock
 * @see pthread_spin_init
 */
errno_t pthread_spin_lock(pthread_spinlock_t *spin) {
  int x;
#if defined(SYSDEBUG) && _LOCKTRACE
  for (;;) {
    x = atomic_exchange_explicit(&spin->_lock, 1, memory_order_acquire);
    if (!x) {
      LOCKTRACE("pthread_spin_lock(%t)", spin);
      break;
    }
    _unassert(x == 1);
    LOCKTRACE("pthread_spin_lock(%t) trying...", spin);
  }
#else
  for (;;) {
    x = atomic_exchange_explicit(&spin->_lock, 1, memory_order_acquire);
    if (!x) break;
    _unassert(x == 1);
  }
#endif
  return 0;
}
