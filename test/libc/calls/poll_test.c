/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
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
#include "libc/sysv/consts/poll.h"
#include "libc/calls/calls.h"
#include "libc/calls/pledge.h"
#include "libc/calls/struct/sigaction.h"
#include "libc/calls/struct/timespec.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/intrin/describeflags.h"
#include "libc/log/libfatal.internal.h"
#include "libc/mem/gc.h"
#include "libc/nexgen32e/rdtsc.h"
#include "libc/nexgen32e/rdtscp.h"
#include "libc/nt/synchronization.h"
#include "libc/sock/sock.h"
#include "libc/sock/struct/pollfd.h"
#include "libc/sock/struct/sockaddr.h"
#include "libc/sysv/consts/af.h"
#include "libc/sysv/consts/inaddr.h"
#include "libc/sysv/consts/ipproto.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/sig.h"
#include "libc/sysv/consts/sock.h"
#include "libc/testlib/testlib.h"
#include "libc/x/x.h"
#include "libc/x/xasprintf.h"
#include "tool/decode/lib/flagger.h"

bool gotsig;

void SetUpOnce(void) {
  testlib_enable_tmp_setup_teardown();
}

void SetUp(void) {
  gotsig = false;
}

void OnSig(int sig) {
  gotsig = true;
}

TEST(poll, allZero_doesNothingPrettyMuch) {
  EXPECT_SYS(0, 0, poll(0, 0, 0));
}

TEST(poll, allZeroWithTimeout_sleeps) {
  struct timespec ts1 = timespec_mono();
  EXPECT_SYS(0, 0, poll(0, 0, 100));
  EXPECT_GE(timespec_tomillis(timespec_sub(timespec_mono(), ts1)), 100);
}

TEST(ppoll, weCanProveItChecksForSignals) {
  if (IsXnu())
    return;
  if (IsNetbsd())
    return;
  int pipefds[2];
  sigset_t set, old;
  struct sigaction oldss;
  struct sigaction sa = {.sa_handler = OnSig};
  EXPECT_SYS(0, 0, pipe(pipefds));
  struct pollfd fds[] = {{pipefds[0], POLLIN}};
  ASSERT_SYS(0, 0, sigaction(SIGUSR1, &sa, &oldss));
  ASSERT_SYS(0, 0, sigfillset(&set));
  ASSERT_SYS(0, 0, sigprocmask(SIG_SETMASK, &set, &old));
  EXPECT_SYS(0, 0, kill(getpid(), SIGUSR1));
  EXPECT_FALSE(gotsig);
  EXPECT_SYS(EINTR, -1, ppoll(fds, 1, 0, &old));
  EXPECT_TRUE(gotsig);
  EXPECT_SYS(0, 0, sigprocmask(SIG_SETMASK, &old, 0));
  EXPECT_SYS(0, 0, sigaction(SIGUSR1, &oldss, 0));
  EXPECT_SYS(0, 0, close(pipefds[0]));
  EXPECT_SYS(0, 0, close(pipefds[1]));
}

TEST(poll, testNegativeOneFd_isIgnored) {
  ASSERT_SYS(0, 3, socket(AF_INET, SOCK_STREAM, IPPROTO_TCP));
  struct sockaddr_in addr = {AF_INET, 0, {htonl(INADDR_LOOPBACK)}};
  ASSERT_SYS(0, 0, bind(3, (struct sockaddr *)&addr, sizeof(addr)));
  ASSERT_SYS(0, 0, listen(3, 10));
  struct pollfd fds[] = {{-1, 0, -1}, {3, 0, -1}};
  EXPECT_SYS(0, 0, poll(fds, ARRAYLEN(fds), 1));
  EXPECT_EQ(-1, fds[0].fd);
  EXPECT_EQ(0, fds[0].revents);
  EXPECT_EQ(3, fds[1].fd);
  EXPECT_EQ(0, fds[1].revents);
  ASSERT_SYS(0, 0, close(3));
}

TEST(poll, testInvalidFd_POLLIN_isChecked) {
  struct pollfd fds[] = {{77, POLLIN, -1}};
  EXPECT_SYS(0, 1, poll(fds, ARRAYLEN(fds), 1));
  EXPECT_EQ(77, fds[0].fd);
  EXPECT_EQ(POLLNVAL, fds[0].revents);
}

TEST(poll, testInvalidFd_POLLOUT_isChecked) {
  struct pollfd fds[] = {{77, POLLOUT, -1}};
  EXPECT_SYS(0, 1, poll(fds, ARRAYLEN(fds), 1));
  EXPECT_EQ(77, fds[0].fd);
  EXPECT_EQ(POLLNVAL, fds[0].revents);
}

