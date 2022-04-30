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
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/fmt/fmt.h"
#include "libc/log/check.h"
#include "libc/mem/mem.h"
#include "libc/runtime/runtime.h"
#include "libc/sysv/consts/o.h"
#include "libc/testlib/testlib.h"
#include "libc/x/x.h"

char testlib_enable_tmp_setup_teardown;

void SetUp(void) {
  errno = 0;
}

TEST(mkdir, testNothingExists_ENOENT) {
  EXPECT_EQ(-1, mkdir("yo/yo/yo", 0755));
  EXPECT_EQ(ENOENT, errno);
}

TEST(mkdir, testDirectoryComponentIsFile_ENOTDIR) {
  EXPECT_NE(-1, touch("yo", 0644));
  EXPECT_EQ(-1, mkdir("yo/yo/yo", 0755));
  EXPECT_EQ(ENOTDIR, errno);
}

TEST(mkdir, testPathIsFile_EEXIST) {
  EXPECT_NE(-1, mkdir("yo", 0755));
  EXPECT_NE(-1, mkdir("yo/yo", 0755));
  EXPECT_NE(-1, touch("yo/yo/yo", 0644));
  EXPECT_EQ(-1, mkdir("yo/yo/yo", 0755));
  EXPECT_EQ(EEXIST, errno);
}

TEST(mkdir, testPathIsDirectory_EEXIST) {
  EXPECT_NE(-1, mkdir("yo", 0755));
  EXPECT_NE(-1, mkdir("yo/yo", 0755));
  EXPECT_NE(-1, mkdir("yo/yo/yo", 0755));
  EXPECT_EQ(-1, mkdir("yo/yo/yo", 0755));
  EXPECT_EQ(EEXIST, errno);
}

TEST(mkdir, enametoolong) {
  int i;
  size_t n = 2048;
  char *d, *s = gc(calloc(1, n));
  for (i = 0; i < n - 1; ++i) s[i] = 'x';
  s[i] = 0;
  EXPECT_SYS(ENAMETOOLONG, -1, mkdir(s, 0644));
}

TEST(makedirs, testEmptyString_EEXIST) {
  EXPECT_EQ(-1, mkdir("", 0755));
  EXPECT_EQ(ENOENT, errno);
}

TEST(mkdirat, testRelativePath_opensRelativeToDirFd) {
  int dirfd;
  ASSERT_NE(-1, mkdir("foo", 0755));
  ASSERT_NE(-1, (dirfd = open("foo", O_RDONLY | O_DIRECTORY)));
  EXPECT_NE(-1, mkdirat(dirfd, "bar", 0755));
  EXPECT_TRUE(isdirectory("foo/bar"));
  EXPECT_EQ(-1, makedirs("", 0755));
  EXPECT_NE(-1, close(dirfd));
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
