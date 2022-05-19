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
#include "libc/calls/calls.h"
#include "libc/calls/sigbits.h"
#include "libc/calls/struct/sigaction.h"
#include "libc/calls/struct/sigset.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/intrin/kprintf.h"
#include "libc/intrin/spinlock.h"
#include "libc/macros.internal.h"
#include "libc/nexgen32e/threaded.h"
#include "libc/rand/rand.h"
#include "libc/runtime/stack.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/clone.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/prot.h"
#include "libc/sysv/consts/sa.h"
#include "libc/sysv/consts/sig.h"
#include "libc/testlib/testlib.h"
#include "libc/time/time.h"

#define THREADS 8
#define ENTRIES 1024

_Atomic(bool) ready;
volatile uint64_t A[THREADS * ENTRIES];

void OnChld(int sig) {
  // do nothing
}

dontinline void Pause(void) {
  // when ftrace is enabled
}

dontinline void Generate(int i) {
  A[i] = rand64();
}

int Thrasher(void *arg) {
  int i, id = (intptr_t)arg;
  while (!ready) Pause();
  for (i = 0; i < ENTRIES; ++i) {
    Generate(id * ENTRIES + i);
  }
  return 0;
}

TEST(rand64, testLcg_doesntProduceIdenticalValues) {
  int i, j;
  bzero(A, sizeof(A));
  for (i = 0; i < ARRAYLEN(A); ++i) {
    A[i] = rand64();
  }
  for (i = 0; i < ARRAYLEN(A); ++i) {
    EXPECT_NE(0, A[i], "i=%d", i);
    for (j = 0; j < ARRAYLEN(A); ++j) {
      if (i == j) continue;
      EXPECT_NE(A[i], A[j], "i=%d j=%d", i, j);
    }
  }
}

TEST(rand64, testThreadSafety_doesntProduceIdenticalValues) {
  char *tls[THREADS];
  sigset_t ss, oldss;
  void *stacks[THREADS];
  int i, j, rc, ws, tid[THREADS];
  struct sigaction oldsa;
  struct sigaction sa = {.sa_handler = OnChld, .sa_flags = SA_RESTART};
  EXPECT_NE(-1, sigaction(SIGCHLD, &sa, &oldsa));
  bzero(A, sizeof(A));
  sigemptyset(&ss);
  sigaddset(&ss, SIGCHLD);
  EXPECT_EQ(0, sigprocmask(SIG_BLOCK, &ss, &oldss));
  ready = false;
  memset(tid, -1, sizeof(tid));
  for (i = 0; i < THREADS; ++i) {
    tls[i] = __initialize_tls(calloc(1, 64));
    stacks[i] = mmap(0, GetStackSize(), PROT_READ | PROT_WRITE,
                     MAP_STACK | MAP_ANONYMOUS, -1, 0);
    ASSERT_NE(-1, clone(Thrasher, stacks[i], GetStackSize(),
                        CLONE_THREAD | CLONE_VM | CLONE_FS | CLONE_FILES |
                            CLONE_SIGHAND | CLONE_SETTLS | CLONE_CHILD_CLEARTID,
                        (void *)(intptr_t)i, 0, tls[i], 64, tid + i));
  }
  ready = true;
  for (i = 0; i < THREADS; ++i) {
    _spinlock(tid + i);
    EXPECT_SYS(0, 0, munmap(stacks[i], GetStackSize()));
    free(tls[i]);
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
}
