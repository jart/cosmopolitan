/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2025 Justine Alexandra Roberts Tunney                              │
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
#include "libc/calls/struct/dirent.h"
#include "libc/calls/struct/stat.h"
#include "libc/limits.h"
#include "libc/mem/gc.h"
#include "libc/mem/mem.h"
#include "libc/sysv/consts/o.h"
#include "libc/testlib/testlib.h"

void SetUpOnce(void) {
  testlib_enable_tmp_setup_teardown();
}

bool FileExists1(const char *path) {
  int fd = open(path, O_RDONLY);
  if (fd == -1)
    return false;
  close(fd);
  return true;
}

bool FileExists2(const char *path) {
  struct stat st;
  return !stat(path, &st);
}

TEST(sym, test) {
  EXPECT_SYS(0, 0, mkdir("dir", 0755));
  EXPECT_SYS(0, 0, touch("dir/hi", 0644));
  EXPECT_SYS(0, 0, symlink("dir", "sym"));
  EXPECT_TRUE(FileExists1("sym"));
  EXPECT_TRUE(FileExists1("sym/"));
  EXPECT_TRUE(FileExists1("sym/hi"));
  EXPECT_FALSE(FileExists1("sym/hi/"));
  EXPECT_TRUE(FileExists2("sym"));
  EXPECT_TRUE(FileExists2("sym/"));
  EXPECT_TRUE(FileExists2("sym/hi"));
  EXPECT_FALSE(FileExists2("sym/hi/"));
  struct dirent **list;
  int n = scandir("sym/", &list, NULL, alphasort);
  ASSERT_EQ(3, n);
  ASSERT_STREQ(".", list[0]->d_name);
  ASSERT_STREQ("..", list[1]->d_name);
  ASSERT_STREQ("hi", list[2]->d_name);
  ASSERT_EQ(NULL, list[3]);
  while (n)
    free(list[--n]);
  free(list);
}

TEST(sym, deep) {
  ASSERT_SYS(0, 0, mkdir("dir", 0755));
  ASSERT_SYS(0, 0, mkdir("dir/sub", 0755));
  ASSERT_SYS(0, 0, touch("dir/sub/hi", 0644));
  ASSERT_SYS(0, 0, symlink("dir/sub", "sym"));
  EXPECT_TRUE(FileExists1("dir/sub"));
  EXPECT_TRUE(FileExists1("sym"));
  EXPECT_TRUE(FileExists1("sym/"));
  EXPECT_TRUE(FileExists1("sym/hi"));
  EXPECT_FALSE(FileExists1("sym/hi/"));
  EXPECT_TRUE(FileExists2("sym"));
  EXPECT_TRUE(FileExists2("sym/"));
  EXPECT_TRUE(FileExists2("sym/hi"));
  EXPECT_FALSE(FileExists2("sym/hi/"));
  struct dirent **list;
  int n = scandir("sym/", &list, NULL, alphasort);
  ASSERT_EQ(3, n);
  ASSERT_STREQ(".", list[0]->d_name);
  ASSERT_STREQ("..", list[1]->d_name);
  ASSERT_STREQ("hi", list[2]->d_name);
  ASSERT_EQ(NULL, list[3]);
  while (n)
    free(list[--n]);
  free(list);
}

TEST(sym, omg1) {
  ASSERT_SYS(0, 0, mkdir("a", 0755));
  ASSERT_SYS(0, 0, mkdir("a/b", 0755));
  ASSERT_SYS(0, 0, touch("a/b/c", 0755));
  ASSERT_SYS(0, 0, symlink("b/c", "a/s"));
  EXPECT_TRUE(FileExists1("a/s"));
}

TEST(sym, omg2) {
  ASSERT_SYS(0, 0, mkdir("a", 0755));
  ASSERT_SYS(0, 0, mkdir("a/b", 0755));
  ASSERT_SYS(0, 0, mkdir("a/b/c", 0755));
  ASSERT_SYS(0, 0, symlink("b/c", "a/s"));
  EXPECT_TRUE(FileExists1("a/s"));
}

TEST(sym, omg3) {
  ASSERT_SYS(0, 0, mkdir("a", 0755));
  ASSERT_SYS(0, 0, mkdir("a/b", 0755));
  ASSERT_SYS(0, 0, mkdir("a/b/c", 0755));
  ASSERT_SYS(0, 0, symlink("b/c/", "a/s"));
  EXPECT_TRUE(FileExists1("a/s"));
}
