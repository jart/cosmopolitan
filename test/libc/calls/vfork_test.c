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
#include "libc/calls/internal.h"
#include "libc/calls/state.internal.h"
#include "libc/runtime/runtime.h"
#include "libc/sysv/consts/o.h"
#include "libc/testlib/testlib.h"

void SetUpOnce(void) {
  testlib_enable_tmp_setup_teardown();
}

TEST(vfork, test) {
  int fd;
  char buf[8] = {0};
  ASSERT_NE(-1, (fd = open("vfork_test", O_RDWR | O_CREAT, 0644)));
  ASSERT_EQ(5, write(fd, "hello", 5));
  ASSERT_NE(-1, lseek(fd, 0, SEEK_SET));
  if (!vfork()) {
    EXPECT_EQ(5, pread(fd, buf, 5, 0));
    ASSERT_NE(-1, lseek(fd, 0, SEEK_SET));
    EXPECT_STREQ("hello", buf);
    EXPECT_NE(-1, close(fd));
    _exit(0);
  }
  EXPECT_EQ(0, __vforked);
  EXPECT_NE(-1, wait(0));
  EXPECT_EQ(5, read(fd, buf, 5));
  EXPECT_STREQ("hello", buf);
  EXPECT_NE(-1, close(fd));
}
