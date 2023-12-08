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
#include "libc/assert.h"
#include "libc/calls/calls.h"
#include "libc/calls/cp.internal.h"
#include "libc/calls/internal.h"
#include "libc/calls/struct/iovec.h"
#include "libc/calls/struct/iovec.internal.h"
#include "libc/calls/struct/rlimit.h"
#include "libc/calls/struct/sigaction.h"
#include "libc/calls/struct/sigset.h"
#include "libc/calls/syscall-sysv.internal.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/log/backtrace.internal.h"
#include "libc/runtime/runtime.h"
#include "libc/sock/internal.h"
#include "libc/sysv/consts/nr.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/rlimit.h"
#include "libc/sysv/consts/sig.h"
#include "libc/testlib/ezbench.h"
#include "libc/testlib/subprocess.h"
#include "libc/testlib/testlib.h"

void SetUpOnce(void) {
  testlib_enable_tmp_setup_teardown();
}

TEST(write, advancesFilePointer) {
  ASSERT_SYS(0, 3, creat("foo", 0644));
  ASSERT_SYS(0, 1, write(3, "x", 1));
  ASSERT_SYS(0, 1, lseek(3, 0, SEEK_CUR));
  ASSERT_SYS(0, 1, write(3, "y", 1));
  ASSERT_SYS(0, 2, lseek(3, 0, SEEK_CUR));
  ASSERT_SYS(0, 0, close(3));
}

TEST(write, notOpen_ebadf) {
  ASSERT_SYS(EBADF, -1, write(-1, 0, 0));
  ASSERT_SYS(EBADF, -1, write(+3, 0, 0));
}

TEST(write, readOnlyFd_ebadf) {
  ASSERT_SYS(0, 0, touch("foo", 0644));
  ASSERT_SYS(0, 3, open("foo", O_RDONLY));
  ASSERT_SYS(EBADF, -1, write(3, "x", 1));
  ASSERT_SYS(0, 0, close(3));
}

TEST(write, badMemory_efault) {
  ASSERT_SYS(EFAULT, -1, write(1, 0, 1));
  if (!IsAsan()) return;
  ASSERT_SYS(EFAULT, -1, write(1, (void *)1, 1));
}

TEST(write, brokenPipe_raisesSigpipe) {
  int fds[2];
  SPAWN(fork);
  signal(SIGPIPE, SIG_DFL);
  ASSERT_SYS(0, 0, pipe(fds));
  ASSERT_SYS(0, 1, write(4, "x", 1));
  ASSERT_SYS(0, 0, close(3));
  write(4, "x", 1);
  TERMS(SIGPIPE);
}

TEST(write, brokenPipe_sigpipeIgnored_returnsEpipe) {
  int fds[2];
  SPAWN(fork);
  signal(SIGPIPE, SIG_IGN);
  ASSERT_SYS(0, 0, pipe(fds));
  ASSERT_SYS(0, 1, write(4, "x", 1));
  ASSERT_SYS(0, 0, close(3));
  ASSERT_SYS(EPIPE, -1, write(4, "x", 1));
  ASSERT_SYS(0, 0, close(4));
  EXITS(0);
}

TEST(write, brokenPipe_sigpipeBlocked_returnsEpipe) {
  int fds[2];
  sigset_t mask;
  SPAWN(fork);
  signal(SIGPIPE, SIG_DFL);
  sigemptyset(&mask);
  sigaddset(&mask, SIGPIPE);
  sigprocmask(SIG_BLOCK, &mask, 0);
  ASSERT_SYS(0, 0, pipe(fds));
  ASSERT_SYS(0, 0, close(3));
  ASSERT_SYS(EPIPE, -1, write(4, "x", 1));
  ASSERT_SYS(0, 0, close(4));
  EXITS(0);
}

TEST(write, rlimitFsizeExceeded_raisesEfbig) {
  if (IsWindows()) return;  // not supported
  struct rlimit rl = {1, 10};
  SPAWN(fork);
  signal(SIGXFSZ, SIG_IGN);
  ASSERT_SYS(0, 0, setrlimit(RLIMIT_FSIZE, &rl));
  ASSERT_SYS(0, 3, creat("foo", 0644));
  ASSERT_SYS(0, 1, write(3, "x", 1));
  ASSERT_SYS(EFBIG, -1, write(3, "x", 1));
  ASSERT_SYS(0, 0, close(3));
  EXITS(0);
}

TEST(pwrite, testWritePastEof_extendsFile) {
  char buf[8] = {0};
  EXPECT_SYS(0, 3, creat("foo", 0644));
  EXPECT_SYS(0, 8, pwrite(3, buf, 8, 100));
  EXPECT_EQ(0, lseek(3, 0, SEEK_CUR));
  EXPECT_EQ(108, lseek(3, 0, SEEK_END));
  EXPECT_SYS(0, 0, close(3));
}

BENCH(write, bench) {
  ASSERT_SYS(0, 3, open("/dev/null", O_WRONLY));
  EZBENCH2("write", donothing, write(3, "hello", 5));
  EZBENCH2("writev", donothing, writev(3, &(struct iovec){"hello", 5}, 1));
  BEGIN_CANCELATION_POINT;
  EZBENCH2("sys_write", donothing, sys_write(3, "hello", 5));
  EZBENCH2("sys_writev", donothing,
           sys_writev(3, &(struct iovec){"hello", 5}, 1));
  END_CANCELATION_POINT;
  ASSERT_SYS(0, 0, close(3));
}
