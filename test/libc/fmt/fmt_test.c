/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
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
#include "libc/fmt/fmt.h"
#include "libc/limits.h"
#include "libc/log/log.h"
#include "libc/math.h"
#include "libc/mem/gc.h"
#include "libc/str/str.h"
#include "libc/testlib/testlib.h"
#include "libc/x/xasprintf.h"

TEST(fmt, d) {
  EXPECT_STREQ("-123", _gc(xasprintf("%d", -123)));
  EXPECT_STREQ("-1", _gc(xasprintf("%d", -1)));
  EXPECT_STREQ("1", _gc(xasprintf("%d", 1)));
  EXPECT_STREQ("16", _gc(xasprintf("%d", 16)));
  EXPECT_STREQ("2147483647", _gc(xasprintf("%d", INT_MAX)));
  EXPECT_STREQ("-2147483648", _gc(xasprintf("%d", INT_MIN)));
  EXPECT_STREQ("      -123", _gc(xasprintf("%10d", -123)));
  EXPECT_STREQ("        -1", _gc(xasprintf("%10d", -1)));
  EXPECT_STREQ("         1", _gc(xasprintf("%10d", 1)));
  EXPECT_STREQ("        16", _gc(xasprintf("%10d", 16)));
  EXPECT_STREQ("2147483647", _gc(xasprintf("%10d", INT_MAX)));
  EXPECT_STREQ("-2147483648", _gc(xasprintf("%10d", INT_MIN)));
  EXPECT_STREQ("-000000123", _gc(xasprintf("%010d", -123)));
  EXPECT_STREQ("-000000001", _gc(xasprintf("%010d", -1)));
  EXPECT_STREQ("0000000001", _gc(xasprintf("%010d", 1)));
  EXPECT_STREQ("0000000016", _gc(xasprintf("%010d", 16)));
  EXPECT_STREQ("2147483647", _gc(xasprintf("%010d", INT_MAX)));
  EXPECT_STREQ("-2147483648", _gc(xasprintf("%010d", INT_MIN)));
  EXPECT_STREQ("-123      ", _gc(xasprintf("%-10d", -123)));
  EXPECT_STREQ("-1        ", _gc(xasprintf("%-10d", -1)));
  EXPECT_STREQ("1         ", _gc(xasprintf("%-10d", 1)));
  EXPECT_STREQ("16        ", _gc(xasprintf("%-10d", 16)));
  EXPECT_STREQ("2147483647", _gc(xasprintf("%-10d", INT_MAX)));
  EXPECT_STREQ("-2147483648", _gc(xasprintf("%-10d", INT_MIN)));
  EXPECT_STREQ("      -123", _gc(xasprintf("%+10d", -123)));
  EXPECT_STREQ("        -1", _gc(xasprintf("%+10d", -1)));
  EXPECT_STREQ("        +1", _gc(xasprintf("%+10d", 1)));
  EXPECT_STREQ("       +16", _gc(xasprintf("%+10d", 16)));
  EXPECT_STREQ("+2147483647", _gc(xasprintf("%+10d", INT_MAX)));
  EXPECT_STREQ("-2147483648", _gc(xasprintf("%+10d", INT_MIN)));
  EXPECT_STREQ("-123", _gc(xasprintf("% d", -123)));
  EXPECT_STREQ("-1", _gc(xasprintf("% d", -1)));
  EXPECT_STREQ(" 1", _gc(xasprintf("% d", 1)));
  EXPECT_STREQ(" 16", _gc(xasprintf("% d", 16)));
  EXPECT_STREQ(" 2147483647", _gc(xasprintf("% d", INT_MAX)));
  EXPECT_STREQ("-2147483648", _gc(xasprintf("% d", INT_MIN)));
}

