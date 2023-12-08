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
#include "libc/assert.h"
#include "libc/calls/calls.h"
#include "libc/calls/internal.h"
#include "libc/calls/struct/iovec.h"
#include "libc/errno.h"
#include "libc/nt/files.h"
#include "libc/sysv/consts/o.h"
#include "libc/testlib/testlib.h"

void SetUpOnce(void) {
  testlib_enable_tmp_setup_teardown();
}

TEST(preadv, ebadf) {
  EXPECT_SYS(EBADF, -1, preadv(-1, 0, 0, 0));
  EXPECT_SYS(EBADF, -1, pwritev(-1, 0, 0, 0));
}

TEST(preadv, testOneIovecWorks_itDelegatesToPread) {
  char b1[2] = "hi";
  struct iovec v[1] = {{b1, 2}};
  ASSERT_SYS(0, 3, open("foo", O_RDWR | O_CREAT | O_TRUNC, 0644));
  EXPECT_SYS(0, 2, pwritev(3, v, 1, 0));
  b1[0] = 0;
  b1[1] = 0;
  EXPECT_SYS(0, 0, preadv(3, 0, 0, 0));
  EXPECT_SYS(0, 0, pwritev(3, 0, 0, 0));
  EXPECT_SYS(0, 2, preadv(3, v, 1, 0));
  ASSERT_EQ('h', b1[0]);
  ASSERT_EQ('i', b1[1]);
  b1[0] = 0;
  b1[1] = 0x55;
  EXPECT_SYS(0, 1, preadv(3, v, 1, 1));
  ASSERT_EQ('i', b1[0]);
  ASSERT_EQ(0x55, b1[1]);
  ASSERT_SYS(0, 0, close(3));
}

TEST(preadv, testTwoIovecWorks_itMustDoItsThing) {
  char b1[1] = "h";
  char b2[1] = "i";
  struct iovec v[2] = {{b1, 1}, {b2, 1}};
  ASSERT_SYS(0, 3, open("foo", O_RDWR | O_CREAT | O_TRUNC, 0644));
  EXPECT_SYS(0, 2, pwritev(3, v, 2, 0));
  b1[0] = 0;
  b2[0] = 0;
  EXPECT_SYS(0, 2, preadv(3, v, 2, 0));
  ASSERT_EQ('h', b1[0]);
  ASSERT_EQ('i', b2[0]);
  b1[0] = 0;
  b2[0] = 0x55;
  EXPECT_SYS(0, 1, preadv(3, v, 2, 1));
  ASSERT_EQ('i', b1[0]);
  ASSERT_EQ(0x55, b2[0]);
  ASSERT_SYS(0, 0, close(3));
}

TEST(pwritev, writePastEof_zeroExtendsFile_alsoDoesntChangeFilePointer) {
  char b[6] = {0x55, 0x55, 0x55, 0x55, 0x55, 0x55};
  char s[2] = "hi";
  struct iovec v[1] = {{s, 2}};
  ASSERT_SYS(0, 3, open("foo", O_RDWR | O_CREAT | O_TRUNC, 0644));
  ASSERT_SYS(EINVAL, -1, pwritev(3, v, 1, -999));
  ASSERT_SYS(0, 2, pwritev(3, v, 1, 0));
  ASSERT_SYS(0, 2, pwritev(3, v, 1, 4));
  ASSERT_SYS(0, 2, preadv(3, v, 1, 0));
  ASSERT_SYS(0, 6, read(3, b, 6));
  ASSERT_EQ('h', b[0]);
  ASSERT_EQ('i', b[1]);
  ASSERT_EQ(0, b[2]);
  ASSERT_EQ(0, b[3]);
  ASSERT_EQ('h', b[4]);
  ASSERT_EQ('i', b[5]);
  ASSERT_SYS(0, 0, close(3));
}
