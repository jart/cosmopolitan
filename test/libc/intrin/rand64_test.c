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
#include "libc/calls/calls.h"
#include "libc/macros.h"
#include "libc/mem/alg.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/rand.h"
#include "libc/str/str.h"
#include "libc/testlib/testlib.h"
#include "libc/thread/thread.h"

#define THREADS    4
#define ITERATIONS 1000

pthread_barrier_t barrier;
int64_t numbers[THREADS * ITERATIONS];
int64_t numbers2[THREADS * ITERATIONS];

void *worker(void *arg) {
  long id = (long)arg;
  pthread_barrier_wait(&barrier);
  for (long i = 0; i < ITERATIONS; ++i)
    numbers[id * ITERATIONS + i] = _rand64();
  return 0;
}

TEST(_rand64, threadSafety) {
  pthread_barrier_init(&barrier, 0, THREADS);
  pthread_t th[THREADS];
  for (long i = 0; i < THREADS; ++i)
    ASSERT_EQ(0, pthread_create(&th[i], 0, worker, (void *)i));
  for (long i = 0; i < THREADS; ++i)
    ASSERT_EQ(0, pthread_join(th[i], 0));
  pthread_barrier_destroy(&barrier);
  radix_sort_int64(numbers, ARRAYLEN(numbers));
  for (long i = 1; i < ARRAYLEN(numbers); ++i)
    ASSERT_NE(numbers[i - 1], numbers[i]);
}

TEST(_rand64, forkSafety) {
  int64_t *theirs = _mapshared(sizeof(int64_t));
  int pid;
  ASSERT_NE(-1, (pid = fork()));
  if (!pid) {
    *theirs = _rand64();
    _Exit(0);
  }
  int64_t mine = _rand64();
  int ws;
  ASSERT_EQ(pid, wait(&ws));
  ASSERT_EQ(0, ws);
  ASSERT_NE(mine, theirs);
}

TEST(_rand64, reseedSafety) {
  _rand64_threadSafety();
  memcpy(numbers2, numbers, sizeof(numbers));
  long v1 = _rand64();
  long *v2 = _mapshared(8);
  if (!fork()) {
    *v2 = _rand64();
    _Exit(0);
  }
  int ws;
  ASSERT_NE(-1, wait(&ws));
  ASSERT_EQ(0, ws);
  ASSERT_NE(v1, *v2);
  ASSERT_NE(_rand64(), *v2);
}
