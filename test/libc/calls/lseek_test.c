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
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/limits.h"
#include "libc/log/check.h"
#include "libc/runtime/runtime.h"
#include "libc/sock/sock.h"
#include "libc/sysv/consts/af.h"
#include "libc/sysv/consts/ipproto.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/sock.h"
#include "libc/testlib/subprocess.h"
#include "libc/testlib/testlib.h"
#include "libc/x/x.h"

void SetUpOnce(void) {
  testlib_enable_tmp_setup_teardown();
  ASSERT_SYS(0, 0, pledge("stdio rpath wpath cpath fattr proc inet", 0));
}

/* TEST(lseek, ebadf) { */
/*   ASSERT_SYS(EBADF, -1, lseek(-1, 0, SEEK_SET)); */
/*   ASSERT_SYS(EBADF, -1, lseek(+3, 0, SEEK_SET)); */
/* } */

/* TEST(lseek, badWhence_einval) { */
/*   ASSERT_SYS(0, 3, creat("foo", 0644)); */
/*   ASSERT_SYS(EINVAL, -1, lseek(3, 0, -1)); */
/*   EXPECT_SYS(0, 0, close(3)); */
/* } */

TEST(lseek, negativeComputedOffset_einval) {
  ASSERT_SYS(0, 3, creat("foo", 0644));
  ASSERT_SYS(EINVAL, -1, lseek(3, -1, SEEK_SET));
  ASSERT_SYS(EINVAL, -1, lseek(3, -1, SEEK_CUR));
  ASSERT_SYS(0, 0, lseek(3, 0, SEEK_END));
  ASSERT_SYS(EINVAL, -1, lseek(3, -1, SEEK_END));
  ASSERT_SYS(0, 10, lseek(3, 10, SEEK_END));
  ASSERT_SYS(0, 5, lseek(3, 5, SEEK_END));
  EXPECT_SYS(0, 0, close(3));
}

/* TEST(lseek, 64bit) { */
/*   ASSERT_SYS(0, 3, creat("foo", 0644)); */
/*   ASSERT_SYS(0, 0x100000001, lseek(3, 0x100000001, SEEK_SET)); */
/*   EXPECT_SYS(0, 0, close(3)); */
/* } */

/* TEST(lseek, isPipe_ESPIPE) { */
/*   int fds[2]; */
/*   char buf[2]; */
/*   ASSERT_SYS(0, 0, pipe(fds)); */
/*   ASSERT_SYS(ESPIPE, -1, lseek(3, 0, SEEK_SET)); */
/*   ASSERT_SYS(ESPIPE, -1, pwrite(4, "hi", 2, 0)); */
/*   ASSERT_SYS(ESPIPE, -1, pread(3, buf, 2, 0)); */
/*   EXPECT_SYS(0, 0, close(4)); */
/*   EXPECT_SYS(0, 0, close(3)); */
/* } */

/* TEST(lseek, isSocket_ESPIPE) { */
/*   char buf[2]; */
/*   ASSERT_SYS(0, 3, socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)); */
/*   ASSERT_SYS(ESPIPE, -1, lseek(3, 0, SEEK_SET)); */
/*   ASSERT_SYS(ESPIPE, -1, pwrite(3, "hi", 2, 0)); */
/*   ASSERT_SYS(ESPIPE, -1, pread(3, buf, 2, 0)); */
/*   EXPECT_SYS(0, 0, close(3)); */
/* } */

/* TEST(lseek, filePositionChanges_areObservableAcrossDup) { */
/*   if (IsWindows()) return;  // do not want to support */
/*   ASSERT_SYS(0, 3, creat("wut", 0644)); */
/*   ASSERT_SYS(0, 4, dup(3)); */
/*   ASSERT_SYS(0, 0, lseek(3, 0, SEEK_CUR)); */
/*   ASSERT_SYS(0, 1, lseek(4, 1, SEEK_SET)); */
/*   ASSERT_SYS(0, 1, lseek(3, 0, SEEK_CUR)); */
/*   EXPECT_SYS(0, 0, close(4)); */
/*   EXPECT_SYS(0, 0, close(3)); */
/* } */

/* TEST(lseek, filePositionChanges_areObservableAcrossProcesses) { */
/*   if (IsWindows()) return;  // do not want to support */
/*   char buf[8] = {0}; */
/*   ASSERT_SYS(0, 3, open("wut", O_RDWR | O_CREAT, 0644)); */
/*   ASSERT_SYS(0, 3, write(3, "wut", 3)); */
/*   ASSERT_SYS(0, 0, lseek(3, 0, SEEK_SET)); */
/*   SPAWN(fork); */
/*   ASSERT_SYS(0, 1, lseek(3, 1, SEEK_SET)); */
/*   EXITS(0); */
/*   EXPECT_SYS(0, 1, read(3, buf, 1)); */
/*   EXPECT_EQ('u', buf[0]); */
/*   EXPECT_SYS(0, 0, close(3)); */
/* } */

/* TEST(lseek, beyondEndOfFile_isZeroExtendedUponSubsequentWrite) { */
/*   char buf[8] = {1, 1}; */
/*   ASSERT_SYS(0, 3, open("foo", O_RDWR | O_CREAT | O_TRUNC, 0644)); */
/*   ASSERT_SYS(0, 2, lseek(3, 2, SEEK_SET)); */
/*   ASSERT_SYS(0, 2, lseek(3, 0, SEEK_CUR)); */
/*   ASSERT_SYS(0, 0, pread(3, buf, 8, 0));  // lseek() alone doesn't extend */
/*   ASSERT_SYS(0, 2, write(3, buf, 2));     // does extend once i/o happens */
/*   ASSERT_SYS(0, 4, pread(3, buf, 8, 0)); */
/*   ASSERT_EQ(0, buf[0]); */
/*   ASSERT_EQ(0, buf[1]); */
/*   ASSERT_EQ(1, buf[2]); */
/*   ASSERT_EQ(1, buf[3]); */
/*   ASSERT_SYS(0, 0, close(3)); */
/* } */
