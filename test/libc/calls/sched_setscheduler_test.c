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
#include "libc/calls/struct/sched_param.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/limits.h"
#include "libc/runtime/runtime.h"
#include "libc/sysv/consts/sched.h"
#include "libc/testlib/testlib.h"

#define DEFAULT_POLICY SCHED_OTHER

void SetUp(void) {
  if (IsFreebsd() && getuid() != 0) {
    exit(0);
  }
  if (IsXnu() || IsWindows() || IsOpenbsd() || IsWindows()) {
    exit(0);
  }
  int oldpolicy = sched_getscheduler(0);
  struct sched_param p = {sched_get_priority_min(DEFAULT_POLICY)};
  EXPECT_SYS(0, oldpolicy, sched_setscheduler(0, DEFAULT_POLICY, &p));
}

bool CanTuneRealtimeSchedulers(void) {
  int e = errno;
  int policy = SCHED_FIFO;
  struct sched_param p = {sched_get_priority_min(policy)};
  if (sched_setscheduler(0, policy, &p) != -1) {
    struct sched_param p = {sched_get_priority_min(DEFAULT_POLICY)};
    EXPECT_SYS(0, policy, sched_setscheduler(0, DEFAULT_POLICY, &p));
    return true;
  } else if (errno == EPERM) {
    errno = e;
    return false;
  } else {
    abort();
  }
}

TEST(sched_getscheduler, einval) {
  ASSERT_SYS(IsLinux() ? EINVAL : ESRCH, -1, sched_getscheduler(INT_MIN));
}

TEST(sched_setscheduler, test) {
  struct sched_param p = {sched_get_priority_min(SCHED_OTHER)};
  EXPECT_SYS(0, DEFAULT_POLICY, sched_setscheduler(0, SCHED_OTHER, &p));
}

TEST(sched_setscheduler, testMidpoint) {
  if (!CanTuneRealtimeSchedulers()) return;
  struct sched_param p = {(sched_get_priority_min(SCHED_FIFO) +
                           sched_get_priority_max(SCHED_FIFO)) /
                          2};
  EXPECT_SYS(0, DEFAULT_POLICY, sched_setscheduler(0, SCHED_FIFO, &p));
}
