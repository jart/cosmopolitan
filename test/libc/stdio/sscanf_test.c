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
#include "libc/fmt/conv.h"
#include "libc/inttypes.h"
#include "libc/limits.h"
#include "libc/math.h"
#include "libc/mem/mem.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/internal.h"
#include "libc/stdio/stdio.h"
#include "libc/str/locale.h"
#include "libc/str/str.h"
#include "libc/testlib/benchmark.h"
#include "libc/testlib/testlib.h"

void SetUp(void) {
  errno = 0;
}

TEST(sscanf, empty) {
  int n;
  ASSERT_EQ(0, sscanf("", "%n", &n));
  ASSERT_EQ(0, n);
}

TEST(sscanf, testNonDirectiveCharacterMatching) {
  ASSERT_EQ(0, sscanf("%", "%%"));
}

TEST(sscanf, testCharacter) {
  char c1 = 0, c2 = c1, c3 = c2, c4 = c3;
  char s1[32] = {0}, s2[32] = {0};
  ASSERT_EQ(1, sscanf("a", "%c", &c1));
  ASSERT_EQ(2, sscanf("ab", "%c %c %c", &c2, &c3, &c4));
  ASSERT_EQ(1, sscanf("abcde", "%5c", s1));
  ASSERT_EQ(1, sscanf("abcd", "%5c", s2));
  ASSERT_EQ('a', c1);
  ASSERT_EQ('a', c2);
  ASSERT_EQ('b', c3);
  ASSERT_STREQ("abcde", &s1[0]);
}

TEST(sscanf, testStringBuffer) {
  char s1[32], s2[32];
  ASSERT_EQ(2, sscanf("abc xyz", "%s %s", s1, s2));
  ASSERT_STREQ("abc", &s1[0]);
  ASSERT_STREQ("xyz", &s2[0]);
}

TEST(sscanf, testStringBuffer_gothicUtf8ToUtf8_roundTrips) {
  char s1[64], s2[64];
  ASSERT_EQ(2, sscanf("𐌰𐌱𐌲𐌳 𐌴𐌵𐌶𐌷", "%63s %63s", s1, s2));
  ASSERT_STREQ("𐌰𐌱𐌲𐌳", s1);
  ASSERT_STREQ("𐌴𐌵𐌶𐌷", s2);
}

TEST(sscanf, testStringBuffer_gothicUtf8to16) {
  char16_t s1[64], s2[64];
  ASSERT_EQ(2, sscanf("𐌰𐌱𐌲𐌳 𐌴𐌵𐌶𐌷", "%63hs %63hs", s1, s2));
  ASSERT_STREQ(u"𐌰𐌱𐌲𐌳", s1);
  ASSERT_STREQ(u"𐌴𐌵𐌶𐌷", s2);
}

TEST(sscanf, testStringBuffer_gothicUtf8ToUtf32) {
  wchar_t s1[64], s2[64];
  ASSERT_EQ(2, sscanf("𐌰𐌱𐌲𐌳 𐌴𐌵𐌶𐌷", "%63ls %63ls", s1, s2));
  ASSERT_STREQ(L"𐌰𐌱𐌲𐌳", s1);
  ASSERT_STREQ(L"𐌴𐌵𐌶𐌷", s2);
}

TEST(sscanf, testStringBuffer_allocatingBehavior) {
  char *s1, *s2;
  ASSERT_EQ(2, sscanf("𐌰𐌱𐌲𐌳 𐌴𐌵𐌶𐌷", "%ms %ms", &s1, &s2));
  ASSERT_STREQ("𐌰𐌱𐌲𐌳", s1);
  ASSERT_STREQ("𐌴𐌵𐌶𐌷", s2);
  free(s1);
  free(s2);
}

TEST(sscanf, testPracticalBusinessApplication) {
  unsigned start, stop;
  char width;
  ASSERT_EQ(1, sscanf("0BF3..", "%x", &start));
  ASSERT_EQ(0x0BF3, start);
  ASSERT_EQ(3, sscanf("0BF3..0BF8;N     # So     [6] TAMIL DAY SIGN",
                      "%x..%x;%c", &start, &stop, &width));
  ASSERT_EQ(0x0BF3, start);
  ASSERT_EQ(0x0BF8, stop);
  ASSERT_EQ('N', width);
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
  char b1[4] = "nop";
  ASSERT_EQ(0, sscanf("hello", "%*s"));
  ASSERT_EQ(0, sscanf("hello ", "%*s"));
  ASSERT_EQ(-1, sscanf("hello", "%*s %3s", b1));
  ASSERT_STREQ("nop", b1);
  ASSERT_EQ(-1, sscanf("hello ", "%*s %3s", b1));
  ASSERT_STREQ("nop", b1);
  ASSERT_EQ(1, sscanf("hello there", "%*s %3s", b1));
  ASSERT_STREQ("the", b1);
}

TEST(sscanf, testFixedWidthFormat_Integer) {
  int r, g, b;
  ASSERT_EQ(3, sscanf("#321030", "#%2x%2b%2d", &r, &g, &b));
  ASSERT_EQ(0x32, r);
  ASSERT_EQ(2, g);
  ASSERT_EQ(30, b);
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
  ASSERT_EQ(1, sscanf("7 2 3 4", "%d%n", &x, &y));
  ASSERT_EQ(7, x);
  ASSERT_EQ(1, y);
}

TEST(sscanf, eofForNoMatching) {
  int y = 666;
  char x[8] = "hi";
  ASSERT_EQ(-1, sscanf("   ", "%s%n", &x, &y));
  ASSERT_STREQ("hi", x);
  ASSERT_EQ(666, y);
}

TEST(sscanf, decimalEofconditions) {
  int x = 666;
  ASSERT_EQ(-1, sscanf("", "%d", &x));
  ASSERT_EQ(666, x);
  ASSERT_EQ(-1, sscanf(" ", "%d", &x));
  ASSERT_EQ(666, x);
  ASSERT_EQ(-1, sscanf("123", "%*d%d", &x));
  ASSERT_EQ(666, x);
  ASSERT_EQ(0, sscanf("123", "%*d%n", &x));
  ASSERT_EQ(3, x);
}

TEST(sscanf, floatEofConditions) {
  int n = 666;
  float x = 666;
  ASSERT_EQ(-1, sscanf("", "%f", &x));
  ASSERT_EQ(666, x);
  ASSERT_EQ(-1, sscanf(" ", "%f", &x));
  ASSERT_EQ(666, x);
  ASSERT_EQ(-1, sscanf("123", "%*f%f", &x));
  ASSERT_EQ(666, x);
  ASSERT_EQ(0, sscanf("123", "%*f%n", &n));
  ASSERT_EQ(3, n);
  ASSERT_EQ(0, sscanf("+", "%*f%n", &n));
  ASSERT_EQ(3, n);
  ASSERT_EQ(0, sscanf("+", "%1f%n", &x, &n));
  ASSERT_EQ(3, n);
  ASSERT_EQ(0, sscanf("+0", "%*f%n", &n));
  ASSERT_EQ(2, n);
  n = 666;
  int y = 666;
  ASSERT_EQ(0, sscanf("+0", "%1f%d%n", &x, &y, &n));
  ASSERT_EQ(666, n);
  ASSERT_EQ(666, y);
}

TEST(sscanf, iron_bank) {
  int x = 666;
  int y = 666;
  int n = 666;
  ASSERT_EQ(0, sscanf("+0", "%1d%d%n", &x, &y, &n));
  ASSERT_EQ(666, x);
  ASSERT_EQ(666, y);
  ASSERT_EQ(666, n);
}

TEST(sscanf, decimal) {
  int x = 666;
  int y = 666;
  ASSERT_EQ(1, sscanf("019", "%d%d", &x, &y));
  ASSERT_EQ(19, x);
  ASSERT_EQ(666, y);
  ASSERT_EQ(2, sscanf("2147483647 -2147483648", "%d%d", &x, &y));
  ASSERT_EQ(INT_MAX, x);
  ASSERT_EQ(INT_MIN, y);
}

TEST(sscanf, octal) {
  int x = 666;
  int y = 666;
  ASSERT_EQ(2, sscanf("019", "%o%d", &x, &y));
  ASSERT_EQ(1, x);
  ASSERT_EQ(9, y);
}

TEST(sscanf, flexdecimal_octal) {
  int x = 666;
  int y = 666;
  ASSERT_EQ(2, sscanf("019", "%i%d", &x, &y));
  ASSERT_EQ(1, x);
  ASSERT_EQ(9, y);
}

