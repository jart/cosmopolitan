/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2023 Justine Alexandra Roberts Tunney                              │
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
#include "libc/nt/files.h"
#include "libc/sysv/consts/f.h"
#include "libc/sysv/consts/o.h"
#include "libc/testlib/testlib.h"

int pipefd[2];
int stdoutBack;

void SetUpOnce(void) {
  testlib_enable_tmp_setup_teardown();
}

void CaptureStdout(void) {
  ASSERT_NE(-1, (stdoutBack = dup(1)));
  ASSERT_SYS(0, 0, pipe(pipefd));
  ASSERT_NE(-1, dup2(pipefd[1], 1));
}

void RestoreStdout(void) {
  ASSERT_SYS(0, 1, dup2(stdoutBack, 1));
  ASSERT_SYS(0, 0, close(stdoutBack));
  ASSERT_SYS(0, 0, close(pipefd[1]));
  ASSERT_SYS(0, 0, close(pipefd[0]));
}

TEST(specialfile, devNull) {
  ASSERT_SYS(0, 3, creat("/dev/null", 0644));
  // qemu-aarch64 defines o_largefile wrong
  ASSERT_EQ(O_WRONLY, fcntl(3, F_GETFL) & ~(O_LARGEFILE | 0x00008000));
  ASSERT_SYS(0, 2, write(3, "hi", 2));
  ASSERT_SYS(0, 2, pwrite(3, "hi", 2, 0));
  ASSERT_SYS(0, 2, pwrite(3, "hi", 2, 2));
  ASSERT_SYS(0, 0, lseek(3, 0, SEEK_END));
  ASSERT_SYS(0, 0, lseek(3, 0, SEEK_CUR));
  if (!IsLinux()) {
    // rhel7 doesn't have this behavior
    ASSERT_SYS(0, 2, lseek(3, 2, SEEK_CUR));
    ASSERT_SYS(0, 2, lseek(3, 2, SEEK_END));
  }
  ASSERT_SYS(0, 0, close(3));
}

TEST(specialfile, devNullRead) {
  char buf[8] = {0};
  ASSERT_SYS(0, 3, open("/dev/null", O_RDONLY));
  // qemu-aarch64 defines o_largefile wrong
  ASSERT_EQ(O_RDONLY, fcntl(3, F_GETFL) & ~(O_LARGEFILE | 0x00008000));
  ASSERT_SYS(0, 0, read(3, buf, 8));
  ASSERT_SYS(0, 0, close(3));
}

TEST(specialfile, devStdout) {
  char buf[8] = {8};
  CaptureStdout();
  ASSERT_SYS(0, 6, creat("/dev/stdout", 0644));
  ASSERT_SYS(0, 2, write(6, "hi", 2));
  ASSERT_EQ(2, read(pipefd[0], buf, 8));
  ASSERT_STREQ("hi", buf);
  ASSERT_SYS(ESPIPE, -1, pwrite(6, "hi", 2, 0));
  ASSERT_SYS(ESPIPE, -1, lseek(6, 0, SEEK_END));
  ASSERT_SYS(0, 0, close(6));
  RestoreStdout();
}
