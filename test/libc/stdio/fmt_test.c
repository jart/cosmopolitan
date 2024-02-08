/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2021 Justine Alexandra Roberts Tunney                              │
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
#include "libc/limits.h"
#include "libc/log/log.h"
#include "libc/math.h"
#include "libc/mem/gc.h"
#include "libc/str/str.h"
#include "libc/testlib/testlib.h"
#include "libc/x/xasprintf.h"

TEST(fmt, d) {
  EXPECT_STREQ("-123", gc(xasprintf("%d", -123)));
  EXPECT_STREQ("-1", gc(xasprintf("%d", -1)));
  EXPECT_STREQ("1", gc(xasprintf("%d", 1)));
  EXPECT_STREQ("16", gc(xasprintf("%d", 16)));
  EXPECT_STREQ("2147483647", gc(xasprintf("%d", INT_MAX)));
  EXPECT_STREQ("-2147483648", gc(xasprintf("%d", INT_MIN)));
  EXPECT_STREQ("      -123", gc(xasprintf("%10d", -123)));
  EXPECT_STREQ("        -1", gc(xasprintf("%10d", -1)));
  EXPECT_STREQ("         1", gc(xasprintf("%10d", 1)));
  EXPECT_STREQ("        16", gc(xasprintf("%10d", 16)));
  EXPECT_STREQ("2147483647", gc(xasprintf("%10d", INT_MAX)));
  EXPECT_STREQ("-2147483648", gc(xasprintf("%10d", INT_MIN)));
  EXPECT_STREQ("-000000123", gc(xasprintf("%010d", -123)));
  EXPECT_STREQ("-000000001", gc(xasprintf("%010d", -1)));
  EXPECT_STREQ("0000000001", gc(xasprintf("%010d", 1)));
  EXPECT_STREQ("0000000016", gc(xasprintf("%010d", 16)));
  EXPECT_STREQ("2147483647", gc(xasprintf("%010d", INT_MAX)));
  EXPECT_STREQ("-2147483648", gc(xasprintf("%010d", INT_MIN)));
  EXPECT_STREQ("-123      ", gc(xasprintf("%-10d", -123)));
  EXPECT_STREQ("-1        ", gc(xasprintf("%-10d", -1)));
  EXPECT_STREQ("1         ", gc(xasprintf("%-10d", 1)));
  EXPECT_STREQ("16        ", gc(xasprintf("%-10d", 16)));
  EXPECT_STREQ("2147483647", gc(xasprintf("%-10d", INT_MAX)));
  EXPECT_STREQ("-2147483648", gc(xasprintf("%-10d", INT_MIN)));
  EXPECT_STREQ("      -123", gc(xasprintf("%+10d", -123)));
  EXPECT_STREQ("        -1", gc(xasprintf("%+10d", -1)));
  EXPECT_STREQ("        +1", gc(xasprintf("%+10d", 1)));
  EXPECT_STREQ("       +16", gc(xasprintf("%+10d", 16)));
  EXPECT_STREQ("+2147483647", gc(xasprintf("%+10d", INT_MAX)));
  EXPECT_STREQ("-2147483648", gc(xasprintf("%+10d", INT_MIN)));
  EXPECT_STREQ("-123", gc(xasprintf("% d", -123)));
  EXPECT_STREQ("-1", gc(xasprintf("% d", -1)));
  EXPECT_STREQ(" 1", gc(xasprintf("% d", 1)));
  EXPECT_STREQ(" 16", gc(xasprintf("% d", 16)));
  EXPECT_STREQ(" 2147483647", gc(xasprintf("% d", INT_MAX)));
  EXPECT_STREQ("-2147483648", gc(xasprintf("% d", INT_MIN)));
  EXPECT_STREQ("042 ", gc(xasprintf("%-4.3d", 42)));
  EXPECT_STREQ("-00054", gc(xasprintf("%-1.5lld", -54ll)));
  EXPECT_STREQ("00109", gc(xasprintf("%-.5lld", 109ll)));
  EXPECT_STREQ("-00116", gc(xasprintf("%-.5lld", -116ll)));
  EXPECT_STREQ("00108   ", gc(xasprintf("%-8.5lld", 108ll)));
  EXPECT_STREQ("-00054  ", gc(xasprintf("%-8.5lld", -54ll)));
}

