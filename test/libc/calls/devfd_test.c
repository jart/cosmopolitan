/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2024 Justine Alexandra Roberts Tunney                              │
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
#include "libc/intrin/strace.internal.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/o.h"
#include "libc/testlib/testlib.h"
#include "libc/x/x.h"

void SetUpOnce(void) {
  testlib_enable_tmp_setup_teardown();
}

TEST(devfd, test) {
  // TODO: What is up with this mysterious ENOENT error?
  //       The code appears like it should support this.
  if (IsFreebsd()) return;
  char buf[8] = {0};
  struct stat st[2] = {0};
  ASSERT_SYS(0, 0, xbarf("hello.txt", "bone", -1));
  ASSERT_SYS(0, 3, open("hello.txt", O_RDONLY));
  ASSERT_SYS(0, 4, open("/dev/fd/3", O_RDONLY));
  ASSERT_SYS(0, 4, read(4, buf, 7));
  ASSERT_STREQ("bone", buf);
  ASSERT_SYS(0, 0, fstat(3, st));
  ASSERT_SYS(0, 0, fstat(4, st + 1));
  ASSERT_EQ(0, memcmp(st, st + 1, sizeof(struct stat)));
  ASSERT_SYS(0, 0, close(4));
  ASSERT_SYS(0, 0, close(3));
}

TEST(devfd, not_DEV_FD_STAT_BROKEN) {
  // fstat() and stat() are inconsistent on bsd systems
  // with xnu it only appears to be st_dev that differs
  if (IsBsd()) return;
  char buf[8] = {0};
  struct stat st[2] = {0};
  ASSERT_SYS(0, 0, xbarf("hello.txt", "bone", -1));
  ASSERT_SYS(0, 3, open("hello.txt", O_RDONLY));
  ASSERT_SYS(0, 4, open("/dev/fd/3", O_RDONLY));
  ASSERT_SYS(0, 4, read(4, buf, 7));
  ASSERT_STREQ("bone", buf);
  ASSERT_SYS(0, 0, fstat(3, st));
  ASSERT_SYS(0, 0, stat("/dev/fd/3", st + 1));
  ASSERT_EQ(0, memcmp(st, st + 1, sizeof(struct stat)));
  ASSERT_SYS(0, 0, close(4));
  ASSERT_SYS(0, 0, close(3));
}
