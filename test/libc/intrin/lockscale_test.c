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
#include "libc/atomic.h"
#include "libc/calls/calls.h"
#include "libc/calls/struct/timespec.h"
#include "libc/mem/gc.h"
#include "libc/mem/mem.h"
#include "libc/stdio/stdio.h"
#include "libc/testlib/testlib.h"
#include "libc/thread/thread.h"
#include "libc/time/time.h"

/**
 * @fileoverview Lock Waiter Scalability Test
 *
 * This test can be run as follows:
 *
 *     make o//test/libc/intrin/lockscale_test.com.runs V=5 TESTARGS=-b
 *
 * It's intended to demonstrate the importance of futexes. On systems
 * that don't have them, this test consumes orders of a magnitude more
 * cpu time, because there's a lot of waiters and they need to wait a
 * very long time.
 */

#define WAITERS    100
#define ITERATIONS 10000

atomic_int x;
pthread_t t[WAITERS];
pthread_mutex_t lock;
pthread_barrier_t barrier;

void Worker(void) {
  pthread_mutex_lock(&lock);
  pthread_barrier_wait(&barrier);
  for (x = 0; x < ITERATIONS; ++x) {
    sched_yield();
  }
  pthread_mutex_unlock(&lock);
}

void *Waiter(void *arg) {
  pthread_barrier_wait(&barrier);
  pthread_mutex_lock(&lock);
  ASSERT_EQ(ITERATIONS, x);
  pthread_mutex_unlock(&lock);
  return 0;
}

BENCH(lock, scalability) {
  int i;
  struct timespec t1, t2;
  t1 = timespec_real();
  pthread_mutex_init(&lock, 0);
  pthread_barrier_init(&barrier, 0, WAITERS + 1);
  for (i = 0; i < WAITERS; ++i) {
    ASSERT_EQ(0, pthread_create(t + i, 0, Waiter, 0));
  }
  Worker();
  for (i = 0; i < WAITERS; ++i) {
    ASSERT_EQ(0, pthread_join(t[i], 0));
  }
  pthread_barrier_destroy(&barrier);
  pthread_mutex_destroy(&lock);
  t2 = timespec_real();
  printf("consumed %10g seconds real time and %10g seconds cpu time\n",
         timespec_tonanos(timespec_sub(t2, t1)) / 1e9,
         (double)clock() / CLOCKS_PER_SEC);
}
