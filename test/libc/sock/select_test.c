/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2021 Justine Alexandra Roberts Tunney                              │
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
#include "libc/sock/select.h"
#include "libc/calls/calls.h"
#include "libc/calls/pledge.h"
#include "libc/calls/struct/sigaction.h"
#include "libc/calls/struct/sigset.h"
#include "libc/calls/struct/timeval.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/runtime/runtime.h"
#include "libc/sock/sock.h"
#include "libc/sysv/consts/sig.h"
#include "libc/testlib/testlib.h"
#include "libc/time/time.h"

bool gotsig;

void SetUpOnce(void) {
  __pledge_mode = PLEDGE_PENALTY_KILL_PROCESS | PLEDGE_STDERR_LOGGING;
  ASSERT_SYS(0, 0, pledge("stdio", 0));
}

void SetUp(void) {
  gotsig = false;
}

void OnSig(int sig) {
  gotsig = true;
}

TEST(select, allZero1) {
  sigset_t set, old;
  struct sigaction oldss;
  struct sigaction sa = {.sa_handler = OnSig};
  ASSERT_SYS(0, 0, sigaction(SIGQUIT, &sa, &oldss));
  ASSERT_SYS(0, 0, sigfillset(&set));
  ASSERT_SYS(0, 0, sigprocmask(SIG_SETMASK, &set, &old));
  EXPECT_SYS(0, 0, kill(getpid(), SIGQUIT));
  EXPECT_SYS(EINTR, -1, pselect(0, 0, 0, 0, 0, &old));
  EXPECT_TRUE(gotsig);
  EXPECT_SYS(0, 0, sigprocmask(SIG_SETMASK, &old, 0));
  EXPECT_SYS(0, 0, sigaction(SIGQUIT, &oldss, 0));
}

TEST(select, pipe_hasInputFromSameProcess) {
  fd_set rfds;
  char buf[2];
  int pipefds[2];
  struct timeval tv = {.tv_usec = 100 * 1000};
  EXPECT_SYS(0, 0, pipe(pipefds));
  FD_ZERO(&rfds);
  FD_SET(pipefds[0], &rfds);
  EXPECT_SYS(0, 2, write(pipefds[1], "hi", 2));
  EXPECT_SYS(0, 1, select(pipefds[0] + 1, &rfds, 0, 0, &tv));
  EXPECT_TRUE(FD_ISSET(pipefds[0], &rfds));
  EXPECT_SYS(0, 2, read(pipefds[0], buf, 2));
  EXPECT_SYS(0, 0, select(pipefds[0] + 1, &rfds, 0, 0, &tv));
  EXPECT_TRUE(!FD_ISSET(pipefds[0], &rfds));
  EXPECT_SYS(0, 0, close(pipefds[0]));
  EXPECT_SYS(0, 0, close(pipefds[1]));
}

#if 0  // flaky
TEST(select, testSleep) {
  int64_t e;
  long double n;
  struct timeval t = {0, 2000};
  n = nowl();
  EXPECT_SYS(0, 0, select(0, 0, 0, 0, &t));
  e = (nowl() - n) * 1e6;
  EXPECT_GT(e, 1000);
  if (IsLinux()) {
    EXPECT_EQ(0, t.tv_sec);
    EXPECT_EQ(0, t.tv_usec);
  }
}
#endif
