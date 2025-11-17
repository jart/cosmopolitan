/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2025 Justine Alexandra Roberts Tunney                              │
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
#include "libc/bsdstdlib.h"
#include "libc/calls/calls.h"
#include "libc/calls/struct/cpuset.h"
#include "libc/calls/struct/timespec.h"
#include "libc/calls/syscall-sysv.internal.h"
#include "libc/cosmotime.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/macros.h"
#include "libc/mem/alg.h"
#include "libc/mem/mem.h"
#include "libc/nt/runtime.h"
#include "libc/runtime/runtime.h"
#include "libc/runtime/sysconf.h"
#include "libc/stdio/rand.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/clock.h"
#include "libc/testlib/testlib.h"
#include "libc/thread/thread.h"
#include "libc/thread/thread2.h"

#define MAX_THREADS 4

pthread_barrier_t barrier;

uint64_t arc4random64(void) {
  uint64_t val;
  arc4random_buf(&val, sizeof(val));
  return val;
}

uint64_t getrandom64(void) {
  uint64_t val;
  unassert(getrandom(&val, sizeof(val), 0) == sizeof(val));
  return val;
}

uint64_t getentropy64(void) {
  uint64_t val;
  unassert(!getentropy(&val, sizeof(val)));
  return val;
}

struct Worker {
  pthread_t th;
  long tid;
  long threads;
  long length;
  int64_t *numbers;
  uint64_t (*rng)(void);
};

void *worker(void *arg) {
  struct Worker *w = (struct Worker *)arg;
  pthread_barrier_wait(&barrier);
  long n = w->length / w->threads;
  for (long i = 0; i < n; ++i)
    w->numbers[w->tid * n + i] = w->rng();
  return 0;
}

void DemonstrateArc4randomThreadSafety(long threads, long length,
                                       uint64_t rng(void), const char *name) {
  ASSERT_EQ(0, length % threads);
  int64_t *numbers = calloc(length, sizeof(int64_t));
  EXPECT_EQ(0, pthread_barrier_init(&barrier, 0, threads));
  struct timespec start, end;
  struct Worker *workers = calloc(threads, sizeof(struct Worker));
  clock_gettime(CLOCK_REALTIME, &start);
  if (threads == 1) {
    // once a thread is created, arc4random64() needs to start using its
    // mutex (since __isthreaded is true) and it will go a little slower
    workers[0].tid = 0;
    workers[0].threads = threads;
    workers[0].length = length;
    workers[0].numbers = numbers;
    workers[0].rng = rng;
    worker(&workers[0]);
  } else {
    for (long i = 0; i < threads; ++i) {
      workers[i].tid = i;
      workers[i].threads = threads;
      workers[i].length = length;
      workers[i].numbers = numbers;
      workers[i].rng = rng;
      EXPECT_EQ(0, pthread_create(&workers[i].th, 0, worker, &workers[i]));
    }
    for (long i = 0; i < threads; ++i)
      EXPECT_EQ(0, pthread_join(workers[i].th, 0));
  }
  clock_gettime(CLOCK_REALTIME, &end);
  printf("%-15s took %4ld ns across %3ld threads on %s\n", name,
         timespec_tonanos(timespec_sub(end, start)) / length, threads,
         __describe_os());
  EXPECT_EQ(0, pthread_barrier_destroy(&barrier));
  ASSERT_EQ(0, radix_sort_int64(numbers, length));
  for (long i = 1; i < length; ++i)
    ASSERT_NE(numbers[i - 1], numbers[i]);
  free(workers);
  free(numbers);
}

void DemonstrateArc4randomThreadSafetyAndScalability(uint64_t rng(void),
                                                     const char *name) {
#define DemonstrateArc4randomThreadSafetyAndScalability(rng) \
  DemonstrateArc4randomThreadSafetyAndScalability(rng, #rng "()")
  long length = 64 * 1024;
  long cpus = sysconf(_SC_NPROCESSORS_ONLN);
  if (cpus > MAX_THREADS)
    cpus = MAX_THREADS;
  printf("\n");
  for (long i = 1;; i <<= 1) {
    long threads = i;
    if (threads >= cpus) {
      threads = cpus;
      DemonstrateArc4randomThreadSafety(threads, length / threads * threads,
                                        rng, name);
      break;
    }
    DemonstrateArc4randomThreadSafety(threads, length / threads * threads, rng,
                                      name);
  }
}

TEST(arc4random, threadSafetyAndScalability) {
  __isthreaded = 1;
  DemonstrateArc4randomThreadSafetyAndScalability(_rand64);
  __isthreaded = 1;
  DemonstrateArc4randomThreadSafetyAndScalability(arc4random64);
  __isthreaded = 1;
  DemonstrateArc4randomThreadSafetyAndScalability(getrandom64);
  __isthreaded = 1;
  DemonstrateArc4randomThreadSafetyAndScalability(getentropy64);
}

#define THREADS    4
#define ITERATIONS 1000

int64_t numbers[THREADS * ITERATIONS];
int64_t numbers2[THREADS * ITERATIONS];

void *worker2(void *arg) {
  long id = (long)arg;
  pthread_barrier_wait(&barrier);
  for (long i = 0; i < ITERATIONS; ++i)
    numbers[id * ITERATIONS + i] = arc4random64();
  return 0;
}

TEST(arc4random64, threadSafety) {
  pthread_barrier_init(&barrier, 0, THREADS);
  pthread_t th[THREADS];
  for (long i = 0; i < THREADS; ++i)
    ASSERT_EQ(0, pthread_create(&th[i], 0, worker2, (void *)i));
  for (long i = 0; i < THREADS; ++i)
    ASSERT_EQ(0, pthread_join(th[i], 0));
  pthread_barrier_destroy(&barrier);
  radix_sort_int64(numbers, ARRAYLEN(numbers));
  for (long i = 1; i < ARRAYLEN(numbers); ++i)
    ASSERT_NE(numbers[i - 1], numbers[i]);
}

TEST(arc4random64, reseedSafety) {
  arc4random64_threadSafety();
  memcpy(numbers2, numbers, sizeof(numbers));
  long v1 = arc4random64();
  long *v2 = _mapshared(8);
  if (!fork()) {
    *v2 = arc4random64();
    _Exit(0);
  }
  int ws;
  ASSERT_NE(-1, wait(&ws));
  ASSERT_EQ(0, ws);
  ASSERT_NE(v1, *v2);
  ASSERT_NE(arc4random64(), *v2);
}
