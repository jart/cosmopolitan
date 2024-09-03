/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2024 Justine Alexandra Roberts Tunney                              │
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
#include "libc/dce.h"
#include "libc/intrin/atomic.h"
#include "libc/macros.h"
#include "libc/runtime/runtime.h"
#include "libc/testlib/subprocess.h"
#include "libc/testlib/testlib.h"
#include "libc/thread/thread.h"
#include "libc/thread/thread2.h"

int cpu_count;

void SetUpOnce(void) {
  cpu_count = __get_cpu_count();
}

////////////////////////////////////////////////////////////////////////////////
// AFFINITY TEST

TEST(sched_getcpu, affinity_test) {

  if (IsXnu())
    return;
  if (IsNetbsd())
    return;
  if (IsOpenbsd())
    return;

  SPAWN(fork);
  int n = cpu_count;
  for (int i = 0; i < n; ++i) {
    cpu_set_t affinity;
    CPU_ZERO(&affinity);
    CPU_SET(i, &affinity);
    ASSERT_EQ(
        0, pthread_setaffinity_np(pthread_self(), sizeof(affinity), &affinity));
    EXPECT_EQ(i, sched_getcpu());
  }
  EXITS(0);
}

////////////////////////////////////////////////////////////////////////////////
// KLUDGE TEST

#define THREADS    2
#define ITERATIONS 100000

int g_hits[256];
atomic_int g_sync;

int call_sched_getcpu(void) {
  int res = sched_getcpu();
  ASSERT_NE(-1, res);
  ASSERT_GE(res, 0);
  ASSERT_LT(res, cpu_count);
  return res;
}

void *worker(void *arg) {
  int ith = (long)arg;
  int nth = THREADS;
  for (int i = 0; i < ITERATIONS; ++i) {
    // help execution of threads be interleaved
    int sync = atomic_fetch_add(&g_sync, 1);
    if (sync % nth == ith) {
      g_hits[call_sched_getcpu() % ARRAYLEN(g_hits)]++;
    }
  }
  return 0;
}

TEST(sched_getcpu, kludge_test) {

#ifdef __x86_64__
  if (IsXnu())
    return;
#endif
  if (IsNetbsd())
    return;
  if (IsOpenbsd())
    return;

  if (cpu_count < THREADS)
    return;
  pthread_t th[THREADS];
  for (int i = 0; i < THREADS; ++i)
    ASSERT_EQ(0, pthread_create(th + i, 0, worker, (void *)(long)i));
  for (int i = 0; i < THREADS; ++i)
    ASSERT_EQ(0, pthread_join(th[i], 0));
  int hit = 0;
  for (int i = 0; i < ARRAYLEN(g_hits); ++i)
    hit += !!g_hits[i];
  ASSERT_GE(hit, THREADS);
}
