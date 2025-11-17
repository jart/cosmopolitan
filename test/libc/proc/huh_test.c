/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2025 Justine Alexandra Roberts Tunney                              │
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
#include "libc/calls/struct/sigset.h"
#include "libc/errno.h"
#include "libc/intrin/kprintf.h"
#include "libc/proc/proc.h"
#include "libc/stdio/stdio.h"
#include "libc/sysv/consts/sig.h"
#include "libc/testlib/testlib.h"

static volatile bool gotsigusr1;
static volatile bool gotsigusr2;

static void OnSigusr1(int sig) {
  gotsigusr1 = true;
}

static void OnSigusr2(int sig) {
  gotsigusr2 = true;
}

TEST(fork, childToChild) {
  sigset_t mask, oldmask;
  int ws, parent, child1, child2;
  gotsigusr1 = false;
  gotsigusr2 = false;
  parent = getpid();
  signal(SIGUSR1, OnSigusr1);
  signal(SIGUSR2, OnSigusr2);
  sigemptyset(&mask);
  sigaddset(&mask, SIGUSR1);
  sigaddset(&mask, SIGUSR2);
  sigprocmask(SIG_BLOCK, &mask, &oldmask);
  ASSERT_NE(-1, (child1 = fork()));
  if (!child1) {
    if (kill(parent, SIGUSR2)) {
      kprintf("%s:%d: error: failed to kill parent: %m\n", __FILE__, __LINE__);
      _Exit(1);
    }
    ASSERT_SYS(EINTR, -1, sigsuspend(0));
    _Exit(!gotsigusr1);
  }
  EXPECT_SYS(EINTR, -1, sigsuspend(0));
  ASSERT_NE(-1, (child2 = fork()));
  if (!child2) {
    if (kill(child1, SIGUSR1)) {
      kprintf("%s:%d: error: failed to kill child1: %m\n", __FILE__, __LINE__);
      _Exit(1);
    }
    _Exit(0);
  }
  ASSERT_NE(-1, wait(&ws));
  EXPECT_EQ(0, ws);
  ASSERT_NE(-1, wait(&ws));
  EXPECT_EQ(0, ws);
  sigprocmask(SIG_SETMASK, &oldmask, 0);
}
