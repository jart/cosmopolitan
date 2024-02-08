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
#include "libc/calls/struct/stat.h"
#include "libc/assert.h"
#include "libc/calls/calls.h"
#include "libc/calls/internal.h"
#include "libc/calls/struct/metastat.internal.h"
#include "libc/calls/struct/stat.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/limits.h"
#include "libc/mem/gc.h"
#include "libc/nt/files.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/at.h"
#include "libc/sysv/consts/nr.h"
#include "libc/testlib/ezbench.h"
#include "libc/testlib/testlib.h"

__static_yoink("zipos");

void SetUpOnce(void) {
  testlib_enable_tmp_setup_teardown();
}

TEST(stat_010, testEmptyFile_sizeIsZero) {
  struct stat st;
  memset(&st, -1, sizeof(st));
  ASSERT_SYS(0, 0, close(creat("hi", 0644)));
  EXPECT_SYS(0, 0, stat("hi", &st));
  EXPECT_EQ(0, st.st_size);
}

TEST(stat, enoent) {
  struct stat st;
  ASSERT_SYS(ENOENT, -1, stat("hi", &st));
  ASSERT_SYS(ENOENT, -1, stat("o/doesnotexist", &st));
}

TEST(stat, enotdir) {
  struct stat st;
  ASSERT_SYS(0, 0, close(creat("yo", 0644)));
  ASSERT_SYS(ENOTDIR, -1, stat("yo/there", &st));
}

TEST(stat, textFileIsntExecutable) {
  struct stat st;
  ASSERT_SYS(0, 0, touch("foo.txt", 0644));
  ASSERT_SYS(0, 0, stat("foo.txt", &st));
  ASSERT_FALSE(st.st_mode & 0111);
}

TEST(stat, shebangIsExecutable) {
  struct stat st;
  ASSERT_SYS(0, 3, creat("foo.sh", 0777));
  ASSERT_SYS(0, 2, write(3, "#!", 2));
  ASSERT_SYS(0, 0, close(3));
  ASSERT_SYS(0, 0, stat("foo.sh", &st));
  ASSERT_TRUE(!!(st.st_mode & 0111));
}

TEST(stat, portableExecutableIsExecutable) {
  struct stat st;
  ASSERT_SYS(0, 3, creat("foo.exe", 0777));
  ASSERT_SYS(0, 2, write(3, "MZ", 2));
  ASSERT_SYS(0, 0, close(3));
  ASSERT_SYS(0, 0, stat("foo.exe", &st));
  ASSERT_TRUE(!!(st.st_mode & 0111));
}

TEST(stat, zipos) {
  struct stat st;
  EXPECT_SYS(0, 0,
             stat("/zip/.python/test/"
                  "tokenize_tests-latin1-coding-cookie-and-utf8-bom-sig.txt",
                  &st));
  EXPECT_SYS(0, 0, stat("/zip", &st));
  EXPECT_SYS(0, 0, stat("/zip/", &st));
  EXPECT_SYS(0, 0, stat("/zip/.python", &st));
  EXPECT_SYS(0, 0, stat("/zip/.python/", &st));
}

BENCH(fstat, bench) {
  struct stat st;
  ASSERT_SYS(0, 3, creat("foo.sh", 0777));
  ASSERT_SYS(0, 2, write(3, "#!", 2));
  EZBENCH2("fstat() fs", donothing, fstat(3, &st));
  ASSERT_SYS(0, 0, close(3));
}

BENCH(stat, bench) {
  char path[PATH_MAX];
  struct stat st;
  union metastat ms;
  EXPECT_SYS(0, 0, makedirs(".python/test", 0755));
  EXPECT_SYS(0, 0,
             touch(".python/test/"
                   "tokenize_tests-latin1-coding-cookie-and-utf8-bom-sig.txt",
                   0644));
  EZBENCH2("stat() fs", donothing,
           stat(".python/test/"
                "tokenize_tests-latin1-coding-cookie-and-utf8-bom-sig.txt",
                &st));
  EZBENCH2("stat() zipos", donothing,
           stat("/zip/.python/test/"
                "tokenize_tests-latin1-coding-cookie-and-utf8-bom-sig.txt",
                &st));
  EZBENCH2("getcwd()", donothing, getcwd(path, PATH_MAX));
  EZBENCH2("__stat2cosmo", donothing, __stat2cosmo(&st, &ms));
}
