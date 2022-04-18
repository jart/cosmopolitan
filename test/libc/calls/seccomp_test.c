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
#include "libc/calls/struct/iovec.h"
#include "libc/intrin/kprintf.h"
#include "libc/runtime/runtime.h"
#include "libc/sock/sock.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/pr.h"
#include "libc/sysv/consts/seccomp.h"
#include "libc/sysv/consts/sig.h"
#include "libc/testlib/testlib.h"

bool __is_linux_2_6_23(void) {
  if (!IsLinux()) return false;
  return prctl(PR_GET_SECCOMP) != -1;  // errno should be EINVAL
}

void SetUp(void) {
  if (!__is_linux_2_6_23()) {
    exit(0);
  }
}

TEST(seccompStrictMode, evilProcess_getsKill9d) {
  int ws, pid;
  ASSERT_NE(-1, (pid = fork()));
  if (!pid) {
    EXPECT_EQ(0, seccomp(SECCOMP_SET_MODE_STRICT, 0, 0));
    open("/etc/passwd", O_RDWR);
    _Exit1(127);
  }
  EXPECT_NE(-1, wait(&ws));
  EXPECT_TRUE(WIFSIGNALED(ws));
  EXPECT_EQ(SIGKILL, WTERMSIG(ws));
}

TEST(seccompStrictMode, goodProcess_isAuthorized) {
  int ws, pid;
  int pfds[2];
  char buf[3] = {0};
  ASSERT_SYS(0, 0, pipe(pfds));
  ASSERT_NE(-1, (pid = fork()));
  if (!pid) {
    EXPECT_EQ(0, seccomp(SECCOMP_SET_MODE_STRICT, 0, 0));
    write(pfds[1], "hi", 3);
    _Exit1(0);
  }
  EXPECT_SYS(0, 0, close(pfds[1]));
  EXPECT_SYS(0, 3, read(pfds[0], buf, 3));
  EXPECT_SYS(0, 0, close(pfds[0]));
  EXPECT_NE(-1, wait(&ws));
  EXPECT_TRUE(WIFEXITED(ws));
  EXPECT_EQ(0, WEXITSTATUS(ws));
  EXPECT_STREQ("hi", buf);
}
