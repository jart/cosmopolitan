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
#include "libc/calls/struct/flock.h"
#include "libc/calls/struct/sigaction.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/log/check.h"
#include "libc/macros.internal.h"
#include "libc/runtime/runtime.h"
#include "libc/sysv/consts/f.h"
#include "libc/sysv/consts/fd.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/sig.h"
#include "libc/testlib/testlib.h"
#include "libc/thread/thread.h"
#include "libc/x/x.h"

__static_yoink("libc/testlib/hyperion.txt");
__static_yoink("zipos");

int Lock(int fd, int type, long start, long len) {
  int e;
  struct flock lock = {
      .l_type = type,
      .l_whence = SEEK_SET,
      .l_start = start,
      .l_len = len,
  };
  e = errno;
  while (fcntl(fd, F_SETLK, &lock)) {
    if (errno == EAGAIN || errno == EACCES) {
      errno = e;
      continue;
    } else {
      return -1;
    }
  }
  return 0;
}

void SetUpOnce(void) {
  testlib_enable_tmp_setup_teardown();
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

TEST(fcntl_setfl, testChangeAppendStatus_proper) {
  char buf[8] = {0};
  ASSERT_SYS(0, 3, open("foo", O_CREAT | O_WRONLY, 0644));
  // F_GETFL on XNU reports FWASWRITTEN (0x00010000) after write()
  int old = fcntl(3, F_GETFL);
  EXPECT_SYS(0, 3, write(3, "foo", 3));
  EXPECT_SYS(0, 0, lseek(3, 0, SEEK_SET));
  EXPECT_SYS(0, 0, fcntl(3, F_SETFL, old | O_APPEND));
  EXPECT_SYS(0, 3, write(3, "bar", 3));
  EXPECT_SYS(0, 0, lseek(3, 0, SEEK_SET));
  EXPECT_SYS(0, 0, fcntl(3, F_SETFL, old));
  EXPECT_SYS(0, 0, close(3));
  ASSERT_SYS(0, 3, open("foo", 0));
  EXPECT_SYS(0, 6, read(3, buf, 6));
  EXPECT_STREQ("foobar", buf);
  EXPECT_SYS(0, 0, close(3));
}

TEST(fcntl_setfl, testChangeAppendStatus_sloppy) {
  char buf[8] = {0};
  ASSERT_SYS(0, 3, open("foo", O_CREAT | O_WRONLY, 0644));
  EXPECT_SYS(0, 3, write(3, "foo", 3));
  EXPECT_SYS(0, 0, lseek(3, 0, SEEK_SET));
  EXPECT_SYS(0, 0, fcntl(3, F_SETFL, O_APPEND));
  EXPECT_SYS(0, 3, write(3, "bar", 3));
  EXPECT_SYS(0, 0, lseek(3, 0, SEEK_SET));
  EXPECT_SYS(0, 0, fcntl(3, F_SETFL, 0));
  EXPECT_SYS(0, 0, close(3));
  ASSERT_SYS(0, 3, open("foo", 0));
  EXPECT_SYS(0, 6, read(3, buf, 6));
  EXPECT_STREQ("foobar", buf);
  EXPECT_SYS(0, 0, close(3));
}

TEST(fcntl, getfd) {
  ASSERT_SYS(0, 3, open("/dev/null", O_RDWR));
  ASSERT_SYS(0, 0, fcntl(3, F_GETFD));
  ASSERT_SYS(0, 4, open("/dev/null", O_RDWR | O_CLOEXEC));
  ASSERT_SYS(0, FD_CLOEXEC, fcntl(4, F_GETFD));
  ASSERT_SYS(0, 0, fcntl(4, F_SETFD, FD_CLOEXEC));
  ASSERT_SYS(0, FD_CLOEXEC, fcntl(4, F_GETFD));
  ASSERT_SYS(0, 0, fcntl(4, F_SETFD, 0));
  ASSERT_SYS(0, 0, fcntl(4, F_GETFD));
  ASSERT_SYS(0, 0, close(4));
  ASSERT_SYS(0, 0, close(3));
}

TEST(fcntl, F_DUPFD_CLOEXEC) {
  ASSERT_SYS(0, 3, open("/dev/null", O_RDWR));
  ASSERT_SYS(0, 5, fcntl(3, F_DUPFD_CLOEXEC, 5));
  ASSERT_SYS(0, FD_CLOEXEC, fcntl(5, F_GETFD));
  ASSERT_SYS(0, 0, close(5));
  ASSERT_SYS(0, 0, close(3));
}

TEST(fcntl, ziposDupFd) {
  char b[8];
  ASSERT_SYS(0, 3, open("/zip/libc/testlib/hyperion.txt", O_RDONLY));
  ASSERT_SYS(0, 4, fcntl(3, F_DUPFD, 4));
  ASSERT_SYS(0, 8, read(3, b, 8));
  ASSERT_SYS(0, 0, lseek(4, 0, SEEK_SET));
  ASSERT_SYS(0, 8, read(4, b, 8));
  ASSERT_SYS(0, 0, close(3));
  ASSERT_SYS(0, 0, close(4));
}

void OnSig(int sig) {
}

TEST(posixAdvisoryLocks, twoProcesses) {
  if (IsWindows()) return;  // due to signals
  if (IsNetbsd()) return;   // TODO: why does sigusr1 kill runitd?

  int ws, pid;
  struct flock lock;
  sigset_t ss, oldss;
  struct sigaction oldsa;
  struct sigaction sa = {.sa_handler = OnSig};
  ASSERT_SYS(0, 0, sigemptyset(&ss));
  ASSERT_SYS(0, 0, sigaddset(&ss, SIGUSR1));
  ASSERT_SYS(0, 0, sigprocmask(SIG_SETMASK, &ss, &oldss));
  ASSERT_SYS(0, 0, sigdelset(&ss, SIGUSR1));
  ASSERT_SYS(0, 0, sigaction(SIGUSR1, &sa, &oldsa));
  ASSERT_NE(-1, (pid = fork()));
  if (!pid) {
    ASSERT_SYS(0, 3, open("foo", O_RDWR | O_CREAT | O_TRUNC, 0644));
    ASSERT_SYS(0, 5, write(3, "hello", 5));
    lock.l_type = F_WRLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = 1;
    lock.l_len = 3;
    lock.l_pid = -2;
    ASSERT_SYS(0, 0, fcntl(3, F_SETLK, &lock));
    EXPECT_EQ(F_WRLCK, lock.l_type);
    EXPECT_EQ(SEEK_SET, lock.l_whence);
    EXPECT_EQ(1, lock.l_start);
    EXPECT_EQ(3, lock.l_len);
    EXPECT_EQ(-2, lock.l_pid);
    ASSERT_SYS(0, 0, kill(getppid(), SIGUSR1));
    ASSERT_SYS(EINTR, -1, sigsuspend(&ss));
    _Exit(0);
  }
  ASSERT_SYS(EINTR, -1, sigsuspend(&ss));
  ASSERT_SYS(0, 3, open("foo", O_RDWR));

  // try lock
  lock.l_type = F_RDLCK;
  lock.l_whence = SEEK_SET;
  lock.l_start = 0;
  lock.l_len = 0;
  lock.l_pid = -1;
  ASSERT_SYS(EAGAIN, -1, fcntl(3, F_SETLK, &lock));

  // get lock information
  lock.l_type = F_RDLCK;
  lock.l_whence = SEEK_SET;
  lock.l_start = 0;
  lock.l_len = 0;
  lock.l_pid = -1;
  ASSERT_SYS(0, 0, fcntl(3, F_GETLK, &lock));
  EXPECT_EQ(F_WRLCK, lock.l_type);
  EXPECT_EQ(SEEK_SET, lock.l_whence);
  EXPECT_EQ(1, lock.l_start);
  EXPECT_EQ(3, lock.l_len);
  EXPECT_EQ(pid, lock.l_pid);

  // get lock information
  lock.l_type = F_RDLCK;
  lock.l_whence = SEEK_END;
  lock.l_start = -3;
  lock.l_len = 0;
  lock.l_pid = -1;
  ASSERT_SYS(0, 0, fcntl(3, F_GETLK, &lock));
  EXPECT_EQ(F_WRLCK, lock.l_type);
  EXPECT_EQ(SEEK_SET, lock.l_whence);
  EXPECT_EQ(1, lock.l_start);
  EXPECT_EQ(3, lock.l_len);
  EXPECT_EQ(pid, lock.l_pid);

  // get lock information
  lock.l_type = F_RDLCK;
  lock.l_whence = SEEK_END;
  lock.l_start = -1;
  lock.l_len = 0;
  lock.l_pid = -1;
  ASSERT_SYS(0, 0, fcntl(3, F_GETLK, &lock));
  EXPECT_EQ(F_UNLCK, lock.l_type);
  EXPECT_EQ(SEEK_END, lock.l_whence);
  EXPECT_EQ(-1, lock.l_start);
  EXPECT_EQ(0, lock.l_len);
  EXPECT_EQ(-1, lock.l_pid);

  ASSERT_SYS(0, 0, kill(pid, SIGUSR1));
  EXPECT_NE(-1, wait(&ws));
  EXPECT_TRUE(WIFEXITED(ws));
  EXPECT_EQ(0, WEXITSTATUS(ws));
  ASSERT_SYS(0, 0, close(3));
  ASSERT_SYS(0, 0, sigaction(SIGUSR1, &oldsa, 0));
  ASSERT_SYS(0, 0, sigprocmask(SIG_SETMASK, &oldss, 0));
}

TEST(fcntl, nonblock) {
  int pfds[2];
  char buf[8];
  ASSERT_SYS(0, 0, pipe(pfds));
  ASSERT_SYS(0, 0, fcntl(pfds[0], F_SETFL, O_RDONLY));
  ASSERT_SYS(0, 0, fcntl(pfds[0], F_SETFL, O_RDONLY | O_NONBLOCK));
  ASSERT_SYS(EAGAIN, -1, read(pfds[0], buf, 8));
  ASSERT_SYS(0, 0, fcntl(pfds[0], F_SETFL, O_RDONLY));
  ASSERT_SYS(0, 0, close(pfds[1]));
  ASSERT_SYS(0, 0, close(pfds[0]));
}