TEST(poll, testInvalidFd_POLLPRI_isChecked) {
  struct pollfd fds[] = {{77, POLLPRI, -1}};
  EXPECT_SYS(0, 1, poll(fds, ARRAYLEN(fds), 1));
  EXPECT_EQ(77, fds[0].fd);
  EXPECT_EQ(POLLNVAL, fds[0].revents);
}

TEST(poll, testInvalidFd_POLLHUP_isChecked) {
  // this behavior has to be polyfilled on xnu
  struct pollfd fds[] = {{77, POLLHUP, -1}};
  EXPECT_SYS(0, 1, poll(fds, ARRAYLEN(fds), 1));
  EXPECT_EQ(77, fds[0].fd);
  EXPECT_EQ(POLLNVAL, fds[0].revents);
}

TEST(poll, testInvalidFd_ZERO_isChecked) {
  // this behavior has to be polyfilled on xnu
  struct pollfd fds[] = {{77, 0, -1}};
  EXPECT_SYS(0, 1, poll(fds, ARRAYLEN(fds), 1));
  EXPECT_EQ(77, fds[0].fd);
  EXPECT_EQ(POLLNVAL, fds[0].revents);
}

TEST(poll, pipe_noInput) {
  // we can't test stdin here since
  // we can't assume it isn't /dev/null
  // since nil is always pollin as eof
  int pipefds[2];
  EXPECT_SYS(0, 0, pipe(pipefds));
  struct pollfd fds[] = {{pipefds[0], POLLIN}};
  EXPECT_SYS(0, 0, poll(fds, 1, 0));
  EXPECT_EQ(0, fds[0].revents);
  EXPECT_SYS(0, 0, close(pipefds[0]));
  EXPECT_SYS(0, 0, close(pipefds[1]));
}

TEST(poll, pipe_broken) {
  int pipefds[2];
  EXPECT_SYS(0, 0, pipe(pipefds));
  EXPECT_SYS(0, 0, close(pipefds[1]));
  struct pollfd fds[] = {{pipefds[0], POLLIN}};
  EXPECT_SYS(0, 1, poll(fds, 1, 0));
  // BSDs also set POLLIN here too even though that's wrong
  EXPECT_TRUE(!!(fds[0].revents & POLLHUP));
  EXPECT_SYS(0, 0, close(pipefds[0]));
}

TEST(poll, pipe_hasInputFromSameProcess) {
  char buf[2];
  int pipefds[2];
  EXPECT_SYS(0, 0, pipe(pipefds));
  struct pollfd fds[] = {{pipefds[0], POLLIN}};
  EXPECT_SYS(0, 2, write(pipefds[1], "hi", 2));
  EXPECT_SYS(0, 1, poll(fds, 1, 1000));  // flake nt!
  EXPECT_TRUE(!!(fds[0].revents & POLLIN));
  EXPECT_SYS(0, 2, read(pipefds[0], buf, 2));
  EXPECT_SYS(0, 0, poll(fds, 1, 0));
  EXPECT_SYS(0, 0, close(pipefds[0]));
  EXPECT_SYS(0, 0, close(pipefds[1]));
}

TEST(poll, pipe_hasInput) {
  char buf[2];
  sigset_t chldmask, savemask;
  int ws, pid, sync[2], pipefds[2];
  (void)sync;
  EXPECT_EQ(0, sigemptyset(&chldmask));
  EXPECT_EQ(0, sigaddset(&chldmask, SIGCHLD));
  EXPECT_EQ(0, sigprocmask(SIG_BLOCK, &chldmask, &savemask));
  EXPECT_SYS(0, 0, pipe(pipefds));
  EXPECT_NE(-1, (pid = fork()));
  if (!pid) {
    EXPECT_SYS(0, 0, close(pipefds[0]));
    EXPECT_SYS(0, 2, write(pipefds[1], "hi", 2));
    EXPECT_SYS(0, 2, write(pipefds[1], "hi", 2));
    EXPECT_SYS(0, 0, close(pipefds[1]));
    _Exit(0);
  }
  EXPECT_SYS(0, 0, close(pipefds[1]));
  EXPECT_SYS(0, 2, read(pipefds[0], buf, 2));
  struct pollfd fds[] = {{pipefds[0], POLLIN}};
  EXPECT_SYS(0, 1, poll(fds, 1, -1));
  EXPECT_TRUE(!!(fds[0].revents & POLLIN));
  EXPECT_SYS(0, 2, read(pipefds[0], buf, 2));
  EXPECT_SYS(0, 0, close(pipefds[0]));
  ASSERT_NE(-1, wait(&ws));
  EXPECT_TRUE(WIFEXITED(ws));
  EXPECT_EQ(0, WEXITSTATUS(ws));
  EXPECT_EQ(0, sigprocmask(SIG_SETMASK, &savemask, 0));
}

