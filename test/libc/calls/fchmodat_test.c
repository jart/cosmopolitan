/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2022 Stephen Gregoratto                                            │
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
#include "libc/macros.h"
#include "libc/runtime/runtime.h"
#include "libc/sysv/consts/at.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/s.h"
#include "libc/testlib/testlib.h"
#include "libc/x/x.h"

void SetUpOnce(void) {
  testlib_enable_tmp_setup_teardown();
  ASSERT_SYS(0, 0, pledge("stdio rpath wpath cpath fattr", 0));
}

TEST(chmod, unreadable) {
  if (getuid() == 0)
    return;

  // create file
  ASSERT_SYS(0, 3, creat("reg", 0644));
  ASSERT_SYS(0, 2, write(3, "hi", 2));
  ASSERT_SYS(0, 0, close(3));

  // make file unreadable
  ASSERT_SYS(0, 0, chmod("reg", 0));

  // verify we can't open it with read access
  ASSERT_SYS(EACCES, -1, open("reg", O_RDONLY));

  // verify stat() shows it's non-readable
  struct stat st;
  ASSERT_SYS(0, 0, stat("reg", &st));
  ASSERT_EQ(0000, st.st_mode & 0700);

  // make file readable
  ASSERT_SYS(0, 0, chmod("reg", 0400));

  // verify we can open it with read access
  ASSERT_SYS(0, 3, open("reg", O_RDONLY));
  ASSERT_SYS(0, 0, close(3));
}

TEST(chmod, creat_unreadable) {
  if (getuid() == 0)
    return;
  ASSERT_SYS(0, 3, creat("reg", 0));
  ASSERT_SYS(0, 2, write(3, "hi", 2));
  ASSERT_SYS(0, 0, close(3));
  ASSERT_SYS(EACCES, -1, open("reg", O_RDONLY));
  struct stat st;
  ASSERT_SYS(0, 0, stat("reg", &st));
  ASSERT_EQ(0000, st.st_mode & 0700);
}

TEST(chmod, unreadable_dir) {
  if (getuid() == 0)
    return;

  // create dir
  ASSERT_SYS(0, 0, mkdir("dir", 0700));

  // verify we can open it with read access
  ASSERT_SYS(0, 3, open("dir", O_RDONLY));
  ASSERT_SYS(0, 0, close(3));

  // make dir unreadable
  ASSERT_SYS(0, 0, chmod("dir", 0100));

  // verify we can't open it with read access
  ASSERT_SYS(EACCES, -1, open("dir", O_RDONLY));

  // verify stat() shows it's non-readable
  struct stat st;
  ASSERT_SYS(0, 0, stat("dir", &st));
  ASSERT_EQ(0100, st.st_mode & 0700);

  // make dir readable
  ASSERT_SYS(0, 0, chmod("dir", 0500));

  // verify stat() shows it's readable
  ASSERT_SYS(0, 0, stat("dir", &st));
  ASSERT_EQ(0500, st.st_mode & 0700);

  // verify we can open it with read access
  ASSERT_SYS(0, 3, open("dir", O_RDONLY));
  ASSERT_SYS(0, 0, close(3));
}

TEST(chmod, mkdir_unreadable) {
  if (getuid() == 0)
    return;
  ASSERT_SYS(0, 0, mkdir("dir", 0100));
  ASSERT_SYS(EACCES, -1, open("dir", O_RDONLY));
  struct stat st;
  ASSERT_SYS(0, 0, stat("dir", &st));
  ASSERT_EQ(0100, st.st_mode & 0700);
}

TEST(fchmodat, testFchmodat) {
  struct stat st;
  umask(022);
  ASSERT_SYS(0, 3,
             open("regfile", O_WRONLY | O_CREAT | O_EXCL | O_TRUNC, 0644));
  ASSERT_SYS(0, 0, close(3));
  ASSERT_SYS(0, 0, symlink("regfile", "symlink"));
  ASSERT_SYS(0, 0, fstatat(AT_FDCWD, "regfile", &st, AT_SYMLINK_NOFOLLOW));
  ASSERT_EQ(0600, st.st_mode & 0700);
  ASSERT_SYS(0, 0, fstatat(AT_FDCWD, "symlink", &st, AT_SYMLINK_NOFOLLOW));
  uint32_t sym_mode = st.st_mode & 0777;
  ASSERT_SYS(0, 0, fchmodat(AT_FDCWD, "regfile", 0640, 0));
  ASSERT_SYS(0, 0, fstatat(AT_FDCWD, "regfile", &st, AT_SYMLINK_NOFOLLOW));
  ASSERT_EQ(0600, st.st_mode & 0700);
  ASSERT_SYS(0, 0, fchmodat(AT_FDCWD, "regfile", 0600, AT_SYMLINK_NOFOLLOW));
  ASSERT_SYS(0, 0, fstatat(AT_FDCWD, "regfile", &st, AT_SYMLINK_NOFOLLOW));
  ASSERT_EQ(0600, st.st_mode & 0700);
  ASSERT_SYS(0, 0, fchmodat(AT_FDCWD, "symlink", 0640, 0));
  ASSERT_SYS(0, 0, fstatat(AT_FDCWD, "regfile", &st, AT_SYMLINK_NOFOLLOW));
  ASSERT_EQ(0600, st.st_mode & 0700);
  ASSERT_SYS(0, 0, fstatat(AT_FDCWD, "symlink", &st, AT_SYMLINK_NOFOLLOW));
  ASSERT_EQ(sym_mode, st.st_mode & 0777);
  int rc = fchmodat(AT_FDCWD, "symlink", 0600, AT_SYMLINK_NOFOLLOW);
  if (rc == -1) {
    ASSERT_EQ(ENOTSUP, errno);
    errno = 0;
  } else {
    ASSERT_SYS(0, 0, fstatat(AT_FDCWD, "symlink", &st, AT_SYMLINK_NOFOLLOW));
    ASSERT_EQ(0600, st.st_mode & 0700);
  }
  ASSERT_SYS(0, 0, fstatat(AT_FDCWD, "regfile", &st, AT_SYMLINK_NOFOLLOW));
  ASSERT_EQ(0600, st.st_mode & 0700);
}