TEST(fmt, u) {
  EXPECT_STREQ("042 ", gc(xasprintf("%-4.3u", 42)));
}

TEST(fmt, c) {
  char buf[8];
  EXPECT_EQ(1, sprintf(buf, "%c", 'h'));
  EXPECT_STREQ("h", buf);
  EXPECT_EQ(3, sprintf(buf, "%`c", 'h'));
  EXPECT_STREQ("'h'", buf);
  EXPECT_EQ(4, sprintf(buf, "%`c", '\t'));
  EXPECT_STREQ("'\\t'", buf);
  EXPECT_EQ(4, sprintf(buf, "%`c", 0));
  EXPECT_STREQ("'\\0'", buf);
  EXPECT_EQ(3, sprintf(buf, "%#c", 1));
  EXPECT_STREQ("☺", buf);
  EXPECT_EQ(2, sprintf(buf, "%#c", 0));
  EXPECT_STREQ(" ", buf);
  EXPECT_EQ(4, sprintf(buf, "%#`c", 0));
  EXPECT_STREQ("'\\0'", buf);
}

TEST(fmt, x) {
  EXPECT_STREQ("0x01   ", gc(xasprintf("%#-07.2x", 1)));
  EXPECT_STREQ("0x00136d  ", gc(xasprintf("%#-010.6x", 4973)));
  EXPECT_STREQ("0X1", gc(xasprintf("%#X", 1)));
  EXPECT_STREQ("0XA", gc(xasprintf("%#X", 10)));
  EXPECT_STREQ("0XFFFF", gc(xasprintf("%#X", 65535)));
  EXPECT_STREQ("0XABCDEF", gc(xasprintf("%#X", 0xABCDEF)));
  EXPECT_STREQ("0X1", gc(xasprintf("%#hX", (short)1)));
  EXPECT_STREQ("0XA", gc(xasprintf("%#hX", (short)10)));
  EXPECT_STREQ("0XFFFF", gc(xasprintf("%#hX", (short)65535)));
  EXPECT_STREQ("0XABCD", gc(xasprintf("%#hX", (short)0xABCD)));
  EXPECT_STREQ("          0X308C6705", gc(xasprintf("%#20X", 814507781)));
  EXPECT_STREQ("0X0000000000308C6705", gc(xasprintf("%#020X", 814507781)));
  EXPECT_STREQ("              0X6705",
               gc(xasprintf("%#20hX", (short)814507781)));
  EXPECT_STREQ("0X000000000000006705",
               gc(xasprintf("%#020hX", (short)814507781)));
  EXPECT_STREQ("            0XABCDEF", gc(xasprintf("%#20X", 0xABCDEF)));
  EXPECT_STREQ("0X000000000000ABCDEF", gc(xasprintf("%#020X", 0xABCDEF)));
  EXPECT_STREQ("              0XCDEF",
               gc(xasprintf("%#20hX", (short)0xABCDEF)));
  EXPECT_STREQ("0X00000000000000CDEF",
               gc(xasprintf("%#020hX", (short)0xABCDEF)));
}

TEST(fmt, o) {
  EXPECT_STREQ("0000000000000037777777634", gc(xasprintf("%#.25o", -100)));
  EXPECT_STREQ("0001777777777777777777634", gc(xasprintf("%#.25lo", -100L)));
  EXPECT_STREQ("0001777777777777777777634", gc(xasprintf("%#.25llo", -100LL)));
  EXPECT_STREQ("0", gc(xasprintf("%#.o", 0)));
  EXPECT_STREQ("  056520", gc(xasprintf("%#8.6o", 23888)));
  EXPECT_STREQ("00144   ", gc(xasprintf("%#-8.5llo", 100ll)));
  EXPECT_STREQ("0 ", gc(xasprintf("%-#02lo", 0l)));
  EXPECT_STREQ("0", gc(xasprintf("%#01lo", 0l)));
  EXPECT_STREQ("00", gc(xasprintf("%#02lo", 0l)));
}

