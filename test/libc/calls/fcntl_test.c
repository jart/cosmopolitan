/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
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
#include "libc/fmt/fmt.h"
#include "libc/log/check.h"
#include "libc/macros.internal.h"
#include "libc/runtime/runtime.h"
#include "libc/sysv/consts/f.h"
#include "libc/sysv/consts/fd.h"
#include "libc/sysv/consts/o.h"
#include "libc/testlib/testlib.h"
#include "libc/x/x.h"

char testlib_enable_tmp_setup_teardown;

void SetUpOnce(void) {
  ASSERT_SYS(0, 0, pledge("stdio rpath wpath cpath fattr", 0));
}

TEST(fcntl_getfl, testRemembersAccessMode) {
  int fd;
  ASSERT_NE(-1, (fd = open("foo", O_CREAT | O_RDWR, 0644)));
  EXPECT_EQ(O_RDWR, fcntl(fd, F_GETFL) & O_ACCMODE);
  EXPECT_NE(-1, close(fd));
  ASSERT_NE(-1, (fd = open("foo", O_WRONLY, 0644)));
  EXPECT_EQ(O_WRONLY, fcntl(fd, F_GETFL) & O_ACCMODE);
  EXPECT_NE(-1, close(fd));
}

TEST(fcntl_setfl, testChangeAppendStatus) {
  if (IsWindows()) {
    // no obvious way to do fcntl(fd, F_SETFL, O_APPEND)
    return;
  }
  int fd;
  char buf[8] = {0};
  ASSERT_NE(-1, (fd = open("foo", O_CREAT | O_RDWR, 0644)));
  EXPECT_EQ(3, write(fd, "foo", 3));
  EXPECT_NE(-1, lseek(fd, 0, SEEK_SET));
  EXPECT_NE(-1, fcntl(fd, F_SETFL, O_APPEND));
  EXPECT_EQ(3, write(fd, "bar", 3));
  EXPECT_NE(-1, lseek(fd, 0, SEEK_SET));
  EXPECT_NE(-1, fcntl(fd, F_SETFL, 0));
  EXPECT_EQ(6, read(fd, buf, 6));
  EXPECT_STREQ("foobar", buf);
  EXPECT_NE(-1, close(fd));
}

TEST(fcntl, getfd) {
  ASSERT_SYS(0, 3, open("/dev/null", O_RDWR));
  ASSERT_SYS(0, 0, fcntl(3, F_GETFD));
  ASSERT_SYS(0, 4, open("/dev/null", O_RDWR | O_CLOEXEC));
  ASSERT_SYS(0, FD_CLOEXEC, fcntl(4, F_GETFD));
  ASSERT_SYS(0, 0, close(4));
  ASSERT_SYS(0, 0, close(3));
}
