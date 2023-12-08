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
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/sysv/consts/o.h"
#include "libc/testlib/testlib.h"

void SetUpOnce(void) {
  testlib_enable_tmp_setup_teardown();
  ASSERT_SYS(0, 0, pledge("stdio rpath wpath cpath fattr", 0));
}

TEST(chdir, efault) {
  ASSERT_SYS(EFAULT, -1, chdir(0));
  if (IsWindows() || !IsAsan()) return;  // not possible
  ASSERT_SYS(EFAULT, -1, chdir((void *)77));
}

TEST(chdir, enoent) {
  ASSERT_SYS(ENOENT, -1, chdir(""));
  ASSERT_SYS(ENOENT, -1, chdir("doesnotexist"));
  ASSERT_SYS(ENOENT, -1, chdir("o/doesnotexist"));
}

TEST(chdir, enotdir) {
  ASSERT_SYS(0, 0, touch("o", 0644));
  ASSERT_SYS(ENOTDIR, -1, chdir("o/doesnotexist"));
}

TEST(chdir, test) {
  ASSERT_SYS(0, 0, mkdir("o", 0755));
  ASSERT_SYS(0, 0, touch("o/file", 0644));
  ASSERT_SYS(0, 3, open("o/file", O_RDONLY));
  ASSERT_SYS(0, 0, close(3));
  ASSERT_SYS(0, 0, chdir("o"));
  ASSERT_SYS(0, 3, open("file", O_RDONLY));
  ASSERT_SYS(0, 0, close(3));
  ASSERT_SYS(ENOENT, -1, open("o/file", O_RDONLY));
}