TEST(fmt, b) {
  EXPECT_STREQ("000010100     ", gc(xasprintf("%-14.9b", 20)));
  EXPECT_STREQ("0b110", gc(xasprintf("%#b", 6)));
}

TEST(fmt, s) {
  EXPECT_STREQ("123456", gc(xasprintf("%4.*s", -5, "123456")));
  EXPECT_STREQ("123456789", gc(xasprintf("%4.*s", -5, "123456789")));
  EXPECT_STREQ("12345678901234567890",
               gc(xasprintf("%4.*s", -5, "12345678901234567890")));
  EXPECT_STREQ("123456789012345678901234567890",
               gc(xasprintf("%4.*s", -5, "123456789012345678901234567890")));
  EXPECT_STREQ(
      "1234567890123456789012345678901234567890",
      gc(xasprintf("%4.*s", -5, "1234567890123456789012345678901234567890")));
  EXPECT_STREQ(
      "12345678901234567890123456789012345678901234567890",
      gc(xasprintf("%4.*s", -5,
                   "12345678901234567890123456789012345678901234567890")));
  EXPECT_STREQ(
      "123456789012345678901234567890123456789012345678901234567890",
      gc(xasprintf(
          "%4.*s", -5,
          "123456789012345678901234567890123456789012345678901234567890")));
  EXPECT_STREQ("Wide character output test",
               gc(xasprintf("%S", L"Wide character output test")));
}

