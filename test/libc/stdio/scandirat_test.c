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
#include "libc/errno.h"
#include "libc/intrin/kprintf.h"
#include "libc/mem/mem.h"
#include "libc/stdio/stdio.h"
#include "libc/sysv/consts/at.h"
#include "libc/sysv/consts/dt.h"
#include "libc/testlib/testlib.h"

__static_yoink("zipos");

void SetUpOnce(void) {
  testlib_enable_tmp_setup_teardown();
}

uint64_t ino(const char *path) {
  struct stat st;
  ASSERT_EQ(0, stat(path, &st));
  return st.st_ino;
}

TEST(scandir, test) {
  touch("b", 0644);
  touch("aaaaa", 0644);
  struct dirent **list;
  int n = scandir(".", &list, NULL, alphasort);
  ASSERT_EQ(4, n);
  ASSERT_STREQ(".", list[0]->d_name);
  ASSERT_EQ(24, list[0]->d_reclen);
  ASSERT_EQ(DT_DIR, list[0]->d_type);
  ASSERT_EQ(ino("."), list[0]->d_ino);
  ASSERT_STREQ("..", list[1]->d_name);
  ASSERT_EQ(24, list[1]->d_reclen);
  ASSERT_EQ(DT_DIR, list[1]->d_type);
  ASSERT_EQ(ino(".."), list[1]->d_ino);
  ASSERT_STREQ("aaaaa", list[2]->d_name);
  ASSERT_EQ(32, list[2]->d_reclen);
  ASSERT_EQ(DT_REG, list[2]->d_type);
  ASSERT_EQ(ino("aaaaa"), list[2]->d_ino);
  ASSERT_STREQ("b", list[3]->d_name);
  ASSERT_EQ(24, list[3]->d_reclen);
  ASSERT_EQ(DT_REG, list[3]->d_type);
  ASSERT_EQ(ino("b"), list[3]->d_ino);
  ASSERT_EQ(NULL, list[4]);
  while (n)
    free(list[--n]);
  free(list);
}

TEST(scandir, zip) {
  struct dirent **list;
  int n = scandir("/zip", &list, NULL, alphasort);
  ASSERT_EQ(4, n);
  ASSERT_STREQ(".", list[0]->d_name);
  ASSERT_EQ(24, list[0]->d_reclen);
  ASSERT_EQ(DT_DIR, list[0]->d_type);
  ASSERT_EQ(ino("/zip"), list[0]->d_ino);
  ASSERT_STREQ("..", list[1]->d_name);
  ASSERT_EQ(24, list[1]->d_reclen);
  ASSERT_EQ(DT_DIR, list[1]->d_type);
  ASSERT_EQ(ino("/"), list[1]->d_ino);
  ASSERT_STREQ(".cosmo", list[2]->d_name);
  ASSERT_EQ(32, list[2]->d_reclen);
  ASSERT_EQ(DT_REG, list[2]->d_type);
  ASSERT_EQ(ino("/zip/.cosmo"), list[2]->d_ino);
  ASSERT_STREQ("echo", list[3]->d_name);
  ASSERT_EQ(24, list[3]->d_reclen);
  ASSERT_EQ(DT_REG, list[3]->d_type);
  ASSERT_EQ(ino("/zip/echo"), list[3]->d_ino);
  ASSERT_EQ(NULL, list[4]);
  while (n)
    free(list[--n]);
  free(list);
}

TEST(scandir, huge) {
  char name[8];
  int count = 256;
  for (int i = 0; i < count; ++i) {
    sprintf(name, "%07d", i);
    touch(name, 0644);
  }
  struct dirent **list;
  int n = scandir(".", &list, NULL, alphasort);
  ASSERT_EQ(count + 2, n);
  ASSERT_STREQ(".", list[0]->d_name);
  ASSERT_STREQ("..", list[1]->d_name);
  for (int i = 0; i < count; ++i) {
    sprintf(name, "%07d", i);
    ASSERT_STREQ(name, list[i + 2]->d_name);
  }
  while (n)
    free(list[--n]);
  free(list);
}
