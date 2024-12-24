/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2024 Justine Alexandra Roberts Tunney                              │
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
#include "libc/cosmo.h"
#include "libc/dce.h"
#include "libc/intrin/atomic.h"
#include "libc/thread/lock.h"
#include "libc/thread/thread.h"

/**
 * Recovers mutex whose owner died.
 *
 * @return 0 on success, or errno on error
 */
int pthread_mutex_consistent(pthread_mutex_t *mutex) {

  // The POSIX concept of robust mutexes is a bit cray. So let's change
  // things up a bit. Rather than implementing all those goofy behaviors
  // we shall simply use this function to weasel around the ownership
  // check in pthread_mutex_unlock().
  uint64_t word = atomic_load_explicit(&mutex->_word, memory_order_relaxed);
  if (IsModeDbg() || MUTEX_TYPE(word) == PTHREAD_MUTEX_ERRORCHECK)
    __deadlock_track(mutex, 0);

  return 0;
}
