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
#include "libc/errno.h"
#include "libc/intrin/atomic.h"
#include "libc/limits.h"
#include "libc/thread/thread.h"

/**
 * Initializes barrier.
 *
 * @param attr may be null
 * @param count is how many threads need to call pthread_barrier_wait()
 *     before the barrier is released, which must be greater than zero
 * @return 0 on success, or error number on failure
 * @raise EINVAL if `count` isn't greater than zero
 */
errno_t pthread_barrier_init(pthread_barrier_t *barrier,
                             const pthread_barrierattr_t *attr,
                             unsigned count) {
  if (!count)
    return EINVAL;
  if (count > INT_MAX)
    return EINVAL;
  barrier->_count = count;
  barrier->_pshared = attr ? *attr : PTHREAD_PROCESS_PRIVATE;
  atomic_store_explicit(&barrier->_counter, count, memory_order_relaxed);
  atomic_store_explicit(&barrier->_waiters, 0, memory_order_relaxed);
  return 0;
}
