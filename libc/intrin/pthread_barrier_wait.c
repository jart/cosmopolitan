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
#include "libc/calls/calls.h"
#include "libc/dce.h"
#include "libc/intrin/atomic.h"
#include "libc/intrin/futex.internal.h"
#include "libc/intrin/intrin.h"
#include "libc/intrin/kprintf.h"
#include "libc/intrin/pthread.h"
#include "libc/limits.h"

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
int pthread_barrier_wait(pthread_barrier_t *barrier) {
  if (atomic_fetch_add(&barrier->waits, 1) + 1 == barrier->count) {
    if (atomic_fetch_add(&barrier->waits, 1) + 1 < barrier->count * 2) {
      atomic_store(&barrier->popped, 1);
      do {
        if (IsLinux() || IsOpenbsd()) {
          if (barrier->attr == PTHREAD_PROCESS_SHARED) {
            _futex_wake_public(&barrier->popped, INT_MAX);
          } else {
            _futex_wake_private(&barrier->popped, INT_MAX);
          }
        } else {
          pthread_yield();
        }
      } while (atomic_load_explicit(&barrier->waits, memory_order_relaxed) <
               barrier->count * 2);
      atomic_store_explicit(&barrier->popped, 0, memory_order_relaxed);
    }
    atomic_store_explicit(&barrier->waits, 0, memory_order_relaxed);
    return PTHREAD_BARRIER_SERIAL_THREAD;
  }
  do {
    if (IsLinux() || IsOpenbsd()) {
      if (barrier->attr == PTHREAD_PROCESS_SHARED) {
        _futex_wait_public(&barrier->popped, 0, 0);
      } else {
        _futex_wait_private(&barrier->popped, 0, 0);
      }
    } else {
      pthread_yield();
    }
  } while (atomic_load_explicit(&barrier->waits, memory_order_relaxed) <
           barrier->count);
  atomic_fetch_add(&barrier->waits, 1);
  return 0;
}
