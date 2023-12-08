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
#include "libc/limits.h"
#include "libc/runtime/runtime.h"
#include "libc/sysv/consts/madv.h"
#include "libc/sysv/consts/posix.h"
#include "libc/testlib/testlib.h"
#include "libc/x/x.h"

void SetUpOnce(void) {
  testlib_enable_tmp_setup_teardown();
}

void SetUp(void) {
  if (IsOpenbsd() || IsXnu()) exit(0);
}

TEST(fadvise, ebadf) {
  ASSERT_SYS(EBADF, -1, fadvise(-1, 0, 0, MADV_SEQUENTIAL));
}

TEST(posix_fadvise, ebadf) {
  ASSERT_SYS(0, EBADF, posix_fadvise(-1, 0, 0, POSIX_FADV_SEQUENTIAL));
}

TEST(fadvise, test) {
  ASSERT_SYS(0, 0, xbarf("foo", "hello", -1));
  ASSERT_SYS(0, 3, open("foo", 0));
  ASSERT_SYS(0, 0, fadvise(3, 0, 0, MADV_SEQUENTIAL));
  ASSERT_SYS(0, 0, close(3));
}

TEST(posix_fadvise, test) {
  ASSERT_SYS(0, 0, xbarf("foo", "hello", -1));
  ASSERT_SYS(0, 3, open("foo", 0));
  ASSERT_SYS(0, 0, posix_fadvise(3, 0, 0, POSIX_FADV_SEQUENTIAL));
  ASSERT_SYS(0, 0, close(3));
}

TEST(fadvise, testBadAdvice) {
  ASSERT_SYS(0, 0, xbarf("foo", "hello", -1));
  ASSERT_SYS(0, 3, open("foo", 0));
  ASSERT_SYS(EINVAL, -1, fadvise(3, 0, 0, 127));
  ASSERT_SYS(0, 0, close(3));
}

TEST(posix_fadvise, testBadAdvice) {
  ASSERT_SYS(0, 0, xbarf("foo", "hello", -1));
  ASSERT_SYS(0, 3, open("foo", 0));
  ASSERT_SYS(0, EINVAL, posix_fadvise(3, 0, 0, 127));
  ASSERT_SYS(0, 0, close(3));
}

TEST(fadvise, testPastEof_isFine) {
  ASSERT_SYS(0, 0, xbarf("foo", "hello", -1));
  ASSERT_SYS(0, 3, open("foo", 0));
  ASSERT_SYS(0, 0, fadvise(3, 100, 100, MADV_SEQUENTIAL));
  ASSERT_SYS(0, 0, close(3));
}

TEST(fadvise, testNegativeLen_isInvalid) {
  ASSERT_SYS(0, 0, xbarf("foo", "hello", -1));
  ASSERT_SYS(0, 3, open("foo", 0));
  ASSERT_SYS(EINVAL, -1, fadvise(3, 0, INT64_MIN, MADV_SEQUENTIAL));
  ASSERT_SYS(0, 0, close(3));
}

TEST(posix_fadvise, testNegativeLen_isInvalid) {
  ASSERT_SYS(0, 0, xbarf("foo", "hello", -1));
  ASSERT_SYS(0, 3, open("foo", 0));
  ASSERT_SYS(0, EINVAL, posix_fadvise(3, 0, INT64_MIN, POSIX_FADV_SEQUENTIAL));
  ASSERT_SYS(0, 0, close(3));
}

TEST(fadvise, espipe) {
  int fds[2];
  ASSERT_SYS(0, 0, pipe(fds));
  ASSERT_SYS(ESPIPE, -1, fadvise(3, 0, 0, MADV_SEQUENTIAL));
  ASSERT_SYS(0, 0, close(4));
  ASSERT_SYS(0, 0, close(3));
}

TEST(posix_fadvise, espipe) {
  int fds[2];
  ASSERT_SYS(0, 0, pipe(fds));
  ASSERT_SYS(0, ESPIPE, posix_fadvise(3, 0, 0, POSIX_FADV_SEQUENTIAL));
  ASSERT_SYS(0, 0, close(4));
  ASSERT_SYS(0, 0, close(3));
}
