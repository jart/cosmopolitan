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
#include "libc/sysv/consts/at.h"
#include "libc/sysv/consts/o.h"
#include "libc/testlib/testlib.h"

void SetUpOnce(void) {
  testlib_enable_tmp_setup_teardown();
  ASSERT_SYS(0, 0, pledge("stdio rpath wpath cpath fattr", 0));
}

TEST(unlink, efault) {
  ASSERT_SYS(EFAULT, -1, unlink(0));
  if (IsWindows() || !IsAsan()) return;  // not possible
  ASSERT_SYS(EFAULT, -1, unlink((void *)77));
}

TEST(unlink, enoent) {
  ASSERT_SYS(ENOENT, -1, unlink(""));
  ASSERT_SYS(ENOENT, -1, unlink("doesnotexist"));
  ASSERT_SYS(ENOENT, -1, unlink("o/doesnotexist"));
}

TEST(unlink, enotdir) {
  ASSERT_SYS(0, 0, touch("o", 0644));
  ASSERT_SYS(ENOTDIR, -1, unlink("o/doesnotexist"));
}

TEST(rmdir, willDeleteRegardlessOfAccessBits) {
  ASSERT_SYS(0, 0, mkdir("foo", 0));
  ASSERT_SYS(0, 0, rmdir("foo/"));
}

TEST(unlink, willDeleteRegardlessOfAccessBits) {
  ASSERT_SYS(0, 0, touch("foo", 0));
  ASSERT_SYS(0, 0, unlink("foo"));
}

TEST(unlinkat, test) {
  int i, fd;
  EXPECT_EQ(0, touch("mytmp", 0644));
  EXPECT_EQ(0, unlinkat(AT_FDCWD, "mytmp", 0));
  for (i = 0; i < 8; ++i) {
    EXPECT_NE(-1, (fd = creat("mytmp", 0644)));
    EXPECT_EQ(0, close(fd));
    EXPECT_EQ(0, unlinkat(AT_FDCWD, "mytmp", 0));
  }
}
