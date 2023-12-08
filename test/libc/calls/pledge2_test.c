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
#include "libc/calls/pledge.internal.h"
#include "libc/calls/struct/seccomp.internal.h"
#include "libc/calls/syscall_support-sysv.internal.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/intrin/promises.internal.h"
#include "libc/runtime/runtime.h"
#include "libc/sock/sock.h"
#include "libc/stdio/stdio.h"
#include "libc/sysv/consts/af.h"
#include "libc/sysv/consts/ipproto.h"
#include "libc/sysv/consts/sig.h"
#include "libc/sysv/consts/sock.h"
#include "libc/testlib/subprocess.h"
#include "libc/testlib/testlib.h"

void SetUp(void) {
  if (pledge(0, 0) == -1) {
    fprintf(stderr, "warning: pledge() not supported on this system\n");
    exit(0);
  }
}

TEST(pledge, testSoftError) {
  if (IsOpenbsd()) return;
  SPAWN(fork);
  __pledge_mode = PLEDGE_PENALTY_RETURN_EPERM;
  ASSERT_SYS(0, 0, pledge("stdio", 0));
  ASSERT_SYS(EPERM, -1, socket(AF_INET, SOCK_STREAM, IPPROTO_TCP));
  _Exit(7);
  EXITS(7);
}

TEST(pledge, testKillThreadMode) {
  SPAWN(fork);
  __pledge_mode = PLEDGE_PENALTY_KILL_THREAD | PLEDGE_STDERR_LOGGING;
  ASSERT_SYS(0, 0, pledge("stdio", 0));
  socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  TERMS(SIGABRT);
}

TEST(pledge, testKillProcessMode) {
  SPAWN(fork);
  __pledge_mode = PLEDGE_PENALTY_KILL_PROCESS | PLEDGE_STDERR_LOGGING;
  ASSERT_SYS(0, 0, pledge("stdio", 0));
  socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  TERMS(SIGABRT);
}

TEST(pledge, testLogMessage_inSoftyMode) {
  if (IsOpenbsd()) return;
  int fds[2];
  char msg[256] = {0};
  ASSERT_SYS(0, 0, pipe(fds));
  SPAWN(fork);
  __pledge_mode = PLEDGE_PENALTY_RETURN_EPERM | PLEDGE_STDERR_LOGGING;
  ASSERT_SYS(0, 2, dup2(fds[1], 2));
  ASSERT_SYS(0, 0, pledge("stdio", 0));
  ASSERT_SYS(EPERM, -1, socket(AF_INET, SOCK_STREAM, IPPROTO_TCP));
  EXITS(0);
  close(fds[1]);
  read(fds[0], msg, sizeof(msg) - 1);
  close(fds[0]);
  if (IsLinux()) {
    ASSERT_STARTSWITH("error: protected syscall socket", msg);
  }
}

TEST(pledge, testLogMessage_onKillProcess) {
  int fds[2];
  char msg[256] = {0};
  ASSERT_SYS(0, 0, pipe(fds));
  SPAWN(fork);
  __pledge_mode = PLEDGE_PENALTY_KILL_THREAD | PLEDGE_STDERR_LOGGING;
  ASSERT_SYS(0, 2, dup2(fds[1], 2));
  ASSERT_SYS(0, 0, pledge("stdio", 0));
  socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  TERMS(SIGABRT);
  close(fds[1]);
  read(fds[0], msg, sizeof(msg));
  close(fds[0]);
  if (IsLinux()) {
    ASSERT_STARTSWITH("error: protected syscall socket", msg);
  }
}

TEST(pledge, testDoublePledge_isFine) {
  SPAWN(fork);
  __pledge_mode = PLEDGE_PENALTY_KILL_THREAD;
  ASSERT_SYS(0, 0, pledge("stdio", 0));
  ASSERT_SYS(0, 0, pledge("stdio", 0));
  EXITS(0);
}

TEST(pledge, testEmptyPledge_doesntUseTrapping) {
  SPAWN(fork);
  __pledge_mode = PLEDGE_PENALTY_KILL_PROCESS;
  ASSERT_SYS(0, 0, pledge("", 0));
  socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  TERMS(IsOpenbsd() ? SIGABRT : SIGSYS);
}
