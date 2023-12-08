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
#include "libc/calls/struct/siginfo.h"
#include "libc/calls/struct/sigset.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/runtime/runtime.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/sa.h"
#include "libc/sysv/consts/sig.h"
#include "libc/testlib/testlib.h"
#include "libc/x/xspawn.h"

volatile bool gotsig1;
volatile bool gotsig2;
volatile bool finished;

void OnSigQueuing(int sig, siginfo_t *si, void *ctx) {
  if (!finished) {
    EXPECT_EQ(SIGUSR2, sig);
    EXPECT_EQ(SIGUSR2, si->si_signo);
    gotsig2 = true;
  } else {
    EXPECT_EQ(SIGUSR1, sig);
    EXPECT_EQ(SIGUSR1, si->si_signo);
    gotsig1 = true;
  }
}

TEST(sigsuspend, testSignalQueuingSelf) {
  sigset_t neu, old, bits;
  struct sigaction oldusr1, oldusr2;
  struct sigaction sa = {.sa_sigaction = OnSigQueuing, .sa_flags = SA_SIGINFO};
  gotsig1 = false;
  gotsig2 = false;
  finished = false;
  sigemptyset(&neu);
  sigaddset(&neu, SIGUSR1);
  sigaddset(&neu, SIGUSR2);
  EXPECT_EQ(0, sigprocmask(SIG_BLOCK, &neu, &old));
  ASSERT_EQ(0, sigaction(SIGUSR1, &sa, &oldusr1));
  ASSERT_EQ(0, sigaction(SIGUSR2, &sa, &oldusr2));
  raise(SIGUSR1);
  raise(SIGUSR2);
  sigdelset(&neu, SIGUSR2);
  EXPECT_EQ(false, gotsig2);
  memcpy(&bits, &neu, sizeof(bits));
  ASSERT_EQ(-1, sigsuspend(&neu));                  // raises SIGUSR2
  EXPECT_EQ(0, memcmp(&bits, &neu, sizeof(bits)));  // constness respected
  EXPECT_EQ(EINTR, errno);
  EXPECT_EQ(true, gotsig2);
  EXPECT_EQ(false, gotsig1);
  finished = true;
  EXPECT_EQ(0, sigprocmask(SIG_SETMASK, &old, NULL));  // raises SIGUSR1
  EXPECT_EQ(true, gotsig1);
  EXPECT_EQ(0, sigaction(SIGUSR1, &oldusr1, 0));
  EXPECT_EQ(0, sigaction(SIGUSR2, &oldusr2, 0));
}

TEST(sigsuspend, testSignalQueuingIpc) {
  if (IsWindows()) {
    // xxx: probably need a signal server to do this kind of signalling
    return;
  }
  int pid, ws;
  sigset_t neu, old, bits;
  struct sigaction oldusr1, oldusr2;
  struct sigaction sa = {.sa_sigaction = OnSigQueuing, .sa_flags = SA_SIGINFO};
  gotsig1 = false;
  gotsig2 = false;
  finished = false;
  sigemptyset(&neu);
  sigaddset(&neu, SIGUSR1);
  sigaddset(&neu, SIGUSR2);
  EXPECT_EQ(0, sigprocmask(SIG_BLOCK, &neu, &old));
  ASSERT_EQ(0, sigaction(SIGUSR1, &sa, &oldusr1));
  ASSERT_EQ(0, sigaction(SIGUSR2, &sa, &oldusr2));
  pid = getpid();
  ASSERT_NE(-1, (ws = xspawn(0)));
  if (ws == -2) {
    kill(pid, SIGUSR1);
    kill(pid, SIGUSR2);
    _exit(0);
  }
  sigdelset(&neu, SIGUSR2);
  EXPECT_EQ(false, gotsig2);
  memcpy(&bits, &neu, sizeof(bits));
  ASSERT_EQ(-1, sigsuspend(&neu));                  // raises SIGUSR2
  EXPECT_EQ(0, memcmp(&bits, &neu, sizeof(bits)));  // constness respected
  EXPECT_EQ(EINTR, errno);
  EXPECT_EQ(true, gotsig2);
  EXPECT_EQ(false, gotsig1);
  finished = true;
  EXPECT_EQ(0, sigprocmask(SIG_SETMASK, &old, NULL));  // raises SIGUSR1
  EXPECT_EQ(true, gotsig1);
  EXPECT_EQ(0, sigaction(SIGUSR1, &oldusr1, 0));
  EXPECT_EQ(0, sigaction(SIGUSR2, &oldusr2, 0));
}
