/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2023 Justine Alexandra Roberts Tunney                              │
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
#include "libc/limits.h"
#include "libc/mem/gc.h"
#include "libc/runtime/runtime.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/at.h"
#include "libc/sysv/consts/o.h"
#include "libc/temp.h"
#include "libc/testlib/testlib.h"
#include "libc/x/x.h"

void SetUpOnce(void) {
  testlib_enable_tmp_setup_teardown();
}

TEST(openatemp, test) {
  char path[] = "foo.XXXXXX";
  ASSERT_SYS(0, 3, openatemp(AT_FDCWD, path, 0, O_CLOEXEC, 0));
  ASSERT_NE(0, strcmp(path, "foo.XXXXXX"));
  EXPECT_TRUE(fileexists(path));
  EXPECT_SYS(0, 0, close(3));
}

TEST(openatemp, unlink) {
  char path[] = "foo.XXXXXX";
  ASSERT_SYS(0, 3, openatemp(AT_FDCWD, path, 0, O_UNLINK, 0));
  EXPECT_SYS(0, 0, close(3));
  EXPECT_FALSE(fileexists(path));
}

TEST(openatemp, mode) {
  if (IsWindows()) return;
  unsigned omask = umask(0);
  char path[] = "foo.XXXXXX";
  ASSERT_SYS(0, 3, openatemp(AT_FDCWD, path, 0, 0, 0764));
  struct stat st;
  ASSERT_SYS(0, 0, stat(path, &st));
  ASSERT_SYS(0, 0, fstat(3, &st));
  EXPECT_EQ(0100764, st.st_mode);
  EXPECT_SYS(0, 0, close(3));
  umask(omask);
}

TEST(openatemp, tooFewX_EINVAL) {
  char path[] = "foo.XXXXX";
  ASSERT_SYS(EINVAL, -1, openatemp(AT_FDCWD, path, 0, O_CLOEXEC, 0));
}

TEST(openatemp, suffixTooFewX_EINVAL) {
  char path[] = "foo.XXXXX.txt";
  ASSERT_SYS(EINVAL, -1, openatemp(AT_FDCWD, path, 4, O_CLOEXEC, 0));
}

TEST(openatemp, badSuffixLen_EINVAL) {
  char path[] = "foo.XXXXXX";
  ASSERT_SYS(EINVAL, -1, openatemp(AT_FDCWD, path, -1, O_CLOEXEC, 0));
  ASSERT_SYS(EINVAL, -1, openatemp(AT_FDCWD, path, -6, O_CLOEXEC, 0));
  ASSERT_SYS(EINVAL, -1, openatemp(AT_FDCWD, path, 11, O_CLOEXEC, 0));
  ASSERT_SYS(EINVAL, -1, openatemp(AT_FDCWD, path, 99, O_CLOEXEC, 0));
}

TEST(openatemp, missingDir_ENOENT) {
  char path[] = "missing/foo.XXXXXX";
  ASSERT_SYS(ENOENT, -1, openatemp(AT_FDCWD, path, 0, O_CLOEXEC, 0));
}

TEST(openatemp, notDir_ENOTDIR) {
  char path[] = "regular/foo.XXXXXX";
  ASSERT_SYS(0, 0, touch("regular", 0644));
  ASSERT_SYS(ENOTDIR, -1, openatemp(AT_FDCWD, path, 0, O_CLOEXEC, 0));
}

TEST(mktemp, fails_returnsEmptyString) {
  char path[] = "regular/foo.XXXXXX";
  ASSERT_SYS(0, 0, touch("regular", 0644));
  ASSERT_STREQ("", mktemp(path));
  ASSERT_STREQ("", path);
  ASSERT_EQ(ENOTDIR, errno);
  errno = 0;
}

TEST(mkstemp, fails_returnsNull) {
  char path[] = "regular/foo.XXXXXX";
  ASSERT_SYS(0, 0, touch("regular", 0644));
  ASSERT_SYS(ENOTDIR, -1, mkstemp(path));
  ASSERT_STREQ("regular/foo.XXXXXX", path);
}