TEST(fmt, g) {
  EXPECT_STREQ("1", _gc(xasprintf("%g", 1.)));
  EXPECT_STREQ(" 1", _gc(xasprintf("% g", 1.)));
  EXPECT_STREQ("+1", _gc(xasprintf("%+g", 1.)));
  EXPECT_STREQ("-1", _gc(xasprintf("%g", -1.)));
  EXPECT_STREQ("10", _gc(xasprintf("%g", 10.)));
  EXPECT_STREQ("-10", _gc(xasprintf("%g", -10.)));
  EXPECT_STREQ("1e+01", _gc(xasprintf("%.0g", 10.)));
  EXPECT_STREQ("-1e+01", _gc(xasprintf("%.0g", -10.)));
  EXPECT_STREQ("1", _gc(xasprintf("%.0g", 1.0)));
  EXPECT_STREQ("1e-05", _gc(xasprintf("%.0g", 0.00001)));
  EXPECT_STREQ("0.0001", _gc(xasprintf("%.0g", 0.0001)));
  EXPECT_STREQ("1e+01", _gc(xasprintf("%.1g", 10.)));
  EXPECT_STREQ("-1e+01", _gc(xasprintf("%.1g", -10.)));
  EXPECT_STREQ("1", _gc(xasprintf("%.1g", 1.0)));
  EXPECT_STREQ("1e-05", _gc(xasprintf("%.1g", 0.00001)));
  EXPECT_STREQ("0.0001", _gc(xasprintf("%.1g", 0.0001)));
  EXPECT_STREQ("0.007812", _gc(xasprintf("%.4g", 0.0078125)));
  EXPECT_STREQ("0.023438", _gc(xasprintf("%.5g", 0.0234375)));
  EXPECT_STREQ("1e+100", _gc(xasprintf("%g", 1e100)));
  EXPECT_STREQ("1e-100", _gc(xasprintf("%g", 1e-100)));
  EXPECT_STREQ("-1e-100", _gc(xasprintf("%g", -1e-100)));
  EXPECT_STREQ("0.123456", _gc(xasprintf("%g", 0.1234564)));
  EXPECT_STREQ("0.123457", _gc(xasprintf("%g", 0.1234566)));
  EXPECT_STREQ("3.14159", _gc(xasprintf("%g", 0x1.921fb54442d1846ap+1)));
  EXPECT_STREQ("0", _gc(xasprintf("%g", 0.)));
  EXPECT_STREQ("-0", _gc(xasprintf("%g", -0.)));
  EXPECT_STREQ("nan", _gc(xasprintf("%g", NAN)));
  EXPECT_STREQ("-nan", _gc(xasprintf("%g", -NAN)));
  EXPECT_STREQ("inf", _gc(xasprintf("%g", INFINITY)));
  EXPECT_STREQ("-inf", _gc(xasprintf("%g", -INFINITY)));
  EXPECT_STREQ("2.22507e-308", _gc(xasprintf("%g", __DBL_MIN__)));
  EXPECT_STREQ("1.79769e+308", _gc(xasprintf("%g", __DBL_MAX__)));
  EXPECT_STREQ("0", _gc(xasprintf("%G", 0.)));
  EXPECT_STREQ("-0", _gc(xasprintf("%G", -0.)));
  EXPECT_STREQ("NAN", _gc(xasprintf("%G", NAN)));
  EXPECT_STREQ("-NAN", _gc(xasprintf("%G", -NAN)));
  EXPECT_STREQ("INF", _gc(xasprintf("%G", INFINITY)));
  EXPECT_STREQ("-INF", _gc(xasprintf("%G", -INFINITY)));
  EXPECT_STREQ("2.22507E-308", _gc(xasprintf("%G", __DBL_MIN__)));
  EXPECT_STREQ("1.79769E+308", _gc(xasprintf("%G", __DBL_MAX__)));
  EXPECT_STREQ("            0", _gc(xasprintf("%13g", 0.)));
  EXPECT_STREQ("           -0", _gc(xasprintf("%13g", -0.)));
  EXPECT_STREQ("          nan", _gc(xasprintf("%13g", NAN)));
  EXPECT_STREQ("         -nan", _gc(xasprintf("%13g", -NAN)));
  EXPECT_STREQ("          inf", _gc(xasprintf("%13g", INFINITY)));
  EXPECT_STREQ("         -inf", _gc(xasprintf("%13g", -INFINITY)));
  EXPECT_STREQ(" 2.22507e-308", _gc(xasprintf("%13g", __DBL_MIN__)));
  EXPECT_STREQ(" 1.79769e+308", _gc(xasprintf("%13g", __DBL_MAX__)));
  EXPECT_STREQ("            0", _gc(xasprintf("%13G", 0.)));
  EXPECT_STREQ("           -0", _gc(xasprintf("%13G", -0.)));
  EXPECT_STREQ("          NAN", _gc(xasprintf("%13G", NAN)));
  EXPECT_STREQ("         -NAN", _gc(xasprintf("%13G", -NAN)));
  EXPECT_STREQ("          INF", _gc(xasprintf("%13G", INFINITY)));
  EXPECT_STREQ("         -INF", _gc(xasprintf("%13G", -INFINITY)));
  EXPECT_STREQ(" 2.22507E-308", _gc(xasprintf("%13G", __DBL_MIN__)));
  EXPECT_STREQ(" 1.79769E+308", _gc(xasprintf("%13G", __DBL_MAX__)));
}