TEST(fmt, g) {
  EXPECT_STREQ("1", gc(xasprintf("%g", 1.)));
  EXPECT_STREQ(" 1", gc(xasprintf("% g", 1.)));
  EXPECT_STREQ("+1", gc(xasprintf("%+g", 1.)));
  EXPECT_STREQ("-1", gc(xasprintf("%g", -1.)));
  EXPECT_STREQ("10", gc(xasprintf("%g", 10.)));
  EXPECT_STREQ("-10", gc(xasprintf("%g", -10.)));
  EXPECT_STREQ("1e+01", gc(xasprintf("%.0g", 10.)));
  EXPECT_STREQ("-1e+01", gc(xasprintf("%.0g", -10.)));
  EXPECT_STREQ("1", gc(xasprintf("%.0g", 1.0)));
  EXPECT_STREQ("1e-05", gc(xasprintf("%.0g", 0.00001)));
  EXPECT_STREQ("0.0001", gc(xasprintf("%.0g", 0.0001)));
  EXPECT_STREQ("1e+01", gc(xasprintf("%.1g", 10.)));
  EXPECT_STREQ("-1e+01", gc(xasprintf("%.1g", -10.)));
  EXPECT_STREQ("1", gc(xasprintf("%.1g", 1.0)));
  EXPECT_STREQ("1e-05", gc(xasprintf("%.1g", 0.00001)));
  EXPECT_STREQ("0.0001", gc(xasprintf("%.1g", 0.0001)));
  EXPECT_STREQ("0.007812", gc(xasprintf("%.4g", 0.0078125)));
  EXPECT_STREQ("0.023438", gc(xasprintf("%.5g", 0.0234375)));
  EXPECT_STREQ("1e+100", gc(xasprintf("%g", 1e100)));
  EXPECT_STREQ("1e-100", gc(xasprintf("%g", 1e-100)));
  EXPECT_STREQ("-1e-100", gc(xasprintf("%g", -1e-100)));
  EXPECT_STREQ("0.123456", gc(xasprintf("%g", 0.1234564)));
  EXPECT_STREQ("0.123457", gc(xasprintf("%g", 0.1234566)));
  EXPECT_STREQ("3.14159", gc(xasprintf("%g", 0x1.921fb54442d1846ap+1)));
  EXPECT_STREQ("0", gc(xasprintf("%g", 0.)));
  EXPECT_STREQ("-0", gc(xasprintf("%g", -0.)));
  EXPECT_STREQ("nan", gc(xasprintf("%g", NAN)));
  EXPECT_STREQ("-nan", gc(xasprintf("%g", -NAN)));
  EXPECT_STREQ("inf", gc(xasprintf("%g", INFINITY)));
  EXPECT_STREQ("-inf", gc(xasprintf("%g", -INFINITY)));
  EXPECT_STREQ("2.22507e-308", gc(xasprintf("%g", __DBL_MIN__)));
  EXPECT_STREQ("1.79769e+308", gc(xasprintf("%g", __DBL_MAX__)));
  EXPECT_STREQ("0", gc(xasprintf("%G", 0.)));
  EXPECT_STREQ("-0", gc(xasprintf("%G", -0.)));
  EXPECT_STREQ("NAN", gc(xasprintf("%G", NAN)));
  EXPECT_STREQ("-NAN", gc(xasprintf("%G", -NAN)));
  EXPECT_STREQ("INF", gc(xasprintf("%G", INFINITY)));
  EXPECT_STREQ("-INF", gc(xasprintf("%G", -INFINITY)));
  EXPECT_STREQ("2.22507E-308", gc(xasprintf("%G", __DBL_MIN__)));
  EXPECT_STREQ("1.79769E+308", gc(xasprintf("%G", __DBL_MAX__)));
  EXPECT_STREQ("            0", gc(xasprintf("%13g", 0.)));
  EXPECT_STREQ("           -0", gc(xasprintf("%13g", -0.)));
  EXPECT_STREQ("          nan", gc(xasprintf("%13g", NAN)));
  EXPECT_STREQ("         -nan", gc(xasprintf("%13g", -NAN)));
  EXPECT_STREQ("          inf", gc(xasprintf("%13g", INFINITY)));
  EXPECT_STREQ("         -inf", gc(xasprintf("%13g", -INFINITY)));
  EXPECT_STREQ(" 2.22507e-308", gc(xasprintf("%13g", __DBL_MIN__)));
  EXPECT_STREQ(" 1.79769e+308", gc(xasprintf("%13g", __DBL_MAX__)));
  EXPECT_STREQ("            0", gc(xasprintf("%13G", 0.)));
  EXPECT_STREQ("           -0", gc(xasprintf("%13G", -0.)));
  EXPECT_STREQ("          NAN", gc(xasprintf("%13G", NAN)));
  EXPECT_STREQ("         -NAN", gc(xasprintf("%13G", -NAN)));
  EXPECT_STREQ("          INF", gc(xasprintf("%13G", INFINITY)));
  EXPECT_STREQ("         -INF", gc(xasprintf("%13G", -INFINITY)));
  EXPECT_STREQ(" 2.22507E-308", gc(xasprintf("%13G", __DBL_MIN__)));
  EXPECT_STREQ(" 1.79769E+308", gc(xasprintf("%13G", __DBL_MAX__)));
}

