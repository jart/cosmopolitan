/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
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
#include "libc/macros.internal.h"
#include "libc/mem/gc.internal.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/o.h"
#include "libc/testlib/testlib.h"
#include "libc/x/x.h"

char testlib_enable_tmp_setup_teardown;

void SetUpOnce(void) {
  ASSERT_SYS(0, 0, pledge("stdio rpath wpath cpath fattr", 0));
}

TEST(open, efault) {
  ASSERT_SYS(EFAULT, -1, open(0, O_RDONLY));
  if (IsWindows() || !IsAsan()) return;  // not possible
  ASSERT_SYS(EFAULT, -1, open((void *)77, O_RDONLY));
}

TEST(open, enoent) {
  ASSERT_SYS(ENOENT, -1, open("", O_RDONLY));
  ASSERT_SYS(ENOENT, -1, open("doesnotexist", O_RDONLY));
  ASSERT_SYS(ENOENT, -1, open("o/doesnotexist", O_RDONLY));
}

TEST(open, enotdir) {
  ASSERT_SYS(0, 0, touch("o", 0644));
  ASSERT_SYS(ENOTDIR, -1, open("o/doesnotexist", O_RDONLY));
}

TEST(open, eexist) {
  ASSERT_SYS(0, 0, touch("exists", 0644));
  ASSERT_SYS(EEXIST, -1, open("exists", O_WRONLY | O_CREAT | O_EXCL, 0644));
}

TEST(open, doubleSlash_worksAndGetsNormalizedOnWindows) {
  ASSERT_SYS(0, 0, mkdir("o", 0755));
  ASSERT_SYS(0, 3,
             open(gc(xjoinpaths(gc(getcwd(0, 0)), "o//deleteme")),
                  O_WRONLY | O_CREAT | O_TRUNC, 0644));
  ASSERT_SYS(0, 0, close(3));
}

TEST(open, enametoolong) {
  size_t n = 260;
  char *s = gc(xcalloc(1, n + 1));
  memset(s, 'J', n);
  ASSERT_SYS(ENAMETOOLONG, -1, creat(s, 0644));
}

TEST(open, testSpaceInFilename) {
  char buf[8] = {0};
  ASSERT_SYS(0, 0, xbarf("hello txt", "hello", -1));
  ASSERT_SYS(0, 3, open("hello txt", O_WRONLY));
  EXPECT_SYS(0, 1, write(3, "H", 1));
  EXPECT_SYS(0, 0, close(3));
  ASSERT_SYS(0, 3, open("hello txt", O_RDONLY));
  EXPECT_SYS(0, 5, read(3, buf, 7));
  EXPECT_STREQ("Hello", buf);
  EXPECT_SYS(0, 0, close(3));
}

TEST(open, testOpenExistingForWriteOnly_seeksToStart) {
  char buf[8] = {0};
  ASSERT_SYS(0, 0, xbarf("hello.txt", "hello", -1));
  ASSERT_SYS(0, 3, open("hello.txt", O_WRONLY));
  EXPECT_SYS(0, 1, write(3, "H", 1));
  EXPECT_SYS(0, 0, close(3));
  ASSERT_SYS(0, 3, open("hello.txt", O_RDONLY));
  EXPECT_SYS(0, 5, read(3, buf, 7));
  EXPECT_STREQ("Hello", buf);
  EXPECT_SYS(0, 0, close(3));
}

TEST(open, testOpenExistingForReadWrite_seeksToStart) {
  char buf[8] = {0};
  ASSERT_SYS(0, 0, xbarf("hello.txt", "hello", -1));
  ASSERT_SYS(0, 3, open("hello.txt", O_RDWR));
  EXPECT_SYS(0, 1, write(3, "H", 1));
  EXPECT_SYS(0, 0, close(3));
  ASSERT_SYS(0, 3, open("hello.txt", O_RDONLY));
  EXPECT_SYS(0, 5, read(3, buf, 7));
  EXPECT_STREQ("Hello", buf);
  EXPECT_SYS(0, 0, close(3));
}

TEST(open, testOpenExistingForAppendWriteOnly_seeksToEnd) {
  char buf[16] = {0};
  ASSERT_SYS(0, 0, xbarf("hello.txt", "hell", -1));
  ASSERT_SYS(0, 3, open("hello.txt", O_WRONLY | O_APPEND));
  EXPECT_SYS(0, 1, write(3, "o", 1));
  EXPECT_SYS(0, 0, lseek(3, 0, SEEK_SET));
  EXPECT_SYS(0, 1, write(3, "!", 1));
  EXPECT_SYS(0, 0, close(3));
  ASSERT_SYS(0, 3, open("hello.txt", O_RDONLY));
  EXPECT_SYS(0, 6, read(3, buf, 8));
  EXPECT_STREQ("hello!", buf);
  EXPECT_SYS(0, 0, close(3));
}

TEST(open, testRelativePath_opensRelativeToDirFd) {
  ASSERT_SYS(0, 0, mkdir("foo", 0755));
  ASSERT_SYS(0, 3, open("foo", O_RDONLY | O_DIRECTORY));
  EXPECT_SYS(0, 4, openat(3, "bar", O_WRONLY | O_TRUNC | O_CREAT, 0755));
  EXPECT_TRUE(fileexists("foo/bar"));
  EXPECT_SYS(0, 0, close(4));
  EXPECT_SYS(0, 0, close(3));
}

int CountFds(void) {
  int i, count;
  for (count = i = 0; i < g_fds.n; ++i) {
    if (g_fds.p[i].kind) {
      ++count;
    }
  }
  return count;
}

TEST(open, lotsOfFds) {
  if (!IsWindows()) return;
  int i, n = 200;
  ASSERT_SYS(0, 0, xbarf("hello.txt", "hello", -1));
  for (i = 3; i < n; ++i) {
    EXPECT_EQ(i, CountFds());
    EXPECT_SYS(0, i, open("hello.txt", O_RDONLY));
  }
  for (i = 3; i < n; ++i) {
    EXPECT_SYS(0, 0, close(i));
  }
}
