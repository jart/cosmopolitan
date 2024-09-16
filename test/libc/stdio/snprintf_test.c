/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2023 Gabriel Ravier                                                │
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
#include "libc/runtime/fenv.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/testlib/testlib.h"

TEST(snprintf, testVeryLargePrecision) {
  char buf[512] = {};
  int i = snprintf(buf, sizeof(buf), "%.9999u", 10);

  ASSERT_EQ(9999, i);
  ASSERT_EQ(511, strlen(buf));
}

TEST(snprintf, testPlusFlagOnChar) {
  char buf[10] = {};
  int i = snprintf(buf, sizeof(buf), "%+c", '=');

  ASSERT_EQ(1, i);
  ASSERT_STREQ("=", buf);
}

TEST(snprintf, testInf) {
  char buf[10] = {};
  int i = snprintf(buf, sizeof(buf), "%f", 1.0 / 0.0);

  ASSERT_EQ(3, i);
  ASSERT_STREQ("inf", buf);

  memset(buf, '\0', 4);
  i = snprintf(buf, sizeof(buf), "%Lf", 1.0L / 0.0L);
  ASSERT_EQ(3, i);
  ASSERT_STREQ("inf", buf);

  memset(buf, '\0', 4);
  i = snprintf(buf, sizeof(buf), "%e", 1.0 / 0.0);
  ASSERT_EQ(3, i);
  ASSERT_STREQ("inf", buf);

  memset(buf, '\0', 4);
  i = snprintf(buf, sizeof(buf), "%Le", 1.0L / 0.0L);
  ASSERT_EQ(3, i);
  ASSERT_STREQ("inf", buf);

  memset(buf, '\0', 4);
  i = snprintf(buf, sizeof(buf), "%g", 1.0 / 0.0);
  ASSERT_EQ(3, i);
  ASSERT_STREQ("inf", buf);

  memset(buf, '\0', 4);
  i = snprintf(buf, sizeof(buf), "%Lg", 1.0L / 0.0L);
  ASSERT_EQ(3, i);
  ASSERT_STREQ("inf", buf);

  for (i = 4; i < 10; ++i)
    ASSERT_EQ('\0', buf[i]);
}

TEST(snprintf, testUppercaseCConversionSpecifier) {
  char buf[10] = {};
  int i = snprintf(buf, sizeof(buf), "%C", L'a');

  ASSERT_EQ(1, i);
  ASSERT_STREQ("a", buf);

  i = snprintf(buf, sizeof(buf), "%C", L'☺');
  ASSERT_EQ(3, i);
  ASSERT_STREQ("☺", buf);
}

// Make sure we don't va_arg the wrong argument size on wide character
// conversion specifiers
TEST(snprintf,
     testWideCConversionSpecifierWithLotsOfArgumentsBeforeAndOneAfter) {
  char buf[20] = {};
  int i = snprintf(buf, sizeof(buf), "%d%d%d%d%d%d%d%d%lc%d", 0, 0, 0, 0, 0, 0,
                   0, 0, L'x', 1);

  ASSERT_EQ(10, i);
  ASSERT_STREQ("00000000x1", buf);

  memset(buf, 0, sizeof(buf));
  i = snprintf(buf, sizeof(buf), "%d%d%d%d%d%d%d%d%C%d", 0, 0, 0, 0, 0, 0, 0, 0,
               L'x', 1);
  ASSERT_EQ(10, i);
  ASSERT_STREQ("00000000x1", buf);
}

static void check_n_buffer_contents(char buf[350]) {
  for (int i = 0; i < 284; ++i)
    ASSERT_EQ(' ', buf[i]);
  ASSERT_STREQ("428463", &buf[284]);
  for (int i = 290; i < 350; ++i)
    ASSERT_EQ('\0', buf[i]);
}

