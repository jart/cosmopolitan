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

static void check_single_double(const char *fmt, const char *expected_str,
                                double value) {
  char buf[30] = {0};
  int i = snprintf(buf, sizeof(buf), fmt, value);

  ASSERT_GE(sizeof(buf), strlen(expected_str));
  ASSERT_EQ(strlen(expected_str), i);
  ASSERT_STREQ(expected_str, buf);
  while (i < sizeof(buf))
    ASSERT_EQ('\0', buf[i++]);
}

static void check_single_long_double(const char *fmt, const char *expected_str,
                                     long double value) {
  char buf[30] = {0};
  int i = snprintf(buf, sizeof(buf), fmt, value);

  ASSERT_GE(sizeof(buf), strlen(expected_str));
  ASSERT_EQ(strlen(expected_str), i);
  ASSERT_STREQ(expected_str, buf);
  while (i < sizeof(buf))
    ASSERT_EQ('\0', buf[i++]);
}

void check_single_long_double_arr_allowed(
    const char *fmt, const char *allowed_strs[], long double value) {
  char buf[30] = {0};
  int res = snprintf(buf, sizeof(buf), fmt, value);

  for (size_t i = 0; allowed_strs[i] != NULL; ++i)
    if (strlen(allowed_strs[i]) == res && strcmp(allowed_strs[i], buf) == 0)
      return;

  printf("Failed to find matching str for %`'s, allowed strs:\n", buf);
  for (size_t i = 0; allowed_strs[i] != NULL; ++i)
    printf("- %`'s\n", allowed_strs[i]);
  fflush(stdout);
  ASSERT_EQ(false, true);
}

static void check_single_int(const char *fmt, const char *expected_str,
                             int value) {
  char buf[30] = {0};
  int i = snprintf(buf, sizeof(buf), fmt, value);

  ASSERT_GE(sizeof(buf), strlen(expected_str));
  ASSERT_EQ(strlen(expected_str), i);
  ASSERT_STREQ(expected_str, buf);
  while (i < sizeof(buf))
    ASSERT_EQ('\0', buf[i++]);
}

static void check_single_wint_t(const char *fmt, const char *expected_str,
                                wint_t value) {
  char buf[30] = {0};
  int i = snprintf(buf, sizeof(buf), fmt, value);

  ASSERT_GE(sizeof(buf), strlen(expected_str));
  ASSERT_EQ(strlen(expected_str), i);
  ASSERT_STREQ(expected_str, buf);
  while (i < sizeof(buf))
    ASSERT_EQ('\0', buf[i++]);
}

TEST(snprintf, testVeryLargePrecision) {
  char buf[512] = {};
  int i = snprintf(buf, sizeof(buf), "%.9999u", 10);

  ASSERT_EQ(9999, i);
  ASSERT_EQ(511, strlen(buf));
}

TEST(snprintf, testPlusFlagOnChar) {
  check_single_int("%+c", "=", '=');
}

TEST(snprintf, testInf) {
  check_single_double("%f", "inf", 1.0 / 0.0);
  check_single_long_double("%Lf", "inf", 1.0L / 0.0L);
  check_single_double("%e", "inf", 1.0 / 0.0);
  check_single_long_double("%Le", "inf", 1.0L / 0.0L);
  check_single_double("%g", "inf", 1.0 / 0.0);
  check_single_long_double("%Lg", "inf", 1.0L / 0.0L);
}

TEST(snprintf, testUppercaseCConversionSpecifier) {
  check_single_wint_t("%C", "a", L'a');
  check_single_wint_t("%C", "☺", L'☺');
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
  check_single_long_double("%Le", "1.234568e+06", 1234567.8L);
}

TEST(snprintf, testLongDoubleRounding) {
  int previous_rounding = fegetround();
  ASSERT_EQ(0, fesetround(FE_DOWNWARD));

  check_single_long_double("%.3Lf", "4.437", 4.4375L);
  check_single_long_double("%.3Lf", "-4.438", -4.4375L);

  ASSERT_EQ(0, fesetround(FE_TOWARDZERO));

  check_single_long_double("%.3Lf", "-4.437", -4.4375L);

  ASSERT_EQ(0, fesetround(previous_rounding));
}

void check_a_conversion_specifier_double_prec_1(const char *expected_str,
                                                double value) {
  check_single_double("%.1a", expected_str, value);
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

  check_single_double("%#a", "0x0.p+0", 0x0.0p0);
  check_single_double("%#A", "0X0.P+0", 0x0.0p0);
  check_single_long_double("%#La", "0x0.p+0", 0x0.0p0L);
  check_single_long_double("%#LA", "0X0.P+0", 0x0.0p0L);

  check_single_double("%.2a", "0x1.00p-1026", 0xf.fffp-1030);

  check_single_double("%.1a", "0x2.0p+0", 1.999);
  const char *acceptable_results1[] = {"0x1.0p+1", "0x2.0p+0", NULL};
  check_single_long_double_arr_allowed(
      "%.1La", acceptable_results1, 1.999L);
}

TEST(snprintf, testApostropheFlag) {
  check_single_int("%'d", "10000000", 10000000);
}

TEST(snprintf, testUppercaseBConversionSpecifier) {
  check_single_int("%B", "0", 0);
  check_single_int("%B", "10", 2);
  check_single_int("%#B", "0B10011", 19);
}
