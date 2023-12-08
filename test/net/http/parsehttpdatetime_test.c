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
#include "libc/str/str.h"
#include "libc/testlib/testlib.h"
#include "libc/time/struct/tm.h"
#include "libc/time/time.h"
#include "net/http/http.h"

#define PARSE(s) ParseHttpDateTime(s, sizeof(s) - 1)

TEST(ParseHttpDateTime, testRoundTrip) {
  int64_t t;
  struct tm tm;
  char b[30], *s = "Mon, 05 Oct 2020 20:23:16 GMT";
  t = ParseHttpDateTime(s, strlen(s));
  EXPECT_EQ(1601929396, t);
  gmtime_r(&t, &tm);
  FormatHttpDateTime(b, &tm);
  EXPECT_STREQ(s, b);
}

TEST(ParseHttpDateTime, test64bit) {
  int64_t t;
  struct tm tm;
  char b[30], *s = "Tue, 05 Oct 2100 20:23:16 GMT";
  t = ParseHttpDateTime(s, strlen(s));
  EXPECT_EQ(4126450996, t);
  gmtime_r(&t, &tm);
  FormatHttpDateTime(b, &tm);
  EXPECT_STREQ(s, b);
}
