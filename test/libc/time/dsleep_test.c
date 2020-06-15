/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
│                                                                              │
│ This program is free software; you can redistribute it and/or modify         │
│ it under the terms of the GNU General Public License as published by         │
│ the Free Software Foundation; version 2 of the License.                      │
│                                                                              │
│ This program is distributed in the hope that it will be useful, but          │
│ WITHOUT ANY WARRANTY; without even the implied warranty of                   │
│ MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU             │
│ General Public License for more details.                                     │
│                                                                              │
│ You should have received a copy of the GNU General Public License            │
│ along with this program; if not, write to the Free Software                  │
│ Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA                │
│ 02110-1301 USA                                                               │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/calls/calls.h"
#include "libc/calls/struct/sigset.h"
#include "libc/log/log.h"
#include "libc/nexgen32e/nexgen32e.h"
#include "libc/sysv/consts/clock.h"
#include "libc/sysv/consts/sig.h"
#include "libc/testlib/testlib.h"
#include "libc/time/time.h"
#include "libc/x/x.h"

TEST(fastdiv, test) {
  long x = 123000000321;
  EXPECT_EQ(123, div1000000000int64(x));
  EXPECT_EQ(321, mod1000000000int64(x));
}

TEST(dsleep, test) {
  sigset_t oldmask;
  long double t1, t2;
  sigprocmask(SIG_BLOCK, &kSigsetFull, &oldmask);
  sched_yield();
  t1 = dtime(CLOCK_MONOTONIC);
  dsleep(0.001L);
  t2 = dtime(CLOCK_MONOTONIC);
  sigprocmask(SIG_SETMASK, &oldmask, NULL);
  ASSERT_LDBL_GT(t2 - t1, 0.0005L);
}
