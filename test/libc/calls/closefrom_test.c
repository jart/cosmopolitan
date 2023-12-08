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
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/runtime/runtime.h"
#include "libc/sysv/consts/f.h"
#include "libc/testlib/subprocess.h"
#include "libc/testlib/testlib.h"

void SetUp(void) {
  if (closefrom(3) == -1) {
    if (IsOpenbsd()) {
      ASSERT_EQ(EBADF, errno);
    } else {
      ASSERT_EQ(ENOSYS, errno);
      exit(0);
    }
  }
}

TEST(closefrom, ebadf) {
  ASSERT_SYS(EBADF, -1, closefrom(-2));
}

TEST(closefrom, test) {
  ASSERT_SYS(0, 3, dup(2));
  ASSERT_SYS(0, 4, dup(2));
  ASSERT_SYS(0, 5, dup(2));
  ASSERT_SYS(0, 6, dup(2));
  EXPECT_SYS(0, 0, closefrom(3));
  ASSERT_SYS(0, 0, fcntl(2, F_GETFD));
  ASSERT_SYS(EBADF, -1, fcntl(3, F_GETFD));
  ASSERT_SYS(EBADF, -1, fcntl(4, F_GETFD));
  ASSERT_SYS(EBADF, -1, fcntl(5, F_GETFD));
  ASSERT_SYS(EBADF, -1, fcntl(6, F_GETFD));
}

TEST(close_range, test) {
  if (IsLinux() || IsFreebsd()) {
    ASSERT_SYS(0, 3, dup(2));
    ASSERT_SYS(0, 4, dup(2));
    ASSERT_SYS(0, 5, dup(2));
    ASSERT_SYS(0, 6, dup(2));
    EXPECT_SYS(0, 0, close_range(3, -1, 0));
    ASSERT_SYS(0, 0, fcntl(2, F_GETFD));
    ASSERT_SYS(EBADF, -1, fcntl(3, F_GETFD));
    ASSERT_SYS(EBADF, -1, fcntl(4, F_GETFD));
    ASSERT_SYS(EBADF, -1, fcntl(5, F_GETFD));
    ASSERT_SYS(EBADF, -1, fcntl(6, F_GETFD));
  } else {
    EXPECT_SYS(ENOSYS, -1, close_range(3, -1, 0));
  }
}

TEST(close_range, ignoresNonexistantRanges) {
  if (!IsLinux() && !IsFreebsd()) return;
  EXPECT_SYS(0, 0, close_range(-2, -1, 0));
}