TEST(fmt, f) {
  EXPECT_STREQ("3.141593", gc(xasprintf("%f", 0x1.921fb54442d1846ap+1)));
  EXPECT_STREQ("3.1415926535897931",
               gc(xasprintf("%.16f", 0x1.921fb54442d1846ap+1)));
  EXPECT_STREQ("100000000000000001590289110975991804683608085639452813"
               "89781327557747838772170381060813469985856815104.000000",
               gc(xasprintf("%f", 1e100)));
  EXPECT_STREQ("0.000000", gc(xasprintf("%f", 0.)));
  EXPECT_STREQ("-0.000000", gc(xasprintf("%f", -0.)));
  EXPECT_STREQ("nan", gc(xasprintf("%f", NAN)));
  EXPECT_STREQ("-nan", gc(xasprintf("%f", -NAN)));
  EXPECT_STREQ("inf", gc(xasprintf("%f", INFINITY)));
  EXPECT_STREQ("-inf", gc(xasprintf("%f", -INFINITY)));
  EXPECT_STREQ("0.000000", gc(xasprintf("%f", __DBL_MIN__)));
  EXPECT_STREQ(
      "179769313486231570814527423731704356798070567525844996598917476803157260"
      "780028538760589558632766878171540458953514382464234321326889464182768467"
      "546703537516986049910576551282076245490090389328944075868508455133942304"
      "583236903222948165808559332123348274797826204144723168738177180919299881"
      "250404026184124858368.000000",
      gc(xasprintf("%f", __DBL_MAX__)));
  EXPECT_STREQ("0.000000", gc(xasprintf("%F", 0.)));
  EXPECT_STREQ("-0.000000", gc(xasprintf("%F", -0.)));
  EXPECT_STREQ("NAN", gc(xasprintf("%F", NAN)));
  EXPECT_STREQ("-NAN", gc(xasprintf("%F", -NAN)));
  EXPECT_STREQ("INF", gc(xasprintf("%F", INFINITY)));
  EXPECT_STREQ("-INF", gc(xasprintf("%F", -INFINITY)));
  EXPECT_STREQ("0.000000", gc(xasprintf("%F", __DBL_MIN__)));
  EXPECT_STREQ(
      "179769313486231570814527423731704356798070567525844996598917476803157260"
      "780028538760589558632766878171540458953514382464234321326889464182768467"
      "546703537516986049910576551282076245490090389328944075868508455133942304"
      "583236903222948165808559332123348274797826204144723168738177180919299881"
      "250404026184124858368.000000",
      gc(xasprintf("%F", __DBL_MAX__)));
  EXPECT_STREQ("  0.000000", gc(xasprintf("%10f", 0.)));
  EXPECT_STREQ(" -0.000000", gc(xasprintf("%10f", -0.)));
  EXPECT_STREQ("       nan", gc(xasprintf("%10f", NAN)));
  EXPECT_STREQ("      -nan", gc(xasprintf("%10f", -NAN)));
  EXPECT_STREQ("       inf", gc(xasprintf("%10f", INFINITY)));
  EXPECT_STREQ("      -inf", gc(xasprintf("%10f", -INFINITY)));
  EXPECT_STREQ("  0.000000", gc(xasprintf("%10f", __DBL_MIN__)));
  EXPECT_STREQ(
      "179769313486231570814527423731704356798070567525844996598917476803157260"
      "780028538760589558632766878171540458953514382464234321326889464182768467"
      "546703537516986049910576551282076245490090389328944075868508455133942304"
      "583236903222948165808559332123348274797826204144723168738177180919299881"
      "250404026184124858368.000000",
      gc(xasprintf("%10f", __DBL_MAX__)));
  EXPECT_STREQ("  0.000000", gc(xasprintf("%10F", 0.)));
  EXPECT_STREQ(" -0.000000", gc(xasprintf("%10F", -0.)));
  EXPECT_STREQ("       NAN", gc(xasprintf("%10F", NAN)));
  EXPECT_STREQ("      -NAN", gc(xasprintf("%10F", -NAN)));
  EXPECT_STREQ("       INF", gc(xasprintf("%10F", INFINITY)));
  EXPECT_STREQ("      -INF", gc(xasprintf("%10F", -INFINITY)));
  EXPECT_STREQ("  0.000000", gc(xasprintf("%10F", __DBL_MIN__)));
  EXPECT_STREQ(
      "179769313486231570814527423731704356798070567525844996598917476803157260"
      "780028538760589558632766878171540458953514382464234321326889464182768467"
      "546703537516986049910576551282076245490090389328944075868508455133942304"
      "583236903222948165808559332123348274797826204144723168738177180919299881"
      "250404026184124858368.000000",
      gc(xasprintf("%10F", __DBL_MAX__)));
}

