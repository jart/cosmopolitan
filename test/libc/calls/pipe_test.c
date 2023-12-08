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
#include "libc/calls/struct/rlimit.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/nexgen32e/vendor.internal.h"
#include "libc/runtime/runtime.h"
#include "libc/sysv/consts/rlimit.h"
#include "libc/testlib/testlib.h"

int ws, pid, f[2];
char buf[6] = {0};
struct rlimit rlim = {0, 10};

TEST(pipe, efault) {
  EXPECT_SYS(EFAULT, -1, pipe(__veil("r", (void *)0)));
}

TEST(pipe, einval) {
  EXPECT_SYS(EINVAL, -1, pipe2(f, -1));
}

TEST(pipe, ebadf) {
  if (IsFreebsd()) return;  // somehow succeeds
  if (IsOpenbsd()) return;  // somehow succeeds
  EXPECT_SYS(0, 0, pipe(f));
  EXPECT_SYS(EBADF, -1, write(f[0], "h", 1));
  EXPECT_SYS(EBADF, -1, read(f[1], buf, 1));
  EXPECT_SYS(0, 0, close(f[0]));
  EXPECT_SYS(0, 0, close(f[1]));
}

TEST(pipe, emfile) {
  if (IsWindows()) return;  // TODO
  if (IsCygwin()) return;
  ASSERT_NE(-1, (pid = fork()));
  if (!pid) {
    ASSERT_EQ(0, setrlimit(RLIMIT_NOFILE, &rlim));
    ASSERT_SYS(EMFILE, -1, pipe(f));
    _exit(0);
  }
  EXPECT_NE(-1, waitpid(pid, &ws, 0));
  EXPECT_TRUE(WIFEXITED(ws));
  EXPECT_EQ(0, WEXITSTATUS(ws));
}

TEST(pipe, usesLowestFileNumbers) {
  EXPECT_SYS(0, 0, pipe(f));
  EXPECT_SYS(0, 3, f[0]);
  EXPECT_SYS(0, 4, f[1]);
  EXPECT_SYS(0, 0, close(f[0]));
  EXPECT_SYS(0, 0, close(f[1]));
}

TEST(pipe, doesBuffering) {
  EXPECT_SYS(0, 0, pipe(f));
  EXPECT_SYS(0, 5, write(f[1], "hello", 5));
  EXPECT_SYS(0, 5, read(f[0], buf, 5));
  EXPECT_STREQ("hello", buf);
  EXPECT_SYS(0, 0, close(f[0]));
  EXPECT_SYS(0, 0, close(f[1]));
}
