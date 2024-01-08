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
#include "libc/dce.h"
#include "libc/errno.h"
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
#include "libc/sysv/consts/sig.h"
#include "libc/sysv/consts/sock.h"
#include "libc/testlib/testlib.h"
#include "libc/x/x.h"
#include "libc/x/xasprintf.h"
#include "tool/decode/lib/flagger.h"

bool gotsig;

void SetUpOnce(void) {
  __pledge_mode = PLEDGE_PENALTY_KILL_PROCESS | PLEDGE_STDERR_LOGGING;
  ASSERT_SYS(0, 0, pledge("stdio proc inet", 0));
}

void SetUp(void) {
  gotsig = false;
}

void OnSig(int sig) {
  gotsig = true;
}

__wur char *FormatPollFd(struct pollfd p[2]) {
  return xasprintf("fd:%d revents:%s\n"
                   "fd:%d revents:%s\n",
                   p[0].fd, "<TODO:kPollNames>", p[1].fd, "<TODO:kPollNames>");
}

TEST(poll, allZero_doesNothingPrettyMuch) {
  EXPECT_SYS(0, 0, poll(0, 0, 0));
}

TEST(ppoll, weCanProveItChecksForSignals) {
  if (IsXnu()) return;
  if (IsNetbsd()) return;
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
  struct pollfd fds[] = {{-1}, {3}};
  EXPECT_SYS(0, 0, poll(fds, ARRAYLEN(fds), 1));
  EXPECT_STREQ("fd:-1 revents:<TODO:kPollNames>\n"
               "fd:3 revents:<TODO:kPollNames>\n",
               gc(FormatPollFd(&fds[0])));
  ASSERT_SYS(0, 0, close(3));
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

TEST(poll, pipe_hasInputFromSameProcess) {
  char buf[2];
  int pipefds[2];
  EXPECT_SYS(0, 0, pipe(pipefds));
  struct pollfd fds[] = {{pipefds[0], POLLIN}};
  EXPECT_SYS(0, 2, write(pipefds[1], "hi", 2));
  EXPECT_SYS(0, 1, poll(fds, 1, 1000));  // flake nt!
  EXPECT_EQ(POLLIN, fds[0].revents);
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
  EXPECT_EQ(POLLIN, fds[0].revents & POLLIN);
  EXPECT_SYS(0, 2, read(pipefds[0], buf, 2));
  EXPECT_SYS(0, 0, close(pipefds[0]));
  ASSERT_NE(-1, wait(&ws));
  EXPECT_TRUE(WIFEXITED(ws));
  EXPECT_EQ(0, WEXITSTATUS(ws));
  EXPECT_EQ(0, sigprocmask(SIG_SETMASK, &savemask, 0));
}

#if 0
TEST(poll, emptyFds_becomesSleep) {
  // timing tests w/o mocks are always the hardest
  int64_t a, b, c, p, i = 0;
  do {
    if (++i == 5) {
      kprintf("too much cpu churn%n");
      return;
    }
    p = TSC_AUX_CORE(rdpid());
    a = rdtsc();
    EXPECT_SYS(0, 0, poll(0, 0, 5));
    b = rdtsc();
    EXPECT_SYS(0, 0, poll(0, 0, 50));
    c = rdtsc();
  } while (TSC_AUX_CORE(rdpid()) != p);
  EXPECT_LT((b - a) * 2, c - b);
}
#endif
