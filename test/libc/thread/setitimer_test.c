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
#include "libc/atomic.h"
#include "libc/calls/calls.h"
#include "libc/calls/struct/itimerval.h"
#include "libc/calls/struct/sigaction.h"
#include "libc/calls/struct/siginfo.h"
#include "libc/calls/struct/sigset.h"
#include "libc/calls/struct/timeval.h"
#include "libc/calls/ucontext.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/intrin/kprintf.h"
#include "libc/limits.h"
#include "libc/runtime/runtime.h"
#include "libc/sysv/consts/itimer.h"
#include "libc/sysv/consts/sa.h"
#include "libc/sysv/consts/sicode.h"
#include "libc/sysv/consts/sig.h"
#include "libc/testlib/subprocess.h"
#include "libc/testlib/testlib.h"
#include "libc/time/time.h"

atomic_int gotsig;
atomic_int gottid;

void SetUpOnce(void) {
  ASSERT_SYS(0, 0, pledge("stdio proc", 0));
}

void SetUp(void) {
  gotsig = 0;
}

TEST(setitimer, badTimer_isInvalid) {
  struct itimerval it = {{0, 0}, {0, 200000}};
  ASSERT_SYS(EINVAL, -1, setitimer(INT_MIN, &it, 0));
}

TEST(setitimer, defaultActionIsToKillProcess) {
  SPAWN(fork);
  struct itimerval singleshot = {{0}, {0, 100}};
  ASSERT_SYS(0, 0, setitimer(ITIMER_REAL, &singleshot, 0));
  pause();
  TERMS(SIGALRM);
}

void OnSigAlrm(int sig, siginfo_t *si, void *ctx) {
  EXPECT_EQ(SIGALRM, sig);
  EXPECT_EQ(SIGALRM, si->si_signo);
  gottid = gettid();
  gotsig = sig;
}

TEST(setitimer, asynchronousDelivery) {
  struct itimerval it = {{0, 0}, {0, 200000}};
  struct sigaction sa = {.sa_sigaction = OnSigAlrm,
                         .sa_flags = SA_RESETHAND | SA_SIGINFO};
  sigaction(SIGALRM, &sa, 0);
  ASSERT_EQ(0, setitimer(ITIMER_REAL, &it, 0));
  while (!gotsig) {
  }
  ASSERT_EQ(gettid(), gottid);
  ASSERT_EQ(SIGALRM, gotsig);
  ASSERT_EQ(SIG_DFL, signal(SIGALRM, SIG_DFL));
}

TEST(setitimer, testSingleShot) {
  sigset_t block, oldmask;
  struct itimerval it = {{0, 0}, {0, 10000}};
  struct sigaction sa = {.sa_sigaction = OnSigAlrm,
                         .sa_flags = SA_RESETHAND | SA_SIGINFO};
  sigemptyset(&block);
  sigaction(SIGALRM, &sa, 0);
  sigaddset(&block, SIGALRM);
  EXPECT_EQ(0, sigprocmask(SIG_BLOCK, &block, &oldmask));
  ASSERT_EQ(0, setitimer(ITIMER_REAL, &it, 0));
  sigdelset(&block, SIGALRM);
  EXPECT_EQ(-1, sigsuspend(&block));
  EXPECT_EQ(0, sigprocmask(SIG_SETMASK, &oldmask, 0));
  ASSERT_EQ(gettid(), gottid);
  ASSERT_EQ(SIGALRM, gotsig);
}

TEST(setitimer, notInheritedAcrossFork) {
  struct itimerval disarm = {0};
  struct itimerval singleshot = {{0}, {100}};
  ASSERT_SYS(0, 0, setitimer(ITIMER_REAL, &singleshot, 0));
  SPAWN(fork);
  struct itimerval it;
  ASSERT_SYS(0, 0, setitimer(ITIMER_REAL, 0, &it));
  ASSERT_TRUE(timeval_iszero(it.it_value));
  EXITS(0);
  ASSERT_SYS(0, 0, setitimer(ITIMER_REAL, &disarm, 0));
}
