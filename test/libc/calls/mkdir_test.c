/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
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
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/log/check.h"
#include "libc/mem/gc.h"
#include "libc/mem/mem.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/at.h"
#include "libc/sysv/consts/o.h"
#include "libc/testlib/testlib.h"
#include "libc/x/x.h"

void SetUpOnce(void) {
  testlib_enable_tmp_setup_teardown();
  ASSERT_SYS(0, 0, pledge("stdio rpath wpath cpath fattr", 0));
}

void SetUp(void) {
  errno = 0;
}

TEST(mkdir, testNothingExists_ENOENT) {
  EXPECT_SYS(ENOENT, -1, mkdir("yo/yo/yo", 0755));
}

TEST(mkdir, testDirectoryComponentIsFile_ENOTDIR) {
  EXPECT_SYS(0, 0, touch("yo", 0644));
  EXPECT_SYS(ENOTDIR, -1, mkdir("yo/yo/yo", 0755));
}

TEST(mkdir, testPathIsFile_EEXIST) {
  EXPECT_SYS(0, 0, mkdir("yo", 0755));
  EXPECT_SYS(0, 0, mkdir("yo/yo", 0755));
  EXPECT_SYS(0, 0, touch("yo/yo/yo", 0644));
  EXPECT_SYS(EEXIST, -1, mkdir("yo/yo/yo", 0755));
}

TEST(mkdir, testPathIsDirectory_EEXIST) {
  EXPECT_SYS(0, 0, mkdir("yo", 0755));
  EXPECT_SYS(0, 0, mkdir("yo/yo", 0755));
  EXPECT_SYS(0, 0, mkdir("yo/yo/yo", 0755));
  EXPECT_SYS(EEXIST, -1, mkdir("yo/yo/yo", 0755));
}

TEST(makedirs, pathExists_isSuccess) {
  EXPECT_SYS(0, 0, makedirs("foo/bar", 0755));
  EXPECT_SYS(0, 0, makedirs("foo/bar", 0755));
}

TEST(mkdir, enametoolong) {
  int i;
  size_t n = 2048;
  char *s = gc(calloc(1, n));
  for (i = 0; i < n - 1; ++i) s[i] = 'x';
  s[i] = 0;
  EXPECT_SYS(ENAMETOOLONG, -1, mkdir(s, 0644));
}

TEST(mkdir, testEmptyString_ENOENT) {
  EXPECT_SYS(ENOENT, -1, mkdir("", 0755));
}

TEST(mkdir, slashSlash) {
  EXPECT_SYS(0, 0, mkdirat(AT_FDCWD, "o//", 0777));
}

TEST(mkdirat, testRelativePath_opensRelativeToDirFd) {
  int dirfd;
  ASSERT_SYS(0, 0, mkdir("foo", 0755));
  ASSERT_SYS(0, 3, (dirfd = open("foo", O_RDONLY | O_DIRECTORY)));
  EXPECT_SYS(0, 0, mkdirat(dirfd, "bar", 0755));
  EXPECT_TRUE(isdirectory("foo/bar"));
  EXPECT_SYS(0, 0, close(dirfd));
}

TEST(mkdir, longname) {
  int i;
  char *d, s[270] = {0};
  for (i = 0; i < sizeof(s) - 1; ++i) s[i] = 'x';
  s[i] = 0;
  ASSERT_NE(NULL, (d = gc(getcwd(0, 0))));
  memcpy(s, d, strlen(d));
  s[strlen(d)] = '/';
  ASSERT_SYS(0, 0, mkdir(s, 0644));
}
