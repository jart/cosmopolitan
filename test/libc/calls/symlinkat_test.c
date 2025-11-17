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
#include "libc/calls/struct/stat.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/fmt/itoa.h"
#include "libc/limits.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/rand.h"
#include "libc/stdio/stdio.h"
#include "libc/sysv/consts/at.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/s.h"
#include "libc/testlib/testlib.h"

char p[2][PATH_MAX];
struct stat st;

void SetUpOnce(void) {
  testlib_enable_tmp_setup_teardown();
  ASSERT_SYS(0, 0, pledge("stdio rpath wpath cpath fattr", 0));
}

TEST(symlink, enoent) {
  ASSERT_SYS(ENOENT, -1, symlink("", "foo"));
  ASSERT_SYS(ENOENT, -1, symlink("o/foo", ""));
  ASSERT_SYS(ENOENT, -1, symlink("o/foo", "o/bar"));
}

TEST(symlinkat, enotdir) {
  ASSERT_SYS(0, 0, close(creat("yo", 0644)));
  ASSERT_SYS(ENOTDIR, -1, symlink("hrcue", "yo/there"));
}

TEST(symlinkat, eexist) {
  ASSERT_SYS(0, 0, touch("foo", 0600));
  ASSERT_SYS(0, 0, touch("bar", 0600));
  ASSERT_SYS(EEXIST, -1, symlink("foo", "bar"));
  ASSERT_SYS(0, 0, symlink("bar", "sym"));
  ASSERT_SYS(EEXIST, -1, symlink("bar", "sym"));
}

TEST(symlinkat, presentReg) {
  char buf[3] = {0};
  char name[128] = {0};
  ASSERT_SYS(0, 0, mkdir("a", 0700));
  ASSERT_SYS(0, 0, mkdir("a/b", 0700));
  ASSERT_SYS(0, 3, creat("a/b/reg", 0600));
  ASSERT_SYS(0, 2, write(3, "hi", 2));
  ASSERT_SYS(0, 0, close(3));
  ASSERT_SYS(0, 0, symlink("b/reg", "a/lnk"));
  ASSERT_SYS(0, 3, open("a/lnk", O_RDONLY));
  ASSERT_SYS(0, 2, read(3, buf, 2));
  ASSERT_STREQ("hi", buf);
  ASSERT_SYS(0, 0, close(3));
  ASSERT_SYS(0, 5, readlink("a/lnk", name, 128));
  ASSERT_STREQ("b/reg", name);
}

TEST(symlinkat, presentDir) {
  char buf[3] = {0};
  char name[128] = {0};
  ASSERT_SYS(0, 0, mkdir("a", 0700));
  ASSERT_SYS(0, 0, mkdir("a/b", 0700));
  ASSERT_SYS(0, 3, creat("a/b/reg", 0600));
  ASSERT_SYS(0, 2, write(3, "hi", 2));
  ASSERT_SYS(0, 0, close(3));
  ASSERT_SYS(0, 0, symlink("b/", "a/lnk"));
  ASSERT_SYS(0, 3, open("a/lnk/reg", O_RDONLY));
  ASSERT_SYS(0, 2, read(3, buf, 2));
  ASSERT_STREQ("hi", buf);
  ASSERT_SYS(0, 0, close(3));
  ASSERT_SYS(0, 2, readlink("a/lnk", name, 128));
  ASSERT_STREQ("b/", name);
}

TEST(symlinkat, broken) {
  char name[128] = {0};
  ASSERT_SYS(0, 0, mkdir("a", 0700));
  ASSERT_SYS(0, 0, mkdir("a/b", 0700));
  ASSERT_SYS(0, 0, symlink("b/mis", "a/lnk"));
  ASSERT_SYS(ENOENT, -1, open("a/lnk", O_RDONLY));
  ASSERT_SYS(0, 5, readlink("a/lnk", name, 128));
  ASSERT_STREQ("b/mis", name);
}

TEST(symlinkat, broken2) {
  char name[128] = {0};
  ASSERT_SYS(0, 0, mkdir("a", 0700));
  ASSERT_SYS(0, 0, mkdir("a/b", 0700));
  ASSERT_SYS(0, 0, symlink("b/mis/", "a/lnk"));
  ASSERT_SYS(ENOENT, -1, open("a/lnk", O_RDONLY));
  ASSERT_NE(-1, readlink("a/lnk", name, 128));
  ASSERT_STREQ("b/mis/", name);
}

TEST(symlinkat, presentButBroken) {
  char name[128] = {0};
  ASSERT_SYS(0, 0, mkdir("a", 0700));
  ASSERT_SYS(0, 0, mkdir("a/b", 0700));
  ASSERT_SYS(0, 0, touch("a/b/reg", 0600));
  ASSERT_SYS(0, 0, symlink("b/reg/", "a/lnk"));
  // TODO(jart): Could we make this raise ENOTDIR on Windows?
  //             We must understand what Windows does here better.
  ASSERT_SYS(IsWindows() ? EISDIR : ENOTDIR, -1, open("a/lnk", O_RDONLY));
  ASSERT_SYS(0, 6, readlink("a/lnk", name, 128));
  ASSERT_STREQ("b/reg/", name);
}

TEST(symlinkat, test) {
  sprintf(p[0], "%s.%d", program_invocation_short_name, rand());
  sprintf(p[1], "%s.%d", program_invocation_short_name, rand());

  EXPECT_EQ(0, touch(p[0], 0644));
  EXPECT_EQ(0, symlink(p[0], p[1]));

  // check the normal file
  EXPECT_FALSE(issymlink(p[0]));
  EXPECT_EQ(0, lstat(p[0], &st));
  EXPECT_FALSE(S_ISLNK(st.st_mode));

  // check the symlink file
  EXPECT_TRUE(issymlink(p[1]));
  EXPECT_EQ(0, lstat(p[1], &st));
  EXPECT_TRUE(S_ISLNK(st.st_mode));

  // symlink isn't a symlink if we use it normally
  EXPECT_EQ(0, stat(p[1], &st));
  EXPECT_FALSE(S_ISLNK(st.st_mode));
}