TEST(fmt, e) {
  EXPECT_STREQ("3.14159", gc(xasprintf("%g", 0x1.921fb54442d1846ap+1)));
  EXPECT_STREQ("3.141592653589793",
               gc(xasprintf("%.16g", 0x1.921fb54442d1846ap+1)));
  EXPECT_STREQ("1.000000e+100", gc(xasprintf("%e", 1e100)));
  EXPECT_STREQ("1.000000E+100", gc(xasprintf("%E", 1e100)));
  EXPECT_STREQ("            0.000000e+00", gc(xasprintf("%24e", 0.)));
  EXPECT_STREQ("           -0.000000e+00", gc(xasprintf("%24e", -0.)));
  EXPECT_STREQ("                     nan", gc(xasprintf("%24e", NAN)));
  EXPECT_STREQ("                    -nan", gc(xasprintf("%24e", -NAN)));
  EXPECT_STREQ("                     inf", gc(xasprintf("%24e", INFINITY)));
  EXPECT_STREQ("                    -inf", gc(xasprintf("%24e", -INFINITY)));
  EXPECT_STREQ("           2.225074e-308", gc(xasprintf("%24e", __DBL_MIN__)));
  EXPECT_STREQ("           1.797693e+308", gc(xasprintf("%24e", __DBL_MAX__)));
  EXPECT_STREQ("            0.000000E+00", gc(xasprintf("%24E", 0.)));
  EXPECT_STREQ("           -0.000000E+00", gc(xasprintf("%24E", -0.)));
  EXPECT_STREQ("                     NAN", gc(xasprintf("%24E", NAN)));
  EXPECT_STREQ("                    -NAN", gc(xasprintf("%24E", -NAN)));
  EXPECT_STREQ("                     INF", gc(xasprintf("%24E", INFINITY)));
  EXPECT_STREQ("                    -INF", gc(xasprintf("%24E", -INFINITY)));
  EXPECT_STREQ("           2.225074E-308", gc(xasprintf("%24E", __DBL_MIN__)));
  EXPECT_STREQ("           1.797693E+308", gc(xasprintf("%24E", __DBL_MAX__)));
  EXPECT_STREQ("           +0.000000e+00", gc(xasprintf("%+24e", 0.)));
  EXPECT_STREQ("           -0.000000e+00", gc(xasprintf("%+24e", -0.)));
  EXPECT_STREQ("                    +nan", gc(xasprintf("%+24e", NAN)));
  EXPECT_STREQ("                    -nan", gc(xasprintf("%+24e", -NAN)));
  EXPECT_STREQ("                    +inf", gc(xasprintf("%+24e", INFINITY)));
  EXPECT_STREQ("                    -inf", gc(xasprintf("%+24e", -INFINITY)));
  EXPECT_STREQ("          +2.225074e-308", gc(xasprintf("%+24e", __DBL_MIN__)));
  EXPECT_STREQ("          +1.797693e+308", gc(xasprintf("%+24e", __DBL_MAX__)));
  EXPECT_STREQ("           +0.000000E+00", gc(xasprintf("%+24E", 0.)));
  EXPECT_STREQ("           -0.000000E+00", gc(xasprintf("%+24E", -0.)));
  EXPECT_STREQ("                    +NAN", gc(xasprintf("%+24E", NAN)));
  EXPECT_STREQ("                    -NAN", gc(xasprintf("%+24E", -NAN)));
  EXPECT_STREQ("                    +INF", gc(xasprintf("%+24E", INFINITY)));
  EXPECT_STREQ("                    -INF", gc(xasprintf("%+24E", -INFINITY)));
  EXPECT_STREQ("          +2.225074E-308", gc(xasprintf("%+24E", __DBL_MIN__)));
  EXPECT_STREQ("          +1.797693E+308", gc(xasprintf("%+24E", __DBL_MAX__)));
}

