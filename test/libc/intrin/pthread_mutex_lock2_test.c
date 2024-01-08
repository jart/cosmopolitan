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
#define EZBENCH_COUNT 10000
#include "libc/atomic.h"
#include "libc/calls/calls.h"
#include "libc/calls/struct/timespec.h"
#include "libc/intrin/atomic.h"
#include "libc/mem/gc.h"
#include "libc/mem/mem.h"
#include "libc/testlib/ezbench.h"
#include "libc/testlib/testlib.h"
#include "libc/thread/posixthread.internal.h"
#include "libc/thread/thread.h"
#include "third_party/nsync/mu.h"

int THREADS = 16;
int ITERATIONS = 100;

int count;
atomic_int started;
atomic_int finished;
pthread_mutex_t lock;
pthread_mutexattr_t attr;

FIXTURE(pthread_mutex_lock, normal) {
  ASSERT_EQ(0, pthread_mutexattr_init(&attr));
  ASSERT_EQ(0, pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_NORMAL));
  ASSERT_EQ(0, pthread_mutex_init(&lock, &attr));
  ASSERT_EQ(0, pthread_mutexattr_destroy(&attr));
}

FIXTURE(pthread_mutex_lock, recursive) {
  ASSERT_EQ(0, pthread_mutexattr_init(&attr));
  ASSERT_EQ(0, pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE));
  ASSERT_EQ(0, pthread_mutex_init(&lock, &attr));
  ASSERT_EQ(0, pthread_mutexattr_destroy(&attr));
}

FIXTURE(pthread_mutex_lock, errorcheck) {
  ASSERT_EQ(0, pthread_mutexattr_init(&attr));
  ASSERT_EQ(0, pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ERRORCHECK));
  ASSERT_EQ(0, pthread_mutex_init(&lock, &attr));
  ASSERT_EQ(0, pthread_mutexattr_destroy(&attr));
}

////////////////////////////////////////////////////////////////////////////////
// TESTS

void *MutexWorker(void *p) {
  int i;
  ++started;
  for (i = 0; i < ITERATIONS; ++i) {
    pthread_mutex_lock(&lock);
    ++count;
    sched_yield();
    pthread_mutex_unlock(&lock);
  }
  ++finished;
  return 0;
}

TEST(pthread_mutex_lock, contention) {
  int i;
  pthread_t *th = gc(malloc(sizeof(pthread_t) * THREADS));
  pthread_mutexattr_init(&attr);
  pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_NORMAL);
  pthread_mutex_init(&lock, &attr);
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
  EXPECT_EQ(0, pthread_mutex_destroy(&lock));
}

////////////////////////////////////////////////////////////////////////////////
// BENCHMARKS

void BenchSpinUnspin(pthread_spinlock_t *s) {
  pthread_spin_lock(s);
  pthread_spin_unlock(s);
}

void BenchLockUnlock(pthread_mutex_t *m) {
  pthread_mutex_lock(m);
  pthread_mutex_unlock(m);
}

void BenchLockUnlockNsync(nsync_mu *m) {
  nsync_mu_lock(m);
  nsync_mu_unlock(m);
}

BENCH(pthread_mutex_lock, bench_uncontended) {
  {
    pthread_spinlock_t s = {0};
    EZBENCH2("spin 1x", donothing, BenchSpinUnspin(&s));
  }
  {
    nsync_mu m = {0};
    EZBENCH2("nsync 1x", donothing, BenchLockUnlockNsync(&m));
  }
  {
    pthread_mutex_t m;
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_NORMAL);
    pthread_mutex_init(&m, &attr);
    EZBENCH2("normal 1x", donothing, BenchLockUnlock(&m));
  }
  {
    pthread_mutex_t m;
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(&m, &attr);
    EZBENCH2("recursive 1x", donothing, BenchLockUnlock(&m));
  }
  {
    pthread_mutex_t m;
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ERRORCHECK);
    pthread_mutex_init(&m, &attr);
    EZBENCH2("errorcheck 1x", donothing, BenchLockUnlock(&m));
  }
}

struct SpinContentionArgs {
  pthread_spinlock_t *spin;
  atomic_char done;
  atomic_char ready;
};

void *SpinContentionWorker(void *arg) {
  struct SpinContentionArgs *a = arg;
  while (!atomic_load_explicit(&a->done, memory_order_relaxed)) {
    pthread_spin_lock(a->spin);
    atomic_store_explicit(&a->ready, 1, memory_order_relaxed);
    pthread_spin_unlock(a->spin);
  }
  return 0;
}

struct MutexContentionArgs {
  pthread_mutex_t *mutex;
  atomic_char done;
  atomic_char ready;
};

void *MutexContentionWorker(void *arg) {
  struct MutexContentionArgs *a = arg;
  while (!atomic_load_explicit(&a->done, memory_order_relaxed)) {
    if (pthread_mutex_lock(a->mutex)) notpossible;
    atomic_store_explicit(&a->ready, 1, memory_order_relaxed);
    if (pthread_mutex_unlock(a->mutex)) notpossible;
  }
  return 0;
}

struct NsyncContentionArgs {
  nsync_mu *nsync;
  atomic_char done;
  atomic_char ready;
};

void *NsyncContentionWorker(void *arg) {
  struct NsyncContentionArgs *a = arg;
  while (!atomic_load_explicit(&a->done, memory_order_relaxed)) {
    nsync_mu_lock(a->nsync);
    atomic_store_explicit(&a->ready, 1, memory_order_relaxed);
    nsync_mu_unlock(a->nsync);
  }
  return 0;
}

BENCH(pthread_mutex_lock, bench_contended) {
  pthread_t t;
  {
    pthread_spinlock_t s = {0};
    struct SpinContentionArgs a = {&s};
    pthread_create(&t, 0, SpinContentionWorker, &a);
    while (!a.ready) sched_yield();
    EZBENCH2("spin 2x", donothing, BenchSpinUnspin(&s));
    a.done = true;
    pthread_join(t, 0);
  }
  {
    nsync_mu m = {0};
    struct NsyncContentionArgs a = {&m};
    pthread_create(&t, 0, NsyncContentionWorker, &a);
    while (!a.ready) sched_yield();
    EZBENCH2("nsync 2x", donothing, BenchLockUnlockNsync(&m));
    a.done = true;
    pthread_join(t, 0);
  }
  {
    pthread_mutex_t m;
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_NORMAL);
    pthread_mutex_init(&m, &attr);
    struct MutexContentionArgs a = {&m};
    pthread_create(&t, 0, MutexContentionWorker, &a);
    while (!a.ready) sched_yield();
    EZBENCH2("normal 2x", donothing, BenchLockUnlock(&m));
    a.done = true;
    pthread_join(t, 0);
  }
  {
    pthread_mutex_t m;
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(&m, &attr);
    struct MutexContentionArgs a = {&m};
    pthread_create(&t, 0, MutexContentionWorker, &a);
    while (!a.ready) sched_yield();
    EZBENCH2("recursive 2x", donothing, BenchLockUnlock(&m));
    a.done = true;
    pthread_join(t, 0);
  }
  {
    pthread_mutex_t m;
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ERRORCHECK);
    pthread_mutex_init(&m, &attr);
    struct MutexContentionArgs a = {&m};
    pthread_create(&t, 0, MutexContentionWorker, &a);
    while (!a.ready) sched_yield();
    EZBENCH2("errorcheck 2x", donothing, BenchLockUnlock(&m));
    a.done = true;
    pthread_join(t, 0);
  }
}
