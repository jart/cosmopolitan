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
#include "libc/calls/struct/sigset.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/intrin/kprintf.h"
#include "libc/runtime/runtime.h"
#include "libc/sysv/consts/prio.h"
#include "libc/sysv/consts/sig.h"
#include "libc/testlib/subprocess.h"
#include "libc/testlib/testlib.h"

void SetUp(void) {
  if (getpriority(PRIO_PROCESS, getpid()) != 0) {
    kprintf("getpriority_test.com must be launched at priority zero\n");
    exit(0);
  }
}

TEST(getpriority, badWhich_einval) {
  ASSERT_SYS(EINVAL, -1, getpriority(-1, 0));
  ASSERT_SYS(EINVAL, -1, setpriority(-1, 0, 0));
}

TEST(getpriority, lowerPriorityOfSelf) {
  SPAWN(fork);
  ASSERT_SYS(0, 0, getpriority(PRIO_PROCESS, 0));
  ASSERT_SYS(0, 0, getpriority(PRIO_PROCESS, getpid()));
  ASSERT_SYS(0, 0, setpriority(PRIO_PROCESS, getpid(), 5));
  ASSERT_SYS(0, 5, getpriority(PRIO_PROCESS, getpid()));
  ASSERT_SYS(0, 5, getpriority(PRIO_PROCESS, 0));
  EXITS(0);
}

TEST(getpriority, higherPriorityOfSelf) {
  if (IsWindows() || getuid() == 0) {
    SPAWN(fork);
    ASSERT_SYS(0, 0, setpriority(PRIO_PROCESS, 0, -5));
    ASSERT_SYS(0, -5, getpriority(PRIO_PROCESS, 0));
    ASSERT_SYS(0, 0, setpriority(PRIO_PROCESS, 0, 0));
    EXITS(0);
  }
}

TEST(getpriority, lowerAndRaiseItAgain_notAllowed) {
  if (1) return;  // this behavior seems limited to modern linux
  SPAWN(fork);
  ASSERT_SYS(0, 0, setpriority(PRIO_PROCESS, 0, 5));
  ASSERT_SYS(EACCES, -1, setpriority(PRIO_PROCESS, 0, 4));
  EXITS(0);
}

TEST(getpriority, lowerPriorityOfSubprocess) {
  int pid, ws;
  sigset_t ss, oldss;
  sigemptyset(&ss);
  sigaddset(&ss, SIGUSR1);
  ASSERT_SYS(0, 0, sigprocmask(SIG_BLOCK, &ss, &oldss));
  ASSERT_NE(-1, (pid = fork()));
  if (!pid) {
    sigemptyset(&ss);
    ASSERT_SYS(EINTR, -1, sigsuspend(&ss));
    _Exit(0);
  }
  ASSERT_SYS(0, 0, getpriority(PRIO_PROCESS, pid));
  ASSERT_SYS(0, 0, setpriority(PRIO_PROCESS, pid, 5));
  ASSERT_SYS(0, 5, getpriority(PRIO_PROCESS, pid));
  ASSERT_SYS(0, 0, kill(pid, SIGUSR1));
  ASSERT_SYS(0, pid, wait(&ws));
  ASSERT_SYS(0, 0, sigprocmask(SIG_SETMASK, &oldss, 0));
}