TEST(poll, file_pollin) {
  int fd;
  EXPECT_SYS(0, 3, (fd = open("boop", O_CREAT | O_RDWR | O_TRUNC, 0644)));
  struct pollfd fds[] = {{fd, POLLIN}};
  EXPECT_SYS(0, 1, poll(fds, 1, -1));
  EXPECT_TRUE(!!(fds[0].revents & POLLIN));
  EXPECT_TRUE(!(fds[0].revents & POLLOUT));
  EXPECT_SYS(0, 0, close(fd));
}

TEST(poll, file_pollout) {
  int fd;
  EXPECT_SYS(0, 3, (fd = open("boop", O_CREAT | O_RDWR | O_TRUNC, 0644)));
  struct pollfd fds[] = {{fd, POLLOUT}};
  EXPECT_SYS(0, 1, poll(fds, 1, -1));
  EXPECT_TRUE(!(fds[0].revents & POLLIN));
  EXPECT_TRUE(!!(fds[0].revents & POLLOUT));
  EXPECT_SYS(0, 0, close(fd));
}

TEST(poll, file_pollinout) {
  int fd;
  EXPECT_SYS(0, 3, (fd = open("boop", O_CREAT | O_RDWR | O_TRUNC, 0644)));
  struct pollfd fds[] = {{fd, POLLIN | POLLOUT}};
  EXPECT_SYS(0, 1, poll(fds, 1, -1));
  EXPECT_TRUE(!!(fds[0].revents & POLLIN));
  EXPECT_TRUE(!!(fds[0].revents & POLLOUT));
  EXPECT_SYS(0, 0, close(fd));
}

TEST(poll, file_rdonly_pollinout) {
  int fd;
  EXPECT_SYS(0, 3, (fd = open("boop", O_CREAT | O_RDWR | O_TRUNC, 0644)));
  EXPECT_SYS(0, 0, close(fd));
  EXPECT_SYS(0, 3, (fd = open("boop", O_RDONLY)));
  struct pollfd fds[] = {{fd, POLLIN | POLLOUT}};
  EXPECT_SYS(0, 1, poll(fds, 1, -1));
  EXPECT_TRUE(!!(fds[0].revents & POLLIN));
  EXPECT_TRUE(!!(fds[0].revents & POLLOUT));  // counter-intuitive
  EXPECT_SYS(0, 0, close(fd));
}

TEST(poll, file_wronly_pollin) {
  int fd;
  EXPECT_SYS(0, 3, (fd = creat("boop", 0644)));
  struct pollfd fds[] = {{fd, POLLIN}};
  EXPECT_SYS(0, 1, poll(fds, 1, -1));
  EXPECT_TRUE(!!(fds[0].revents & POLLIN));
  EXPECT_TRUE(!(fds[0].revents & POLLOUT));
  EXPECT_SYS(0, 0, close(fd));
}

TEST(poll, file_wronly_pollout) {
  int fd;
  EXPECT_SYS(0, 3, (fd = creat("boop", 0644)));
  struct pollfd fds[] = {{fd, POLLOUT}};
  EXPECT_SYS(0, 1, poll(fds, 1, -1));
  EXPECT_TRUE(!(fds[0].revents & POLLIN));
  EXPECT_TRUE(!!(fds[0].revents & POLLOUT));
  EXPECT_SYS(0, 0, close(fd));
}

TEST(poll, file_wronly_pollinout) {
  int fd;
  EXPECT_SYS(0, 3, (fd = creat("boop", 0644)));
  struct pollfd fds[] = {{fd, POLLIN | POLLOUT}};
  EXPECT_SYS(0, 1, poll(fds, 1, -1));
  EXPECT_TRUE(!!(fds[0].revents & POLLIN));
  EXPECT_TRUE(!!(fds[0].revents & POLLOUT));
  EXPECT_SYS(0, 0, close(fd));
}

TEST(poll, file_rdwr_pollinoutpri) {
  int fd;
  EXPECT_SYS(0, 3, (fd = open("boop", O_CREAT | O_RDWR | O_TRUNC, 0644)));
  struct pollfd fds[] = {{fd, POLLIN | POLLOUT | POLLPRI}};
  EXPECT_SYS(0, 1, poll(fds, 1, -1));
  EXPECT_TRUE(!!(fds[0].revents & POLLIN));
  EXPECT_TRUE(!!(fds[0].revents & POLLOUT));
  if (IsXnu())
    EXPECT_TRUE(!!(fds[0].revents & POLLPRI));  // wut
  else
    EXPECT_TRUE(!(fds[0].revents & POLLPRI));
  EXPECT_SYS(0, 0, close(fd));
}

