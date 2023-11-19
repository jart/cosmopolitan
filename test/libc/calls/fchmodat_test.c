/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
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
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/macros.internal.h"
#include "libc/sysv/consts/s.h"
// #include "libc/mem/gc.internal.h"
#include "libc/calls/struct/stat.h"
#include "libc/runtime/runtime.h"
#include "libc/sysv/consts/at.h"
#include "libc/sysv/consts/o.h"
#include "libc/testlib/testlib.h"
#include "libc/x/x.h"

void SetUpOnce(void) {
  testlib_enable_tmp_setup_teardown();
  // ASSERT_SYS(0, 0, pledge("stdio rpath wpath cpath", 0));
}

static void ExpectMode(const char *filename, uint32_t mode) {
  struct stat st;
  ASSERT_SYS(0, 0, fstatat(AT_FDCWD, filename, &st, AT_SYMLINK_NOFOLLOW));
  ASSERT_TRUE((st.st_mode & 0777) == mode);
}

TEST(fchmodat, testFchmodat) {
  ASSERT_SYS(0, 3,
             open("regfile", O_WRONLY | O_CREAT | O_EXCL | O_TRUNC, 0644));
  ASSERT_SYS(0, 0, close(3));
  ASSERT_SYS(0, 0, symlink("regfile", "symlink"));
  ExpectMode("regfile", 0644);
  struct stat st;
  ASSERT_SYS(0, 0, fstatat(AT_FDCWD, "symlink", &st, AT_SYMLINK_NOFOLLOW));
  uint32_t sym_mode = st.st_mode & 0777;
  ASSERT_SYS(0, 0, fchmodat(AT_FDCWD, "regfile", 0640, 0));
  ExpectMode("regfile", 0640);
  ASSERT_SYS(0, 0, fchmodat(AT_FDCWD, "regfile", 0600, AT_SYMLINK_NOFOLLOW));
  ExpectMode("regfile", 0600);
  ASSERT_SYS(0, 0, fchmodat(AT_FDCWD, "symlink", 0640, 0));
  ExpectMode("regfile", 0640);
  ExpectMode("symlink", sym_mode);
  int rc = fchmodat(AT_FDCWD, "symlink", 0600, AT_SYMLINK_NOFOLLOW);
  if (rc == -1) {
    ASSERT_TRUE(errno == ENOTSUP);
    errno = 0;
  } else {
    ExpectMode("symlink", 0600);
  }
  ExpectMode("regfile", 0640);
}
