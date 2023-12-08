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
#include "net/http/http.h"

TEST(FormatHttpDateTime, test) {
  char p[30];
  struct tm *tm;
  int64_t t = 0x62820bcd;
  tm = gmtime(&t);
  FormatHttpDateTime(p, tm);
  ASSERT_STREQ("Mon, 16 May 2022 08:31:09 GMT", p);
}

TEST(FormatHttpDateTime, testStrftime) {
  char p[30];
  struct tm *tm;
  int64_t t = 0x62820bcd;
  tm = gmtime(&t);
  strftime(p, sizeof(p), "%a, %d %b %Y %H:%M:%S %Z", tm);
  ASSERT_STREQ("Mon, 16 May 2022 08:31:09 GMT", p);
}

BENCH(FormatHttpDateTime, bench) {
  char p[30];
  struct tm *tm;
  int64_t t = 0x62820bcd;
  tm = gmtime(&t);
  EZBENCH2("FormatHttpDateTime", donothing, FormatHttpDateTime(p, tm));
  EZBENCH2("strftime", donothing,
           strftime(p, sizeof(p), "%a, %d %b %Y %H:%M:%S %Z", tm));
}
