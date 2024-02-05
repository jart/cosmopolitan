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
#include "libc/errno.h"
#include "libc/inttypes.h"
#include "libc/limits.h"
#include "libc/math.h"
#include "libc/mem/mem.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/internal.h"
#include "libc/str/str.h"
#include "libc/testlib/testlib.h"

#define sscanf1(STR, FMT)               \
  ({                                    \
    errno = 0;                          \
    int128_t x = 0;                     \
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
  EXPECT_EQ(123, sscanf1("123", "%u"));
  EXPECT_EQ((uint32_t)-123, sscanf1("-123", "%d"));
}

TEST(sscanf, testHex) {
  EXPECT_EQ(0x123, sscanf1("123", "%x"));
  EXPECT_EQ(0x123, sscanf1("0x123", "%x"));
  EXPECT_EQ(0x123, sscanf1("0123", "%x"));
  EXPECT_EQ(INT128_MAX,
            sscanf1("170141183460469231731687303715884105727", "%jjd"));
  EXPECT_EQ(INT128_MIN,
            sscanf1("-170141183460469231731687303715884105728", "%jjd"));
  EXPECT_EQ(UINT128_MAX, sscanf1("0xffffffffffffffffffffffffffffffff", "%jjx"));
}

TEST(sscanf, testOctal) {
  EXPECT_EQ(0123, sscanf1("123", "%o"));
  EXPECT_EQ(0123, sscanf1("0123", "%o"));
}

TEST(sscanf, testNonDirectiveCharacterMatching) {
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

TEST(sscanf, testStringBuffer_gothicUtf8to16) {
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

TEST(sscanf, testFixedWidthFormat_Integer) {
  int r, g, b;
  ASSERT_EQ(3, sscanf("#321030", "#%2x%2b%2d", &r, &g, &b));
  ASSERT_EQ(0x32, r);
  ASSERT_EQ(2, g);
  ASSERT_EQ(30, b);
}

TEST(sscanf, testInttypes_macros) {
  int8_t i8 = (int8_t)0xFFFFFFFFFFFFFFFF;
  uint8_t u8 = (uint8_t)0xFFFFFFFFFFFFFFFF;
  int16_t i16 = (int16_t)0xFFFFFFFFFFFFFFFF;
  uint16_t u16 = (uint16_t)0xFFFFFFFFFFFFFFFF;
  int32_t i32 = (int32_t)0xFFFFFFFFFFFFFFFF;
  uint32_t u32 = (uint32_t)0xFFFFFFFFFFFFFFFF;
  int64_t i64 = (int64_t)0xFFFFFFFFFFFFFFFF;
  uint64_t u64 = (uint64_t)0xFFFFFFFFFFFFFFFF;
  intmax_t imax = (intmax_t)0xFFFFFFFFFFFFFFFF;
  uintmax_t umax = (uintmax_t)0xFFFFFFFFFFFFFFFF;
  int_least8_t il8 = (int_least8_t)0xFFFFFFFFFFFFFFFF;
  uint_least8_t ul8 = (uint_least8_t)0xFFFFFFFFFFFFFFFF;
  int_least16_t il16 = (int_least16_t)0xFFFFFFFFFFFFFFFF;
  uint_least16_t ul16 = (uint_least16_t)0xFFFFFFFFFFFFFFFF;
  int_least32_t il32 = (int_least32_t)0xFFFFFFFFFFFFFFFF;
  uint_least32_t ul32 = (uint_least32_t)0xFFFFFFFFFFFFFFFF;
  int_least64_t il64 = (int_least64_t)0xFFFFFFFFFFFFFFFF;
  uint_least64_t ul64 = (uint_least64_t)0xFFFFFFFFFFFFFFFF;
  int_fast8_t if8 = (int_fast8_t)0xFFFFFFFFFFFFFFFF;
  uint_fast8_t uf8 = (uint_fast8_t)0xFFFFFFFFFFFFFFFF;
  int_fast16_t if16 = (int_fast16_t)0xFFFFFFFFFFFFFFFF;
  uint_fast16_t uf16 = (uint_fast16_t)0xFFFFFFFFFFFFFFFF;
  int_fast32_t if32 = (int_fast32_t)0xFFFFFFFFFFFFFFFF;
  uint_fast32_t uf32 = (uint_fast32_t)0xFFFFFFFFFFFFFFFF;
  int_fast64_t if64 = (int_fast64_t)0xFFFFFFFFFFFFFFFF;
  uint_fast64_t uf64 = (uint_fast64_t)0xFFFFFFFFFFFFFFFF;

  ASSERT_EQ(sscanf("1", "%" SCNd8, &i8), 1);
  ASSERT_EQ(sscanf("1", "%" SCNu8, &u8), 1);
  ASSERT_EQ(sscanf("1", "%" SCNd16, &i16), 1);
  ASSERT_EQ(sscanf("1", "%" SCNu16, &u16), 1);
  ASSERT_EQ(sscanf("1", "%" SCNd32, &i32), 1);
  ASSERT_EQ(sscanf("1", "%" SCNu32, &u32), 1);
  ASSERT_EQ(sscanf("1", "%" SCNd64, &i64), 1);
  ASSERT_EQ(sscanf("1", "%" SCNu64, &u64), 1);
  ASSERT_EQ(sscanf("1", "%" SCNdMAX, &imax), 1);
  ASSERT_EQ(sscanf("1", "%" SCNuMAX, &umax), 1);
  ASSERT_EQ(sscanf("1", "%" SCNdLEAST8, &il8), 1);
  ASSERT_EQ(sscanf("1", "%" SCNuLEAST8, &ul8), 1);
  ASSERT_EQ(sscanf("1", "%" SCNdLEAST16, &il16), 1);
  ASSERT_EQ(sscanf("1", "%" SCNuLEAST16, &ul16), 1);
  ASSERT_EQ(sscanf("1", "%" SCNdLEAST32, &il32), 1);
  ASSERT_EQ(sscanf("1", "%" SCNuLEAST32, &ul32), 1);
  ASSERT_EQ(sscanf("1", "%" SCNdLEAST64, &il64), 1);
  ASSERT_EQ(sscanf("1", "%" SCNuLEAST64, &ul64), 1);
  ASSERT_EQ(sscanf("1", "%" SCNdFAST8, &if8), 1);
  ASSERT_EQ(sscanf("1", "%" SCNuFAST8, &uf8), 1);
  ASSERT_EQ(sscanf("1", "%" SCNdFAST16, &if16), 1);
  ASSERT_EQ(sscanf("1", "%" SCNuFAST16, &uf16), 1);
  ASSERT_EQ(sscanf("1", "%" SCNdFAST32, &if32), 1);
  ASSERT_EQ(sscanf("1", "%" SCNuFAST32, &uf32), 1);
  ASSERT_EQ(sscanf("1", "%" SCNdFAST64, &if64), 1);
  ASSERT_EQ(sscanf("1", "%" SCNuFAST64, &uf64), 1);

  ASSERT_EQ(i8, 1);
  ASSERT_EQ(u8, 1);
  ASSERT_EQ(i16, 1);
  ASSERT_EQ(u16, 1);
  ASSERT_EQ(i32, 1);
  ASSERT_EQ(u32, 1);
  ASSERT_EQ(i64, 1);
  ASSERT_EQ(u64, 1);
  ASSERT_EQ(imax, 1);
  ASSERT_EQ(umax, 1);
  ASSERT_EQ(il8, 1);
  ASSERT_EQ(ul8, 1);
  ASSERT_EQ(il16, 1);
  ASSERT_EQ(ul16, 1);
  ASSERT_EQ(il32, 1);
  ASSERT_EQ(ul32, 1);
  ASSERT_EQ(il64, 1);
  ASSERT_EQ(ul64, 1);
  ASSERT_EQ(if16, 1);
  ASSERT_EQ(uf16, 1);
  ASSERT_EQ(if32, 1);
  ASSERT_EQ(uf32, 1);
  ASSERT_EQ(if64, 1);
  ASSERT_EQ(uf64, 1);
}

TEST(sscanf, test0) {
  int v;

  v = 0xFFFFFFFF;
  ASSERT_EQ(sscanf("0", "%x", &v), 1);
  ASSERT_EQ(v, 0);

  v = 0xFFFFFFFF;
  ASSERT_EQ(sscanf("0", "%X", &v), 1);
  ASSERT_EQ(v, 0);

  v = 0xFFFFFFFF;
  ASSERT_EQ(sscanf("0", "%d", &v), 1);
  ASSERT_EQ(v, 0);

  v = 0xFFFFFFFF;
  ASSERT_EQ(sscanf("0", "%o", &v), 1);
  ASSERT_EQ(v, 0);

  v = 0xFFFFFFFF;
  ASSERT_EQ(sscanf("0", "%u", &v), 1);
  ASSERT_EQ(v, 0);

  v = 0xFFFFFFFF;
  ASSERT_EQ(sscanf("0", "%b", &v), 1);
  ASSERT_EQ(v, 0);
}

TEST(sscanf, n) {
  int x, y;
  EXPECT_EQ(1, sscanf("7 2 3 4", "%d%n", &x, &y));
  EXPECT_EQ(7, x);
  EXPECT_EQ(1, y);
}

TEST(sscanf, eofForNoMatching) {
  int y = 666;
  char x[8] = "hi";
  EXPECT_EQ(-1, sscanf("   ", "%s%n", &x, &y));
  EXPECT_STREQ("hi", x);
  EXPECT_EQ(666, y);
}

TEST(sscanf, eofConditions) {
  int x = 666;
  EXPECT_EQ(-1, sscanf("", "%d", &x));
  EXPECT_EQ(666, x);
  EXPECT_EQ(-1, sscanf(" ", "%d", &x));
  EXPECT_EQ(666, x);
  EXPECT_EQ(-1, sscanf("123", "%*d%d", &x));
  EXPECT_EQ(666, x);
  EXPECT_EQ(-1, sscanf("123", "%*d%n", &x));
  EXPECT_EQ(666, x);
}

TEST(sscanf, decimal) {
  int x = 666;
  int y = 666;
  EXPECT_EQ(1, sscanf("019", "%d%d", &x, &y));
  EXPECT_EQ(19, x);
  EXPECT_EQ(666, y);
}

TEST(sscanf, octal) {
  int x = 666;
  int y = 666;
  EXPECT_EQ(2, sscanf("019", "%o%d", &x, &y));
  EXPECT_EQ(1, x);
  EXPECT_EQ(9, y);
}

TEST(sscanf, flexdecimal_octal) {
  int x = 666;
  int y = 666;
  EXPECT_EQ(2, sscanf("019", "%i%d", &x, &y));
  EXPECT_EQ(1, x);
  EXPECT_EQ(9, y);
}

TEST(sscanf, flexdecimal_decimal) {
  int x = 666;
  int y = 666;
  EXPECT_EQ(1, sscanf("109a", "%i%d", &x, &y));
  EXPECT_EQ(109, x);
  EXPECT_EQ(666, y);
}

TEST(sscanf, flexdecimal_hex) {
  int x = 666;
  int y = 666;
  EXPECT_EQ(1, sscanf("0x19a", "%i%d", &x, &y));
  EXPECT_EQ(0x19a, x);
  EXPECT_EQ(666, y);
}

TEST(sscanf, floating_point_simple) {
  float x = 666.666f, y = x, z = y;
  EXPECT_EQ(3, sscanf("0.3715 .3715 3715", "%f %f %f", &x, &y, &z));
  EXPECT_EQ(0.3715f, x);
  EXPECT_EQ(0.3715f, y);
  EXPECT_EQ(3715.0f, z);
}

TEST(sscanf, floating_point_simple_double_precision) {
  double x = 666.666, y = x, z = y;
  EXPECT_EQ(3, sscanf("0.3715 .3715 3715", "%lf %lf %lf", &x, &y, &z));
  EXPECT_EQ(0.3715, x);
  EXPECT_EQ(0.3715, y);
  EXPECT_EQ(3715.0, z);
}

TEST(sscanf, floating_point_nan) {
  float a = 666.666f, b = a, c = b, d = c, e = d, f = e;
  EXPECT_EQ(4, sscanf("nan -NAN nAn NaN", "%f %f %f %f", &a, &b, &c, &d));
  EXPECT_EQ(2, sscanf("nan(2) -NaN(_ABCDzxcv1234_)", "%f %f", &e, &f));
  EXPECT_TRUE(isnan(a));
  EXPECT_TRUE(isnan(b));
  EXPECT_TRUE(isnan(c));
  EXPECT_TRUE(isnan(d));
  EXPECT_TRUE(isnan(e));
  EXPECT_TRUE(isnan(f));
}

TEST(sscanf, floating_point_nan_double_precision) {
  double a = 666.666, b = a, c = b, d = c, e = d, f = e;
  EXPECT_EQ(4, sscanf("nan -NAN nAn NaN", "%lf %lf %lf %lf", &a, &b, &c, &d));
  EXPECT_EQ(2, sscanf("nan(2) -NAN(_ABCDzxcv1234_)", "%lf %lf", &e, &f));
  EXPECT_TRUE(isnan(a));
  EXPECT_TRUE(isnan(b));
  EXPECT_TRUE(isnan(c));
  EXPECT_TRUE(isnan(d));
  EXPECT_TRUE(isnan(e));
  EXPECT_TRUE(isnan(f));
}

TEST(sscanf, floating_point_infinity) {
  float a = 666.666f, b = a, c = b, d = c, e = d, f = e, g = f;
  EXPECT_EQ(4, sscanf("inf +INF -iNf InF", "%f %f %f %f", &a, &b, &c, &d));
  EXPECT_EQ(3, sscanf("+infinity -INFINITY iNfInItY", "%f %f %f", &e, &f, &g));
  EXPECT_TRUE(isinf(a));
  EXPECT_TRUE(isinf(b));
  EXPECT_TRUE(isinf(c));
  EXPECT_TRUE(isinf(d));
  EXPECT_TRUE(isinf(e));
  EXPECT_TRUE(isinf(f));
  EXPECT_TRUE(isinf(g));
}

TEST(sscanf, floating_point_infinity_double_precision) {
  double a = 666.666, b = a, c = b, d = c, e = d, f = e, g = f;
  EXPECT_EQ(4, sscanf("inf +INF -iNf InF", "%lf %lf %lf %lf", &a, &b, &c, &d));
  EXPECT_EQ(3,
            sscanf("+infinity -INFINITY iNfInItY", "%lf %lf %lf", &e, &f, &g));
  EXPECT_TRUE(isinf(a));
  EXPECT_TRUE(isinf(b));
  EXPECT_TRUE(isinf(c));
  EXPECT_TRUE(isinf(d));
  EXPECT_TRUE(isinf(e));
  EXPECT_TRUE(isinf(f));
  EXPECT_TRUE(isinf(g));
}

TEST(sscanf, floating_point_documentation_examples) {
  float a = 666.666f, b = a, c = b, d = c, e = d, f = e, g = f, h = g, i = h,
        j = i;

  EXPECT_EQ(2, sscanf("111.11 -2.22", "%f %f", &a, &b));
  EXPECT_EQ(3, sscanf("Nan nan(2) inF", "%f %f %f", &c, &d, &e));
  EXPECT_EQ(
      5, sscanf("0X1.BC70A3D70A3D7P+6 1.18973e+4932zzz -0.0000000123junk junk",
                "%f %f %f %f %f", &f, &g, &h, &i, &j));

  EXPECT_EQ(111.11f, a);
  EXPECT_EQ(-2.22f, b);
  EXPECT_TRUE(isnan(c));
  EXPECT_TRUE(isnan(d));
  EXPECT_TRUE(isinf(e));
  EXPECT_EQ(0X1.BC70A3D70A3D7P+6f, f);
  EXPECT_TRUE(isinf(g));
  EXPECT_EQ(-0.0000000123f, h);
  EXPECT_EQ(.0f, i);
  EXPECT_EQ(.0f, j);
}

TEST(sscanf, floating_point_documentation_examples_double_precision) {
  double a = 666.666, b = a, c = b, d = c, e = d, f = e, g = f, h = g, i = h,
         j = i;

  EXPECT_EQ(2, sscanf("111.11 -2.22", "%lf %lf", &a, &b));
  EXPECT_EQ(3, sscanf("Nan nan(2) inF", "%lf %lf %lf", &c, &d, &e));
  EXPECT_EQ(
      5, sscanf("0X1.BC70A3D70A3D7P+6 1.18973e+4932zzz -0.0000000123junk junk",
                "%lf %lf %lf %lf %lf", &f, &g, &h, &i, &j));

  EXPECT_EQ(111.11, a);
  EXPECT_EQ(-2.22, b);
  EXPECT_TRUE(isnan(c));
  EXPECT_TRUE(isnan(d));
  EXPECT_TRUE(isinf(e));
  EXPECT_EQ(0X1.BC70A3D70A3D7P+6, f);
  EXPECT_TRUE(isinf(g));
  EXPECT_EQ(-0.0000000123, h);
  EXPECT_EQ(.0, i);
  EXPECT_EQ(.0, j);
}

TEST(sscanf, luplus) {
  long x = 666;
  EXPECT_EQ(1, sscanf("+123", "%lu", &x));
  EXPECT_EQ(123, x);
}

TEST(sscanf, lupluser) {
  long x = 666;
  EXPECT_EQ(1, sscanf("+123", "%li", &x));
  EXPECT_EQ(123, x);
}

TEST(fscanf, stuff) {
  int x;
  char *s = "1 12 123\n"
            "4 44\n";
  FILE *f = fmemopen(s, strlen(s), "r+");
  EXPECT_EQ(1, fscanf(f, "%d", &x));
  EXPECT_EQ(1, x);
  EXPECT_EQ(1, fscanf(f, "%d", &x));
  EXPECT_EQ(12, x);
  EXPECT_EQ(1, fscanf(f, "%d", &x));
  EXPECT_EQ(123, x);
  EXPECT_EQ(1, fscanf(f, "%d", &x));
  EXPECT_EQ(4, x);
  EXPECT_EQ(1, fscanf(f, "%d", &x));
  EXPECT_EQ(44, x);
  EXPECT_EQ(-1, fscanf(f, "%d", &x));
  fclose(f);
}

TEST(fscanf, wantDecimalButGotLetter_returnsZeroMatches) {
  int x = 666;
  char *s = "a1\n";
  FILE *f = fmemopen(s, strlen(s), "r+");
  EXPECT_EQ(0, fscanf(f, "%d", &x));
  EXPECT_EQ(666, x);
  fclose(f);
}

TEST(scanf, n) {
  int rc;
  unsigned int a, b, c, d, port, len;
  rc = sscanf("1.2.3.4:1848", "%u.%u.%u.%u:%u%n", &a, &b, &c, &d, &port, &len);
  ASSERT_EQ(5, rc);
  ASSERT_EQ(1, a);
  ASSERT_EQ(2, b);
  ASSERT_EQ(3, c);
  ASSERT_EQ(4, d);
  ASSERT_EQ(1848, port);
  ASSERT_EQ(12, len);
}
