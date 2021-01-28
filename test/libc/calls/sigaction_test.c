/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
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
#include "libc/calls/calls.h"
#include "libc/calls/sigbits.h"
#include "libc/calls/struct/sigaction.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/runtime/runtime.h"
#include "libc/sysv/consts/sa.h"
#include "libc/sysv/consts/sig.h"
#include "libc/testlib/testlib.h"

bool gotsigint;

void OnSigInt(int sig) {
  gotsigint = true;
}

TEST(sigaction, test) {
  /* TODO(jart): Why does RHEL5 behave differently? */
  /* TODO(jart): Windows needs huge signal overhaul */
  if (IsWindows()) return;
  int pid, status;
  sigset_t block, ignore, oldmask;
  struct sigaction saint = {.sa_handler = OnSigInt};
  sigemptyset(&block);
  sigaddset(&block, SIGINT);
  EXPECT_NE(-1, sigprocmask(SIG_BLOCK, &block, &oldmask));
  sigfillset(&ignore);
  sigdelset(&ignore, SIGINT);
  EXPECT_NE(-1, sigaction(SIGINT, &saint, NULL));
  ASSERT_NE(-1, (pid = fork()));
  if (!pid) {
    EXPECT_NE(-1, kill(getppid(), SIGINT));
    EXPECT_EQ(-1, sigsuspend(&ignore));
    EXPECT_EQ(EINTR, errno);
    EXPECT_TRUE(gotsigint);
    _exit(0);
  }
  EXPECT_EQ(-1, sigsuspend(&ignore));
  EXPECT_NE(-1, kill(pid, SIGINT));
  EXPECT_NE(-1, waitpid(pid, &status, 0));
  EXPECT_EQ(1, WIFEXITED(status));
  EXPECT_EQ(0, WEXITSTATUS(status));
  EXPECT_EQ(0, WTERMSIG(status));
  EXPECT_NE(-1, sigprocmask(SIG_BLOCK, &oldmask, NULL));
}