TEST(poll, pipein_pollout_blocks) {
  if (IsFreebsd() || IsOpenbsd())
    return;
  int pipefds[2];
  EXPECT_SYS(0, 0, pipe(pipefds));
  struct pollfd fds[] = {{pipefds[0], POLLOUT}};
  EXPECT_SYS(0, 0, poll(fds, 1, 0));
  struct timespec ts1 = timespec_mono();
  EXPECT_SYS(0, 0, poll(fds, 1, 10));
  EXPECT_GE(timespec_tomillis(timespec_sub(timespec_mono(), ts1)), 10);
  EXPECT_SYS(0, 0, close(pipefds[1]));
  EXPECT_SYS(0, 0, close(pipefds[0]));
}

TEST(poll, pipein_file_noblock) {
  if (IsFreebsd() || IsOpenbsd())
    return;
  int pipefds[2];
  EXPECT_SYS(0, 3, open("boop", O_CREAT | O_RDWR | O_TRUNC, 0644));
  EXPECT_SYS(0, 0, pipe(pipefds));
  struct pollfd fds[] = {{pipefds[0], POLLIN}, {3, POLLIN}};
  EXPECT_SYS(0, 1, poll(fds, 2, -1u));
  EXPECT_TRUE(!!(fds[1].revents & POLLIN));
  EXPECT_TRUE(!(fds[1].revents & POLLOUT));
  EXPECT_SYS(0, 0, close(pipefds[1]));
  EXPECT_SYS(0, 0, close(pipefds[0]));
  EXPECT_SYS(0, 0, close(3));
}

TEST(poll, pipein_file_noblock2) {
  if (IsFreebsd() || IsOpenbsd())
    return;
  int pipefds[2];
  EXPECT_SYS(0, 3, open("boop", O_CREAT | O_RDWR | O_TRUNC, 0644));
  EXPECT_SYS(0, 0, pipe(pipefds));
  EXPECT_SYS(0, 1, write(5, "x", 1));
  struct pollfd fds[] = {{pipefds[0], POLLIN}, {3, POLLIN | POLLOUT}};
  EXPECT_SYS(0, 2, poll(fds, 2, -1u));
  EXPECT_TRUE(!!(fds[0].revents & POLLIN));
  EXPECT_TRUE(!(fds[0].revents & POLLOUT));
  EXPECT_TRUE(!!(fds[1].revents & POLLIN));
  EXPECT_TRUE(!!(fds[1].revents & POLLOUT));
  EXPECT_SYS(0, 0, close(pipefds[1]));
  EXPECT_SYS(0, 0, close(pipefds[0]));
  EXPECT_SYS(0, 0, close(3));
}

TEST(poll, pipeout_pollout) {
  int pipefds[2];
  EXPECT_SYS(0, 0, pipe(pipefds));
  struct pollfd fds[] = {{pipefds[1], POLLOUT}};
  EXPECT_SYS(0, 1, poll(fds, 1, 0));
  EXPECT_TRUE(!(fds[0].revents & POLLIN));
  EXPECT_TRUE(!!(fds[0].revents & POLLOUT));
  EXPECT_SYS(0, 1, poll(fds, 1, 1));
  EXPECT_TRUE(!(fds[0].revents & POLLIN));
  EXPECT_TRUE(!!(fds[0].revents & POLLOUT));
  EXPECT_SYS(0, 0, close(pipefds[1]));
  EXPECT_SYS(0, 0, close(pipefds[0]));
}

TEST(poll, pipein_pollin_timeout) {
  int pipefds[2];
  EXPECT_SYS(0, 0, pipe(pipefds));
  struct pollfd fds[] = {{pipefds[0], POLLIN}};
  struct timespec ts1 = timespec_mono();
  EXPECT_SYS(0, 0, poll(fds, 1, 10));
  EXPECT_GE(timespec_tomillis(timespec_sub(timespec_mono(), ts1)), 10);
  EXPECT_SYS(0, 0, close(pipefds[1]));
  EXPECT_SYS(0, 0, close(pipefds[0]));
}

TEST(poll, pipein_pollinout_timeout) {
  if (IsFreebsd() || IsOpenbsd())
    return;
  int pipefds[2];
  EXPECT_SYS(0, 0, pipe(pipefds));
  struct pollfd fds[] = {{pipefds[0], POLLIN | POLLOUT}};
  EXPECT_SYS(0, 0, poll(fds, 1, 0));
  struct timespec ts1 = timespec_mono();
  EXPECT_SYS(0, 0, poll(fds, 1, 10));
  EXPECT_GE(timespec_tomillis(timespec_sub(timespec_mono(), ts1)), 10);
  EXPECT_SYS(0, 0, close(pipefds[1]));
  EXPECT_SYS(0, 0, close(pipefds[0]));
}
