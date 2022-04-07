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
#include "libc/macros.internal.h"
#include "libc/rand/rand.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/clone.h"
#include "libc/sysv/consts/sa.h"
#include "libc/sysv/consts/sig.h"
#include "libc/testlib/testlib.h"

#define THREADS 1
#define ENTRIES 10

volatile bool ready;
volatile uint64_t A[THREADS * ENTRIES];

void OnChld(int sig) {
  // do nothing
}

int Thrasher(void *arg) {
  int i, id = (intptr_t)arg;
  while (!ready) asm("pause");
  for (i = 0; i < ENTRIES; ++i) {
    A[id * ENTRIES + i] = rand64();
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
  char *stack;
  sigset_t ss, oldss;
  int i, j, rc, ws, tid[THREADS], ptid[THREADS], ctid[THREADS], tls[THREADS];
  if (!IsLinux() && !IsNetbsd()) return;
  struct sigaction oldsa;
  struct sigaction sa = {.sa_handler = OnChld, .sa_flags = SA_RESTART};
  EXPECT_NE(-1, sigaction(SIGCHLD, &sa, &oldsa));
  bzero(A, sizeof(A));
  rand64();  // for effect
  sigemptyset(&ss);
  sigaddset(&ss, SIGCHLD);
  EXPECT_EQ(0, sigprocmask(SIG_BLOCK, &ss, &oldss));
  ready = false;
  for (i = 0; i < THREADS; ++i) {
    stack = gc(malloc(FRAMESIZE));
    tid[i] = clone(Thrasher, stack + FRAMESIZE,
                   CLONE_VM | CLONE_FS | CLONE_FILES | CLONE_SIGHAND | SIGCHLD,
                   (void *)(intptr_t)i, ptid + i, tls + i, ctid + i);
    EXPECT_NE(-1, tid[i]);
  }
  ready = true;
  for (i = 0; i < THREADS; ++i) {
    EXPECT_NE(-1, (rc = waitpid(0, &ws, 0)));
    EXPECT_TRUE(WIFEXITED(ws));
    EXPECT_EQ(0, WEXITSTATUS(ws));
  }
  EXPECT_EQ(0, sigaction(SIGCHLD, &oldsa, 0));
  EXPECT_EQ(0, sigprocmask(SIG_BLOCK, &oldss, 0));
  for (i = 0; i < ARRAYLEN(A); ++i) {
    EXPECT_NE(0, A[i], "i=%d", i);
    for (j = 0; j < ARRAYLEN(A); ++j) {
      if (i == j) continue;
      EXPECT_NE(A[i], A[j], "i=%d j=%d", i, j);
    }
  }
}
