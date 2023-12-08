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
#include "libc/atomic.h"
#include "libc/calls/calls.h"
#include "libc/calls/cp.internal.h"
#include "libc/calls/internal.h"
#include "libc/calls/struct/iovec.h"
#include "libc/calls/struct/iovec.internal.h"
#include "libc/calls/struct/sigaction.h"
#include "libc/calls/syscall-sysv.internal.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/runtime/runtime.h"
#include "libc/sock/internal.h"
#include "libc/sysv/consts/nr.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/sig.h"
#include "libc/testlib/ezbench.h"
#include "libc/testlib/testlib.h"
#include "libc/thread/thread.h"

void SetUpOnce(void) {
  testlib_enable_tmp_setup_teardown();
}

TEST(read, eof) {
  char b[8] = "hello";
  ASSERT_SYS(0, 3, creat("foo", 0644));
  ASSERT_SYS(0, 4, open("foo", O_RDONLY));
  ASSERT_SYS(0, 0, read(4, b, 8));
  ASSERT_SYS(0, 8, write(3, b, 8));
  ASSERT_SYS(0, 8, read(4, b, 8));
  ASSERT_SYS(0, 0, read(4, b, 8));
  ASSERT_SYS(0, 0, close(4));
  ASSERT_SYS(0, 0, close(3));
}

volatile bool got_sigalrm;

void OnSigalrm(int sig) {
  got_sigalrm = true;
}

TEST(read_pipe, canBeInterruptedByAlarm) {
  int fds[2];
  char buf[1];
  struct sigaction sa;
  alarm(1);
  sa.sa_flags = 0;
  sa.sa_handler = OnSigalrm;
  sigemptyset(&sa.sa_mask);
  sigaction(SIGALRM, &sa, 0);
  ASSERT_SYS(0, 0, pipe(fds));
  ASSERT_SYS(ESPIPE, -1, pread(fds[0], buf, 1, 777));
  ASSERT_SYS(EINTR, -1, read(fds[0], buf, 1));
  ASSERT_TRUE(got_sigalrm);
  signal(SIGALRM, SIG_DFL);
  close(fds[1]);
  close(fds[0]);
}

TEST(read_directory, eisdir) {
  // TODO(jart): what
  if (IsWindows() || IsFreebsd()) return;
  ASSERT_SYS(0, 0, mkdir("boop", 0755));
  ASSERT_SYS(0, 3, open("boop", O_RDONLY | O_DIRECTORY));
  ASSERT_SYS(EISDIR, -1, read(3, 0, 0));
  ASSERT_SYS(0, 0, close(3));
}

int fds[2];
jmp_buf jb;
pthread_t th;
atomic_bool isdone;

void *GenerateSignals(void *arg) {
  while (!isdone) {
    usleep(123);
    pthread_kill(th, SIGINT);
  }
  return 0;
}

void *GenerateData(void *arg) {
  for (;;) {
    usleep(223);
    int rc = write(fds[1], "hi", 2);
    if (rc == -1 && errno == EPIPE) break;
    ASSERT_EQ(2, rc);
  }
  return 0;
}

void OnSig(int sig) {
  char buf[8];
  ASSERT_SYS(0, 2, read(fds[0], buf, 8));
  longjmp(jb, 1);
}

TEST(read, whatEmacsDoes) {
  pthread_t sigth;
  sighandler_t sh1 = signal(SIGINT, SIG_IGN);
  sighandler_t sh2 = signal(SIGPIPE, SIG_IGN);
  ASSERT_SYS(0, 0, pipe(fds));
  ASSERT_EQ(0, pthread_create(&th, 0, GenerateData, 0));
  ASSERT_EQ(0, pthread_create(&sigth, 0, GenerateSignals, 0));
  for (int i = 0; i < 100; ++i) {
    if (!setjmp(jb)) {
      char buf[8];
      ASSERT_GE(read(fds[0], buf, 8), 2);
    }
  }
  isdone = true;
  ASSERT_SYS(0, 0, close(fds[0]));
  ASSERT_EQ(0, pthread_join(sigth, 0));
  ASSERT_EQ(0, pthread_join(th, 0));
  ASSERT_SYS(0, 0, close(fds[1]));
  signal(SIGPIPE, sh2);
  signal(SIGINT, sh1);
}

////////////////////////////////////////////////////////////////////////////////

BENCH(read, bench) {
  char buf[16];
  BEGIN_CANCELATION_POINT;
  ASSERT_SYS(0, 3, open("/dev/zero", O_RDONLY));
  EZBENCH2("read", donothing, read(3, buf, 5));
  EZBENCH2("pread", donothing, pread(3, buf, 5, 0));
  EZBENCH2("readv₁", donothing, readv(3, &(struct iovec){buf, 5}, 1));
  EZBENCH2("readv₂", donothing,
           readv(3, (struct iovec[]){{buf, 1}, {buf + 1, 4}}, 2));
  EZBENCH2("preadv₁", donothing, preadv(3, &(struct iovec){buf, 5}, 1, 0));
  EZBENCH2("preadv₂", donothing,
           preadv(3, (struct iovec[]){{buf, 1}, {buf + 1, 4}}, 2, 0));
  EZBENCH2("sys_read", donothing, sys_read(3, buf, 5));
  EZBENCH2("sys_readv", donothing, sys_readv(3, &(struct iovec){buf, 5}, 1));
  ASSERT_SYS(0, 0, close(3));
  END_CANCELATION_POINT;
}