TEST(snprintf, testNConversionSpecifier) {
  char buf[350] = {};

  int n_res_int = -1;
  int i = snprintf(buf, sizeof(buf), "%286d%d%n%d", 42, 84, &n_res_int, 63);
  ASSERT_EQ(290, i);
  check_n_buffer_contents(buf);
  ASSERT_EQ(288, n_res_int);

  memset(&buf, '\0', sizeof(buf));
  long n_res_long = -1;
  i = snprintf(buf, sizeof(buf), "%286ld%ld%ln%ld", 42L, 84L, &n_res_long, 63L);
  ASSERT_EQ(290, i);
  check_n_buffer_contents(buf);
  ASSERT_EQ(288, n_res_long);

  memset(&buf, '\0', sizeof(buf));
  long long n_res_long_long = -1;
  i = snprintf(buf, sizeof(buf), "%286lld%lld%lln%lld", 42LL, 84LL,
               &n_res_long_long, 63LL);
  ASSERT_EQ(290, i);
  check_n_buffer_contents(buf);
  ASSERT_EQ(288, n_res_long_long);

  ASSERT_EQ(sizeof(short), 2);
  ASSERT_EQ(sizeof(int), 4);
  memset(&buf, '\0', sizeof(buf));
  short n_res_short = -1;
  i = snprintf(buf, sizeof(buf), "%286hd%hd%hn%hd", (42 | 0xFFFF0000),
               (84 | 0xFFFF0000), &n_res_short, (63 | 0xFFFF0000));
  ASSERT_EQ(290, i);
  check_n_buffer_contents(buf);
  ASSERT_EQ(288, n_res_short);

  ASSERT_EQ(sizeof(unsigned char), 1);
  memset(&buf, '\0', sizeof(buf));
  signed char n_res_char = -1;
  i = snprintf(buf, sizeof(buf), "%286hhd%hhd%hhn%hhd", (42 | 0xFFFFFF00),
               (84 | 0xFFFFFF00), &n_res_char, (63 | 0xFFFFFF00));
  ASSERT_EQ(290, i);
  check_n_buffer_contents(buf);
  ASSERT_EQ((signed char)288, n_res_char);

  memset(&buf, '\0', sizeof(buf));
  ssize_t n_res_size_t = -1;
  i = snprintf(buf, sizeof(buf), "%286zd%zd%zn%zd", (ssize_t)42, (ssize_t)84,
               &n_res_size_t, (ssize_t)63);
  ASSERT_EQ(290, i);
  check_n_buffer_contents(buf);
  ASSERT_EQ(288, n_res_size_t);

  memset(&buf, '\0', sizeof(buf));
  intmax_t n_res_intmax_t = -1;
  i = snprintf(buf, sizeof(buf), "%286jd%jd%jn%jd", (intmax_t)42, (intmax_t)84,
               &n_res_intmax_t, (intmax_t)63);
  ASSERT_EQ(290, i);
  check_n_buffer_contents(buf);
  ASSERT_EQ(288, n_res_intmax_t);

  memset(&buf, '\0', sizeof(buf));
  int128_t n_res_int128_t = -1;
  i = snprintf(buf, sizeof(buf), "%286jjd%jjd%jjn%jjd", (int128_t)42,
               (int128_t)84, &n_res_int128_t, (int128_t)63);
  ASSERT_EQ(290, i);
  check_n_buffer_contents(buf);
  ASSERT_EQ(288, n_res_int128_t);

  memset(&buf, '\0', sizeof(buf));
  ptrdiff_t n_res_ptrdiff_t = -1;
  i = snprintf(buf, sizeof(buf), "%286td%td%tn%td", (ptrdiff_t)42,
               (ptrdiff_t)84, &n_res_ptrdiff_t, (ptrdiff_t)63);
  ASSERT_EQ(290, i);
  check_n_buffer_contents(buf);
  ASSERT_EQ(288, n_res_ptrdiff_t);
}

TEST(snprintf, testLongDoubleEConversionSpecifier) {
  char buf[20] = {};
  int i = snprintf(buf, sizeof(buf), "%Le", 1234567.8L);

  ASSERT_EQ(12, i);
  ASSERT_STREQ("1.234568e+06", buf);
}

