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
#include "libc/calls/struct/stat.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/limits.h"
#include "libc/log/log.h"
#include "libc/mem/gc.h"
#include "libc/mem/gc.h"
#include "libc/runtime/symbols.internal.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/at.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/s.h"
#include "libc/testlib/testlib.h"
#include "libc/x/x.h"

void SetUpOnce(void) {
  testlib_enable_tmp_setup_teardown();
  ASSERT_SYS(0, 0, pledge("stdio rpath wpath cpath fattr", 0));
}

TEST(readlink, enoent) {
  char buf[32];
  ASSERT_SYS(ENOENT, -1, readlink("doesnotexist", buf, 32));
  ASSERT_SYS(ENOENT, -1, readlink("o/doesnotexist", buf, 32));
}

TEST(readlink, enotdir) {
  char buf[32];
  ASSERT_SYS(0, 0, touch("o", 0644));
  ASSERT_SYS(ENOTDIR, -1, readlink("o/", buf, 32));
  ASSERT_SYS(ENOTDIR, -1, readlink("o/o/..", buf, 32));
  ASSERT_SYS(ENOTDIR, -1, readlink("o/doesnotexist", buf, 32));
}

TEST(readlinkat, test) {
  char buf[128];
  memset(buf, -1, sizeof(buf));
  ASSERT_NE(-1, xbarf("hello→", "hi", -1));
  ASSERT_STREQ("hi", gc(xslurp("hello→", 0)));
  ASSERT_NE(-1, symlink("hello→", "there→"));
  ASSERT_TRUE(isregularfile("hello→"));
  ASSERT_TRUE(issymlink("there→"));
  ASSERT_FALSE(isregularfile("there→"));
  ASSERT_SYS(0, 8, readlink("there→", buf, sizeof(buf)));
  EXPECT_EQ(255, buf[8] & 255);
  buf[8] = 0;
  EXPECT_STREQ("hello→", buf);
}

TEST(readlinkat, efault) {
  char buf[128];
  ASSERT_SYS(EFAULT, -1, readlink(0, buf, sizeof(buf)));
}

TEST(readlinkat, notexist) {
  char buf[128];
  ASSERT_SYS(ENOENT, -1, readlink("hi", buf, sizeof(buf)));
}

TEST(readlinkat, notalink) {
  char buf[128];
  ASSERT_SYS(0, 0, close(creat("hi", 0644)));
  ASSERT_SYS(EINVAL, -1, readlink("hi", buf, sizeof(buf)));
}

TEST(readlinkat, frootloop) {
  int fd;
  char buf[128];
  ASSERT_SYS(0, 0, symlink("froot", "froot"));
  ASSERT_SYS(ELOOP, -1, readlink("froot/loop", buf, sizeof(buf)));
  if (O_NOFOLLOW) {
    ASSERT_SYS(ELOOP, -1, open("froot", O_RDONLY | O_NOFOLLOW));
    if (0 && O_PATH) { /* need rhel5 test */
      ASSERT_NE(-1, (fd = open("froot", O_RDONLY | O_NOFOLLOW | O_PATH)));
      ASSERT_NE(-1, close(fd));
    }
  }
}

TEST(readlinkat, statReadsNameLength_countsUtf8Bytes) {
  struct stat st;
  ASSERT_SYS(0, 0, symlink("froÒt", "froÒt"));
  ASSERT_SYS(0, 0, fstatat(AT_FDCWD, "froÒt", &st, AT_SYMLINK_NOFOLLOW));
  EXPECT_TRUE(S_ISLNK(st.st_mode));
  EXPECT_EQ(6, st.st_size);
}

TEST(readlinkat, realpathReturnsLongPath) {
  char buf[PATH_MAX];
  if (!IsWindows()) return;
  if (!startswith(getcwd(buf, PATH_MAX), "/c/")) return;
  ASSERT_SYS(0, 0, touch("froot", 0644));
  ASSERT_STARTSWITH("/c/", realpath("froot", buf));
}
