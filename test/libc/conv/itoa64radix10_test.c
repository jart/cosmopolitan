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
#include "libc/conv/conv.h"
#include "libc/conv/itoa.h"
#include "libc/limits.h"
#include "libc/testlib/testlib.h"

TEST(int64toarray_radix10, test) {
  char buf[21];
  EXPECT_EQ(1, int64toarray_radix10(0, buf));
  EXPECT_STREQ("0", buf);
  EXPECT_EQ(1, int64toarray_radix10(1, buf));
  EXPECT_STREQ("1", buf);
  EXPECT_EQ(2, int64toarray_radix10(-1, buf));
  EXPECT_STREQ("-1", buf);
  EXPECT_EQ(19, int64toarray_radix10(INT64_MAX, buf));
  EXPECT_STREQ("9223372036854775807", buf);
  EXPECT_EQ(20, int64toarray_radix10(INT64_MIN, buf));
  EXPECT_STREQ("-9223372036854775808", buf);
}

TEST(uint64toarray_radix10, test) {
  char buf[21];
  EXPECT_EQ(1, uint64toarray_radix10(0, buf));
  EXPECT_STREQ("0", buf);
  EXPECT_EQ(20, uint64toarray_radix10(UINT64_MAX, buf));
  EXPECT_STREQ("18446744073709551615", buf);
  EXPECT_EQ(19, uint64toarray_radix10(INT64_MIN, buf));
  EXPECT_STREQ("9223372036854775808", buf);
}

TEST(int128toarray_radix10, test) {
  char buf[41];
  EXPECT_EQ(1, int128toarray_radix10(0, buf));
  EXPECT_STREQ("0", buf);
  EXPECT_EQ(39, int128toarray_radix10(INT128_MAX, buf));
  EXPECT_STREQ("170141183460469231731687303715884105727", buf);
  EXPECT_EQ(40, int128toarray_radix10(INT128_MIN, buf));
  EXPECT_STREQ("-170141183460469231731687303715884105728", buf);
}

TEST(uint128toarray_radix10, test) {
  char buf[40];
  EXPECT_EQ(1, uint128toarray_radix10(0, buf));
  EXPECT_STREQ("0", buf);
  EXPECT_EQ(39, uint128toarray_radix10(UINT128_MAX, buf));
  EXPECT_STREQ("340282366920938463463374607431768211455", buf);
  EXPECT_EQ(39, uint128toarray_radix10(INT128_MIN, buf));
  EXPECT_STREQ("170141183460469231731687303715884105728", buf);
}