TEST(sscanf, flexdecimal_decimal) {
  int x = 666;
  int y = 666;
  ASSERT_EQ(1, sscanf("109a", "%i%d", &x, &y));
  ASSERT_EQ(109, x);
  ASSERT_EQ(666, y);
}

TEST(sscanf, flexdecimal_hex) {
  int x = 666;
  int y = 666;
  ASSERT_EQ(1, sscanf("0x19a", "%i%d", &x, &y));
  ASSERT_EQ(0x19a, x);
  ASSERT_EQ(666, y);
}

TEST(sscanf, floating_point_simple) {
  float x = 666.666f, y = x, z = y;
  ASSERT_EQ(3, sscanf("0.3715 .3715 3715", "%f %f %f", &x, &y, &z));
  ASSERT_FLOAT_EXACTLY_EQ(0.3715f, x);
  ASSERT_FLOAT_EXACTLY_EQ(0.3715f, y);
  ASSERT_FLOAT_EXACTLY_EQ(3715.0f, z);
}

TEST(sscanf, floating_point_simple_double_precision) {
  double x = 666.666, y = x, z = y;
  ASSERT_EQ(3, sscanf("0.3715 .3715 3715", "%lf %lf %lf", &x, &y, &z));
  ASSERT_DOUBLE_EXACTLY_EQ(0.3715, x);
  ASSERT_DOUBLE_EXACTLY_EQ(0.3715, y);
  ASSERT_DOUBLE_EXACTLY_EQ(3715.0, z);
}

TEST(sscanf, floating_point_nan) {
  int n;
  float a = 666.666f, b = a, c = b, d = c, e = d, f = e;
  ASSERT_EQ(4, sscanf("nan -NAN nAn NaN", "%f %f %f %f%n", &a, &b, &c, &d, &n));
  ASSERT_EQ(strlen("nan -NAN nAn NaN"), n);
#ifdef __COSMOPOLITAN__
  ASSERT_EQ(2, sscanf("nan(2) -NaN(_ABCDzxcv1234_)", "%f %f", &e, &f));
#else
  ASSERT_EQ(2, sscanf("nan -NaN", "%f %f", &e, &f));
#endif
  ASSERT_TRUE(isnan(a));
  ASSERT_TRUE(isnan(b));
  ASSERT_TRUE(isnan(c));
  ASSERT_TRUE(isnan(d));
  ASSERT_TRUE(isnan(e));
  ASSERT_TRUE(isnan(f));
}

TEST(sscanf, floating_point_nan_double_precision) {
  double a = 666.666, b = a, c = b, d = c, e = d, f = e;
  ASSERT_EQ(4, sscanf("nan -NAN nAn NaN", "%lf %lf %lf %lf", &a, &b, &c, &d));
  ASSERT_EQ(2, sscanf("nan(2) -NAN(_ABCDzxcv1234_)", "%lf %lf", &e, &f));
  ASSERT_TRUE(isnan(a));
  ASSERT_TRUE(isnan(b));
  ASSERT_TRUE(isnan(c));
  ASSERT_TRUE(isnan(d));
  ASSERT_TRUE(isnan(e));
  ASSERT_TRUE(isnan(f));
}

TEST(sscanf, floating_point_infinity) {
  float a = 666.666f, b = a, c = b, d = c, e = d, f = e, g = f;
  ASSERT_EQ(4, sscanf("inf +INF -iNf InF", "%f %f %f %f", &a, &b, &c, &d));
  ASSERT_EQ(3, sscanf("+infinity -INFINITY iNfInItY", "%f %f %f", &e, &f, &g));
  ASSERT_TRUE(isinf(a));
  ASSERT_TRUE(isinf(b));
  ASSERT_TRUE(isinf(c));
  ASSERT_TRUE(isinf(d));
  ASSERT_TRUE(isinf(e));
  ASSERT_TRUE(isinf(f));
  ASSERT_TRUE(isinf(g));
}

TEST(sscanf, floating_point_infinity_double_precision) {
  double a = 666.666, b = a, c = b, d = c, e = d, f = e, g = f;
  ASSERT_EQ(4, sscanf("inf +INF -iNf InF", "%lf %lf %lf %lf", &a, &b, &c, &d));
  ASSERT_EQ(3,
            sscanf("+infinity -INFINITY iNfInItY", "%lf %lf %lf", &e, &f, &g));
  ASSERT_TRUE(isinf(a));
  ASSERT_TRUE(isinf(b));
  ASSERT_TRUE(isinf(c));
  ASSERT_TRUE(isinf(d));
  ASSERT_TRUE(isinf(e));
  ASSERT_TRUE(isinf(f));
  ASSERT_TRUE(isinf(g));
}

TEST(sscanf, floating_point_invalid) {
  float dummy;
  ASSERT_EQ(0, sscanf("junk", "%f", &dummy));
  ASSERT_EQ(0, sscanf("e9", "%f", &dummy));
  ASSERT_EQ(0, sscanf("-e9", "%f", &dummy));
}

TEST(sscanf, floating_point_invalid_double_precision) {
  double dummy;
  ASSERT_EQ(0, sscanf("junk", "%lf", &dummy));
  ASSERT_EQ(0, sscanf("e9", "%lf", &dummy));
  ASSERT_EQ(0, sscanf("-e9", "%lf", &dummy));
}

TEST(sscanf, floating_point_documentation_examples) {
  float a = 666.666f, b = a, c = b, d = c, e = d, f = e, g = f, h = g, i = h,
        j = i;
  ASSERT_EQ(2, sscanf("111.11 -2.22", "%f %f", &a, &b));
#ifdef __COSMOPOLITAN__
  ASSERT_EQ(3, sscanf("Nan nan(2) inF", "%f %f %f", &c, &d, &e));
#else
  ASSERT_EQ(3, sscanf("Nan nan inF", "%f %f %f", &c, &d, &e));
#endif
  ASSERT_EQ(
      2, sscanf("0X1.BC70A3D70A3D7P+6 1.18973e+4932zzz -0.0000000123junk junk",
                "%f %f %f %f %f", &f, &g, &h, &i, &j));
  ASSERT_FLOAT_EXACTLY_EQ(111.11f, a);
  ASSERT_FLOAT_EXACTLY_EQ(-2.22f, b);
  ASSERT_TRUE(isnan(c));
  ASSERT_TRUE(isnan(d));
  ASSERT_TRUE(isinf(e));
  ASSERT_FLOAT_EXACTLY_EQ(0X1.BC70A3D70A3D7P+6f, f);
  ASSERT_TRUE(isinf(g));
}

TEST(sscanf, floating_point_documentation_examples_double_precision) {
  double a = 666.666, b = a, c = b, d = c, e = d, f = e, g = f, h = g, i = h,
         j = i;

  ASSERT_EQ(2, sscanf("111.11 -2.22", "%lf %lf", &a, &b));
  ASSERT_EQ(3, sscanf("Nan nan(2) inF", "%lf %lf %lf", &c, &d, &e));
  ASSERT_EQ(
      2, sscanf("0X1.BC70A3D70A3D7P+6 1.18973e+4932zzz -0.0000000123junk junk",
                "%lf %lf %lf %lf %lf", &f, &g, &h, &i, &j));

  ASSERT_DOUBLE_EXACTLY_EQ(111.11, a);
  ASSERT_DOUBLE_EXACTLY_EQ(-2.22, b);
  ASSERT_TRUE(isnan(c));
  ASSERT_TRUE(isnan(d));
  ASSERT_TRUE(isinf(e));
  ASSERT_DOUBLE_EXACTLY_EQ(0X1.BC70A3D70A3D7P+6, f);
  ASSERT_TRUE(isinf(g));
}

TEST(sscanf, luplus) {
  long x = 666;
  ASSERT_EQ(1, sscanf("+123", "%lu", &x));
  ASSERT_EQ(123, x);
}

TEST(sscanf, lupluser) {
  long x = 666;
  ASSERT_EQ(1, sscanf("+123", "%li", &x));
  ASSERT_EQ(123, x);
}

TEST(fscanf, stuff) {
  int x;
  char *s = "1 12 123\n"
            "4 44\n";
  FILE *f = fmemopen(s, strlen(s), "r+");
  ASSERT_EQ(1, fscanf(f, "%d", &x));
  ASSERT_EQ(1, x);
  ASSERT_EQ(1, fscanf(f, "%d", &x));
  ASSERT_EQ(12, x);
  ASSERT_EQ(1, fscanf(f, "%d", &x));
  ASSERT_EQ(123, x);
  ASSERT_EQ(1, fscanf(f, "%d", &x));
  ASSERT_EQ(4, x);
  ASSERT_EQ(1, fscanf(f, "%d", &x));
  ASSERT_EQ(44, x);
  ASSERT_EQ(-1, fscanf(f, "%d", &x));
  fclose(f);
}