TEST(fmt, a) {
  EXPECT_STREQ("0x0p+0", gc(xasprintf("%a", 0.)));
  EXPECT_STREQ("0x0p+0", gc(xasprintf("%.a", 0.)));
  EXPECT_STREQ(" 0x0p+0", gc(xasprintf("%7.a", 0.)));
  EXPECT_STREQ("0x0.000p+0", gc(xasprintf("%.3a", 0.)));
  // EXPECT_STREQ(" 0x0.000p+0\n", gc(xasprintf("%11.3a\n", 0.))); // TODO
  EXPECT_STREQ("inf\n", gc(xasprintf("%g\n", INFINITY)));
  EXPECT_STREQ("  inf\n", gc(xasprintf("%5g\n", INFINITY)));
  EXPECT_STREQ(" +inf\n", gc(xasprintf("%+5g\n", INFINITY)));
  EXPECT_STREQ(" inf\n", gc(xasprintf("% g\n", INFINITY)));
  EXPECT_STREQ("-inf\n", gc(xasprintf("% g\n", -INFINITY)));
  EXPECT_STREQ("0x1.921fb54442d18p+1",
               gc(xasprintf("%a", 0x1.921fb54442d1846ap+1)));
  EXPECT_STREQ("0X1.921FB54442D18P+1",
               gc(xasprintf("%A", 0x1.921fb54442d1846ap+1)));
  EXPECT_STREQ("                  0x0p+0", gc(xasprintf("%24a", 0.)));
  EXPECT_STREQ("                 -0x0p+0", gc(xasprintf("%24a", -0.)));
  EXPECT_STREQ("                     nan", gc(xasprintf("%24a", NAN)));
  EXPECT_STREQ("                    -nan", gc(xasprintf("%24a", -NAN)));
  EXPECT_STREQ("                     inf", gc(xasprintf("%24a", INFINITY)));
  EXPECT_STREQ("                    -inf", gc(xasprintf("%24a", -INFINITY)));
  EXPECT_STREQ("               0x1p-1022", gc(xasprintf("%24a", __DBL_MIN__)));
  EXPECT_STREQ(" 0x1.fffffffffffffp+1023", gc(xasprintf("%24a", __DBL_MAX__)));
  EXPECT_STREQ("                  0X0P+0", gc(xasprintf("%24A", 0.)));
  EXPECT_STREQ("                 -0X0P+0", gc(xasprintf("%24A", -0.)));
  EXPECT_STREQ("                     NAN", gc(xasprintf("%24A", NAN)));
  EXPECT_STREQ("                    -NAN", gc(xasprintf("%24A", -NAN)));
  EXPECT_STREQ("                     INF", gc(xasprintf("%24A", INFINITY)));
  EXPECT_STREQ("                    -INF", gc(xasprintf("%24A", -INFINITY)));
  EXPECT_STREQ("               0X1P-1022", gc(xasprintf("%24A", __DBL_MIN__)));
  EXPECT_STREQ(" 0X1.FFFFFFFFFFFFFP+1023", gc(xasprintf("%24A", __DBL_MAX__)));
  EXPECT_STREQ("   0X1.E9A488E8A71DEP+14", gc(xasprintf("%24A", 31337.1337)));
  EXPECT_STREQ("  -0X1.E9A488E8A71DEP+14", gc(xasprintf("%24A", -31337.1337)));
}

