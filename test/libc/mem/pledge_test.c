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
#include "libc/calls/internal.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/mem/mem.h"
#include "libc/runtime/runtime.h"
#include "libc/sysv/consts/o.h"
#include "libc/testlib/testlib.h"

void SetUp(void) {
  if (!__is_linux_2_6_23() && !IsOpenbsd()) {
    exit(0);
  }
}

TEST(pledge, default_allowsExit) {
  int ws, pid;
  ASSERT_NE(-1, (pid = fork()));
  if (!pid) {
    ASSERT_SYS(0, 0, pledge("", 0));
    _Exit(0);
  }
  EXPECT_NE(-1, wait(&ws));
  EXPECT_TRUE(WIFEXITED(ws));
  EXPECT_EQ(0, WEXITSTATUS(ws));
}

TEST(pledge, stdio_forbidsOpeningPasswd) {
  int ws, pid;
  ASSERT_NE(-1, (pid = fork()));
  if (!pid) {
    ASSERT_SYS(0, 0, pledge("stdio", 0));
    ASSERT_SYS(EPERM, -1, open("/etc/passwd", O_RDWR));
    _Exit(0);
  }
  EXPECT_NE(-1, wait(&ws));
  if (IsLinux()) {
    EXPECT_TRUE(WIFEXITED(ws));
    EXPECT_EQ(0, WEXITSTATUS(ws));
  } else {
    EXPECT_TRUE(WIFSIGNALED(ws));
    EXPECT_EQ(SIGABRT, WTERMSIG(ws));
  }
}