TEST(snprintf, testLongDoubleRounding) {
  int previous_rounding = fegetround();
  ASSERT_EQ(0, fesetround(FE_DOWNWARD));

  char buf[20];
  int i = snprintf(buf, sizeof(buf), "%.3Lf", 4.4375L);
  ASSERT_EQ(5, i);
  ASSERT_STREQ("4.437", buf);

  i = snprintf(buf, sizeof(buf), "%.3Lf", -4.4375L);
  ASSERT_EQ(6, i);
  ASSERT_STREQ("-4.438", buf);

  ASSERT_EQ(0, fesetround(FE_TOWARDZERO));

  i = snprintf(buf, sizeof(buf), "%.3Lf", -4.4375L);
  ASSERT_EQ(6, i);
  ASSERT_STREQ("-4.437", buf);

  ASSERT_EQ(0, fesetround(previous_rounding));
}

void check_a_conversion_specifier_double(const char *fmt, const char *expected_str, double value) {
  char buf[30] = {0};
  int i = snprintf(buf, sizeof(buf), fmt, value);

  ASSERT_EQ(strlen(expected_str), i);
  ASSERT_STREQ(expected_str, buf);
}

void check_a_conversion_specifier_long_double(const char *fmt, const char *expected_str, long double value) {
  char buf[30] = {0};
  int i = snprintf(buf, sizeof(buf), fmt, value);

  ASSERT_EQ(strlen(expected_str), i);
  ASSERT_STREQ(expected_str, buf);
}

void check_a_conversion_specifier_double_prec_1(const char *expected_str, double value) {
  check_a_conversion_specifier_double("%.1a", expected_str, value);
}

TEST(snprintf, testAConversionSpecifierRounding) {
  int previous_rounding = fegetround();

  ASSERT_EQ(0, fesetround(FE_DOWNWARD));
  check_a_conversion_specifier_double_prec_1("0x1.fp+4", 0x1.fffffp+4);

  ASSERT_EQ(0, fesetround(FE_UPWARD));
  check_a_conversion_specifier_double_prec_1("0x2.0p+4", 0x1.f8p+4);

  ASSERT_EQ(0, fesetround(previous_rounding));
}

// This test specifically checks that we round to even, accordingly to IEEE
// rules
TEST(snprintf, testAConversionSpecifier) {
  check_a_conversion_specifier_double_prec_1("0x1.8p+4", 0x1.7800000000001p+4);
  check_a_conversion_specifier_double_prec_1("0x1.8p+4", 0x1.78p+4);
  check_a_conversion_specifier_double_prec_1("0x1.8p+4", 0x1.88p+4);
  check_a_conversion_specifier_double_prec_1("0x1.6p+4", 0x1.58p+4);
  check_a_conversion_specifier_double_prec_1("0x1.6p+4", 0x1.68p+4);
  check_a_conversion_specifier_double_prec_1("0x1.ap+4", 0x1.98p+4);
  check_a_conversion_specifier_double_prec_1("0x1.ap+4", 0x1.a8p+4);

  check_a_conversion_specifier_double("%#a", "0x0.p+0", 0x0.0p0);
  check_a_conversion_specifier_double("%#A", "0X0.P+0", 0x0.0p0);
  check_a_conversion_specifier_long_double("%#La", "0x0.p+0", 0x0.0p0L);
  check_a_conversion_specifier_long_double("%#LA", "0X0.P+0", 0x0.0p0L);

  check_a_conversion_specifier_double("%.2a", "0x1.00p-1026", 0xf.fffp-1030);
  check_a_conversion_specifier_long_double("%.1La", "0x1.0p+1", 1.999L);
}

TEST(snprintf, apostropheFlag) {
  char buf[20];
  int i = snprintf(buf, sizeof(buf), "%'d", 1000000);
  ASSERT_EQ(7, i);
  ASSERT_STREQ("1000000", buf);
}
