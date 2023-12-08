/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2022 Gavin Arthur Hayes                                            │
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
#include "libc/calls/struct/siginfo.h"
#include "libc/calls/struct/sigset.h"
#include "libc/errno.h"
#include "libc/sysv/consts/sa.h"
#include "libc/sysv/consts/sig.h"
#include "libc/testlib/testlib.h"

static unsigned OnSignalCnt = 0;

void OnSignal(int sig, siginfo_t *si, void *ctx) {
  OnSignalCnt++;
  EXPECT_EQ(SIGUSR1, sig);
}

TEST(sigpending, efault) {
  ASSERT_SYS(EFAULT, -1, sigpending(0));
}

TEST(sigpending, test) {
  sigset_t pending;
  sigfillset(&pending);
  EXPECT_EQ(0, sigpending(&pending));
  EXPECT_EQ(0, sigcountset(&pending));

  struct sigaction sa = {.sa_sigaction = OnSignal, .sa_flags = SA_SIGINFO};
  ASSERT_EQ(0, sigaction(SIGUSR1, &sa, NULL));
  sigset_t blocked;
  sigemptyset(&blocked);
  sigaddset(&blocked, SIGUSR1);
  ASSERT_EQ(0, sigprocmask(SIG_SETMASK, &blocked, NULL));
  ASSERT_EQ(0, raise(SIGUSR1));
  sigfillset(&pending);
  EXPECT_EQ(0, sigpending(&pending));
  EXPECT_EQ(1, sigcountset(&pending));
  EXPECT_EQ(1, sigismember(&pending, SIGUSR1));

  ASSERT_NE(SIG_ERR, signal(SIGUSR1, SIG_IGN));
  sigemptyset(&pending);
  EXPECT_EQ(0, sigpending(&pending));
  EXPECT_EQ(0, sigcountset(&pending));

  ASSERT_EQ(0, sigaction(SIGUSR1, &sa, NULL));
  ASSERT_EQ(0, raise(SIGUSR1));
  EXPECT_EQ(0, sigpending(&pending));
  EXPECT_EQ(1, sigcountset(&pending));
  EXPECT_EQ(1, sigismember(&pending, SIGUSR1));

  sigemptyset(&blocked);
  ASSERT_EQ(0, sigprocmask(SIG_SETMASK, &blocked, NULL));
  EXPECT_EQ(0, sigpending(&pending));
  EXPECT_EQ(0, sigcountset(&pending));
  EXPECT_EQ(1, OnSignalCnt);
}