TEST(fscanf, wantDecimalButGotLetter_returnsZeroMatches) {
  int x = 666;
  char *s = "a1\n";
  FILE *f = fmemopen(s, strlen(s), "r+");
  ASSERT_EQ(0, fscanf(f, "%d", &x));
  ASSERT_EQ(666, x);
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

TEST(sscanf, floating_point_hexadecimal) {
  double a = 0;
  ASSERT_EQ(1, sscanf("0x1.5014c3472bc2c0000000p-123", "%lf", &a));
  ASSERT_DOUBLE_EXACTLY_EQ(0x1.5014c3472bc2c0000000p-123, a);
}

TEST(scanf, str_size) {
  char s[4];
  ASSERT_EQ(1, sscanf("hello", "%3s", s));
  ASSERT_STREQ("hel", s);
  char t[5];
  ASSERT_EQ(2, sscanf("hello there", "%3s%4s", s, t));
  ASSERT_STREQ("hel", s);
  ASSERT_STREQ("lo", t);
}

TEST(scanf, mismatch) {
  ASSERT_EQ(0, sscanf("b", "a"));
}

TEST(scanf, notenough) {
  ASSERT_EQ(-1, sscanf("ab", "abc"));
}

TEST(scanf, numba) {
  int x = -2;
  int y = -2;
  ASSERT_EQ(-1, sscanf("   ", "%d%d", &x, &y));
  ASSERT_EQ(-2, x);
  ASSERT_EQ(-2, y);
  ASSERT_EQ(0, sscanf(" e  ", "%d%d", &x, &y));
  ASSERT_EQ(-2, x);
  ASSERT_EQ(-2, y);
  ASSERT_EQ(0, sscanf(" +  ", "%d%d", &x, &y));
  ASSERT_EQ(-2, x);
  ASSERT_EQ(-2, y);
  ASSERT_EQ(0, sscanf(" +1  ", "%1d", &x));
  ASSERT_EQ(-2, x);
  ASSERT_EQ(-2, y);
  ASSERT_EQ(1, sscanf(" +1  ", "%2d", &x));
  ASSERT_EQ(1, x);
  ASSERT_EQ(-2, y);
  ASSERT_EQ(1, sscanf(" 0x  ", "%d%d", &x, &y));
  ASSERT_EQ(0, x);
  ASSERT_EQ(-2, y);
  ASSERT_EQ(1, sscanf(" 0x", "%d%d", &x, &y));
  ASSERT_EQ(0, x);
  ASSERT_EQ(-2, y);
  ASSERT_EQ(1, sscanf(" 0x3", "%x", &x));
  ASSERT_EQ(3, x);
  ASSERT_EQ(-2, y);
  ASSERT_EQ(1, sscanf(" 0t3", "%x", &x));
  ASSERT_EQ(0, x);
  ASSERT_EQ(-2, y);
  ASSERT_EQ(1, sscanf("  123 ", "%d", &x));
  ASSERT_EQ(123, x);
  ASSERT_EQ(1, sscanf("  123 ", "%d%d", &x, &y));
  ASSERT_EQ(123, x);
  ASSERT_EQ(-2, y);
  ASSERT_EQ(2, sscanf("  123 ", "%1d%d", &x, &y));
  ASSERT_EQ(1, x);
  ASSERT_EQ(23, y);
}

TEST(scanf, numbastr) {
  int x = -2;
  char s[32] = "nope";
  ASSERT_EQ(2, sscanf("  123abc ", "%d%s", &x, s));
  ASSERT_EQ(123, x);
  ASSERT_STREQ("abc", s);
}

TEST(scanf, space) {
  int n;
  ASSERT_EQ(0, sscanf("", " \t"));
  ASSERT_EQ(0, sscanf(" \t", " \t"));
  ASSERT_EQ(0, sscanf(" \t", "\t "));
  char hope[32];
  ASSERT_EQ(1, sscanf(" \t \nhope", "\t %s", hope));
  ASSERT_STREQ("hope", hope);
  char mope[32];
  ASSERT_EQ(1, sscanf(" \t \nmope", "%s", mope));
  ASSERT_STREQ("mope", mope);
  char dope[32];
  ASSERT_EQ(1, sscanf(" \t \ndope", " %[^z]", dope));
  ASSERT_STREQ("dope", dope);
  char cope[32];
  n = -2;
  ASSERT_EQ(1, sscanf(" \t \ncope", "%[^z]%n", cope, &n));
  ASSERT_STREQ(" \t \ncope", cope);
  ASSERT_EQ(8, n);
  char trope[32] = "snow";
  n = -2;
  ASSERT_EQ(0, sscanf(" \t \ntrope", "\u2000%[^z]%n", trope, &n));
  ASSERT_STREQ("snow", trope);
  ASSERT_EQ(-2, n);
  char zope[32] = "snow";
  ASSERT_EQ(1, sscanf("\u2000zope", "%s", zope));
  ASSERT_STREQ("\u2000zope", zope);
}

TEST(scanf, string) {
  int n;
  char s[32] = "nope";
  char t[32] = "nope";
  ASSERT_EQ(-1, sscanf("", "%s", s));
  ASSERT_STREQ("nope", s);
  ASSERT_STREQ("nope", t);
  ASSERT_EQ(1, sscanf("1", "%s%s", s, t));
  ASSERT_STREQ("1", s);
  ASSERT_STREQ("nope", t);
  ASSERT_EQ(2, sscanf("11", "%1s%s%n", s, t, &n));
  ASSERT_STREQ("1", s);
  ASSERT_STREQ("1", t);
  ASSERT_EQ(2, n);
}

TEST(scanf, widestring) {
  wchar_t s[32] = L"nope";
  wchar_t t[32] = L"nope";
  ASSERT_EQ(2, sscanf("11", "%1ls%ls", s, t));
  ASSERT_WCSEQ(L"1", s);
  ASSERT_WCSEQ(L"1", t);
  ASSERT_EQ(2, sscanf("𝗮𝗮", "%1ls%ls", s, t));
  ASSERT_WCSEQ(L"𝗮", s);
  ASSERT_WCSEQ(L"𝗮", t);
}

TEST(scanf, square) {
  char version[128];
  int major, minor;
  ASSERT_EQ(3, sscanf("SSH-2.0-OpenSSH_9.7 FreeBSD-20240806\nhi",
                      "SSH-%d.%d-%[^\n]\n", &major, &minor, version));
  ASSERT_EQ(2, major);
  ASSERT_EQ(0, minor);
  ASSERT_STREQ("OpenSSH_9.7 FreeBSD-20240806", version);
}

TEST(scanf, malloc_square) {
  char *os = 0;
  char *version = 0;
  int major, minor;
  ASSERT_EQ(4, sscanf("SSH-2.0-𝐎𝗽𝗲𝗻𝐒𝐒𝐇-9.7 FreeBSD-20240806",
                      "SSH-%d.%d-%m[-0-9.A-Za-z𝐀-𝐙𝗮-𝘇]%ms", &major, &minor,
                      &version, &os));
  ASSERT_EQ(2, major);
  ASSERT_EQ(0, minor);
  ASSERT_STREQ("𝐎𝗽𝗲𝗻𝐒𝐒𝐇-9.7", version);
  ASSERT_STREQ("FreeBSD-20240806", os);
  free(version);
  free(os);
}

TEST(scanf, square_unicode) {
  char s[32];
  ASSERT_EQ(1, sscanf("𝗵𝗲𝗹𝗹𝗼z", "%[𝗮-𝘇]", s));
  ASSERT_STREQ("𝗵𝗲𝗹𝗹𝗼", s);
}

TEST(scanf, square_wide) {
  int n = -2;
  wchar_t s[32], t[32];
  ASSERT_EQ(1, sscanf("𝗵𝗲𝗹𝗹𝗼z", "%l[𝗮-𝘇]%n", s, &n));
  ASSERT_WCSEQ(L"𝗵𝗲𝗹𝗹𝗼", s);
  ASSERT_EQ(strlen("𝗵𝗲𝗹𝗹𝗼"), n);
  ASSERT_EQ(2, sscanf("𝗵𝗲𝗹𝗹𝗼z", "%l[𝗮-𝘇]%ls%n", s, t, &n));
  ASSERT_WCSEQ(L"𝗵𝗲𝗹𝗹𝗼", s);
  ASSERT_WCSEQ(L"z", t);
  ASSERT_EQ(strlen("𝗵𝗲𝗹𝗹𝗼z"), n);
  ASSERT_EQ(2, sscanf("𝗵𝗲𝗹𝗹𝗼 𝟯", "%l[𝗮-𝘇]%ls%n", s, t, &n));
  ASSERT_WCSEQ(L"𝗵𝗲𝗹𝗹𝗼", s);
  ASSERT_WCSEQ(L"𝟯", t);
  ASSERT_EQ(strlen("𝗵𝗲𝗹𝗹𝗼 𝟯"), n);
  ASSERT_EQ(2, sscanf("hello3", "%l[a-z]%ls%n", s, t, &n));
  ASSERT_WCSEQ(L"hello", s);
  ASSERT_WCSEQ(L"3", t);
  ASSERT_EQ(6, n);
#ifdef __COSMOPOLITAN__
  // glibc and musl are broken here
  ASSERT_EQ(2, sscanf("𝗵𝗲𝗹𝗹𝗼𝟯", "%l[𝗮-𝘇]%ls%n", s, t, &n));
  ASSERT_WCSEQ(L"𝗵𝗲𝗹𝗹𝗼", s);
  ASSERT_WCSEQ(L"𝟯", t);
  ASSERT_EQ(strlen("𝗵𝗲𝗹𝗹𝗼𝟯"), n);
#endif
}

TEST(scanf, square_drowned_god) {
  char s[16] = "untouched";
  ASSERT_EQ(0, sscanf(" hi", "%[a-z]", s));
  ASSERT_STREQ("untouched", s);
}

TEST(scanf, character) {
  char s[16] = "nope";
  ASSERT_EQ(1, sscanf("xxx", "%c", s));
  ASSERT_STREQ("xope", s);
  ASSERT_EQ(1, sscanf("xxx", "%2c", s));
  ASSERT_STREQ("xxpe", s);
  char t[16] = "nope";
  ASSERT_EQ(-1, sscanf("", "%2c", t));
  ASSERT_STREQ("nope", t);
  ASSERT_EQ(1, sscanf("x", "%2c", t));
  ASSERT_STREQ("xope", t);
  char *doge;
  ASSERT_EQ(1, sscanf("xxx", "%mc", &doge));
  ASSERT_EQ('x', doge[0]);
  free(doge);
}

TEST(scanf, character_leadspace) {
  char s[16] = "nope";
  ASSERT_EQ(1, sscanf(" xxx", "%c", s));
  ASSERT_STREQ(" ope", s);
}

TEST(scanf, character_utf8) {
  char s[16] = "nope";
  char t[16] = "nope";
  ASSERT_EQ(2, sscanf("𝘅𝘅𝘅", "%c%c", s, t));
  ASSERT_STREQ("\360ope", s);
  ASSERT_STREQ("\235ope", t);
}

TEST(scanf, huge) {
  // glibc behavior is to just overflow
  // fil-c has a runtime panic
  if (1)
    return;
  size_t n = (size_t)INT_MAX + 1;
  char *huge = malloc(n);
  memset(huge, ' ', n);
  int got = -2;
  ASSERT_EQ(0, sscanf(huge, " %n", &got));
  ASSERT_EQ(INT_MIN, got);
  free(huge);
}

TEST(sscanf, float) {
  int n;
  float x, y;
  ASSERT_EQ(1, sscanf("123", "%f", &x));
  ASSERT_FLOAT_EXACTLY_EQ(123, x);
  ASSERT_EQ(2, sscanf("123", "%1f%f%n", &x, &y, &n));
  ASSERT_FLOAT_EXACTLY_EQ(1, x);
  ASSERT_FLOAT_EXACTLY_EQ(23, y);
  ASSERT_EQ(3, n);
  ASSERT_EQ(1, sscanf("123 456", "%*f%f%n", &x, &n));
  ASSERT_FLOAT_EXACTLY_EQ(456, x);
  ASSERT_EQ(7, n);
  ASSERT_EQ(1, sscanf("1.19209289550781250000000000000000000e-7", "%f", &x));
  ASSERT_FLOAT_EXACTLY_EQ(FLT_EPSILON, x);
  ASSERT_EQ(1, sscanf("1.17549435082228750796873653722224568e-38", "%f", &x));
  ASSERT_FLOAT_EXACTLY_EQ(FLT_MIN, x);
  ASSERT_EQ(1, sscanf("3.40282346638528859811704183484516925e+38", "%f", &x));
  ASSERT_FLOAT_EXACTLY_EQ(FLT_MAX, x);
}

TEST(sscanf, dubble) {
  int n;
  double d;
  ASSERT_EQ(1, sscanf("123.456", "%lf%n", &d, &n));
  ASSERT_DOUBLE_EXACTLY_EQ(123.456, d);
  ASSERT_EQ(7, n);
  ASSERT_EQ(1, sscanf("2.22044604925031308084726333618164062e-16", "%lf", &d));
  ASSERT_DOUBLE_EXACTLY_EQ(DBL_EPSILON, d);
  ASSERT_EQ(1, sscanf("2.22507385850720138309023271733240406e-308", "%lf", &d));
  ASSERT_DOUBLE_EXACTLY_EQ(DBL_MIN, d);
  ASSERT_EQ(1, sscanf("1.79769313486231570814527423731704357e+308", "%lf", &d));
  ASSERT_DOUBLE_EXACTLY_EQ(DBL_MAX, d);
}

TEST(sscanf, longdubble) {
  int n;
  long double d;
  ASSERT_EQ(1, sscanf(" 2", "%Lf%n", &d, &n));
  ASSERT_LDBL_EXACTLY_EQ(2.l, d);
  ASSERT_EQ(2, n);
#if LDBL_MANT_DIG == 64
  ASSERT_EQ(1, sscanf("1.18973149535723176502126385303097021e+4932", "%Lf%n",
                      &d, &n));
  if (1)
    return;  // TODO(jart): fix bug in gdtoa with strtold rounding
  ASSERT_LDBL_EXACTLY_EQ(1.18973149535723176502126385303097021e+4932L, d);
  ASSERT_EQ(strlen("1.18973149535723176502126385303097021e+4932"), n);
  /* [jart] todo: prevent infinite loop w/ x86 LDBL_EPSILON ;_; */
#elif LDBL_MANT_DIG == 113
  ASSERT_EQ(1, sscanf("1.18973149535723176508575932662800702e+4932", "%Lf%n",
                      &d, &n));
  ASSERT_LDBL_EXACTLY_EQ(1.18973149535723176508575932662800702e+4932L, d);
  ASSERT_EQ(strlen("1.18973149535723176508575932662800702e+4932"), n);
  ASSERT_EQ(
      1, sscanf("1.92592994438723585305597794258492732e-34", "%Lf%n", &d, &n));
  ASSERT_LDBL_EXACTLY_EQ(1.92592994438723585305597794258492732e-34L, d);
  ASSERT_EQ(strlen("1.92592994438723585305597794258492732e-34"), n);
#endif
}

TEST(sscanf, int_overflow) {
  // glibc doesn't check overflow
  int x;
  ASSERT_EQ(1, sscanf("2147483648", "%d", &x));
  ASSERT_EQ(0, errno);
  ASSERT_EQ(INT_MIN, x);
}

TEST(sscanf, extreme_widths) {
  char buf[10] = {0};
  ASSERT_EQ(1, sscanf("hello", "%999s", buf));
  ASSERT_STREQ("hello", buf);
}

TEST(sscanf, boundary_integers) {
  long long x;
  unsigned long long ux;
  // Test maximum values
  ASSERT_EQ(1, sscanf("9223372036854775807", "%lld", &x));
  ASSERT_EQ(LLONG_MAX, x);
  ASSERT_EQ(1, sscanf("18446744073709551615", "%llu", &ux));
  ASSERT_EQ(ULLONG_MAX, ux);
  // Test minimum signed value
  ASSERT_EQ(1, sscanf("-9223372036854775808", "%lld", &x));
  ASSERT_EQ(LLONG_MIN, x);
}

TEST(sscanf, pathological_floating_point) {
  double d = 666.0;
  // Very long decimal expansion
  ASSERT_EQ(1, sscanf("1.23456789012345678901234567890123456789", "%lf", &d));
  ASSERT_TRUE(d > 1.23 && d < 1.25);
  // Extremely large exponent
  int n;
  ASSERT_EQ(1, sscanf("1e3000", "%lf%n", &d, &n));
  ASSERT_EQ(6, n);
  ASSERT_TRUE(isinf(d));
  // Extremely small exponent
  ASSERT_EQ(1, sscanf("1e-324", "%lf", &d));
  ASSERT_TRUE(d == 0.0 || d > 0.0);
}

TEST(sscanf, buffer_overflow_attempts) {
  char small[4];
  // String longer than buffer (should truncate)
  strcpy(small, "old");
  ASSERT_EQ(1, sscanf("verylongstring", "%3s", small));
  ASSERT_STREQ("ver", small);
  // Character field longer than buffer
  memset(small, 'x', sizeof(small));
  ASSERT_EQ(1, sscanf("abc", "%3c", small));
  ASSERT_EQ('a', small[0]);
  ASSERT_EQ('b', small[1]);
  ASSERT_EQ('c', small[2]);
}

TEST(sscanf, unicode_edge_cases) {
  char buf[64];
  wchar_t wbuf[32];
  // Invalid UTF-8 sequences
  ASSERT_EQ(1, sscanf("\xff\xfe hello", "%s", buf));
  // Surrogate pairs in wide strings
  ASSERT_EQ(1, sscanf("𝓗𝓮𝓵𝓵𝓸", "%ls", wbuf));
  ASSERT_WCSEQ(L"𝓗𝓮𝓵𝓵𝓸", wbuf);
}

TEST(sscanf, charset_edge_cases) {
  char buf[32];
  // charset with only dash
  ASSERT_EQ(1, sscanf("-hello", "%[-]", buf));
  ASSERT_STREQ("-", buf);
  // Negated charset with dash
  ASSERT_EQ(1, sscanf("hello-", "%[^-]", buf));
  ASSERT_STREQ("hello", buf);
  // Charset with closing bracket
  ASSERT_EQ(1, sscanf("]hello", "%[]a-z]", buf));
  ASSERT_STREQ("]hello", buf);
}

TEST(sscanf, memory_allocation_stress) {
  char *ptr1, *ptr2, *ptr3;
  // Multiple malloc strings in one call
  ASSERT_EQ(3, sscanf("one two three", "%ms %ms %ms", &ptr1, &ptr2, &ptr3));
  ASSERT_STREQ("one", ptr1);
  ASSERT_STREQ("two", ptr2);
  ASSERT_STREQ("three", ptr3);
  free(ptr1);
  free(ptr2);
  free(ptr3);
  // Very long malloc string
  char *longstr = malloc(10000);
  memset(longstr, 'a', 9999);
  longstr[9999] = '\0';
  char *result;
  ASSERT_EQ(1, sscanf(longstr, "%ms", &result));
  ASSERT_EQ(9999, strlen(result));
  free(result);
  free(longstr);
}

TEST(sscanf, integer_base_confusion) {
  int x = 666;
  // Hex-looking decimal
  ASSERT_EQ(1, sscanf("0xGHI", "%d", &x));
  ASSERT_EQ(0, x);
  // Octal with invalid digits
  ASSERT_EQ(1, sscanf("089", "%o", &x));
  ASSERT_EQ(0, x);  // Should stop at '8'
  // Binary with invalid digits
  ASSERT_EQ(1, sscanf("01012", "%b", &x));
  ASSERT_EQ(5, x);  // Should read "0101" = 5
}

TEST(sscanf, whitespace_edge_cases) {
  int x = 666, y = 777;
  // Various Unicode whitespace
  ASSERT_EQ(1, sscanf("123\u2000\u2001\u2028456", "%d%d", &x, &y));
  ASSERT_EQ(123, x);
  ASSERT_EQ(777, y);
  // No whitespace where expected
  ASSERT_EQ(1, sscanf("123456", "%d %d", &x, &y));
  ASSERT_EQ(123456, x);
  ASSERT_EQ(777, y);
}

TEST(sscanf, scan_set_ranges) {
  char buf[32];
  // Range crossing byte boundaries
  ASSERT_EQ(1, sscanf("ABC123", "%[A-Z0-9]", buf));
  ASSERT_STREQ("ABC123", buf);
  // Invalid range (b-a)
  ASSERT_EQ(0, sscanf("xyz", "%[z-a]", buf));
  // Range with special chars
  ASSERT_EQ(1, sscanf("!@#$", "%[!-$]", buf));
  ASSERT_STREQ("!", buf);
}

TEST(sscanf, percent_n_edge_cases) {
  int n1 = -1, n2 = -2, x = 666;
  // %n at start
  ASSERT_EQ(1, sscanf("123", "%n%d", &n1, &x));
  ASSERT_EQ(0, n1);
  ASSERT_EQ(123, x);
  // %n after failed conversion
  ASSERT_EQ(0, sscanf("abc", "%d%n", &x, &n2));
  ASSERT_EQ(123, x);
  ASSERT_EQ(-2, n2);
}

TEST(sscanf, mixed_width_and_modifiers) {
  long x = 666;
  // Width with long modifier
  ASSERT_EQ(1, sscanf("1234567890", "%5ld", &x));
  ASSERT_EQ(12345, x);
}

TEST(sscanf, floating_point_edge_cases) {
  double d = 666.0;
  // Leading zeros in exponent
  ASSERT_EQ(1, sscanf("1e00000002", "%lf", &d));
  ASSERT_DOUBLE_EXACTLY_EQ(100.0, d);
  // Hex float with no fractional part
  ASSERT_EQ(1, sscanf("0x1p4", "%lf", &d));
  ASSERT_DOUBLE_EXACTLY_EQ(16.0, d);
  // Just a decimal point
  ASSERT_EQ(0, sscanf(".", "%lf", &d));
  ASSERT_EQ(16.0, d);
  // Multiple signs
  ASSERT_EQ(0, sscanf("++123", "%lf", &d));
  ASSERT_EQ(16.0, d);
}

TEST(sscanf, character_field_boundaries) {
  char buf[8] = "initial";
  // Read exactly buffer size
  ASSERT_EQ(1, sscanf("1234567", "%6c", buf));
  ASSERT_EQ('1', buf[0]);
  ASSERT_EQ('l', buf[6]);
  // Read more than available
  strcpy(buf, "initial");
  ASSERT_EQ(1, sscanf("123", "%7c", buf));
  ASSERT_STREQ("123tial", buf);
}

TEST(sscanf, suppress_assignment_edge_cases) {
  int x = 666;
  // Multiple suppressions
  ASSERT_EQ(1, sscanf("1 2 3 4", "%*d %*d %*d %d", &x));
  ASSERT_EQ(4, x);
  // Suppressed string allocation (should not allocate)
  ASSERT_EQ(0, sscanf("hello", "%*ms"));
  // Suppressed character field
  ASSERT_EQ(1, sscanf("abc123", "%*3c%d", &x));
  ASSERT_EQ(123, x);  // Should fail to read integer
  ASSERT_EQ(0, sscanf("abcdef", "%*3c%d", &x));
  ASSERT_EQ(123, x);  // Should fail to read integer
}

TEST(sscanf, input_exhaustion) {
  int x = 666;
  int y = 777;
  int z = 888;
  // Input shorter than format expects
  ASSERT_EQ(2, sscanf("1 2", "%d %d %d", &x, &y, &z));
  ASSERT_EQ(1, x);
  ASSERT_EQ(2, y);
  ASSERT_EQ(888, z);
  // Empty input with complex format
  float f;
  char b[1];
  ASSERT_EQ(-1, sscanf("", "%d %s %f", &x, b, &f));
}

TEST(sscanf, thousands_separator) {
  int x = 666;
  ASSERT_EQ(1, sscanf("100,000", "%'d", &x));
  ASSERT_EQ(100000, x);
  ASSERT_EQ(0, sscanf("100,000e", "%'*d%n", &x));
  ASSERT_EQ(7, x);
  ASSERT_EQ(0, sscanf("100,000e", "%*'d%n", &x));
  ASSERT_EQ(7, x);
}

TEST(sscanf, the_king_of_the_north) {
  int x = 666;
  int y = 666;
  ASSERT_EQ(2, sscanf("12", "%1d %d", &x, &y));
  ASSERT_EQ(1, x);
  ASSERT_EQ(2, y);
}

TEST(sscanf, short) {
  union {
    unsigned i;
    short s;
  } u = {-1};
  ASSERT_EQ(1, sscanf("2", "%hd", &u.s));
  ASSERT_EQ(0xffffu << 16 | 2, u.i);
  ASSERT_EQ(1, sscanf("-2", "%hd", &u.s));
  ASSERT_EQ(0xffffu << 16 | (unsigned short)-2, u.i);
  ASSERT_EQ(1, sscanf("32767", "%hd", &u.s));
  ASSERT_EQ(0xffffu << 16 | 32767, u.i);
  ASSERT_EQ(1, sscanf("65535", "%hd", &u.s));
  ASSERT_EQ(0xffffu << 16 | 65535, u.i);
  ASSERT_EQ(1, sscanf("131071", "%hd", &u.s));
  ASSERT_EQ(0xffffu << 16 | 65535, u.i);
}

TEST(sscanf, char) {
  union {
    unsigned i;
    char c;
  } u = {-1};
  ASSERT_EQ(1, sscanf("2", "%hhd", &u.c));
  ASSERT_EQ(0xffffff00u | 2, u.i);
  ASSERT_EQ(1, sscanf("127", "%hhd", &u.c));
  ASSERT_EQ(0xffffff00u | 127, u.i);
  ASSERT_EQ(1, sscanf("255", "%hhd", &u.c));
  ASSERT_EQ(0xffffff00u | 255, u.i);
  ASSERT_EQ(1, sscanf("131071", "%hhd", &u.c));
  ASSERT_EQ(0xffffff00u | 65535, u.i);
}

TEST(sscanf, hex) {
  unsigned x;
  ASSERT_EQ(1, sscanf("abc", "%x", &x));
  ASSERT_EQ(0xabc, x);
  ASSERT_EQ(1, sscanf("0xabc", "%x", &x));
  ASSERT_EQ(0xabc, x);
  ASSERT_EQ(1, sscanf("0xabc", "%i", &x));
  ASSERT_EQ(0xabc, x);
}

TEST(sscanf, octal2) {
  unsigned x;
  ASSERT_EQ(1, sscanf("77", "%o", &x));
  ASSERT_EQ(077, x);
  ASSERT_EQ(1, sscanf("077", "%i", &x));
  ASSERT_EQ(077, x);
}

TEST(sscanf, binary) {
  unsigned x;
  ASSERT_EQ(1, sscanf("111000111", "%b", &x));
  ASSERT_EQ(0b111000111, x);
  ASSERT_EQ(1, sscanf("0b111000111", "%b", &x));
  ASSERT_EQ(0b111000111, x);
  ASSERT_EQ(1, sscanf("0b111000111", "%i", &x));
  ASSERT_EQ(0b111000111, x);
  ASSERT_EQ(1, sscanf("0b111000112", "%i", &x));
  ASSERT_EQ(0b11100011, x);
}

TEST(sscanf, lasthyphen) {
  char buf[16];
  ASSERT_EQ(1, sscanf("1-2-3", "%[0-9-]", buf));
  ASSERT_STREQ("1-2-3", buf);
}

TEST(sscanf, firsthyphen) {
  char buf[16];
  ASSERT_EQ(1, sscanf("1-2-3", "%[-0-9]", buf));
  ASSERT_STREQ("1-2-3", buf);
}

TEST(sscanf, badutf8) {
  char buf[16];
  ASSERT_EQ(1, sscanf("\300hi", "%s", buf));
  ASSERT_EQ(0, errno);
  ASSERT_STREQ("\300hi", buf);
}

TEST(sscanf, badutf8wide1) {
  int n = 666;
  wchar_t buf[16] = {' ', ' ', ' ', ' ', ' '};
  errno = 0;
  ASSERT_EQ(0, sscanf("\300hi", "%ls%n", buf, &n));
  ASSERT_EQ(EILSEQ, errno);
  ASSERT_WCSEQ(L"     ", buf);
  ASSERT_EQ(666, n);
}

TEST(sscanf, badutf8wide2) {
  int n = 666;
  wchar_t buf[16] = {' ', ' ', ' ', ' ', ' '};
  errno = 0;
  ASSERT_EQ(0, sscanf("hi\300hi", "%ls%n", buf, &n));
  ASSERT_EQ(EILSEQ, errno);
  ASSERT_WCSEQ(L"hi   ", buf);
  ASSERT_EQ(666, n);
}

TEST(sscanf, widechar) {
  wchar_t c = -1;
  ASSERT_EQ(1, sscanf("h", "%lc", &c));
  ASSERT_EQ(L'h', c);
}

TEST(sscanf, unsigned_negative) {
  unsigned x;
  ASSERT_EQ(1, sscanf("-123", "%u", &x));
  ASSERT_EQ(-123u, x);
  ASSERT_EQ(1, sscanf("+123", "%u", &x));
  ASSERT_EQ(+123u, x);
}

TEST(sscanf, hex_negative) {
  unsigned x;
  ASSERT_EQ(1, sscanf("-0x123", "%x", &x));
  ASSERT_EQ(-0x123u, x);
}

TEST(sscanf, partial) {
  char *b;
  int x = 666;
  ASSERT_EQ(1, sscanf("hello kitty", "%ms%d", &b, &x));
  ASSERT_STREQ("hello", b);
  ASSERT_EQ(666, x);
  free(b);
}

TEST(sscanf, beyond_the_astral_planes) {
  int n;
  wchar_t s[128];
  wchar_t w[128];
  ASSERT_EQ(2, sscanf("𝗲\xf4\x9d\x97\xb9𝗹𝗼𝟯", "%l[𝗮-𝘇]%ls%n", s, w, &n));
  ASSERT_WCSEQ(L"𝗲", s);
  // max UNICODE codepoint is officially 10FFFD
  wchar_t want[128] = {0x11D5F9, L'𝗹', L'𝗼', L'𝟯'};
  ASSERT_WCSEQ(want, w);
  ASSERT_EQ(strlen("𝗲\xf4\x9d\x97\xb9𝗹𝗼𝟯"), n);
}

TEST(sscanf, yatta) {
  int n;
  wchar_t s[128];
  ASSERT_EQ(1, sscanf("𝗲𱬹𝗹𝗼𝗹𝗹𝗼𝟟", "%lls%n", s, &n));
  ASSERT_WCSEQ(L"𝗲𱬹𝗹𝗼𝗹𝗹𝗼𝟟", s);
  ASSERT_EQ(strlen("𝗲𱬹𝗹𝗼𝗹𝗹𝗼𝟟"), n);
}

TEST(sscanf, base36) {
  int x;
  ASSERT_EQ(1, sscanf("21I3V9", "%y", &x));
  ASSERT_EQ(123456789, x);
}

TEST(sscanf, bad_to_the_bone) {
  // glibc just returns zero
  ASSERT_EQ(-1, sscanf("hi", "%l"));
  ASSERT_EQ(EINVAL, errno);
}

TEST(sscanf, bad_to_the_bone2) {
  // glibc just returns zero
  ASSERT_EQ(-1, sscanf("hi", "%"));
  ASSERT_EQ(EINVAL, errno);
}

TEST(sscanf, bad_to_the_bone3) {
  // glibc just returns zero
  ASSERT_EQ(-1, sscanf("hi", "%-3d"));
  ASSERT_EQ(EINVAL, errno);
}

TEST(sscanf, bad_to_the_bone4) {
  ASSERT_EQ(0, sscanf("hi", "%s", (char *)0));
}

TEST(sscanf, rawline) {
  char line[32];
  ASSERT_EQ(1, sscanf("_\377_\200_", "%[^\n]", line));
  ASSERT_STREQ("_\377_\200_", line);
}

TEST(sscanf, fear) {
  long l = {-1};
  ASSERT_EQ(0, sscanf("hello", "%*s%ln", &l));
  ASSERT_EQ(5, l);
}

TEST(sscanf, nan) {
  float f;
  char s[16];
  ASSERT_EQ(2, sscanf("nanhi", "%f%s", &f, s));
  ASSERT_TRUE(isnan(f));
  ASSERT_STREQ("hi", s);
  ASSERT_EQ(2, sscanf("nanhi", "%f %s", &f, s));
  ASSERT_TRUE(isnan(f));
  ASSERT_STREQ("hi", s);
}

TEST(sscanf, charset) {
  int n;
  char s[16];
  ASSERT_EQ(1, sscanf("abcd", "%[a-c]%n", s, &n));
  ASSERT_STREQ("abc", s);
  ASSERT_EQ(3, n);
  ASSERT_EQ(1, sscanf("abcd", "%[c-a]%n", s, &n));
  ASSERT_STREQ("a", s);
  ASSERT_EQ(1, n);
}

TEST(sscanf, floating_point_f_exact_stop) {
  float f = 666.0f;
  int n = -1;
  char remainder[32];

  // Test %f stops at invalid character
  ASSERT_EQ(1, sscanf("123.45abc", "%f%n", &f, &n));
  ASSERT_FLOAT_EXACTLY_EQ(123.45f, f);
  ASSERT_EQ(6, n);

  // Test %f with next format specifier
  ASSERT_EQ(2, sscanf("123.45abc", "%f%s", &f, remainder));
  ASSERT_FLOAT_EXACTLY_EQ(123.45f, f);
  ASSERT_STREQ("abc", remainder);

  // Test %f stops at space
  ASSERT_EQ(1, sscanf("123.45 789", "%f%n", &f, &n));
  ASSERT_FLOAT_EXACTLY_EQ(123.45f, f);
  ASSERT_EQ(6, n);

  // Test %f with scientific notation stops at invalid char
  ASSERT_EQ(1, sscanf("1.23e5xyz", "%f%n", &f, &n));
  ASSERT_FLOAT_EXACTLY_EQ(1.23e5f, f);
  ASSERT_EQ(6, n);

  // Test %f stops immediately at invalid start
  f = 666.0f;
  n = -1;
  ASSERT_EQ(0, sscanf("abc123", "%f%n", &f, &n));
  ASSERT_FLOAT_EXACTLY_EQ(666.0f, f);
  ASSERT_EQ(-1, n);

  // Test %f with just decimal point
  ASSERT_EQ(0, sscanf(".abc", "%f%n", &f, &n));
  ASSERT_FLOAT_EXACTLY_EQ(666.0f, f);
  ASSERT_EQ(-1, n);

  // Test %f with valid decimal
  ASSERT_EQ(1, sscanf(".123abc", "%f%n", &f, &n));
  ASSERT_FLOAT_EXACTLY_EQ(0.123f, f);
  ASSERT_EQ(4, n);

  // Test %f stops at second decimal point
  ASSERT_EQ(1, sscanf("12.34.56", "%f%n", &f, &n));
  ASSERT_FLOAT_EXACTLY_EQ(12.34f, f);
  ASSERT_EQ(5, n);

  // Test %f stops at invalid exponent
  ASSERT_EQ(1, sscanf("12.34eabc", "%f%n", &f, &n));
  ASSERT_FLOAT_EXACTLY_EQ(12.34f, f);
  ASSERT_EQ(6, n);

  // Test %f with incomplete exponent
  ASSERT_EQ(1, sscanf("12.34e+", "%f%n", &f, &n));
  ASSERT_FLOAT_EXACTLY_EQ(12.34f, f);
  ASSERT_EQ(7, n);
}

TEST(sscanf, floating_point_g_exact_stop) {
  float f = 666.0f;
  int n = -1;
  char remainder[32];

  // Test %g stops at invalid character
  ASSERT_EQ(1, sscanf("123.45abc", "%g%n", &f, &n));
  ASSERT_FLOAT_EXACTLY_EQ(123.45f, f);
  ASSERT_EQ(6, n);

  // Test %g with next format specifier
  ASSERT_EQ(2, sscanf("123.45abc", "%g%s", &f, remainder));
  ASSERT_FLOAT_EXACTLY_EQ(123.45f, f);
  ASSERT_STREQ("abc", remainder);

  // Test %g with scientific notation stops at invalid char
  ASSERT_EQ(1, sscanf("1.23e5xyz", "%g%n", &f, &n));
  ASSERT_FLOAT_EXACTLY_EQ(1.23e5f, f);
  ASSERT_EQ(6, n);

  // Test %g stops immediately at invalid start
  f = 666.0f;
  n = -1;
  ASSERT_EQ(0, sscanf("abc123", "%g%n", &f, &n));
  ASSERT_FLOAT_EXACTLY_EQ(666.0f, f);
  ASSERT_EQ(-1, n);

  // Test %g with valid decimal
  ASSERT_EQ(1, sscanf(".123abc", "%g%n", &f, &n));
  ASSERT_FLOAT_EXACTLY_EQ(0.123f, f);
  ASSERT_EQ(4, n);

  // Test %g stops at second decimal point
  ASSERT_EQ(1, sscanf("12.34.56", "%g%n", &f, &n));
  ASSERT_FLOAT_EXACTLY_EQ(12.34f, f);
  ASSERT_EQ(5, n);
}

TEST(sscanf, floating_point_e_exact_stop) {
  float f = 666.0f;
  int n = -1;
  char remainder[32];

  // Test %e stops at invalid character
  ASSERT_EQ(1, sscanf("1.23e5abc", "%e%n", &f, &n));
  ASSERT_FLOAT_EXACTLY_EQ(1.23e5f, f);
  ASSERT_EQ(6, n);

  // Test %e with next format specifier
  ASSERT_EQ(2, sscanf("1.23e5abc", "%e%s", &f, remainder));
  ASSERT_FLOAT_EXACTLY_EQ(1.23e5f, f);
  ASSERT_STREQ("abc", remainder);

  // Test %e stops immediately at invalid start
  f = 666.0f;
  n = -1;
  ASSERT_EQ(0, sscanf("abc123", "%e%n", &f, &n));
  ASSERT_FLOAT_EXACTLY_EQ(666.0f, f);
  ASSERT_EQ(-1, n);

  // Test %e with just mantissa (no exponent)
  ASSERT_EQ(1, sscanf("123.45xyz", "%e%n", &f, &n));
  ASSERT_FLOAT_EXACTLY_EQ(123.45f, f);
  ASSERT_EQ(6, n);

  // Test %e stops at invalid exponent
  ASSERT_EQ(1, sscanf("12.34eabc", "%e%n", &f, &n));
  ASSERT_FLOAT_EXACTLY_EQ(12.34f, f);
  ASSERT_EQ(6, n);

  // Test %e with incomplete exponent
  ASSERT_EQ(1, sscanf("12.34e+", "%e%n", &f, &n));
  ASSERT_FLOAT_EXACTLY_EQ(12.34f, f);
  ASSERT_EQ(7, n);
}

TEST(sscanf, floating_point_a_exact_stop) {
  float f = 666.0f;
  int n = -1;
  char remainder[32];

  // Test %a stops at invalid character
  ASSERT_EQ(1, sscanf("0x1.23p4abc", "%a%n", &f, &n));
  ASSERT_FLOAT_EXACTLY_EQ(0x1.23p4f, f);
  ASSERT_EQ(8, n);

  // Test %a with next format specifier
  ASSERT_EQ(2, sscanf("0x1.23p4abc", "%a%s", &f, remainder));
  ASSERT_FLOAT_EXACTLY_EQ(0x1.23p4f, f);
  ASSERT_STREQ("abc", remainder);

  // Test %a stops immediately at invalid start
  f = 666.0f;
  n = -1;
  ASSERT_EQ(0, sscanf("abc123", "%a%n", &f, &n));
  ASSERT_FLOAT_EXACTLY_EQ(666.0f, f);
  ASSERT_EQ(-1, n);

  // Test %a with just hex prefix
  ASSERT_EQ(1, sscanf("0x1.5xyz", "%a%n", &f, &n));
  ASSERT_FLOAT_EXACTLY_EQ(0x1.5p0f, f);
  ASSERT_EQ(5, n);

  // Test %a stops at invalid hex digit in mantissa
  ASSERT_EQ(1, sscanf("0x1.23ghi", "%a%n", &f, &n));
  ASSERT_FLOAT_EXACTLY_EQ(0x1.23p0f, f);
  ASSERT_EQ(6, n);

  // Test %a stops at invalid exponent
  ASSERT_EQ(1, sscanf("0x1.23pabc", "%a%n", &f, &n));
  ASSERT_FLOAT_EXACTLY_EQ(0x1.23p0f, f);
  ASSERT_EQ(7, n);

  // Test %a with incomplete exponent
  ASSERT_EQ(1, sscanf("0x1.23p+", "%a%n", &f, &n));
  ASSERT_FLOAT_EXACTLY_EQ(0x1.23p0f, f);
  ASSERT_EQ(8, n);
}

TEST(sscanf, floating_point_mixed_sequences) {
  float f = 666.0f;
  int n = -1;

  // Test mixed valid/invalid sequences
  ASSERT_EQ(1, sscanf("123abc456", "%f%n", &f, &n));
  ASSERT_FLOAT_EXACTLY_EQ(123.0f, f);
  ASSERT_EQ(3, n);

  // Test multiple dots
  ASSERT_EQ(1, sscanf("12.34.56.78", "%f%n", &f, &n));
  ASSERT_FLOAT_EXACTLY_EQ(12.34f, f);
  ASSERT_EQ(5, n);

  // Test exponent followed by invalid
  ASSERT_EQ(1, sscanf("1e2x3e4", "%f%n", &f, &n));
  ASSERT_FLOAT_EXACTLY_EQ(1e2f, f);
  ASSERT_EQ(3, n);

  // Test sign followed by invalid
  ASSERT_EQ(0, sscanf("+abc", "%f%n", &f, &n));
  ASSERT_FLOAT_EXACTLY_EQ(100.0f, f);
  ASSERT_EQ(3, n);

  // Test valid sign followed by invalid
  ASSERT_EQ(1, sscanf("+123abc", "%f%n", &f, &n));
  ASSERT_FLOAT_EXACTLY_EQ(123.0f, f);
  ASSERT_EQ(4, n);

  // Test infinity variants
  ASSERT_EQ(1, sscanf("infabc", "%f%n", &f, &n));
  ASSERT_TRUE(isinf(f));
  ASSERT_EQ(3, n);

  ASSERT_EQ(1, sscanf("infinityabc", "%f%n", &f, &n));
  ASSERT_TRUE(isinf(f));
  ASSERT_EQ(8, n);

  // Test NaN variants
  ASSERT_EQ(1, sscanf("nanabc", "%f%n", &f, &n));
  ASSERT_TRUE(isnan(f));
  ASSERT_EQ(3, n);
}

TEST(sscanf, floating_point_width_stop) {
  float f = 666.0f;
  int n = -1;
  char remainder[32];

  // Test width limiting stops parsing
  ASSERT_EQ(1, sscanf("123.456789", "%5f%n", &f, &n));
  ASSERT_FLOAT_EXACTLY_EQ(123.4f, f);
  ASSERT_EQ(5, n);

  // Test width with following format specifier
  ASSERT_EQ(2, sscanf("123.456789", "%5f%s", &f, remainder));
  ASSERT_FLOAT_EXACTLY_EQ(123.4f, f);
  ASSERT_STREQ("56789", remainder);

  // Test width cuts off in middle of number
  ASSERT_EQ(1, sscanf("123.456", "%3f%n", &f, &n));
  ASSERT_FLOAT_EXACTLY_EQ(123.0f, f);
  ASSERT_EQ(3, n);

  // Test width with scientific notation
  ASSERT_EQ(1, sscanf("1.23e567", "%6f%n", &f, &n));
  ASSERT_FLOAT_EXACTLY_EQ(1.23e5f, f);
  ASSERT_EQ(6, n);

  // Test width cuts off exponent
  ASSERT_EQ(1, sscanf("1.23e+567", "%7f%n", &f, &n));
  ASSERT_FLOAT_EXACTLY_EQ(1.23e5f, f);
  ASSERT_EQ(7, n);

  // Test width smaller than minimum valid number
  f = 666.0f;
  n = -1;
  ASSERT_EQ(1, sscanf("1.23", "%1f%n", &f, &n));
  ASSERT_FLOAT_EXACTLY_EQ(1, f);
  ASSERT_EQ(1, n);

  // Test width allows single digit
  ASSERT_EQ(1, sscanf("1.23", "%1f%n", &f, &n));
  ASSERT_FLOAT_EXACTLY_EQ(1.0f, f);
  ASSERT_EQ(1, n);
}

TEST(sscanf, floating_point_edge_cases2) {
  float f = 666.0f;
  double d = 666.0;
  int n = -1;

  // Test multiple signs
  ASSERT_EQ(0, sscanf("++123", "%f%n", &f, &n));
  ASSERT_FLOAT_EXACTLY_EQ(666.0f, f);
  ASSERT_EQ(-1, n);

  ASSERT_EQ(0, sscanf("--123", "%f%n", &f, &n));
  ASSERT_FLOAT_EXACTLY_EQ(666.0f, f);
  ASSERT_EQ(-1, n);

  ASSERT_EQ(0, sscanf("+-123", "%f%n", &f, &n));
  ASSERT_FLOAT_EXACTLY_EQ(666.0f, f);
  ASSERT_EQ(-1, n);

  // Test lone decimal point
  ASSERT_EQ(0, sscanf(".", "%f%n", &f, &n));
  ASSERT_FLOAT_EXACTLY_EQ(666.0f, f);
  ASSERT_EQ(-1, n);

  // Test lone sign
  ASSERT_EQ(0, sscanf("+", "%f%n", &f, &n));
  ASSERT_FLOAT_EXACTLY_EQ(666.0f, f);
  ASSERT_EQ(-1, n);

  ASSERT_EQ(0, sscanf("-", "%f%n", &f, &n));
  ASSERT_FLOAT_EXACTLY_EQ(666.0f, f);
  ASSERT_EQ(-1, n);

  // Test lone exponent marker
  ASSERT_EQ(0, sscanf("e", "%f%n", &f, &n));
  ASSERT_FLOAT_EXACTLY_EQ(666.0f, f);
  ASSERT_EQ(-1, n);

  ASSERT_EQ(0, sscanf("E", "%f%n", &f, &n));
  ASSERT_FLOAT_EXACTLY_EQ(666.0f, f);
  ASSERT_EQ(-1, n);

  // Test number followed immediately by exponent marker
  ASSERT_EQ(1, sscanf("123e", "%f%n", &f, &n));
  ASSERT_FLOAT_EXACTLY_EQ(123.0f, f);
  ASSERT_EQ(4, n);

  // Test exponent with no digits
  ASSERT_EQ(1, sscanf("123e+", "%f%n", &f, &n));
  ASSERT_FLOAT_EXACTLY_EQ(123.0f, f);
  ASSERT_EQ(5, n);

  // Test hexadecimal float edge cases
  ASSERT_EQ(0, sscanf("0x", "%a%n", &f, &n));
  ASSERT_FLOAT_EXACTLY_EQ(123.0f, f);
  ASSERT_EQ(5, n);

  ASSERT_EQ(1, sscanf("0x1p", "%a%n", &f, &n));
  ASSERT_FLOAT_EXACTLY_EQ(0x1p0f, f);
  ASSERT_EQ(4, n);

  // Test case insensitive hex digits
  ASSERT_EQ(1, sscanf("0xaBcDeF.12p3xyz", "%a%n", &f, &n));
  ASSERT_FLOAT_EXACTLY_EQ(0xaBcDeF.12p3f, f);
  ASSERT_EQ(13, n);

  // Double precision tests
  ASSERT_EQ(1, sscanf("123.456abc", "%lf%n", &d, &n));
  ASSERT_DOUBLE_EXACTLY_EQ(123.456, d);
  ASSERT_EQ(7, n);
}

BENCH(sscanf, bench) {
  char b1[128];
  char b2[128];
  BENCHMARK(1000, 1, sscanf("X", "X"));
  BENCHMARK(1000, 1, sscanf("2", "%s", b1));
  BENCHMARK(1000, 1, sscanf("chaosisaladder onceuponatime", "%s %s", b1, b2));
  BENCHMARK(1000, 1, sscanf("chaosi𝘀𝗮𝗹𝗮𝗱𝗱𝗲𝗿 onceup𝗼𝗻𝗮𝘁𝗶𝗺𝗲", "%s %s", b1, b2));
  char16_t b16_1[128];
  char16_t b16_2[128];
  BENCHMARK(1000, 1,
            sscanf("chaosi𝘀𝗮𝗹𝗮𝗱𝗱𝗲𝗿 onceup𝗼𝗻𝗮𝘁𝗶𝗺𝗲", "%hs %hs", b16_1, b16_2));
  BENCHMARK(
      1000, 1,
      sscanf("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789",
             "%[0-9A-Za-z]", b1));
  int a, b, c;
  BENCHMARK(1000, 1, sscanf("2", "%d", &a));
  BENCHMARK(1000, 1,
            sscanf("2147483647 2147483647 -2147483648", "%d%d%d", &a, &b, &c));
  float f;
  BENCHMARK(1000, 1, sscanf("2", "%f", &f));
  BENCHMARK(1000, 1,
            sscanf("1.19209289550781250000000000000000000e-7", "%f", &f));
  BENCHMARK(1000, 1,
            sscanf("1.17549435082228750796873653722224568e-38", "%f", &f));
  BENCHMARK(1000, 1,
            sscanf("3.40282346638528859811704183484516925e+38", "%f", &f));
  double d;
  BENCHMARK(1000, 1, sscanf("2", "%lf", &d));
  BENCHMARK(1000, 1,
            sscanf("2.22044604925031308084726333618164062e-16", "%lf", &d));
  BENCHMARK(1000, 1,
            sscanf("2.22507385850720138309023271733240406e-308", "%lf", &d));
  BENCHMARK(1000, 1,
            sscanf("1.79769313486231570814527423731704357e+308", "%lf", &d));
}