TEST(fmt, f) {
  EXPECT_STREQ("3.141593", _gc(xasprintf("%f", 0x1.921fb54442d1846ap+1)));
  EXPECT_STREQ("3.1415926535897931",
               _gc(xasprintf("%.16f", 0x1.921fb54442d1846ap+1)));
  EXPECT_STREQ("100000000000000001590289110975991804683608085639452813"
               "89781327557747838772170381060813469985856815104.000000",
               _gc(xasprintf("%f", 1e100)));
  EXPECT_STREQ("0.000000", _gc(xasprintf("%f", 0.)));
  EXPECT_STREQ("-0.000000", _gc(xasprintf("%f", -0.)));
  EXPECT_STREQ("nan", _gc(xasprintf("%f", NAN)));
  EXPECT_STREQ("-nan", _gc(xasprintf("%f", -NAN)));
  EXPECT_STREQ("inf", _gc(xasprintf("%f", INFINITY)));
  EXPECT_STREQ("-inf", _gc(xasprintf("%f", -INFINITY)));
  EXPECT_STREQ("0.000000", _gc(xasprintf("%f", __DBL_MIN__)));
  EXPECT_STREQ(
      "179769313486231570814527423731704356798070567525844996598917476803157260"
      "780028538760589558632766878171540458953514382464234321326889464182768467"
      "546703537516986049910576551282076245490090389328944075868508455133942304"
      "583236903222948165808559332123348274797826204144723168738177180919299881"
      "250404026184124858368.000000",
      _gc(xasprintf("%f", __DBL_MAX__)));
  EXPECT_STREQ("0.000000", _gc(xasprintf("%F", 0.)));
  EXPECT_STREQ("-0.000000", _gc(xasprintf("%F", -0.)));
  EXPECT_STREQ("NAN", _gc(xasprintf("%F", NAN)));
  EXPECT_STREQ("-NAN", _gc(xasprintf("%F", -NAN)));
  EXPECT_STREQ("INF", _gc(xasprintf("%F", INFINITY)));
  EXPECT_STREQ("-INF", _gc(xasprintf("%F", -INFINITY)));
  EXPECT_STREQ("0.000000", _gc(xasprintf("%F", __DBL_MIN__)));
  EXPECT_STREQ(
      "179769313486231570814527423731704356798070567525844996598917476803157260"
      "780028538760589558632766878171540458953514382464234321326889464182768467"
      "546703537516986049910576551282076245490090389328944075868508455133942304"
      "583236903222948165808559332123348274797826204144723168738177180919299881"
      "250404026184124858368.000000",
      _gc(xasprintf("%F", __DBL_MAX__)));
  EXPECT_STREQ("  0.000000", _gc(xasprintf("%10f", 0.)));
  EXPECT_STREQ(" -0.000000", _gc(xasprintf("%10f", -0.)));
  EXPECT_STREQ("       nan", _gc(xasprintf("%10f", NAN)));
  EXPECT_STREQ("      -nan", _gc(xasprintf("%10f", -NAN)));
  EXPECT_STREQ("       inf", _gc(xasprintf("%10f", INFINITY)));
  EXPECT_STREQ("      -inf", _gc(xasprintf("%10f", -INFINITY)));
  EXPECT_STREQ("  0.000000", _gc(xasprintf("%10f", __DBL_MIN__)));
  EXPECT_STREQ(
      "179769313486231570814527423731704356798070567525844996598917476803157260"
      "780028538760589558632766878171540458953514382464234321326889464182768467"
      "546703537516986049910576551282076245490090389328944075868508455133942304"
      "583236903222948165808559332123348274797826204144723168738177180919299881"
      "250404026184124858368.000000",
      _gc(xasprintf("%10f", __DBL_MAX__)));
  EXPECT_STREQ("  0.000000", _gc(xasprintf("%10F", 0.)));
  EXPECT_STREQ(" -0.000000", _gc(xasprintf("%10F", -0.)));
  EXPECT_STREQ("       NAN", _gc(xasprintf("%10F", NAN)));
  EXPECT_STREQ("      -NAN", _gc(xasprintf("%10F", -NAN)));
  EXPECT_STREQ("       INF", _gc(xasprintf("%10F", INFINITY)));
  EXPECT_STREQ("      -INF", _gc(xasprintf("%10F", -INFINITY)));
  EXPECT_STREQ("  0.000000", _gc(xasprintf("%10F", __DBL_MIN__)));
  EXPECT_STREQ(
      "179769313486231570814527423731704356798070567525844996598917476803157260"
      "780028538760589558632766878171540458953514382464234321326889464182768467"
      "546703537516986049910576551282076245490090389328944075868508455133942304"
      "583236903222948165808559332123348274797826204144723168738177180919299881"
      "250404026184124858368.000000",
      _gc(xasprintf("%10F", __DBL_MAX__)));
}

