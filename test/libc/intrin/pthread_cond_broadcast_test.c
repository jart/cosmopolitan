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
#include "libc/calls/struct/timespec.h"
#include "libc/errno.h"
#include "libc/intrin/atomic.h"
#include "libc/intrin/kprintf.h"
#include "libc/intrin/pthread.h"
#include "libc/intrin/pthread2.h"
#include "libc/mem/mem.h"
#include "libc/runtime/gc.internal.h"
#include "libc/testlib/testlib.h"
#include "libc/thread/spawn.h"

_Atomic(int) bReady;
pthread_cond_t bCond;
pthread_mutex_t bMutex;

int BroadcastWorker(void *arg, int tid) {
  ASSERT_EQ(0, pthread_mutex_lock(&bMutex));
  atomic_fetch_add(&bReady, 1);
  ASSERT_EQ(0, pthread_cond_wait(&bCond, &bMutex));
  ASSERT_EQ(0, pthread_mutex_unlock(&bMutex));
  return 0;
}

TEST(pthread_cond_broadcast, test) {
  pthread_condattr_t cAttr;
  pthread_mutexattr_t mAttr;
  ASSERT_EQ(0, pthread_mutexattr_init(&mAttr));
  ASSERT_EQ(0, pthread_mutexattr_settype(&mAttr, PTHREAD_MUTEX_ERRORCHECK));
  ASSERT_EQ(0, pthread_mutex_init(&bMutex, &mAttr));
  ASSERT_EQ(0, pthread_condattr_init(&cAttr));
  ASSERT_EQ(0, pthread_condattr_setpshared(&cAttr, PTHREAD_PROCESS_PRIVATE));
  ASSERT_EQ(0, pthread_cond_init(&bCond, &cAttr));
  int i, n = 16;
  struct spawn *t = gc(malloc(sizeof(struct spawn) * n));
  for (i = 0; i < n; ++i) {
    ASSERT_SYS(0, 0, _spawn(BroadcastWorker, 0, t + i));
  }
  for (;;) {
    if (atomic_load_explicit(&bReady, memory_order_relaxed) == n) {
      break;
    } else {
      pthread_yield();
    }
  }
  ASSERT_EQ(0, pthread_mutex_lock(&bMutex));
  ASSERT_EQ(0, pthread_cond_broadcast(&bCond));
  ASSERT_EQ(0, pthread_mutex_unlock(&bMutex));
  for (i = 0; i < n; ++i) {
    EXPECT_SYS(0, 0, _join(t + i));
  }
  ASSERT_EQ(0, pthread_mutex_destroy(&bMutex));
  ASSERT_EQ(0, pthread_cond_destroy(&bCond));
}

TEST(pthread_cond_timedwait, testTimeoutInPast_timesOutImmediately) {
  struct timespec t = {100000};
  pthread_cond_t c = PTHREAD_COND_INITIALIZER;
  pthread_mutex_t m = {PTHREAD_MUTEX_ERRORCHECK};
  ASSERT_EQ(0, pthread_mutex_lock(&m));
  ASSERT_EQ(ETIMEDOUT, pthread_cond_timedwait(&c, &m, &t));
  ASSERT_EQ(0, pthread_mutex_unlock(&m));
}

_Atomic(int) tReady;
pthread_cond_t tCond;
pthread_mutex_t tMutex;

int TimedWorker(void *arg, int tid) {
  struct timespec ts;
  ASSERT_EQ(0, pthread_mutex_lock(&tMutex));
  atomic_fetch_add(&tReady, 1);
  ts = _timespec_add(_timespec_mono(), _timespec_frommillis(30000));
  ASSERT_EQ(0, pthread_cond_timedwait(&tCond, &tMutex, &ts));
  ASSERT_EQ(0, pthread_mutex_unlock(&tMutex));
  return 0;
}

TEST(pthread_cond_timedwait, testThirtySeconds_doesntTimeOut) {
  pthread_condattr_t cAttr;
  pthread_mutexattr_t mAttr;
  ASSERT_EQ(0, pthread_mutexattr_init(&mAttr));
  ASSERT_EQ(0, pthread_mutexattr_settype(&mAttr, PTHREAD_MUTEX_ERRORCHECK));
  ASSERT_EQ(0, pthread_mutex_init(&tMutex, &mAttr));
  ASSERT_EQ(0, pthread_condattr_init(&cAttr));
  ASSERT_EQ(0, pthread_condattr_setpshared(&cAttr, PTHREAD_PROCESS_PRIVATE));
  ASSERT_EQ(0, pthread_cond_init(&tCond, &cAttr));
  struct spawn t;
  ASSERT_SYS(0, 0, _spawn(TimedWorker, 0, &t));
  for (;;) {
    if (atomic_load_explicit(&tReady, memory_order_relaxed)) {
      break;
    } else {
      pthread_yield();
    }
  }
  ASSERT_EQ(0, pthread_mutex_lock(&tMutex));
  ASSERT_EQ(0, pthread_cond_signal(&tCond));
  ASSERT_EQ(0, pthread_mutex_unlock(&tMutex));
  EXPECT_SYS(0, 0, _join(&t));
  ASSERT_EQ(0, pthread_mutex_destroy(&tMutex));
  ASSERT_EQ(0, pthread_cond_destroy(&tCond));
}