TEST(fmt, p) {
  EXPECT_STREQ("0x1", gc(xasprintf("%p", 1)));
  EXPECT_STREQ("0x10", gc(xasprintf("%p", 16)));
  EXPECT_STREQ("0x31337", gc(xasprintf("%p", 0x31337)));
  EXPECT_STREQ("0xffffffff", gc(xasprintf("%p", 0xffffffff)));
  EXPECT_STREQ("0xffff800000031337", gc(xasprintf("%p", 0xffff800000031337)));
  EXPECT_STREQ("       0x1", gc(xasprintf("%10p", 1)));
  EXPECT_STREQ("      0x10", gc(xasprintf("%10p", 16)));
  EXPECT_STREQ("   0x31337", gc(xasprintf("%10p", 0x31337)));
  EXPECT_STREQ("0xffffffff", gc(xasprintf("%10p", 0xffffffff)));
  EXPECT_STREQ("0xffff800000031337", gc(xasprintf("%10p", 0xffff800000031337)));
  EXPECT_STREQ("0x00000001", gc(xasprintf("%010p", 1)));
  EXPECT_STREQ("0x00000010", gc(xasprintf("%010p", 16)));
  EXPECT_STREQ("0x00031337", gc(xasprintf("%010p", 0x31337)));
  EXPECT_STREQ("0xffffffff", gc(xasprintf("%010p", 0xffffffff)));
  EXPECT_STREQ("0xffff800000031337",
               gc(xasprintf("%010p", 0xffff800000031337)));
  EXPECT_STREQ("0x1       ", gc(xasprintf("%-10p", 1)));
  EXPECT_STREQ("0x10      ", gc(xasprintf("%-10p", 16)));
  EXPECT_STREQ("0x31337   ", gc(xasprintf("%-10p", 0x31337)));
  EXPECT_STREQ("0xffffffff", gc(xasprintf("%-10p", 0xffffffff)));
  EXPECT_STREQ("0xffff800000031337",
               gc(xasprintf("%-10p", 0xffff800000031337)));
  EXPECT_STREQ("       0x1", gc(xasprintf("%+10p", 1)));
  EXPECT_STREQ("      0x10", gc(xasprintf("%+10p", 16)));
  EXPECT_STREQ("   0x31337", gc(xasprintf("%+10p", 0x31337)));
  EXPECT_STREQ("0xffffffff", gc(xasprintf("%+10p", 0xffffffff)));
  EXPECT_STREQ("0xffff800000031337",
               gc(xasprintf("%+10p", 0xffff800000031337)));
  EXPECT_STREQ("       0x1", gc(xasprintf("% 10p", 1)));
  EXPECT_STREQ("      0x10", gc(xasprintf("% 10p", 16)));
  EXPECT_STREQ("   0x31337", gc(xasprintf("% 10p", 0x31337)));
  EXPECT_STREQ("0xffffffff", gc(xasprintf("% 10p", 0xffffffff)));
  EXPECT_STREQ("0xffff800000031337",
               gc(xasprintf("% 10p", 0xffff800000031337)));
}

TEST(fmt, quoted) {
  ASSERT_STREQ("   \"hello\"", gc(xasprintf("%`*.*s", 10, 5, "hello")));
  ASSERT_STREQ("\"hello\"   ", gc(xasprintf("%-`*.*s", 10, 5, "hello")));
}

TEST(fmt, nulCharacter) {
  char b[3] = {1, 1, 1};
  ASSERT_EQ(1, snprintf(0, 0, "%c", 0));
  ASSERT_EQ(1, snprintf(b, 3, "%c", 0));
  ASSERT_EQ(0, b[0]);
  ASSERT_EQ(0, b[1]);
  ASSERT_EQ(1, b[2]);
}

TEST(fmt, regress) {
  char buf[512];
  const char *meth = "GET";
  const char *path = "/";
  const char *host = "10.10.10.124";
  const char *port = "8080";
  const char *agent = "hurl/1.o (https://github.com/jart/cosmopolitan)";
  ASSERT_EQ(
      strlen("GET / HTTP/1.1\r\n"
             "Host: 10.10.10.124:8080\r\n"
             "Connection: close\r\n"
             "User-Agent: hurl/1.o (https://github.com/jart/cosmopolitan)\r\n"),
      sprintf(buf,
              "%s %s HTTP/1.1\r\n"
              "Host: %s:%s\r\n"
              "Connection: close\r\n"
              "User-Agent: %s\r\n",
              meth, path, host, port, agent));
  ASSERT_STREQ(
      "GET / HTTP/1.1\r\n"
      "Host: 10.10.10.124:8080\r\n"
      "Connection: close\r\n"
      "User-Agent: hurl/1.o (https://github.com/jart/cosmopolitan)\r\n",
      buf);
}
