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
#include "libc/intrin/atomic.h"
#include "libc/mem/gc.h"
#include "libc/mem/mem.h"
#include "libc/stdalign.h"
#include "libc/stdio/rand.h"
#include "libc/testlib/testlib.h"
#include "libc/thread/thread.h"

#define READERS    8
#define WRITERS    2
#define ITERATIONS 1000

atomic_bool done;
alignas(128) int foo;
alignas(128) int bar;
pthread_rwlock_t lock;
pthread_rwlockattr_t attr;
pthread_barrier_t barrier;

void delay(int k) {
  int n = rand() % k;
  for (volatile int i = 0; i < n; ++i) {
  }
}

void SetUp(void) {
  ASSERT_EQ(0, pthread_rwlockattr_init(&attr));
  ASSERT_EQ(0, pthread_rwlockattr_setpshared(&attr, PTHREAD_PROCESS_SHARED));
  ASSERT_EQ(0, pthread_rwlock_init(&lock, &attr));
  ASSERT_EQ(0, pthread_rwlockattr_destroy(&attr));
}

void TearDown(void) {
  ASSERT_EQ(0, pthread_rwlock_destroy(&lock));
}

void *Reader(void *arg) {
  pthread_barrier_wait(&barrier);
  while (!atomic_load_explicit(&done, memory_order_relaxed)) {
    ASSERT_EQ(0, pthread_rwlock_rdlock(&lock));
    // cosmo_trace_begin("reader");
    int x = foo;
    usleep(1);  // delay(100000);
    int y = bar;
    ASSERT_EQ(x, y);
    // cosmo_trace_end("reader");
    ASSERT_EQ(0, pthread_rwlock_unlock(&lock));
    usleep(1);  // delay(100000);
  }
  return 0;
}

void *Writer(void *arg) {
  pthread_barrier_wait(&barrier);
  for (int i = 0; i < ITERATIONS; ++i) {
    ASSERT_EQ(0, pthread_rwlock_wrlock(&lock));
    // cosmo_trace_begin("writer");
    ++foo;
    delay(10);
    ++bar;
    // cosmo_trace_end("writer");
    ASSERT_EQ(0, pthread_rwlock_unlock(&lock));
    delay(10);
  }
  done = true;
  return 0;
}

TEST(pthread_rwlock_rdlock, test) {
  int i;
  pthread_t *t = gc(malloc(sizeof(pthread_t) * (READERS + WRITERS)));
  ASSERT_EQ(0, pthread_barrier_init(&barrier, 0, READERS + WRITERS));
  for (i = 0; i < READERS + WRITERS; ++i)
    ASSERT_SYS(0, 0,
               pthread_create(t + i, 0, i < READERS ? Reader : Writer, 0));
  for (i = 0; i < READERS + WRITERS; ++i)
    EXPECT_SYS(0, 0, pthread_join(t[i], 0));
  EXPECT_EQ(WRITERS * ITERATIONS, foo);
  EXPECT_EQ(WRITERS * ITERATIONS, bar);
  ASSERT_EQ(0, pthread_barrier_destroy(&barrier));
}
