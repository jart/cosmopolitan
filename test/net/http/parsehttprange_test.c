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
#include "net/http/http.h"

TEST(ParseHttpRange, testEmptyHack) {
  long start, length;
  const char *s = "bytes=-0";
  EXPECT_TRUE(ParseHttpRange(s, strlen(s), 100, &start, &length));
  EXPECT_EQ(100, start);
  EXPECT_EQ(0, length);
}

TEST(ParseHttpRange, testEmptyRange_isntEmpty) {
  long start, length;
  const char *s = "bytes=0-0";
  EXPECT_TRUE(ParseHttpRange(s, strlen(s), 100, &start, &length));
  EXPECT_EQ(0, start);
  EXPECT_EQ(1, length);
}

TEST(ParseHttpRange, testInclusiveIndexing) {
  long start, length;
  const char *s = "bytes=0-10";
  EXPECT_TRUE(ParseHttpRange(s, strlen(s), 100, &start, &length));
  EXPECT_EQ(0, start);
  EXPECT_EQ(11, length);
}

TEST(ParseHttpRange, testOffset) {
  long start, length;
  const char *s = "bytes=1-10";
  EXPECT_TRUE(ParseHttpRange(s, strlen(s), 100, &start, &length));
  EXPECT_EQ(1, start);
  EXPECT_EQ(10, length);
}

TEST(ParseHttpRange, testToEnd) {
  long start, length;
  const char *s = "bytes=40";
  EXPECT_TRUE(ParseHttpRange(s, strlen(s), 100, &start, &length));
  EXPECT_EQ(40, start);
  EXPECT_EQ(60, length);
}

TEST(ParseHttpRange, testFromEnd) {
  long start, length;
  const char *s = "bytes=-40";
  EXPECT_TRUE(ParseHttpRange(s, strlen(s), 100, &start, &length));
  EXPECT_EQ(60, start);
  EXPECT_EQ(40, length);
}

TEST(ParseHttpRange, testOutOfRange) {
  long start, length;
  const char *s = "bytes=0-100";
  EXPECT_FALSE(ParseHttpRange(s, strlen(s), 100, &start, &length));
  EXPECT_EQ(0, start);
  EXPECT_EQ(101, length);
}

TEST(ParseHttpRange, testInvalidRange) {
  long start, length;
  const char *s = "bytes=10-0";
  EXPECT_FALSE(ParseHttpRange(s, strlen(s), 100, &start, &length));
  EXPECT_EQ(0, start);
  EXPECT_EQ(0, length);
}

TEST(ParseHttpRange, testOverflow_duringIntepretation_doesntSetRanges) {
  long start, length;
  const char *s = "bytes=99-9223372036854775808";
  EXPECT_FALSE(ParseHttpRange(s, strlen(s), 100, &start, &length));
  EXPECT_EQ(0, start);
  EXPECT_EQ(0, length);
}

TEST(ParseHttpRange, testOverflow_duringAddition_setsErrorRange) {
  long start, length;
  const char *s = "bytes=4611686018427387904-4611686018427387915";
  EXPECT_FALSE(ParseHttpRange(s, strlen(s), 100, &start, &length));
  EXPECT_EQ(4611686018427387904, start);
  EXPECT_EQ(12, length);
}
