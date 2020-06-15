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
#include "libc/runtime/runtime.h"
#include "libc/testlib/testlib.h"
#include "libc/time/time.h"

textstartup static void strftime_test_init(void) { setenv("TZ", "GST", true); }
const void *const strftime_test_ctor[] initarray = {strftime_test_init};

testonly char *FormatTime(const char *fmt, struct tm *tm) {
  static char buf[64];
  strftime(buf, sizeof(buf), fmt, tm);
  return &buf[0];
}

TEST(strftime_100, iso8601_ShakaZuluTime) {
  int64_t t = 0x5cd04d0e;
  ASSERT_STREQ("2019-05-06T15:04:46Z",
               FormatTime("%Y-%m-%dT%H:%M:%SZ", gmtime(&t)));
}

TEST(strftime_100, rfc2822_ShakaZuluTime) {
  int64_t t = 0x5cd04d0e;
  ASSERT_STREQ("Mon, 06 May 2019 15:04:46 +0000",
               FormatTime("%a, %d %b %Y %T %z", gmtime(&t)));
}

TEST(strftime_100, rfc822_ShakaZuluTime) {
  int64_t t = 0x5cd04d0e;
  ASSERT_STREQ("Mon, 06 May 19 15:04:46 +0000",
               FormatTime("%a, %d %b %y %T %z", gmtime(&t)));
}

TEST(strftime_201, iso8601_GoogleStandardTime) {
  int64_t t = 0x5cd04d0e;
  ASSERT_STREQ("2019-05-06T08:04:46PDT",
               FormatTime("%Y-%m-%dT%H:%M:%S%Z", localtime(&t)));
}

TEST(strftime_201, rfc2822_GoogleStandardTime) {
  int64_t t = 0x5cd04d0e;
  ASSERT_STREQ("Mon, 06 May 2019 08:04:46 -0700",
               FormatTime("%a, %d %b %Y %T %z", localtime(&t)));
}

TEST(strftime_201, rfc822_GoogleStandardTime) {
  int64_t t = 0x5cd04d0e;
  ASSERT_STREQ("Mon, 06 May 19 08:04:46 -0700",
               FormatTime("%a, %d %b %y %T %z", localtime(&t)));
}
