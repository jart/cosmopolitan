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
#include "libc/bits/bits.h"
#include "libc/errno.h"
#include "libc/fmt/fmt.h"
#include "libc/limits.h"
#include "libc/mem/mem.h"
#include "libc/testlib/testlib.h"

#define sscanf1(STR, FMT)               \
  ({                                    \
    errno = 0;                          \
    intmax_t x = 0;                     \
    EXPECT_EQ(1, sscanf(STR, FMT, &x)); \
    x;                                  \
  })

TEST(sscanf, testMultiple) {
  int a, b, c;
  ASSERT_EQ(3, sscanf("123 123 123", "%d %x %o", &a, &b, &c));
  EXPECT_EQ(123, a);
  EXPECT_EQ(0x123, b);
  EXPECT_EQ(0123, c);
}

TEST(sscanf, testDecimal) {
  EXPECT_EQ(123, sscanf1("123", "%d"));
  EXPECT_EQ(123, sscanf1("123", "%n"));
  EXPECT_EQ(123, sscanf1("123", "%u"));
  EXPECT_EQ((uint32_t)-123, sscanf1("-123", "%d"));
}

TEST(sscanf, testHex) {
  EXPECT_EQ(0x123, sscanf1("123", "%x"));
  EXPECT_EQ(0x123, sscanf1("0x123", "%x"));
  EXPECT_EQ(0x123, sscanf1("0123", "%x"));
  EXPECT_EQ(INTMAX_MAX,
            sscanf1("170141183460469231731687303715884105727", "%jd"));
  EXPECT_EQ(INTMAX_MIN,
            sscanf1("-170141183460469231731687303715884105728", "%jd"));
  EXPECT_EQ(UINTMAX_MAX, sscanf1("0xffffffffffffffffffffffffffffffff", "%jx"));
}

TEST(sscanf, testOctal) {
  EXPECT_EQ(0123, sscanf1("123", "%o"));
  EXPECT_EQ(0123, sscanf1("0123", "%o"));
}

TEST(sscanf, testNonDirectiveCharacterMatching) {
  EXPECT_EQ(0, sscanf("", ""));
  EXPECT_EQ(0, sscanf("%", "%%"));
}

TEST(sscanf, testCharacter) {
  char c = 0;
  EXPECT_EQ(1, sscanf("a", "%c", &c));
  EXPECT_EQ('a', c);
}

TEST(sscanf, testStringBuffer) {
  char s1[32], s2[32];
  ASSERT_EQ(2, sscanf("abc xyz", "%s %s", s1, s2));
  EXPECT_STREQ("abc", &s1[0]);
  EXPECT_STREQ("xyz", &s2[0]);
}

TEST(sscanf, testStringBuffer_gothicUtf8ToUtf8_roundTrips) {
  char s1[64], s2[64];
  ASSERT_EQ(2, sscanf("𐌰𐌱𐌲𐌳 𐌴𐌵𐌶𐌷", "%63s %63s", s1, s2));
  EXPECT_STREQ("𐌰𐌱𐌲𐌳", s1);
  EXPECT_STREQ("𐌴𐌵𐌶𐌷", s2);
}

TEST(sscanf, testStringBuffer_gothicUtf8ToUtf16) {
  char16_t s1[64], s2[64];
  ASSERT_EQ(2, sscanf("𐌰𐌱𐌲𐌳 𐌴𐌵𐌶𐌷", "%63hs %63hs", s1, s2));
  EXPECT_STREQ(u"𐌰𐌱𐌲𐌳", s1);
  EXPECT_STREQ(u"𐌴𐌵𐌶𐌷", s2);
}

TEST(sscanf, testStringBuffer_gothicUtf8ToUtf32) {
  wchar_t s1[64], s2[64];
  ASSERT_EQ(2, sscanf("𐌰𐌱𐌲𐌳 𐌴𐌵𐌶𐌷", "%63ls %63ls", s1, s2));
  EXPECT_STREQ(L"𐌰𐌱𐌲𐌳", s1);
  EXPECT_STREQ(L"𐌴𐌵𐌶𐌷", s2);
}

TEST(sscanf, testStringBuffer_allocatingBehavior) {
  char *s1, *s2;
  ASSERT_EQ(2, sscanf("𐌰𐌱𐌲𐌳 𐌴𐌵𐌶𐌷", "%ms %ms", &s1, &s2));
  EXPECT_STREQ("𐌰𐌱𐌲𐌳", s1);
  EXPECT_STREQ("𐌴𐌵𐌶𐌷", s2);
  free(s1);
  free(s2);
}

TEST(sscanf, testPracticalBusinessApplication) {
  unsigned start, stop;
  char width;
  ASSERT_EQ(1, sscanf("0BF3..", "%x", &start));
  EXPECT_EQ(0x0BF3, start);
  ASSERT_EQ(3, sscanf("0BF3..0BF8;N     # So     [6] TAMIL DAY SIGN",
                      "%x..%x;%c", &start, &stop, &width));
  EXPECT_EQ(0x0BF3, start);
  EXPECT_EQ(0x0BF8, stop);
  EXPECT_EQ('N', width);
}

TEST(sscanf, socketListenUri) {
  char proto[4];
  unsigned char ip4[4];
  uint16_t port;
  ASSERT_EQ(6, sscanf("tcp:127.0.0.1:31337", "%3s:%hhu.%hhu.%hhu.%hhu:%hu",
                      proto, &ip4[0], &ip4[1], &ip4[2], &ip4[3], &port));
  ASSERT_STREQ("tcp", proto);
  ASSERT_EQ(127, ip4[0]);
  ASSERT_EQ(0, ip4[1]);
  ASSERT_EQ(0, ip4[2]);
  ASSERT_EQ(1, ip4[3]);
  ASSERT_EQ(31337, port);
}

TEST(sscanf, testDiscard_notIncludedInCount) {
  char buf[8];
  ASSERT_EQ(1, sscanf("hello there", "%*s %8s", buf));
  EXPECT_STREQ("there", buf);
}
