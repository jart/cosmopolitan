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
#include "libc/calls/struct/itimerval.h"
#include "libc/calls/struct/sigaction.h"
#include "libc/calls/struct/timespec.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/intrin/describeflags.internal.h"
#include "libc/intrin/strace.internal.h"
#include "libc/runtime/runtime.h"
#include "libc/sysv/consts/clock.h"
#include "libc/sysv/consts/itimer.h"
#include "libc/sysv/consts/sa.h"
#include "libc/sysv/consts/sig.h"
#include "libc/testlib/testlib.h"
#include "libc/time/time.h"

void OnAlrm(int sig) {
  // do nothing
  STRACE("OnAlrm()");
}

TEST(nanosleep, testInvalid) {
  struct timespec ts = {0, -1};
  EXPECT_SYS(EINVAL, -1, nanosleep(&ts, 0));
}

TEST(nanosleep, testNoSignalIsDelivered) {
  struct timespec ts = {0, 1};
  ASSERT_SYS(0, 0, nanosleep(&ts, &ts));
  ASSERT_SYS(0, 0, nanosleep(&ts, 0));
}

TEST(nanosleep, testInterrupt_remIsUpdated) {
  struct sigaction sa = {
      .sa_handler = OnAlrm,
      .sa_flags = SA_RESETHAND,
  };
  ASSERT_SYS(0, 0, sigaction(SIGALRM, &sa, 0));
  struct itimerval it = {{0, 0}, {0, 100000}};  // 100ms singleshot
  ASSERT_SYS(0, 0, setitimer(ITIMER_REAL, &it, 0));
  struct timespec ts = {500, 0};
  ASSERT_SYS(EINTR, -1, nanosleep(&ts, &ts));
  ASSERT_LT(ts.tv_sec, 500);
  ASSERT_GT(ts.tv_sec, 400);
  ASSERT_NE(0, ts.tv_nsec);
}

TEST(clock_nanosleep, testNoSignalIsDelivered) {
  struct timespec ts = {0, 1};
  ASSERT_EQ(0, clock_nanosleep(CLOCK_REALTIME, 0, &ts, &ts));
  ASSERT_EQ(0, clock_nanosleep(CLOCK_REALTIME, 0, &ts, 0));
}

TEST(clock_nanosleep, testInterrupt_remIsUpdated) {
  struct sigaction sa = {
      .sa_handler = OnAlrm,
      .sa_flags = SA_RESETHAND,
  };
  ASSERT_SYS(0, 0, sigaction(SIGALRM, &sa, 0));
  struct itimerval it = {{0, 0}, {0, 100000}};  // 100ms singleshot
  ASSERT_SYS(0, 0, setitimer(ITIMER_REAL, &it, 0));
  struct timespec ts = {500, 0};
  ASSERT_EQ(EINTR, clock_nanosleep(CLOCK_REALTIME, 0, &ts, &ts));
  ASSERT_LT(ts.tv_sec, 500);
  ASSERT_GT(ts.tv_sec, 400);
  ASSERT_NE(0, ts.tv_nsec);
}
