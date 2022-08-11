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
#include "libc/errno.h"
#include "libc/sysv/consts/at.h"
#include "libc/testlib/testlib.h"
#include "libc/x/x.h"

char testlib_enable_tmp_setup_teardown;

void SetUpOnce(void) {
  ASSERT_SYS(0, 0, pledge("stdio rpath wpath cpath fattr", 0));
}

TEST(rename, enoent) {
  EXPECT_SYS(ENOENT, -1, rename("foo", ""));
  EXPECT_SYS(ENOENT, -1, rename("", "foo"));
  EXPECT_SYS(ENOENT, -1, rename("foo", "o/bar"));
  EXPECT_SYS(ENOENT, -1, rename("o/bar", "foo"));
}

TEST(renameat, enotdir) {
  EXPECT_SYS(0, 0, close(creat("yo", 0644)));
  EXPECT_SYS(ENOTDIR, -1, rename("yo/there", "hrcue"));
  // this test makes platforms crazy
  // EXPECT_SYS(ENOTDIR, -1, rename("zoo", "yo/there"));
}

TEST(renameat, testNull_returnsEfault) {
  ASSERT_SYS(0, 0, close(creat("hello", 0644)));
  EXPECT_SYS(EFAULT, -1, renameat(AT_FDCWD, 0, AT_FDCWD, 0));
  EXPECT_SYS(EFAULT, -1, renameat(AT_FDCWD, 0, AT_FDCWD, "hello"));
  EXPECT_SYS(EFAULT, -1, renameat(AT_FDCWD, "hello", AT_FDCWD, 0));
}

TEST(renameat, test) {
  ASSERT_SYS(0, 0, close(creat("first", 0644)));
  EXPECT_TRUE(fileexists("first"));
  EXPECT_TRUE(!fileexists("second"));
  EXPECT_SYS(0, 0, renameat(AT_FDCWD, "first", AT_FDCWD, "second"));
  EXPECT_TRUE(!fileexists("first"));
  EXPECT_TRUE(fileexists("second"));
}
