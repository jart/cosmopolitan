/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2023 Justine Alexandra Roberts Tunney                              │
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
#include "libc/dce.h"
#include "libc/runtime/runtime.h"
#include "libc/testlib/subprocess.h"
#include "libc/testlib/testlib.h"
#include "libc/thread/thread.h"

// create a bunch of threads with nsync semaphore waiter objects
// then fork and verify that unlocking in child safely cleans up
// this test matters on netbsd, where waiters have a file number

#define N 10

pthread_t thread[N];
atomic_uint countdown = N;
atomic_uint final_countdown = N;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

void *WaitWorker(void *arg) {
  ASSERT_EQ(0, pthread_mutex_lock(&lock));
  --countdown;
  ASSERT_EQ(0, pthread_cond_wait(&cond, &lock));
  ASSERT_EQ(0, pthread_mutex_unlock(&lock));
  ASSERT_EQ(0, pthread_mutex_lock(&lock));
  --final_countdown;
  ASSERT_EQ(0, pthread_cond_wait(&cond, &lock));
  ASSERT_EQ(0, pthread_mutex_unlock(&lock));
  return 0;
}

TEST(mu_semaphore_sem, test) {
  for (int i = 0; i < N; ++i) {
    ASSERT_EQ(0, pthread_create(thread + i, 0, WaitWorker, 0));
  }
  while (countdown) pthread_yield();
  SPAWN(fork);
  CheckForFileLeaks();
  EXITS(0);
  ASSERT_EQ(0, pthread_mutex_lock(&lock));
  ASSERT_EQ(0, pthread_cond_broadcast(&cond));
  ASSERT_EQ(0, pthread_mutex_unlock(&lock));
  while (final_countdown) pthread_yield();  // make extra sure still works
  ASSERT_EQ(0, pthread_mutex_lock(&lock));
  ASSERT_EQ(0, pthread_cond_broadcast(&cond));
  ASSERT_EQ(0, pthread_mutex_unlock(&lock));
  for (int i = 0; i < N; ++i) {
    ASSERT_EQ(0, pthread_join(thread[i], 0));
  }
  CheckForFileLeaks();
}
