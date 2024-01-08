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
#include "libc/assert.h"
#include "libc/atomic.h"
#include "libc/calls/calls.h"
#include "libc/calls/state.internal.h"
#include "libc/errno.h"
#include "libc/intrin/strace.internal.h"
#include "libc/log/check.h"
#include "libc/macros.internal.h"
#include "libc/math.h"
#include "libc/mem/gc.h"
#include "libc/mem/mem.h"
#include "libc/runtime/internal.h"
#include "libc/runtime/runtime.h"
#include "libc/runtime/stack.h"
#include "libc/sysv/consts/clone.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/prot.h"
#include "libc/sysv/consts/rlimit.h"
#include "libc/testlib/ezbench.h"
#include "libc/testlib/testlib.h"
#include "libc/thread/thread.h"
#include "libc/thread/tls.h"
#include "third_party/nsync/mu.h"

#define THREADS    8
#define ITERATIONS 512

#define MAX_RECURSIVE_LOCKS 64

int count;
atomic_int started;
atomic_int finished;
pthread_mutex_t mylock;
pthread_spinlock_t slock;
pthread_t th[THREADS];

void SetUpOnce(void) {
  ASSERT_SYS(0, 0, pledge("stdio rpath", 0));
}

TEST(pthread_mutex_lock, normal) {
  pthread_mutex_t lock;
  pthread_mutexattr_t attr;
  ASSERT_EQ(0, pthread_mutexattr_init(&attr));
  ASSERT_EQ(0, pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_NORMAL));
  ASSERT_EQ(0, pthread_mutex_init(&lock, &attr));
  ASSERT_EQ(0, pthread_mutexattr_destroy(&attr));
  ASSERT_EQ(0, pthread_mutex_init(&lock, 0));
  ASSERT_EQ(0, pthread_mutex_lock(&lock));
  ASSERT_EQ(EBUSY, pthread_mutex_trylock(&lock));
  ASSERT_EQ(0, pthread_mutex_unlock(&lock));
  ASSERT_EQ(0, pthread_mutex_trylock(&lock));
  ASSERT_EQ(0, pthread_mutex_unlock(&lock));
  ASSERT_EQ(0, pthread_mutex_lock(&lock));
  ASSERT_EQ(0, pthread_mutex_unlock(&lock));
  ASSERT_EQ(0, pthread_mutex_destroy(&lock));
}

TEST(pthread_mutex_lock, recursive) {
  pthread_mutex_t lock;
  pthread_mutexattr_t attr;
  ASSERT_EQ(0, pthread_mutexattr_init(&attr));
  ASSERT_EQ(0, pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE));
  ASSERT_EQ(0, pthread_mutex_init(&lock, &attr));
  ASSERT_EQ(0, pthread_mutexattr_destroy(&attr));
  for (int i = 0; i < MAX_RECURSIVE_LOCKS; ++i) {
    if (i & 1) {
      ASSERT_EQ(0, pthread_mutex_lock(&lock));
    } else {
      ASSERT_EQ(0, pthread_mutex_trylock(&lock));
    }
  }
  ASSERT_EQ(EAGAIN, pthread_mutex_lock(&lock));
  ASSERT_EQ(EAGAIN, pthread_mutex_trylock(&lock));
  for (int i = 0; i < MAX_RECURSIVE_LOCKS; ++i) {
    ASSERT_EQ(0, pthread_mutex_unlock(&lock));
  }
  ASSERT_EQ(0, pthread_mutex_lock(&lock));
  ASSERT_EQ(0, pthread_mutex_unlock(&lock));
  ASSERT_EQ(0, pthread_mutex_unlock(&lock));
  ASSERT_EQ(0, pthread_mutex_destroy(&lock));
}

TEST(pthread_mutex_lock, errorcheck) {
  pthread_mutex_t lock;
  pthread_mutexattr_t attr;
  ASSERT_EQ(0, pthread_mutexattr_init(&attr));
  ASSERT_EQ(0, pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ERRORCHECK));
  ASSERT_EQ(0, pthread_mutex_init(&lock, &attr));
  ASSERT_EQ(0, pthread_mutexattr_destroy(&attr));
  ASSERT_EQ(0, pthread_mutex_lock(&lock));
  ASSERT_EQ(EDEADLK, pthread_mutex_lock(&lock));
  ASSERT_EQ(EDEADLK, pthread_mutex_trylock(&lock));
  ASSERT_EQ(0, pthread_mutex_unlock(&lock));
  ASSERT_EQ(0, pthread_mutex_destroy(&lock));
}

