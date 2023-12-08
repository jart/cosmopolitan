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
#include "libc/calls/calls.h"
#include "libc/calls/struct/sigaction.h"
#include "libc/calls/struct/siginfo.h"
#include "libc/calls/struct/sigset.h"
#include "libc/calls/struct/sigset.internal.h"
#include "libc/calls/ucontext.h"
#include "libc/dce.h"
#include "libc/intrin/kprintf.h"
#include "libc/runtime/runtime.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/sa.h"
#include "libc/sysv/consts/sig.h"
#include "libc/testlib/testlib.h"

volatile int n;

void SetUpOnce(void) {
  ASSERT_SYS(0, 0, pledge("stdio proc", 0));
}

void OnSig(int sig, siginfo_t *si, void *ctx) {
  ++n;
}

const char *DescribeMask(void) {
  sigset_t ss;
  _Thread_local static char buf[128];
  unassert(!sigprocmask(SIG_SETMASK, 0, &ss));
  return (DescribeSigset)(buf, 0, &ss);
}

TEST(sigprocmask, testMultipleBlockedDeliveries) {
  sigset_t neu, old;
  struct sigaction oldusr1, oldusr2;
  struct sigaction sa = {.sa_sigaction = OnSig, .sa_flags = SA_SIGINFO};
  n = 0;
  sigemptyset(&neu);
  sigaddset(&neu, SIGUSR1);
  sigaddset(&neu, SIGUSR2);
  EXPECT_EQ(0, sigprocmask(SIG_BLOCK, &neu, &old));
  ASSERT_EQ(0, sigaction(SIGUSR1, &sa, &oldusr1));
  ASSERT_EQ(0, sigaction(SIGUSR2, &sa, &oldusr2));
  ASSERT_EQ(0, raise(SIGUSR1));
  ASSERT_EQ(0, raise(SIGUSR2));
  EXPECT_EQ(0, n);
  EXPECT_EQ(0, sigprocmask(SIG_SETMASK, &old, NULL));
  EXPECT_EQ(0, sigaction(SIGUSR1, &oldusr1, 0));
  EXPECT_EQ(0, sigaction(SIGUSR2, &oldusr2, 0));
  ASSERT_EQ(2, n);
}

TEST(sigprocmask, testMultipleBlockedDeliveriesOfSameSignal) {
  sigset_t neu, old;
  struct sigaction oldusr2;
  struct sigaction sa = {.sa_sigaction = OnSig, .sa_flags = SA_SIGINFO};
  n = 0;
  sigemptyset(&neu);
  sigaddset(&neu, SIGUSR2);
  EXPECT_EQ(0, sigprocmask(SIG_BLOCK, &neu, &old));
  ASSERT_EQ(0, sigaction(SIGUSR2, &sa, &oldusr2));
  ASSERT_EQ(0, raise(SIGUSR2));
  ASSERT_EQ(0, raise(SIGUSR2));
  EXPECT_EQ(0, n);
  EXPECT_EQ(0, sigprocmask(SIG_SETMASK, &old, NULL));
  EXPECT_EQ(0, sigaction(SIGUSR2, &oldusr2, 0));
  if (IsFreebsd()) {
    EXPECT_EQ(2, n);
  } else {
    EXPECT_EQ(1, n);
  }
}

TEST(sigprocmask, testBlockingSIGINT) {
  sigset_t neu, old;
  struct sigaction oldsigint;
  struct sigaction sa = {.sa_sigaction = OnSig, .sa_flags = SA_SIGINFO};
  n = 0;
  sigemptyset(&neu);
  sigaddset(&neu, SIGINT);
  EXPECT_EQ(0, sigprocmask(SIG_BLOCK, &neu, &old));
  ASSERT_EQ(0, sigaction(SIGINT, &sa, &oldsigint));
  ASSERT_EQ(0, raise(SIGINT));
  EXPECT_EQ(0, n);
  EXPECT_EQ(0, sigprocmask(SIG_SETMASK, &old, NULL));
  EXPECT_EQ(0, sigaction(SIGINT, &oldsigint, 0));
}
