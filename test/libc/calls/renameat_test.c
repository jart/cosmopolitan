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
#include "libc/runtime/runtime.h"
#include "libc/sysv/consts/at.h"
#include "libc/testlib/testlib.h"
#include "libc/x/x.h"

void SetUpOnce(void) {
  testlib_enable_tmp_setup_teardown();
  ASSERT_SYS(0, 0, pledge("stdio rpath wpath cpath fattr", 0));
}

TEST(rename, enoent) {
  EXPECT_SYS(ENOENT, -1, rename("foo", ""));
  EXPECT_SYS(ENOENT, -1, rename("", "foo"));
  EXPECT_SYS(ENOENT, -1, rename("foo", "o/bar"));
  EXPECT_SYS(ENOENT, -1, rename("o/bar", "foo"));
}

TEST(renameat, enotdir) {
  EXPECT_SYS(0, 0, close(creat("yo", 0644)));
  EXPECT_SYS(ENOTDIR, -1, rename("yo/there", "hrcue"));
  // this test makes platforms crazy
  // EXPECT_SYS(ENOTDIR, -1, rename("zoo", "yo/there"));
}

TEST(rename, eisdir) {
  // new is a directory but old is not a directory
  ASSERT_SYS(0, 0, mkdir("foo", 0755));
  ASSERT_SYS(0, 0, touch("foo/bar", 0644));
  ASSERT_SYS(0, 0, touch("lol", 0644));
  ASSERT_SYS(EISDIR, -1, rename("lol", "foo"));
}

TEST(rename, enotdir) {
  // old is a directory but new is not a directory
  ASSERT_SYS(0, 0, mkdir("lol", 0755));
  ASSERT_SYS(0, 0, touch("foo", 0644));
  ASSERT_SYS(ENOTDIR, -1, rename("lol", "foo"));
}

TEST(rename, eisdir_again) {
  // old is a directory but new is not a directory
  ASSERT_SYS(0, 0, touch("foo", 0644));
  ASSERT_SYS(0, 0, touch("bar", 0644));
  ASSERT_SYS(ENOTDIR, -1, rename("foo/", "bar"));
  ASSERT_SYS(ENOTDIR, -1, rename("foo", "bar/"));
}

TEST(rename, moveDirectoryOverDirectory_replacesOldDirectory) {
  ASSERT_SYS(0, 0, mkdir("foo//", 0755));
  ASSERT_SYS(0, 0, mkdir("lol", 0755));
  ASSERT_SYS(0, 0, rename("lol", "foo"));
  ASSERT_TRUE(fileexists("foo"));
  ASSERT_FALSE(fileexists("lol"));
}

TEST(rename, enotempty) {
  // POSIX specifies EEXIST or ENOTEMPTY when new path is non-empty dir.
  // Old version of Linux (e.g. RHEL7) return EEXIST.
  // Everything else returns ENOTEMPTY.
  int rc;
  ASSERT_SYS(0, 0, mkdir("foo", 0755));
  ASSERT_SYS(0, 0, touch("foo/bar", 0644));
  ASSERT_SYS(0, 0, mkdir("lol", 0755));
  ASSERT_EQ(-1, (rc = rename("lol", "foo")));
  ASSERT_TRUE(errno == ENOTEMPTY || errno == EEXIST);
  errno = 0;
}

TEST(rename, moveIntoNonWritableDirectory_raisesEacces) {
  // old versions of linux allow this
  // new versions of linux report exdev?!
  if (IsLinux()) return;
  // netbsd and openbsd allow this
  if (IsNetbsd() || IsOpenbsd()) return;
  // windows doesn't really have permissions
  if (IsWindows()) return;
  // looks like a freebsd kernel bug
  if (IsAarch64() && IsFreebsd()) return;
  // posix specifies this behavior
  ASSERT_SYS(0, 0, mkdir("foo", 0111));
  ASSERT_SYS(0, 0, touch("lol", 0644));
  ASSERT_SYS(EACCES, -1, rename("lol", "foo/bar"));
}

TEST(renameat, testNull_returnsEfault) {
  ASSERT_SYS(0, 0, close(creat("hello", 0644)));
  ASSERT_SYS(EFAULT, -1, renameat(AT_FDCWD, 0, AT_FDCWD, 0));
  ASSERT_SYS(EFAULT, -1, renameat(AT_FDCWD, 0, AT_FDCWD, "hello"));
  ASSERT_SYS(EFAULT, -1, renameat(AT_FDCWD, "hello", AT_FDCWD, 0));
}

TEST(renameat, test) {
  ASSERT_SYS(0, 0, close(creat("first", 0644)));
  ASSERT_TRUE(fileexists("first"));
  ASSERT_TRUE(!fileexists("second"));
  ASSERT_SYS(0, 0, renameat(AT_FDCWD, "first", AT_FDCWD, "second"));
  ASSERT_TRUE(!fileexists("first"));
  ASSERT_TRUE(fileexists("second"));
}
