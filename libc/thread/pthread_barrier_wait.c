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
#include "libc/thread/thread.h"
#include "third_party/nsync/counter.h"

/**
 * Waits for all threads to arrive at barrier.
 *
 * When the barrier is broken, the state becomes reset to what it was
 * when pthread_barrier_init() was called, so that the barrior may be
 * used again in the same way. The last thread to arrive shall be the
 * last to leave and it returns a magic value.
 *
 * @return 0 on success, `PTHREAD_BARRIER_SERIAL_THREAD` to one lucky
 *     thread which was the last arrival, or an errno on error
 */
errno_t pthread_barrier_wait(pthread_barrier_t *barrier) {
  if (nsync_counter_add(barrier->_nsync, -1)) {
    nsync_counter_wait(barrier->_nsync, nsync_time_no_deadline);
    return 0;
  } else {
    return PTHREAD_BARRIER_SERIAL_THREAD;
  }
}