TEST(fmt, e) {
  EXPECT_STREQ("3.14159", _gc(xasprintf("%g", 0x1.921fb54442d1846ap+1)));
  EXPECT_STREQ("3.141592653589793",
               _gc(xasprintf("%.16g", 0x1.921fb54442d1846ap+1)));
  EXPECT_STREQ("1.000000e+100", _gc(xasprintf("%e", 1e100)));
  EXPECT_STREQ("1.000000E+100", _gc(xasprintf("%E", 1e100)));
  EXPECT_STREQ("            0.000000e+00", _gc(xasprintf("%24e", 0.)));
  EXPECT_STREQ("           -0.000000e+00", _gc(xasprintf("%24e", -0.)));
  EXPECT_STREQ("                     nan", _gc(xasprintf("%24e", NAN)));
  EXPECT_STREQ("                    -nan", _gc(xasprintf("%24e", -NAN)));
  EXPECT_STREQ("                     inf", _gc(xasprintf("%24e", INFINITY)));
  EXPECT_STREQ("                    -inf", _gc(xasprintf("%24e", -INFINITY)));
  EXPECT_STREQ("           2.225074e-308", _gc(xasprintf("%24e", __DBL_MIN__)));
  EXPECT_STREQ("           1.797693e+308", _gc(xasprintf("%24e", __DBL_MAX__)));
  EXPECT_STREQ("            0.000000E+00", _gc(xasprintf("%24E", 0.)));
  EXPECT_STREQ("           -0.000000E+00", _gc(xasprintf("%24E", -0.)));
  EXPECT_STREQ("                     NAN", _gc(xasprintf("%24E", NAN)));
  EXPECT_STREQ("                    -NAN", _gc(xasprintf("%24E", -NAN)));
  EXPECT_STREQ("                     INF", _gc(xasprintf("%24E", INFINITY)));
  EXPECT_STREQ("                    -INF", _gc(xasprintf("%24E", -INFINITY)));
  EXPECT_STREQ("           2.225074E-308", _gc(xasprintf("%24E", __DBL_MIN__)));
  EXPECT_STREQ("           1.797693E+308", _gc(xasprintf("%24E", __DBL_MAX__)));
  EXPECT_STREQ("           +0.000000e+00", _gc(xasprintf("%+24e", 0.)));
  EXPECT_STREQ("           -0.000000e+00", _gc(xasprintf("%+24e", -0.)));
  EXPECT_STREQ("                    +nan", _gc(xasprintf("%+24e", NAN)));
  EXPECT_STREQ("                    -nan", _gc(xasprintf("%+24e", -NAN)));
  EXPECT_STREQ("                    +inf", _gc(xasprintf("%+24e", INFINITY)));
  EXPECT_STREQ("                    -inf", _gc(xasprintf("%+24e", -INFINITY)));
  EXPECT_STREQ("          +2.225074e-308",
               _gc(xasprintf("%+24e", __DBL_MIN__)));
  EXPECT_STREQ("          +1.797693e+308",
               _gc(xasprintf("%+24e", __DBL_MAX__)));
  EXPECT_STREQ("           +0.000000E+00", _gc(xasprintf("%+24E", 0.)));
  EXPECT_STREQ("           -0.000000E+00", _gc(xasprintf("%+24E", -0.)));
  EXPECT_STREQ("                    +NAN", _gc(xasprintf("%+24E", NAN)));
  EXPECT_STREQ("                    -NAN", _gc(xasprintf("%+24E", -NAN)));
  EXPECT_STREQ("                    +INF", _gc(xasprintf("%+24E", INFINITY)));
  EXPECT_STREQ("                    -INF", _gc(xasprintf("%+24E", -INFINITY)));
  EXPECT_STREQ("          +2.225074E-308",
               _gc(xasprintf("%+24E", __DBL_MIN__)));
  EXPECT_STREQ("          +1.797693E+308",
               _gc(xasprintf("%+24E", __DBL_MAX__)));
}

