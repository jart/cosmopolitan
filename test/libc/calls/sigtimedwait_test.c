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
#include "libc/calls/sigtimedwait.h"
#include "libc/calls/struct/siginfo.h"
#include "libc/calls/struct/siginfo.internal.h"
#include "libc/calls/struct/sigset.h"
#include "libc/calls/struct/timespec.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/runtime/runtime.h"
#include "libc/sysv/consts/sicode.h"
#include "libc/sysv/consts/sig.h"
#include "libc/testlib/testlib.h"

void SetUp(void) {
  if (IsXnu()) exit(0);
  if (IsMetal()) exit(0);
  if (IsWindows()) exit(0);
  if (IsOpenbsd()) exit(0);
}

TEST(sigtimedwait, nullSet_efault) {
  ASSERT_SYS(EFAULT, -1, sigtimedwait(0, 0, 0));
}

TEST(sigtimedwait, emptySet_timesOut) {
  sigset_t ss = {0};
  struct timespec ts = {0, 0};
  ASSERT_SYS(EAGAIN, -1, sigtimedwait(&ss, 0, &ts));
}

TEST(sigtimedwait, badTimestamp_einval) {
  sigset_t ss = {0};
  struct timespec ts = {0, -1};
  ASSERT_SYS(EINVAL, -1, sigtimedwait(&ss, 0, &ts));
}

TEST(sigtimedwait, test) {
  int pid, ws;
  siginfo_t info;
  sigset_t ss, oldss;
  struct timespec ts = {1, 0};
  sigemptyset(&ss);
  sigaddset(&ss, SIGUSR1);
  ASSERT_SYS(0, 0, sigprocmask(SIG_BLOCK, &ss, &oldss));
  ASSERT_NE(-1, (pid = fork()));
  if (!pid) {
    ASSERT_SYS(0, SIGUSR1, sigtimedwait(&ss, &info, &ts));
    ASSERT_EQ(SIGUSR1, info.si_signo);
    ASSERT_EQ(SI_USER, info.si_code);
    ASSERT_EQ(getuid(), info.si_uid);
    _Exit(0);
  }
  ASSERT_SYS(0, 0, kill(pid, SIGUSR1));
  ASSERT_SYS(0, pid, wait(&ws));
  ASSERT_EQ(0, ws);
  ASSERT_SYS(0, 0, sigprocmask(SIG_SETMASK, &oldss, 0));
}
