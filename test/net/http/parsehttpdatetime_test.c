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