TEST(fmt, a) {
  EXPECT_STREQ("0x1.921fb54442d18p+1",
               _gc(xasprintf("%a", 0x1.921fb54442d1846ap+1)));
  EXPECT_STREQ("0X1.921FB54442D18P+1",
               _gc(xasprintf("%A", 0x1.921fb54442d1846ap+1)));
  EXPECT_STREQ("                  0x0p+0", _gc(xasprintf("%24a", 0.)));
  EXPECT_STREQ("                 -0x0p+0", _gc(xasprintf("%24a", -0.)));
  EXPECT_STREQ("                     nan", _gc(xasprintf("%24a", NAN)));
  EXPECT_STREQ("                    -nan", _gc(xasprintf("%24a", -NAN)));
  EXPECT_STREQ("                     inf", _gc(xasprintf("%24a", INFINITY)));
  EXPECT_STREQ("                    -inf", _gc(xasprintf("%24a", -INFINITY)));
  EXPECT_STREQ("               0x1p-1022", _gc(xasprintf("%24a", __DBL_MIN__)));
  EXPECT_STREQ(" 0x1.fffffffffffffp+1023", _gc(xasprintf("%24a", __DBL_MAX__)));
  EXPECT_STREQ("                  0X0P+0", _gc(xasprintf("%24A", 0.)));
  EXPECT_STREQ("                 -0X0P+0", _gc(xasprintf("%24A", -0.)));
  EXPECT_STREQ("                     NAN", _gc(xasprintf("%24A", NAN)));
  EXPECT_STREQ("                    -NAN", _gc(xasprintf("%24A", -NAN)));
  EXPECT_STREQ("                     INF", _gc(xasprintf("%24A", INFINITY)));
  EXPECT_STREQ("                    -INF", _gc(xasprintf("%24A", -INFINITY)));
  EXPECT_STREQ("               0X1P-1022", _gc(xasprintf("%24A", __DBL_MIN__)));
  EXPECT_STREQ(" 0X1.FFFFFFFFFFFFFP+1023", _gc(xasprintf("%24A", __DBL_MAX__)));
  EXPECT_STREQ("   0X1.E9A488E8A71DEP+14", _gc(xasprintf("%24A", 31337.1337)));
  EXPECT_STREQ("  -0X1.E9A488E8A71DEP+14", _gc(xasprintf("%24A", -31337.1337)));
}

