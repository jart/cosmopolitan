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
#include "libc/calls/struct/timespec.h"
#include "libc/intrin/atomic.h"
#include "libc/testlib/testlib.h"
#include "libc/thread/thread.h"
#include "third_party/zip/zip.h"

#define THREADS    8
#define ITERATIONS 20000

atomic_int number;
pthread_t th[THREADS];
pthread_spinlock_t lock;

void *SpinWorker(void *arg) {
  for (int i = 0; i < ITERATIONS; ++i) {
    ASSERT_EQ(0, pthread_spin_lock(&lock));
    atomic_store_explicit(
        &number, atomic_load_explicit(&number, memory_order_relaxed) + 1,
        memory_order_relaxed);
    ASSERT_EQ(0, pthread_spin_unlock(&lock));
  }
  return 0;
}

TEST(pthread_spin_lock, torture) {
  int i;
  number = 0;
  ASSERT_EQ(0, pthread_spin_init(&lock, PTHREAD_PROCESS_PRIVATE));
  for (i = 0; i < THREADS; ++i) {
    ASSERT_EQ(0, pthread_create(th + i, 0, SpinWorker, 0));
  }
  for (i = 0; i < THREADS; ++i) {
    ASSERT_EQ(0, pthread_join(th[i], 0));
  }
  ASSERT_EQ(0, pthread_spin_destroy(&lock));
  ASSERT_EQ(THREADS * ITERATIONS, number);
}

TEST(pthread_spin_lock, macros) {
  ASSERT_EQ(0, pthread_spin_init(&lock, PTHREAD_PROCESS_PRIVATE));
  ASSERT_EQ(0, pthread_spin_lock(&lock));
  ASSERT_EQ(0, pthread_spin_unlock(&lock));
  ASSERT_EQ(0, pthread_spin_destroy(&lock));
}

TEST(pthread_spin_lock, linked) {
  ASSERT_EQ(0, (pthread_spin_init)(&lock, PTHREAD_PROCESS_PRIVATE));
  ASSERT_EQ(0, (pthread_spin_lock)(&lock));
  ASSERT_EQ(0, (pthread_spin_unlock)(&lock));
  ASSERT_EQ(0, (pthread_spin_destroy)(&lock));
}
