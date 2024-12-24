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
#include "libc/intrin/atomic.h"
#include "libc/intrin/strace.h"
#include "libc/thread/thread.h"

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
 * This function has undefined behavior when `spin` wasn't intialized or
 * was destroyed, and if the lock is already held by the calling thread.
 *
 * You can debug locks the acquisition of locks by building your program
 * with `cosmocc -mdbg` and passing the `--strace` flag to your program.
 * This will cause a line to be logged each time a mutex or spin lock is
 * locked or unlocked. When locking, this is printed after the lock gets
 * acquired. The entry to the lock operation will be logged too but only
 * if the lock couldn't be immediately acquired. Lock logging works best
 * when `mutex` refers to a static variable, in which case its name will
 * be printed in the log.
 *
 * @return 0 on success, or errno on error
 * @see pthread_spin_trylock
 * @see pthread_spin_unlock
 * @see pthread_spin_init
 */
errno_t pthread_spin_lock(pthread_spinlock_t *spin) {
  if (atomic_exchange_explicit(&spin->_lock, 1, memory_order_acquire)) {
    LOCKTRACE("acquiring pthread_spin_lock(%t)...", spin);
    for (;;) {
      for (;;)
        if (!atomic_load_explicit(&spin->_lock, memory_order_relaxed))
          break;
      if (!atomic_exchange_explicit(&spin->_lock, 1, memory_order_acquire))
        break;
    }
  }
  LOCKTRACE("pthread_spin_lock(%t)", spin);
  return 0;
}