TEST(fmt, p) {
  EXPECT_STREQ("0x1", _gc(xasprintf("%p", 1)));
  EXPECT_STREQ("0x10", _gc(xasprintf("%p", 16)));
  EXPECT_STREQ("0x31337", _gc(xasprintf("%p", 0x31337)));
  EXPECT_STREQ("0xffffffff", _gc(xasprintf("%p", 0xffffffff)));
  EXPECT_STREQ("0xffff800000031337", _gc(xasprintf("%p", 0xffff800000031337)));
  EXPECT_STREQ("       0x1", _gc(xasprintf("%10p", 1)));
  EXPECT_STREQ("      0x10", _gc(xasprintf("%10p", 16)));
  EXPECT_STREQ("   0x31337", _gc(xasprintf("%10p", 0x31337)));
  EXPECT_STREQ("0xffffffff", _gc(xasprintf("%10p", 0xffffffff)));
  EXPECT_STREQ("0xffff800000031337",
               _gc(xasprintf("%10p", 0xffff800000031337)));
  EXPECT_STREQ("0x00000001", _gc(xasprintf("%010p", 1)));
  EXPECT_STREQ("0x00000010", _gc(xasprintf("%010p", 16)));
  EXPECT_STREQ("0x00031337", _gc(xasprintf("%010p", 0x31337)));
  EXPECT_STREQ("0xffffffff", _gc(xasprintf("%010p", 0xffffffff)));
  EXPECT_STREQ("0xffff800000031337",
               _gc(xasprintf("%010p", 0xffff800000031337)));
  EXPECT_STREQ("0x1       ", _gc(xasprintf("%-10p", 1)));
  EXPECT_STREQ("0x10      ", _gc(xasprintf("%-10p", 16)));
  EXPECT_STREQ("0x31337   ", _gc(xasprintf("%-10p", 0x31337)));
  EXPECT_STREQ("0xffffffff", _gc(xasprintf("%-10p", 0xffffffff)));
  EXPECT_STREQ("0xffff800000031337",
               _gc(xasprintf("%-10p", 0xffff800000031337)));
  EXPECT_STREQ("       0x1", _gc(xasprintf("%+10p", 1)));
  EXPECT_STREQ("      0x10", _gc(xasprintf("%+10p", 16)));
  EXPECT_STREQ("   0x31337", _gc(xasprintf("%+10p", 0x31337)));
  EXPECT_STREQ("0xffffffff", _gc(xasprintf("%+10p", 0xffffffff)));
  EXPECT_STREQ("0xffff800000031337",
               _gc(xasprintf("%+10p", 0xffff800000031337)));
  EXPECT_STREQ("       0x1", _gc(xasprintf("% 10p", 1)));
  EXPECT_STREQ("      0x10", _gc(xasprintf("% 10p", 16)));
  EXPECT_STREQ("   0x31337", _gc(xasprintf("% 10p", 0x31337)));
  EXPECT_STREQ("0xffffffff", _gc(xasprintf("% 10p", 0xffffffff)));
  EXPECT_STREQ("0xffff800000031337",
               _gc(xasprintf("% 10p", 0xffff800000031337)));
}

TEST(fmt, quoted) {
  ASSERT_STREQ("   \"hello\"", _gc(xasprintf("%`*.*s", 10, 5, "hello")));
  ASSERT_STREQ("\"hello\"   ", _gc(xasprintf("%-`*.*s", 10, 5, "hello")));
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
