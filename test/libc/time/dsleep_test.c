/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
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
  EXPECT_EQ(321, rem1000000000int64(x));
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
