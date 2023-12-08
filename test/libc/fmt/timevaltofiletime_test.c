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
#include "libc/time/time.h"
#include "libc/calls/struct/timeval.h"
#include "libc/fmt/wintime.internal.h"
#include "libc/nt/struct/filetime.h"
#include "libc/testlib/testlib.h"

TEST(TimeValToFileTime, roundTrip) {
  struct timeval tv1, tv2;
  tv1.tv_sec = 31337;
  tv1.tv_usec = 1337;
  tv2 = FileTimeToTimeVal(TimeValToFileTime(tv1));
  EXPECT_EQ(31337, tv2.tv_sec);
  EXPECT_EQ(1337, tv2.tv_usec);
}

TEST(TimeSpecToFileTime, roundTrip_withSomeTruncation) {
  struct timespec tv1, tv2;
  tv1.tv_sec = 31337;
  tv1.tv_nsec = 1337;
  tv2 = FileTimeToTimeSpec(TimeSpecToFileTime(tv1));
  EXPECT_EQ(31337, tv2.tv_sec);
  EXPECT_EQ(1300, tv2.tv_nsec);
}