void *MutexWorker(void *p) {
  int i;
  ++started;
  for (i = 0; i < ITERATIONS; ++i) {
    if (pthread_mutex_lock(&mylock)) {
      ASSERT_EQ(0, pthread_mutex_lock(&mylock));
    }
    ++count;
    ASSERT_EQ(0, pthread_mutex_unlock(&mylock));
  }
  ++finished;
  return 0;
}

TEST(pthread_mutex_lock, contention) {
  int i;
  pthread_mutexattr_t attr;
  pthread_mutexattr_init(&attr);
  pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_NORMAL);
  pthread_mutex_init(&mylock, &attr);
  pthread_mutexattr_destroy(&attr);
  count = 0;
  started = 0;
  finished = 0;
  for (i = 0; i < THREADS; ++i) {
    ASSERT_SYS(0, 0,
               pthread_create(th + i, 0, MutexWorker, (void *)(intptr_t)i));
  }
  for (i = 0; i < THREADS; ++i) {
    ASSERT_SYS(0, 0, pthread_join(th[i], 0));
  }
  EXPECT_EQ(THREADS, started);
  EXPECT_EQ(THREADS, finished);
  EXPECT_EQ(THREADS * ITERATIONS, count);
  EXPECT_EQ(0, pthread_mutex_destroy(&mylock));
}

TEST(pthread_mutex_lock, rcontention) {
  int i;
  pthread_mutexattr_t attr;
  pthread_mutexattr_init(&attr);
  pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
  pthread_mutex_init(&mylock, &attr);
  pthread_mutexattr_destroy(&attr);
  count = 0;
  started = 0;
  finished = 0;
  for (i = 0; i < THREADS; ++i) {
    ASSERT_EQ(0, pthread_create(th + i, 0, MutexWorker, (void *)(intptr_t)i));
  }
  for (i = 0; i < THREADS; ++i) {
    ASSERT_EQ(0, pthread_join(th[i], 0));
  }
  EXPECT_EQ(THREADS, started);
  EXPECT_EQ(THREADS, finished);
  EXPECT_EQ(THREADS * ITERATIONS, count);
  EXPECT_EQ(0, pthread_mutex_destroy(&mylock));
}

TEST(pthread_mutex_lock, econtention) {
  int i;
  pthread_mutexattr_t attr;
  pthread_mutexattr_init(&attr);
  pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ERRORCHECK);
  pthread_mutex_init(&mylock, &attr);
  pthread_mutexattr_destroy(&attr);
  count = 0;
  started = 0;
  finished = 0;
  for (i = 0; i < THREADS; ++i) {
    ASSERT_NE(-1, pthread_create(th + i, 0, MutexWorker, (void *)(intptr_t)i));
  }
  for (i = 0; i < THREADS; ++i) {
    pthread_join(th[i], 0);
  }
  EXPECT_EQ(THREADS, started);
  EXPECT_EQ(THREADS, finished);
  EXPECT_EQ(THREADS * ITERATIONS, count);
  EXPECT_EQ(0, pthread_mutex_destroy(&mylock));
}

void *SpinlockWorker(void *p) {
  int i;
  ++started;
  for (i = 0; i < ITERATIONS; ++i) {
    pthread_spin_lock(&slock);
    ++count;
    pthread_spin_unlock(&slock);
  }
  ++finished;
  STRACE("SpinlockWorker Finished %d", gettid());
  return 0;
}

TEST(pthread_spin_lock, test) {
  int i;
  count = 0;
  started = 0;
  finished = 0;
  EXPECT_EQ(0, pthread_spin_init(&slock, 0));
  EXPECT_EQ(0, pthread_spin_trylock(&slock));
  EXPECT_EQ(EBUSY, pthread_spin_trylock(&slock));
  EXPECT_EQ(0, pthread_spin_unlock(&slock));
  EXPECT_EQ(0, pthread_spin_lock(&slock));
  EXPECT_EQ(EBUSY, pthread_spin_trylock(&slock));
  EXPECT_EQ(0, pthread_spin_unlock(&slock));
  for (i = 0; i < THREADS; ++i) {
    ASSERT_EQ(0,
              pthread_create(th + i, 0, SpinlockWorker, (void *)(intptr_t)i));
  }
  for (i = 0; i < THREADS; ++i) {
    ASSERT_EQ(0, pthread_join(th[i], 0));
  }
  EXPECT_EQ(THREADS, started);
  EXPECT_EQ(THREADS, finished);
  EXPECT_EQ(THREADS * ITERATIONS, count);
  EXPECT_EQ(0, pthread_spin_destroy(&slock));
}
