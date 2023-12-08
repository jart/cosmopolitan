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
#include "libc/calls/struct/flock.h"
#include "libc/calls/struct/sigaction.h"
#include "libc/calls/struct/sigset.h"
#include "libc/errno.h"
#include "libc/sysv/consts/at.h"
#include "libc/sysv/consts/f.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/sig.h"
#include "libc/testlib/subprocess.h"
#include "libc/testlib/testlib.h"

void SetUpOnce(void) {
  testlib_enable_tmp_setup_teardown();
}

TEST(lock, wholeFile) {
  ASSERT_SYS(0, 3, open("db", O_RDWR | O_CREAT | O_EXCL, 0644));
  ASSERT_SYS(0, 0, fcntl(3, F_SETLK, &(struct flock){.l_type = F_RDLCK}));
  ASSERT_SYS(0, 0, fcntl(3, F_SETLK, &(struct flock){.l_type = F_UNLCK}));
  ASSERT_SYS(0, 0, close(3));
}

TEST(lock, testUpgradeFromReadToWriteLock) {
  ASSERT_SYS(0, 3, open("db", O_RDWR | O_CREAT | O_EXCL, 0644));
  ASSERT_SYS(0, 0,
             fcntl(3, F_SETLK,
                   &(struct flock){
                       .l_type = F_RDLCK,
                       .l_start = 0x40000000,
                       .l_len = 1,
                   }));
  ASSERT_SYS(0, 0,
             fcntl(3, F_SETLK,
                   &(struct flock){
                       .l_type = F_WRLCK,
                       .l_start = 0x40000000,
                       .l_len = 1,
                   }));
  ASSERT_SYS(0, 0, close(3));
}

TEST(lock, testUpgradeWriteToWriteLock) {
  ASSERT_SYS(0, 3, open("db", O_RDWR | O_CREAT | O_EXCL, 0644));
  ASSERT_SYS(0, 0,
             fcntl(3, F_SETLK,
                   &(struct flock){
                       .l_type = F_WRLCK,
                       .l_start = 0x40000000,
                       .l_len = 1,
                   }));
  ASSERT_SYS(0, 0,
             fcntl(3, F_SETLK,
                   &(struct flock){
                       .l_type = F_WRLCK,
                       .l_start = 0x40000000,
                       .l_len = 1,
                   }));
  ASSERT_SYS(0, 0, close(3));
}

TEST(lock, unlockEverything_unlocksSmallerRanges) {
  int fd, pi[2];
  char buf[8] = {0};
  ASSERT_SYS(0, 3, creat("db", 0644));
  ASSERT_SYS(0, 0, close(3));
  ASSERT_SYS(0, 0, pipe(pi));
  SPAWN(fork);
  ASSERT_SYS(0, 5, open("db", O_RDWR));
  ASSERT_SYS(0, 0, close(4));
  ASSERT_SYS(0, 8, read(3, buf, 8));
  ASSERT_SYS(0, 0, close(3));
  ASSERT_SYS(0, 0,
             fcntl(5, F_SETLK,
                   &(struct flock){
                       .l_type = F_WRLCK,
                       .l_start = 0x40000000,
                       .l_len = 1,
                   }));
  ASSERT_SYS(0, 0,
             fcntl(5, F_SETLK,
                   &(struct flock){
                       .l_type = F_WRLCK,
                       .l_start = 0x40000005,
                       .l_len = 1,
                   }));
  ASSERT_SYS(0, 0, close(5));
  PARENT();
  ASSERT_SYS(0, 0, close(3));
  ASSERT_NE(-1, (fd = open("db", O_RDWR)));
  ASSERT_SYS(0, 0,
             fcntl(fd, F_SETLK,
                   &(struct flock){
                       .l_type = F_WRLCK,
                       .l_start = 0x40000000,
                       .l_len = 1,
                   }));
  ASSERT_SYS(0, 0,
             fcntl(fd, F_SETLK,
                   &(struct flock){
                       .l_type = F_WRLCK,
                       .l_start = 0x40000005,
                       .l_len = 1,
                   }));
  ASSERT_SYS(0, 0,
             fcntl(fd, F_SETLK,
                   &(struct flock){
                       .l_type = F_UNLCK,
                   }));
  ASSERT_SYS(0, 8, write(4, buf, 8));
  ASSERT_SYS(0, 0, close(4));
  WAIT(exit, 0);
  ASSERT_SYS(0, 0, close(fd));
}

TEST(lock, close_releasesLocks) {
  int fd, pi[2];
  char buf[8] = {0};
  ASSERT_SYS(0, 3, creat("db", 0644));
  ASSERT_SYS(0, 0, close(3));
  ASSERT_SYS(0, 0, pipe(pi));
  SPAWN(fork);
  ASSERT_SYS(0, 5, open("db", O_RDWR));
  ASSERT_SYS(0, 0, close(4));
  ASSERT_SYS(0, 8, read(3, buf, 8));
  ASSERT_SYS(0, 0, close(3));
  ASSERT_SYS(0, 0,
             fcntl(5, F_SETLK,
                   &(struct flock){
                       .l_type = F_WRLCK,
                       .l_start = 0x40000000,
                       .l_len = 1,
                   }));
  ASSERT_SYS(0, 0, close(5));
  PARENT();
  ASSERT_SYS(0, 0, close(3));
  ASSERT_NE(-1, (fd = open("db", O_RDWR)));
  ASSERT_SYS(0, 0,
             fcntl(fd, F_SETLK,
                   &(struct flock){
                       .l_type = F_WRLCK,
                       .l_start = 0x40000000,
                       .l_len = 1,
                   }));
  ASSERT_SYS(0, 0, close(fd));
  ASSERT_SYS(0, 8, write(4, buf, 8));
  ASSERT_SYS(0, 0, close(4));
  WAIT(exit, 0);
}
