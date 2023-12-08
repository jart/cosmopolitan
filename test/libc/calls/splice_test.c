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
#include "libc/runtime/runtime.h"
#include "libc/sysv/consts/o.h"
#include "libc/testlib/testlib.h"

void SetUpOnce(void) {
  testlib_enable_tmp_setup_teardown();
}

void SetUp(void) {
  int e = errno;
  if (splice(-1, 0, -1, 0, 0, 0) == -1 && errno == ENOSYS) {
    exit(0);
  }
  errno = e;
}

TEST(splice, einval) {
  ASSERT_SYS(0, 0, splice(0, 0, 1, 0, 0, -1));
}

TEST(splice, espipe) {
  int64_t x;
  int fds[2];
  ASSERT_SYS(0, 0, pipe(fds));
  ASSERT_SYS(0, 0, splice(0, 0, 4, &x, 0, 0));
  ASSERT_SYS(0, 0, close(4));
  ASSERT_SYS(0, 0, close(3));
}

TEST(splice, noOffsets_usesFilePointer) {
  // this test fails on rhel5 and rhel7
  int fds[2];
  char buf[16] = {0};
  ASSERT_SYS(0, 0, pipe(fds));
  ASSERT_SYS(0, 5, open("foo", O_RDWR | O_CREAT | O_TRUNC, 0644));
  ASSERT_SYS(0, 5, pwrite(5, "hello", 5, 0));
  ASSERT_SYS(0, 5, splice(5, 0, 4, 0, 5, 0));
  ASSERT_SYS(0, 5, splice(3, 0, 5, 0, 5, 0));
  ASSERT_SYS(0, 10, pread(5, buf, sizeof(buf), 0));
  ASSERT_STREQ("hellohello", buf);
  ASSERT_SYS(0, 0, close(5));
  ASSERT_SYS(0, 0, close(4));
  ASSERT_SYS(0, 0, close(3));
}

TEST(splice, offsets_doesntChangePointerAndIsReadOnly) {
  int fds[2];
  int64_t x = 0;
  char buf[16] = {0};
  ASSERT_SYS(0, 0, pipe(fds));
  ASSERT_SYS(0, 5, open("foo", O_RDWR | O_CREAT | O_TRUNC, 0644));
  ASSERT_SYS(0, 5, pwrite(5, "hello", 5, 0));
  ASSERT_SYS(0, 5, splice(5, &x, 4, 0, 5, 0));
  ASSERT_EQ(5, x);
  ASSERT_SYS(0, 5, splice(3, 0, 5, &x, 5, 0));
  ASSERT_EQ(10, x);
  ASSERT_SYS(0, 10, read(5, buf, sizeof(buf)));
  ASSERT_STREQ("hellohello", buf);
  ASSERT_SYS(0, 0, close(5));
  ASSERT_SYS(0, 0, close(4));
  ASSERT_SYS(0, 0, close(3));
}
