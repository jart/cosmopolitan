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
#include "libc/fmt/libgen.h"
#include "libc/limits.h"
#include "libc/macros.internal.h"
#include "libc/mem/gc.h"
#include "libc/serialize.h"
#include "libc/str/str.h"
#include "libc/testlib/testlib.h"

void SetUpOnce(void) {
  testlib_enable_tmp_setup_teardown();
  ASSERT_SYS(0, 0, pledge("stdio rpath cpath fattr", 0));
}

TEST(__getcwd, zero) {
  if (IsQemuUser()) return;
  ASSERT_SYS(ERANGE, -1, __getcwd(0, 0));
}

TEST(__getcwd, returnsLengthIncludingNul) {
  char cwd1[PATH_MAX];
  char cwd2[PATH_MAX];
  ASSERT_NE(-1, __getcwd(cwd1, PATH_MAX));
  ASSERT_EQ(strlen(cwd1) + 1, __getcwd(cwd2, PATH_MAX));
}

TEST(__getcwd, tooShort_negOneReturned_bufferIsntModified) {
  char cwd[4] = {0x55, 0x55, 0x55, 0x55};
  ASSERT_SYS(ERANGE, -1, __getcwd(cwd, 4));
  ASSERT_EQ(0x55555555, READ32LE(cwd));
}

TEST(__getcwd, noRoomForNul) {
  char cwd1[PATH_MAX];
  char cwd2[PATH_MAX];
  ASSERT_NE(-1, __getcwd(cwd1, PATH_MAX));
  ASSERT_SYS(ERANGE, -1, __getcwd(cwd2, strlen(cwd1)));
}

TEST(__getcwd, alwaysStartsWithSlash) {
  char cwd[PATH_MAX];
  ASSERT_NE(-1, __getcwd(cwd, PATH_MAX));
  ASSERT_EQ('/', *cwd);
}

TEST(__getcwd, notInRootDir_neverEndsWithSlash) {
  char cwd[PATH_MAX];
  ASSERT_NE(-1, __getcwd(cwd, PATH_MAX));
  ASSERT_FALSE(endswith(cwd, "/"));
}

TEST(getcwd, test) {
  char buf[PATH_MAX];
  EXPECT_SYS(0, 0, mkdir("subdir", 0755));
  EXPECT_SYS(0, 0, chdir("subdir"));
  EXPECT_STREQ("subdir", basename(getcwd(buf, ARRAYLEN(buf))));
}

TEST(getcwd, testNullBuf_allocatesResult) {
  EXPECT_SYS(0, 0, mkdir("subdir", 0755));
  EXPECT_SYS(0, 0, chdir("subdir"));
  EXPECT_STREQ("subdir", basename(gc(getcwd(0, 0))));
}

TEST(getcwd, testWindows_addsFunnyPrefix) {
  if (!IsWindows()) return;
  char path[PATH_MAX];
  ASSERT_NE(0, getcwd(path, sizeof(path)));
  path[1] = tolower(path[1]);
  EXPECT_STARTSWITH("/c/", path);
}
