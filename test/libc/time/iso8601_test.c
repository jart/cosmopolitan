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
#include "libc/testlib/ezbench.h"
#include "libc/testlib/testlib.h"
#include "libc/time/struct/tm.h"
#include "libc/time/time.h"

TEST(iso8601, test) {
  char p[20];
  struct tm tm;
  int64_t t = 0x62820bcd;
  gmtime_r(&t, &tm);
  EXPECT_EQ(p + 19, iso8601(p, &tm));
  EXPECT_STREQ("2022-05-16T08:31:09", p);
}

TEST(iso8601us, test) {
  char p[27];
  struct tm tm;
  int64_t t = 0x62820bcd;
  gmtime_r(&t, &tm);
  EXPECT_EQ(p + 26, iso8601us(p, &tm, 1234000));
  EXPECT_STREQ("2022-05-16T08:31:09.001234", p);
}

BENCH(iso8601, bench) {
  char p[27];
  struct tm tm;
  int64_t t = 0x62820bcd;
  gmtime_r(&t, &tm);
  EZBENCH2("iso8601", donothing, iso8601(p, &tm));
  EZBENCH2("iso8601us", donothing, iso8601us(p, &tm, 123456));
  EZBENCH2("strftime", donothing,
           strftime(p, sizeof(p), "%Y-%m-%dT%H:%M:%S", &tm));
}
