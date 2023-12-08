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
#include "libc/calls/calls.h"
#include "libc/calls/struct/sigaction.h"
#include "libc/macros.internal.h"
#include "libc/runtime/internal.h"
#include "libc/stdio/rand.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/sa.h"
#include "libc/sysv/consts/sig.h"
#include "libc/testlib/ezbench.h"
#include "libc/testlib/testlib.h"
#include "libc/thread/thread.h"

#define THREADS 8
#define ENTRIES 100

volatile uint64_t A[THREADS * ENTRIES];
pthread_barrier_t barrier;

void SetUpOnce(void) {
  ASSERT_SYS(0, 0, pledge("stdio", 0));
}

void OnChld(int sig) {
  // do nothing
}

dontinline void Generate(int i) {
  A[i] = _rand64();
}

void *Thrasher(void *arg) {
  int i, id = (intptr_t)arg;
  pthread_barrier_wait(&barrier);
  for (i = 0; i < ENTRIES; ++i) {
    Generate(id * ENTRIES + i);
  }
  return 0;
}

TEST(_rand64, testLcg_doesntProduceIdenticalValues) {
  int i, j;
  bzero((void *)A, sizeof(A));
  for (i = 0; i < ARRAYLEN(A); ++i) {
    A[i] = _rand64();
  }
  for (i = 0; i < ARRAYLEN(A); ++i) {
    EXPECT_NE(0, A[i], "i=%d", i);
    for (j = 0; j < ARRAYLEN(A); ++j) {
      if (i == j) continue;
      EXPECT_NE(A[i], A[j], "i=%d j=%d", i, j);
    }
  }
}

TEST(_rand64, testThreadSafety_doesntProduceIdenticalValues) {
  int i, j;
  sigset_t ss, oldss;
  pthread_t th[THREADS];
  struct sigaction oldsa;
  struct sigaction sa = {.sa_handler = OnChld, .sa_flags = SA_RESTART};
  EXPECT_NE(-1, sigaction(SIGCHLD, &sa, &oldsa));
  bzero((void *)A, sizeof(A));
  sigemptyset(&ss);
  sigaddset(&ss, SIGCHLD);
  EXPECT_EQ(0, sigprocmask(SIG_BLOCK, &ss, &oldss));
  ASSERT_EQ(0, pthread_barrier_init(&barrier, 0, THREADS));
  for (i = 0; i < THREADS; ++i) {
    ASSERT_EQ(0, pthread_create(th + i, 0, Thrasher, (void *)(intptr_t)i));
  }
  for (i = 0; i < THREADS; ++i) {
    ASSERT_EQ(0, pthread_join(th[i], 0));
  }
  sigaction(SIGCHLD, &oldsa, 0);
  sigprocmask(SIG_BLOCK, &oldss, 0);
  for (i = 0; i < ARRAYLEN(A); ++i) {
    EXPECT_NE(0, A[i], "i=%d", i);
    for (j = 0; j < ARRAYLEN(A); ++j) {
      if (i == j) continue;
      EXPECT_NE(A[i], A[j], "i=%d j=%d", i, j);
    }
  }
  ASSERT_EQ(0, pthread_barrier_destroy(&barrier));
}
