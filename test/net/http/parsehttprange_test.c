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
#include "net/http/http.h"

TEST(ParseHttpRange, testEmptyHack_refusedBecauseItWontEncodeInContentRange) {
  long start, length;
  const char *s = "bytes=-0";
  EXPECT_FALSE(ParseHttpRange(s, strlen(s), 100, &start, &length));
  EXPECT_EQ(0, start);
  EXPECT_EQ(0, length);
}

TEST(ParseHttpRange, testEmptyRange_isntEmpty) {
  long start, length;
  const char *s = "bytes=0-0";
  EXPECT_TRUE(ParseHttpRange(s, strlen(s), 100, &start, &length));
  EXPECT_EQ(0, start);
  EXPECT_EQ(1, length);
}

TEST(ParseHttpRange, testEmptyRangeOfOneByteFile_itWorks) {
  long start, length;
  const char *s = "bytes=0-0";
  EXPECT_TRUE(ParseHttpRange(s, strlen(s), 1, &start, &length));
  EXPECT_EQ(0, start);
  EXPECT_EQ(1, length);
}

TEST(ParseHttpRange, testEmptyRangeOfEmptyFile_outOfRange) {
  long start, length;
  const char *s = "bytes=0-0";  // requesting 1 byte, but have 0
  EXPECT_TRUE(ParseHttpRange(s, strlen(s), 0, &start, &length));
  EXPECT_EQ(0, start);
  EXPECT_EQ(0, length);
  s = "bytes=1-1";  // but this can't be truncated
  EXPECT_FALSE(ParseHttpRange(s, strlen(s), 0, &start, &length));
  EXPECT_EQ(0, start);
  EXPECT_EQ(0, length);
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

TEST(ParseHttpRange, testEmptySecond) {
  long start, length;
  const char *s = "bytes=0-";
  EXPECT_TRUE(ParseHttpRange(s, strlen(s), 100, &start, &length));
  EXPECT_EQ(0, start);
  EXPECT_EQ(100, length);
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
  const char *s = "bytes=0-100";  // requesting 101 bytes, but have 100
  EXPECT_TRUE(ParseHttpRange(s, strlen(s), 100, &start, &length));
  EXPECT_EQ(0, start);
  EXPECT_EQ(100, length);
}

TEST(ParseHttpRange, testInvalidRange) {
  long start, length;
  const char *s = "bytes=10-0";
  EXPECT_FALSE(ParseHttpRange(s, strlen(s), 100, &start, &length));
  EXPECT_EQ(0, start);
  EXPECT_EQ(0, length);
}

TEST(ParseHttpRange, testOverflow_duringInterpretation_doesntSetRanges) {
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
  EXPECT_EQ(0, start);
  EXPECT_EQ(0, length);
}

TEST(ParseHttpRange, testMultipartRange_notImplemented) {
  long start, length;
  const char *s = "bytes=0-100,200-300";
  EXPECT_FALSE(ParseHttpRange(s, strlen(s), 100, &start, &length));
  EXPECT_EQ(0, start);
  EXPECT_EQ(0, length);
}

TEST(ParseHttpRange, rangeTooLong_shortensToActualLength) {
  long start, length;
  const char *s = "bytes=0-134217727";
  EXPECT_TRUE(ParseHttpRange(s, strlen(s), 1000, &start, &length));
  EXPECT_EQ(0, start);
  EXPECT_EQ(1000, length);
}
