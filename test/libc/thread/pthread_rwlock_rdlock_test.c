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
#include "libc/mem/gc.h"
#include "libc/mem/mem.h"
#include "libc/testlib/testlib.h"
#include "libc/thread/thread.h"

#define READERS           8
#define WRITERS           2
#define READER_ITERATIONS 10000
#define WRITER_ITERATIONS 1000

int writes;
atomic_int reads;
pthread_rwlock_t lock;
pthread_rwlockattr_t attr;
pthread_barrier_t barrier;

FIXTURE(pthread_rwlock, private) {
  reads = 0;
  writes = 0;
  ASSERT_EQ(0, pthread_rwlockattr_init(&attr));
  ASSERT_EQ(0, pthread_rwlockattr_setpshared(&attr, PTHREAD_PROCESS_PRIVATE));
  ASSERT_EQ(0, pthread_rwlock_init(&lock, &attr));
  ASSERT_EQ(0, pthread_rwlockattr_destroy(&attr));
}

FIXTURE(pthread_rwlock, pshared) {
  reads = 0;
  writes = 0;
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
  for (int i = 0; i < READER_ITERATIONS; ++i) {
    ASSERT_EQ(0, pthread_rwlock_rdlock(&lock));
    ++reads;
    ASSERT_EQ(0, pthread_rwlock_unlock(&lock));
  }
  return 0;
}

void *Writer(void *arg) {
  pthread_barrier_wait(&barrier);
  for (int i = 0; i < WRITER_ITERATIONS; ++i) {
    ASSERT_EQ(0, pthread_rwlock_wrlock(&lock));
    ++writes;
    ASSERT_EQ(0, pthread_rwlock_unlock(&lock));
    for (volatile int i = 0; i < 100; ++i)
      pthread_pause_np();
  }
  return 0;
}

TEST(pthread_rwlock_rdlock, test) {
  int i;
  pthread_t *t = gc(malloc(sizeof(pthread_t) * (READERS + WRITERS)));
  ASSERT_EQ(0, pthread_barrier_init(&barrier, 0, READERS + WRITERS));
  for (i = 0; i < READERS + WRITERS; ++i) {
    ASSERT_SYS(0, 0,
               pthread_create(t + i, 0, i < READERS ? Reader : Writer, 0));
  }
  for (i = 0; i < READERS + WRITERS; ++i) {
    EXPECT_SYS(0, 0, pthread_join(t[i], 0));
  }
  EXPECT_EQ(READERS * READER_ITERATIONS, reads);
  EXPECT_EQ(WRITERS * WRITER_ITERATIONS, writes);
  ASSERT_EQ(0, pthread_barrier_destroy(&barrier));
}
