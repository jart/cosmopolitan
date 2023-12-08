/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
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
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/intrin/safemacros.internal.h"
#include "libc/limits.h"
#include "libc/mem/mem.h"
#include "libc/runtime/runtime.h"
#include "libc/testlib/testlib.h"

uint64_t i;
char *oldpath;
char tmp[PATH_MAX];
char pathbuf[PATH_MAX];

void SetUpOnce(void) {
  testlib_enable_tmp_setup_teardown();
  ASSERT_SYS(0, 0, pledge("stdio rpath wpath cpath fattr", 0));
}

void SetUp(void) {
  mkdir("bin", 0755);
  mkdir("home", 0755);
  oldpath = strdup(nulltoempty(getenv("PATH")));
  ASSERT_NE(-1, setenv("PATH", "bin", true));
}

void TearDown(void) {
  ASSERT_NE(-1, setenv("PATH", oldpath, true));
  free(oldpath);
}

TEST(commandv, testPathSearch) {
  EXPECT_SYS(0, 3, creat("bin/sh", 0755));
  EXPECT_SYS(0, 2, write(3, "MZ", 2));
  EXPECT_SYS(0, 0, close(3));
  EXPECT_STREQ("bin/sh", commandv("sh", pathbuf, sizeof(pathbuf)));
}

TEST(commandv, testSlashes_wontSearchPath_butChecksAccess) {
  EXPECT_SYS(0, 3, creat("home/sh.com", 0755));
  EXPECT_SYS(0, 2, write(3, "MZ", 2));
  EXPECT_SYS(0, 0, close(3));
  EXPECT_STREQ("home/sh.com",
               commandv("home/sh.com", pathbuf, sizeof(pathbuf)));
}

TEST(commandv, testSameDir_doesntHappenByDefaultUnlessItsWindows) {
  EXPECT_SYS(0, 3, creat("bog.com", 0755));
  EXPECT_SYS(0, 2, write(3, "MZ", 2));
  EXPECT_SYS(0, 0, close(3));
  EXPECT_STREQ(NULL, commandv("bog.com", pathbuf, sizeof(pathbuf)));
  EXPECT_EQ(ENOENT, errno);
}

TEST(commandv, testSameDir_willHappenWithColonBlank) {
  ASSERT_NE(-1, setenv("PATH", "bin:", true));
  EXPECT_SYS(0, 3, creat("bog.com", 0755));
  EXPECT_SYS(0, 2, write(3, "MZ", 2));
  EXPECT_SYS(0, 0, close(3));
  EXPECT_STREQ("bog.com", commandv("bog.com", pathbuf, sizeof(pathbuf)));
}

TEST(commandv, testSameDir_willHappenWithColonBlank2) {
  ASSERT_NE(-1, setenv("PATH", ":bin", true));
  EXPECT_SYS(0, 3, creat("bog.com", 0755));
  EXPECT_SYS(0, 2, write(3, "MZ", 2));
  EXPECT_SYS(0, 0, close(3));
  EXPECT_STREQ("bog.com", commandv("bog.com", pathbuf, sizeof(pathbuf)));
}

TEST(commandv, test_DirPaths_wontConsiderDirectoriesExecutable) {
  ASSERT_NE(-1, setenv("PATH", ":bin", true));
  EXPECT_SYS(0, 0, mkdir("Cursors", 0755));
  EXPECT_STREQ(NULL, commandv("Cursors", pathbuf, sizeof(pathbuf)));
  EXPECT_EQ(ENOENT, errno);
}

TEST(commandv, test_DirPaths_wontConsiderDirectoriesExecutable2) {
  ASSERT_NE(-1, setenv("PATH", ":bin", true));
  EXPECT_SYS(0, 0, mkdir("this_is_a_directory.com", 0755));
  EXPECT_STREQ(NULL,
               commandv("this_is_a_directory.com", pathbuf, sizeof(pathbuf)));
  EXPECT_EQ(ENOENT, errno);
}

TEST(commandv, test_nonExecutableFile_willEacces) {
  if (IsWindows()) return;  // TODO: fixme
  setenv("PATH", "foo", true);
  EXPECT_SYS(0, 0, mkdir("foo", 0755));
  EXPECT_SYS(0, 0, touch("foo/bar", 0400));
  EXPECT_SYS(EACCES, NULL, commandv("bar", pathbuf, sizeof(pathbuf)));
  EXPECT_SYS(EACCES, NULL, commandv("foo/bar", pathbuf, sizeof(pathbuf)));
}
