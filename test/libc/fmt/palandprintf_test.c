/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=8 sts=2 sw=2 fenc=utf-8                                :vi│
╚══════════════════════════════════════════════════════════════════════════════╝
│ @author (c) Marco Paland (info@paland.com)                                   │
│             2014-2019, PALANDesign Hannover, Germany                         │
│                                                                              │
│ @license The MIT License (MIT)                                               │
│                                                                              │
│ Permission is hereby granted, free of charge, to any person obtaining a copy │
│ of this software and associated documentation files (the "Software"), to deal│
│ in the Software without restriction, including without limitation the rights │
│ to use, copy, modify, merge, publish, distribute, sublicense, and/or sell    │
│ copies of the Software, and to permit persons to whom the Software is        │
│ furnished to do so, subject to the following conditions:                     │
│                                                                              │
│ The above copyright notice and this permission notice shall be included in   │
│ all copies or substantial portions of the Software.                          │
│                                                                              │
│ THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR   │
│ IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,     │
│ FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE  │
│ AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER       │
│ LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,│
│ OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN    │
│ THE SOFTWARE.                                                                │
└─────────────────────────────────────────────────────────────────────────────*/
#include "libc/bits/progn.internal.h"
#include "libc/bits/pushpop.h"
#include "libc/bits/safemacros.h"
#include "libc/errno.h"
#include "libc/fmt/fmt.h"
#include "libc/fmt/itoa.h"
#include "libc/limits.h"
#include "libc/math.h"
#include "libc/mem/mem.h"
#include "libc/runtime/gc.h"
#include "libc/str/str.h"
#include "libc/testlib/ezbench.h"
#include "libc/testlib/testlib.h"
#include "libc/x/x.h"

static char buffer[128];
#define Format(...) PROGN(snprintf(buffer, sizeof(buffer), __VA_ARGS__), buffer)

TEST(sprintf, test_space_flag) {
  EXPECT_STREQ(" 42", Format("% d", 42));
  EXPECT_STREQ("-42", Format("% d", -42));
  EXPECT_STREQ("   42", Format("% 5d", 42));
  EXPECT_STREQ("  -42", Format("% 5d", -42));
  EXPECT_STREQ("             42", Format("% 15d", 42));
  EXPECT_STREQ("            -42", Format("% 15d", -42));
  EXPECT_STREQ("            -42", Format("% 15d", -42));
  EXPECT_STREQ("        -42.987", Format("% 15.3f", -42.987));
  EXPECT_STREQ("         42.987", Format("% 15.3f", 42.987));
  EXPECT_STREQ("Hello testing", Format("% s", "Hello testing"));
  EXPECT_STREQ(" 1024", Format("% d", 1024));
  EXPECT_STREQ("-1024", Format("% d", -1024));
  EXPECT_STREQ(" 1024", Format("% i", 1024));
  EXPECT_STREQ("-1024", Format("% i", -1024));
  EXPECT_STREQ("1024", Format("% u", 1024));
  EXPECT_STREQ("4294966272", Format("% u", 4294966272U));
  EXPECT_STREQ("777", Format("% o", 511));
  EXPECT_STREQ("37777777001", Format("% o", 4294966785U));
  EXPECT_STREQ("1234abcd", Format("% x", 305441741));
  EXPECT_STREQ("edcb5433", Format("% x", 3989525555U));
  EXPECT_STREQ("1234ABCD", Format("% X", 305441741));
  EXPECT_STREQ("EDCB5433", Format("% X", 3989525555U));
  EXPECT_STREQ("x", Format("% c", 'x'));
}

TEST(sprintf, test_plus_flag) {
  EXPECT_STREQ("+42", Format("%+d", 42));
  EXPECT_STREQ("-42", Format("%+d", -42));
  EXPECT_STREQ("  +42", Format("%+5d", 42));
  EXPECT_STREQ("  -42", Format("%+5d", -42));
  EXPECT_STREQ("            +42", Format("%+15d", 42));
  EXPECT_STREQ("            -42", Format("%+15d", -42));
  EXPECT_STREQ("+1024", Format("%+d", 1024));
  EXPECT_STREQ("-1024", Format("%+d", -1024));
  EXPECT_STREQ("+1024", Format("%+i", 1024));
  EXPECT_STREQ("-1024", Format("%+i", -1024));
  EXPECT_STREQ("1024", Format("%+u", 1024));
  EXPECT_STREQ("4294966272", Format("%+u", 4294966272U));
  EXPECT_STREQ("777", Format("%+o", 511));
  EXPECT_STREQ("37777777001", Format("%+o", 4294966785U));
  EXPECT_STREQ("1234abcd", Format("%+x", 305441741));
  EXPECT_STREQ("edcb5433", Format("%+x", 3989525555U));
  EXPECT_STREQ("1234ABCD", Format("%+X", 305441741));
  EXPECT_STREQ("EDCB5433", Format("%+X", 3989525555U));
  EXPECT_STREQ("+", Format("%+.0d", 0));
}

TEST(sprintf, test_flag0) {
  EXPECT_STREQ("42", Format("%0d", 42));
  EXPECT_STREQ("42", Format("%0ld", 42L));
  EXPECT_STREQ("-42", Format("%0d", -42));
  EXPECT_STREQ("00042", Format("%05d", 42));
  EXPECT_STREQ("-0042", Format("%05d", -42));
  EXPECT_STREQ("000000000000042", Format("%015d", 42));
  EXPECT_STREQ("-00000000000042", Format("%015d", -42));
  EXPECT_STREQ("000000000042.12", Format("%015.2f", 42.1234));
  EXPECT_STREQ("00000000042.988", Format("%015.3f", 42.9876));
  EXPECT_STREQ("-00000042.98760", Format("%015.5f", -42.9876));
}

TEST(sprintf, test_flag_minus) {
  EXPECT_STREQ("42", Format("%-d", 42));
  EXPECT_STREQ("-42", Format("%-d", -42));
  EXPECT_STREQ("42   ", Format("%-5d", 42));
  EXPECT_STREQ("-42  ", Format("%-5d", -42));
  EXPECT_STREQ("42             ", Format("%-15d", 42));
  EXPECT_STREQ("-42            ", Format("%-15d", -42));
  EXPECT_STREQ("42", Format("%-0d", 42));
  EXPECT_STREQ("-42", Format("%-0d", -42));
  EXPECT_STREQ("42   ", Format("%-05d", 42));
  EXPECT_STREQ("-42  ", Format("%-05d", -42));
  EXPECT_STREQ("42             ", Format("%-015d", 42));
  EXPECT_STREQ("-42            ", Format("%-015d", -42));
  EXPECT_STREQ("42", Format("%0-d", 42));
  EXPECT_STREQ("-42", Format("%0-d", -42));
  EXPECT_STREQ("42   ", Format("%0-5d", 42));
  EXPECT_STREQ("-42  ", Format("%0-5d", -42));
  EXPECT_STREQ("42             ", Format("%0-15d", 42));
  EXPECT_STREQ("-42            ", Format("%0-15d", -42));
}

TEST(sprintf, testAlternativeFormatting) {
  EXPECT_STREQ("0", Format("%#x", 0));
  EXPECT_STREQ("", Format("%#.0x", 0));
  EXPECT_STREQ("0", Format("%#.1x", 0));
  EXPECT_STREQ("", Format("%#.0llx", (long long)0));
  EXPECT_STREQ("0x0000614e", Format("%#.8x", 0x614e));
  EXPECT_STREQ("0b110", Format("%#b", 6));
}

TEST(sprintf, testHexBasePrefix_onlyConsumesLeadingZeroes) {
  /* TODO(jart): Upstream bug fix for this behavior. */
  EXPECT_STREQ("0x01", Format("%#04x", 0x1));
  EXPECT_STREQ("0x12", Format("%#04x", 0x12));
  EXPECT_STREQ("0x123", Format("%#04x", 0x123));
  EXPECT_STREQ("0x1234", Format("%#04x", 0x1234));
}

TEST(sprintf, testBinaryBasePrefix_onlyConsumesLeadingZeroes) {
  EXPECT_STREQ("0b01", Format("%#04b", 0b1));
  EXPECT_STREQ("0b10", Format("%#04b", 0b10));
  EXPECT_STREQ("0b101", Format("%#04b", 0b101));
  EXPECT_STREQ("0b1010", Format("%#04b", 0b1010));
}

TEST(sprintf, testOctalBasePrefix_onlyConsumesLeadingZeroes) {
  EXPECT_STREQ("0001", Format("%#04o", 01));
  EXPECT_STREQ("0010", Format("%#04o", 010));
  EXPECT_STREQ("0101", Format("%#04o", 0101));
  EXPECT_STREQ("01010", Format("%#04o", 01010));
}

TEST(sprintf, test_specifier) {
  EXPECT_STREQ("Hello testing", Format("Hello testing"));
  EXPECT_STREQ("Hello testing", Format("%s", "Hello testing"));
  EXPECT_STREQ("1024", Format("%d", 1024));
  EXPECT_STREQ("-1024", Format("%d", -1024));
  EXPECT_STREQ("1024", Format("%i", 1024));
  EXPECT_STREQ("-1024", Format("%i", -1024));
  EXPECT_STREQ("1024", Format("%u", 1024));
  EXPECT_STREQ("4294966272", Format("%u", 4294966272U));
  EXPECT_STREQ("777", Format("%o", 511));
  EXPECT_STREQ("37777777001", Format("%o", 4294966785U));
  EXPECT_STREQ("1234abcd", Format("%x", 305441741));
  EXPECT_STREQ("edcb5433", Format("%x", 3989525555U));
  EXPECT_STREQ("1234ABCD", Format("%X", 305441741));
  EXPECT_STREQ("EDCB5433", Format("%X", 3989525555U));
  EXPECT_STREQ("%", Format("%%"));
}

TEST(sprintf, test_width) {
  EXPECT_STREQ("Hello testing", Format("%1s", "Hello testing"));
  EXPECT_STREQ("1024", Format("%1d", 1024));
  EXPECT_STREQ("-1024", Format("%1d", -1024));
  EXPECT_STREQ("1024", Format("%1i", 1024));
  EXPECT_STREQ("-1024", Format("%1i", -1024));
  EXPECT_STREQ("1024", Format("%1u", 1024));
  EXPECT_STREQ("4294966272", Format("%1u", 4294966272U));
  EXPECT_STREQ("777", Format("%1o", 511));
  EXPECT_STREQ("37777777001", Format("%1o", 4294966785U));
  EXPECT_STREQ("1234abcd", Format("%1x", 305441741));
  EXPECT_STREQ("edcb5433", Format("%1x", 3989525555U));
  EXPECT_STREQ("1234ABCD", Format("%1X", 305441741));
  EXPECT_STREQ("EDCB5433", Format("%1X", 3989525555U));
  EXPECT_STREQ("x", Format("%1c", 'x'));
}

TEST(sprintf, test_width_20) {
  /* TODO(jart): wut */
  /* sprintf(buffer, "%20s", "Hello"); */
  /* printf("'%s' %d\n", buffer, strlen(buffer)); */
  /* EXPECT_STREQ("               Hello", buffer); */
  EXPECT_STREQ("                1024", Format("%20d", 1024));
  EXPECT_STREQ("               -1024", Format("%20d", -1024));
  EXPECT_STREQ("                1024", Format("%20i", 1024));
  EXPECT_STREQ("               -1024", Format("%20i", -1024));
  EXPECT_STREQ("                1024", Format("%20u", 1024));
  EXPECT_STREQ("          4294966272", Format("%20u", 4294966272U));
  EXPECT_STREQ("                 777", Format("%20o", 511));
  EXPECT_STREQ("         37777777001", Format("%20o", 4294966785U));
  EXPECT_STREQ("            1234abcd", Format("%20x", 305441741));
  EXPECT_STREQ("            edcb5433", Format("%20x", 3989525555U));
  EXPECT_STREQ("            1234ABCD", Format("%20X", 305441741));
  EXPECT_STREQ("            EDCB5433", Format("%20X", 3989525555U));
  EXPECT_STREQ("                   x", Format("%20c", 'x'));
}

TEST(sprintf, test_width_star_20) {
  EXPECT_STREQ("               Hello", Format("%*s", 20, "Hello"));
  EXPECT_STREQ("                1024", Format("%*d", 20, 1024));
  EXPECT_STREQ("               -1024", Format("%*d", 20, -1024));
  EXPECT_STREQ("                1024", Format("%*i", 20, 1024));
  EXPECT_STREQ("               -1024", Format("%*i", 20, -1024));
  EXPECT_STREQ("                1024", Format("%*u", 20, 1024));
  EXPECT_STREQ("          4294966272", Format("%*u", 20, 4294966272U));
  EXPECT_STREQ("                 777", Format("%*o", 20, 511));
  EXPECT_STREQ("         37777777001", Format("%*o", 20, 4294966785U));
  EXPECT_STREQ("            1234abcd", Format("%*x", 20, 305441741));
  EXPECT_STREQ("            edcb5433", Format("%*x", 20, 3989525555U));
  EXPECT_STREQ("            1234ABCD", Format("%*X", 20, 305441741));
  EXPECT_STREQ("            EDCB5433", Format("%*X", 20, 3989525555U));
  EXPECT_STREQ("                   x", Format("%*c", 20, 'x'));
}

TEST(sprintf, test_width_minus_20) {
  EXPECT_STREQ("Hello               ", Format("%-20s", "Hello"));
  EXPECT_STREQ("1024                ", Format("%-20d", 1024));
  EXPECT_STREQ("-1024               ", Format("%-20d", -1024));
  EXPECT_STREQ("1024                ", Format("%-20i", 1024));
  EXPECT_STREQ("-1024               ", Format("%-20i", -1024));
  EXPECT_STREQ("1024                ", Format("%-20u", 1024));
  EXPECT_STREQ("1024.1234           ", Format("%-20.4f", 1024.1234));
  EXPECT_STREQ("4294966272          ", Format("%-20u", 4294966272U));
  EXPECT_STREQ("777                 ", Format("%-20o", 511));
  EXPECT_STREQ("37777777001         ", Format("%-20o", 4294966785U));
  EXPECT_STREQ("1234abcd            ", Format("%-20x", 305441741));
  EXPECT_STREQ("edcb5433            ", Format("%-20x", 3989525555U));
  EXPECT_STREQ("1234ABCD            ", Format("%-20X", 305441741));
  EXPECT_STREQ("EDCB5433            ", Format("%-20X", 3989525555U));
  EXPECT_STREQ("x                   ", Format("%-20c", 'x'));
  EXPECT_STREQ("|    9| |9 | |    9|", Format("|%5d| |%-2d| |%5d|", 9, 9, 9));
  EXPECT_STREQ("|   10| |10| |   10|",
               Format("|%5d| |%-2d| |%5d|", 10, 10, 10));
  EXPECT_STREQ("|    9| |9           | |    9|",
               Format("|%5d| |%-12d| |%5d|", 9, 9, 9));
  EXPECT_STREQ("|   10| |10          | |   10|",
               Format("|%5d| |%-12d| |%5d|", 10, 10, 10));
}

TEST(sprintf, test_width_0_minus_20) {
  EXPECT_STREQ("Hello               ", Format("%0-20s", "Hello"));
  EXPECT_STREQ("1024                ", Format("%0-20d", 1024));
  EXPECT_STREQ("-1024               ", Format("%0-20d", -1024));
  EXPECT_STREQ("1024                ", Format("%0-20i", 1024));
  EXPECT_STREQ("-1024               ", Format("%0-20i", -1024));
  EXPECT_STREQ("1024                ", Format("%0-20u", 1024));
  EXPECT_STREQ("4294966272          ", Format("%0-20u", 4294966272U));
  EXPECT_STREQ("777                 ", Format("%0-20o", 511));
  EXPECT_STREQ("37777777001         ", Format("%0-20o", 4294966785U));
  EXPECT_STREQ("1234abcd            ", Format("%0-20x", 305441741));
  EXPECT_STREQ("edcb5433            ", Format("%0-20x", 3989525555U));
  EXPECT_STREQ("1234ABCD            ", Format("%0-20X", 305441741));
  EXPECT_STREQ("EDCB5433            ", Format("%0-20X", 3989525555U));
  EXPECT_STREQ("x                   ", Format("%0-20c", 'x'));
}

TEST(sprintf, test_padding_20) {
  EXPECT_STREQ("00000000000000001024", Format("%020d", 1024));
  EXPECT_STREQ("-0000000000000001024", Format("%020d", -1024));
  EXPECT_STREQ("00000000000000001024", Format("%020i", 1024));
  EXPECT_STREQ("-0000000000000001024", Format("%020i", -1024));
  EXPECT_STREQ("00000000000000001024", Format("%020u", 1024));
  EXPECT_STREQ("00000000004294966272", Format("%020u", 4294966272U));
  EXPECT_STREQ("00000000000000000777", Format("%020o", 511));
  EXPECT_STREQ("00000000037777777001", Format("%020o", 4294966785U));
  EXPECT_STREQ("0000000000001234abcd", Format("%020x", 305441741));
  EXPECT_STREQ("000000000000edcb5433", Format("%020x", 3989525555U));
  EXPECT_STREQ("0000000000001234ABCD", Format("%020X", 305441741));
  EXPECT_STREQ("000000000000EDCB5433", Format("%020X", 3989525555U));
}

TEST(sprintf, test_padding_dot_20) {
  EXPECT_STREQ("00000000000000001024", Format("%.20d", 1024));
  EXPECT_STREQ("-00000000000000001024", Format("%.20d", -1024));
  EXPECT_STREQ("00000000000000001024", Format("%.20i", 1024));
  EXPECT_STREQ("-00000000000000001024", Format("%.20i", -1024));
  EXPECT_STREQ("00000000000000001024", Format("%.20u", 1024));
  EXPECT_STREQ("00000000004294966272", Format("%.20u", 4294966272U));
  EXPECT_STREQ("00000000000000000777", Format("%.20o", 511));
  EXPECT_STREQ("00000000037777777001", Format("%.20o", 4294966785U));
  EXPECT_STREQ("0000000000001234abcd", Format("%.20x", 305441741));
  EXPECT_STREQ("000000000000edcb5433", Format("%.20x", 3989525555U));
  EXPECT_STREQ("0000000000001234ABCD", Format("%.20X", 305441741));
  EXPECT_STREQ("000000000000EDCB5433", Format("%.20X", 3989525555U));
}

TEST(sprintf, test_padding_pound_020) {
  EXPECT_STREQ("00000000000000001024", Format("%#020d", 1024));
  EXPECT_STREQ("-0000000000000001024", Format("%#020d", -1024));
  EXPECT_STREQ("00000000000000001024", Format("%#020i", 1024));
  EXPECT_STREQ("-0000000000000001024", Format("%#020i", -1024));
  EXPECT_STREQ("00000000000000001024", Format("%#020u", 1024));
  EXPECT_STREQ("00000000004294966272", Format("%#020u", 4294966272U));
  EXPECT_STREQ("00000000000000000777", Format("%#020o", 511));
  EXPECT_STREQ("00000000037777777001", Format("%#020o", 4294966785U));
  EXPECT_STREQ("0x00000000001234abcd", Format("%#020x", 305441741));
  EXPECT_STREQ("0x0000000000edcb5433", Format("%#020x", 3989525555U));
  EXPECT_STREQ("0x00000000001234ABCD", Format("%#020X", 305441741));
  EXPECT_STREQ("0x0000000000EDCB5433", Format("%#020X", 3989525555U));
}

TEST(sprintf, test_padding_pound_20) {
  EXPECT_STREQ("                1024", Format("%#20d", 1024));
  EXPECT_STREQ("               -1024", Format("%#20d", -1024));
  EXPECT_STREQ("                1024", Format("%#20i", 1024));
  EXPECT_STREQ("               -1024", Format("%#20i", -1024));
  EXPECT_STREQ("                1024", Format("%#20u", 1024));
  EXPECT_STREQ("          4294966272", Format("%#20u", 4294966272U));
  EXPECT_STREQ("                0777", Format("%#20o", 511));
  EXPECT_STREQ("        037777777001", Format("%#20o", 4294966785U));
  EXPECT_STREQ("          0x1234abcd", Format("%#20x", 305441741));
  EXPECT_STREQ("          0xedcb5433", Format("%#20x", 3989525555U));
  EXPECT_STREQ("          0x1234ABCD", Format("%#20X", 305441741));
  EXPECT_STREQ("          0xEDCB5433", Format("%#20X", 3989525555U));
}

TEST(sprintf, test_padding_20_point_5) {
  EXPECT_STREQ("               01024", Format("%20.5d", 1024));
  EXPECT_STREQ("              -01024", Format("%20.5d", -1024));
  EXPECT_STREQ("               01024", Format("%20.5i", 1024));
  EXPECT_STREQ("              -01024", Format("%20.5i", -1024));
  EXPECT_STREQ("               01024", Format("%20.5u", 1024));
  EXPECT_STREQ("          4294966272", Format("%20.5u", 4294966272U));
  EXPECT_STREQ("               00777", Format("%20.5o", 511));
  EXPECT_STREQ("         37777777001", Format("%20.5o", 4294966785U));
  EXPECT_STREQ("            1234abcd", Format("%20.5x", 305441741));
  EXPECT_STREQ("          00edcb5433", Format("%20.10x", 3989525555U));
  EXPECT_STREQ("            1234ABCD", Format("%20.5X", 305441741));
  EXPECT_STREQ("          00EDCB5433", Format("%20.10X", 3989525555U));
}

TEST(sprintf, test_padding_neg_numbers) {
  /* space padding */
  EXPECT_STREQ("-5", Format("% 1d", -5));
  EXPECT_STREQ("-5", Format("% 2d", -5));
  EXPECT_STREQ(" -5", Format("% 3d", -5));
  EXPECT_STREQ("  -5", Format("% 4d", -5));
  /* zero padding */
  EXPECT_STREQ("-5", Format("%01d", -5));
  EXPECT_STREQ("-5", Format("%02d", -5));
  EXPECT_STREQ("-05", Format("%03d", -5));
  EXPECT_STREQ("-005", Format("%04d", -5));
}

TEST(sprintf, test_float_padding_neg_numbers) {
  /* space padding */
  EXPECT_STREQ("-5.0", Format("% 3.1f", -5.));
  EXPECT_STREQ("-5.0", Format("% 4.1f", -5.));
  EXPECT_STREQ(" -5.0", Format("% 5.1f", -5.));
  /* zero padding */
  EXPECT_STREQ("-5.0", Format("%03.1f", -5.));
  EXPECT_STREQ("-5.0", Format("%04.1f", -5.));
  EXPECT_STREQ("-05.0", Format("%05.1f", -5.));
  /* zero padding no decimal point */
  EXPECT_STREQ("-5", Format("%01.0f", -5.));
  EXPECT_STREQ("-5", Format("%02.0f", -5.));
  EXPECT_STREQ("-05", Format("%03.0f", -5.));
}

TEST(sprintf, test_length) {
  EXPECT_STREQ("", Format("%.0s", "Hello testing"));
  EXPECT_STREQ("                    ", Format("%20.0s", "Hello testing"));
  EXPECT_STREQ("", Format("%.s", "Hello testing"));
  EXPECT_STREQ("                    ", Format("%20.s", "Hello testing"));
  EXPECT_STREQ("                1024", Format("%20.0d", 1024));
  EXPECT_STREQ("               -1024", Format("%20.0d", -1024));
  EXPECT_STREQ("                    ", Format("%20.d", 0));
  EXPECT_STREQ("                1024", Format("%20.0i", 1024));
  EXPECT_STREQ("               -1024", Format("%20.i", -1024));
  EXPECT_STREQ("                    ", Format("%20.i", 0));
  EXPECT_STREQ("                1024", Format("%20.u", 1024));
  EXPECT_STREQ("          4294966272", Format("%20.0u", 4294966272U));
  EXPECT_STREQ("                    ", Format("%20.u", 0U));
  EXPECT_STREQ("                 777", Format("%20.o", 511));
  EXPECT_STREQ("         37777777001", Format("%20.0o", 4294966785U));
  EXPECT_STREQ("                    ", Format("%20.o", 0U));
  EXPECT_STREQ("            1234abcd", Format("%20.x", 305441741));
  EXPECT_STREQ("                                          1234abcd",
               Format("%50.x", 305441741));
  EXPECT_STREQ("                                          1234abcd     12345",
               Format("%50.x%10.u", 305441741, 12345));
  EXPECT_STREQ("            edcb5433", Format("%20.0x", 3989525555U));
  EXPECT_STREQ("                    ", Format("%20.x", 0U));
  EXPECT_STREQ("            1234ABCD", Format("%20.X", 305441741));
  EXPECT_STREQ("            EDCB5433", Format("%20.0X", 3989525555U));
  EXPECT_STREQ("                    ", Format("%20.X", 0U));
  EXPECT_STREQ("  ", Format("%02.0u", 0U));
  EXPECT_STREQ("  ", Format("%02.0d", 0));
}

TEST(sprintf, test_float) {
#ifndef __FINITE_MATH_ONLY__
  EXPECT_STREQ("nan", Format("%.4f", NAN));
#endif
  EXPECT_STREQ("3.1415", Format("%.4f", 3.1415354));
  EXPECT_STREQ("30343.142", Format("%.3f", 30343.1415354));
  EXPECT_STREQ("34", Format("%.0f", 34.1415354));
  EXPECT_STREQ("1", Format("%.0f", 1.3));
  EXPECT_STREQ("2", Format("%.0f", 1.55));
  EXPECT_STREQ("1.6", Format("%.1f", 1.64));
  EXPECT_STREQ("42.90", Format("%.2f", 42.8952));
  EXPECT_STREQ("42.895200000", Format("%.9f", 42.8952));
  EXPECT_STREQ("42.8952230000", Format("%.10f", 42.895223));
  EXPECT_STREQ("42.89522312345678", Format("%.14f", 42.89522312345678));
  EXPECT_STREQ("42.89522387654321", Format("%.14f", 42.89522387654321));
  EXPECT_STREQ(" 42.90", Format("%6.2f", 42.8952));
  EXPECT_STREQ("+42.90", Format("%+6.2f", 42.8952));
  EXPECT_STREQ("+42.9", Format("%+5.1f", 42.9252));
  EXPECT_STREQ("42.500000", Format("%f", 42.5));
  EXPECT_STREQ("42.5", Format("%.1f", 42.5));
  EXPECT_STREQ("42167.000000", Format("%f", 42167.0));
  EXPECT_STREQ("-12345.987654321", Format("%.9f", -12345.987654321));
  EXPECT_STREQ("4.0", Format("%.1f", 3.999));
  EXPECT_STREQ("4", Format("%.0f", 3.5));
  EXPECT_STREQ("4", Format("%.0f", 4.5));
  EXPECT_STREQ("3", Format("%.0f", 3.49));
  EXPECT_STREQ("3.5", Format("%.1f", 3.49));
  EXPECT_STREQ("a0.5  ", Format("a%-5.1f", 0.5));
  EXPECT_STREQ("a0.5  end", Format("a%-5.1fend", 0.5));
  /* out of range in the moment, need to be fixed by someone */
  EXPECT_STREQ("inf", Format("%.1f", 1E20));
}

TEST(sprintf, test_types) {
  EXPECT_STREQ("0", Format("%i", 0));
  EXPECT_STREQ("1234", Format("%i", 1234));
  EXPECT_STREQ("32767", Format("%i", 32767));
  EXPECT_STREQ("-32767", Format("%i", -32767));
  EXPECT_STREQ("30", Format("%li", 30L));
  EXPECT_STREQ("-2147483647", Format("%li", -2147483647L));
  EXPECT_STREQ("2147483647", Format("%li", 2147483647L));
  EXPECT_STREQ("30", Format("%lli", 30LL));
  EXPECT_STREQ("-9223372036854775807", Format("%lli", -9223372036854775807LL));
  EXPECT_STREQ("9223372036854775807", Format("%lli", 9223372036854775807LL));
  EXPECT_STREQ("100000", Format("%lu", 100000L));
  EXPECT_STREQ("4294967295", Format("%lu", 0xFFFFFFFFL));
  EXPECT_STREQ("281474976710656", Format("%llu", 281474976710656LLU));
  EXPECT_STREQ("18446744073709551615", Format("%llu", 18446744073709551615LLU));
  EXPECT_STREQ("2147483647", Format("%zu", 2147483647UL));
  EXPECT_STREQ("2147483647", Format("%zd", 2147483647UL));
  if (sizeof(size_t) == sizeof(long)) {
    EXPECT_STREQ("-2147483647", Format("%zi", -2147483647L));
  } else {
    EXPECT_STREQ("-2147483647", Format("%zi", -2147483647LL));
  }
  EXPECT_STREQ("1110101001100000", Format("%b", 60000));
  EXPECT_STREQ("101111000110000101001110", Format("%lb", 12345678L));
  EXPECT_STREQ("165140", Format("%o", 60000));
  EXPECT_STREQ("57060516", Format("%lo", 12345678L));
  EXPECT_STREQ("12345678", Format("%lx", 0x12345678L));
  EXPECT_STREQ("1234567891234567", Format("%llx", 0x1234567891234567LLU));
  EXPECT_STREQ("abcdefab", Format("%lx", 0xabcdefabL));
  EXPECT_STREQ("ABCDEFAB", Format("%lX", 0xabcdefabL));
  EXPECT_STREQ("v", Format("%c", 'v'));
  EXPECT_STREQ("wv", Format("%cv", 'w'));
  EXPECT_STREQ("A Test", Format("%s", "A Test"));
  EXPECT_STREQ("255", Format("%hhu", 0xFFFFUL));
  EXPECT_STREQ("a", Format("%tx", &buffer[10] - &buffer[0]));
  /* TBD */
  EXPECT_STREQ("-2147483647", Format("%ji", (intmax_t)-2147483647L));
}

TEST(sprintf, testOverflow_truncationNotSaturation) {
  errno = 0;
  EXPECT_STREQ("13398", Format("%hu", 0x123456UL));
  errno = 0;
  EXPECT_STREQ("Test16 65535", Format("%s%hhi %hu", "Test", 10000, 0xFFFFFFFF));
}

TEST(sprintf, test_pointer) {
  sprintf(buffer, "%p", (void *)0x1234U);
  if (sizeof(void *) == 4U) {
    EXPECT_STREQ("00001234", buffer);
  } else {
    EXPECT_STREQ("000000001234", buffer);
  }
  sprintf(buffer, "%p", (void *)0x12345678U);
  if (sizeof(void *) == 4U) {
    EXPECT_STREQ("12345678", buffer);
  } else {
    EXPECT_STREQ("000012345678", buffer);
  }
  sprintf(buffer, "%p-%p", (void *)0x12345678U, (void *)0x7EDCBA98U);
  if (sizeof(void *) == 4U) {
    EXPECT_STREQ("12345678-7edcba98", buffer);
  } else {
    EXPECT_STREQ("000012345678-00007edcba98", buffer);
  }
  if (sizeof(uintptr_t) == sizeof(uint64_t)) {
    sprintf(buffer, "%p", (void *)(uintptr_t)0xFFFFFFFFU);
    EXPECT_STREQ("0000ffffffff", buffer);
  } else {
    sprintf(buffer, "%p", (void *)(uintptr_t)0xFFFFFFFFU);
    EXPECT_STREQ("ffffffff", buffer);
  }
}

TEST(sprintf, test_unknown_flag) {
  EXPECT_STREQ("kmarco", Format("%kmarco", 42, 37));
}

TEST(sprintf, test_buffer_length) {
  int ret;
  /* EXPECT_EQ(4, snprintf(pushpop(NULL), 10, "%s", "Test")); */
  EXPECT_EQ(4, snprintf(pushpop(NULL), 0, "%s", "Test"));
  buffer[0] = (char)0xA5;
  ret = snprintf(buffer, 0, "%s", "Test");
  EXPECT_TRUE(buffer[0] == (char)0xA5);
  EXPECT_TRUE(ret == 4);
  buffer[0] = (char)0xCC;
  snprintf(buffer, 1, "%s", pushpop(&"Test"[0]));
  EXPECT_TRUE(buffer[0] == '\0');
  snprintf(buffer, 2, "%s", pushpop(&"Hello"[0]));
  EXPECT_STREQ("H", buffer);
}

TEST(sprintf, test_ret_value) {
  int ret;
  ret = snprintf(buffer, 6, "0%s", "1234");
  EXPECT_STREQ("01234", buffer);
  EXPECT_TRUE(ret == 5);
  ret = snprintf(buffer, 6, "0%s", pushpop(&"12345"[0]));
  EXPECT_STREQ("01234", buffer);
  EXPECT_TRUE(ret == 6); /* '5' is truncated */
  ret = snprintf(buffer, 6, "0%s", pushpop(&"1234567"[0]));
  EXPECT_STREQ("01234", buffer);
  EXPECT_TRUE(ret == 8); /* '567' are truncated */
  ret = snprintf(buffer, 10, pushpop(&"hello, world"[0]));
  EXPECT_TRUE(ret == 12);
  ret = snprintf(buffer, 3, "%d", pushpop(10000));
  EXPECT_TRUE(ret == 5);
  EXPECT_TRUE(strlen(buffer) == 2U);
  EXPECT_TRUE(buffer[0] == '1');
  EXPECT_TRUE(buffer[1] == '0');
  EXPECT_TRUE(buffer[2] == '\0');
}

TEST(sprintf, test_misc) {
  EXPECT_STREQ("53000atest-20 bit",
               Format("%u%u%ctest%d %s", 5, 3000, 'a', -20, "bit"));
  EXPECT_STREQ("0.33", Format("%.*f", 2, 0.33333333));
  EXPECT_STREQ("1", Format("%.*d", -1, 1));
  EXPECT_STREQ("foo", Format("%.3s", "foobar"));
  EXPECT_STREQ(" ", Format("% .0d", 0));
  EXPECT_STREQ("     00004", Format("%10.5d", 4));
  EXPECT_STREQ("hi x", Format("%*sx", -3, "hi"));
}

TEST(sprintf, test_snprintf) {
  snprintf(buffer, 100U, "%d", -1000);
  EXPECT_STREQ("-1000", buffer);
  snprintf(buffer, 3U, "%d", pushpop(-1000));
  EXPECT_STREQ("-1", buffer);
}

testonly void vsnprintf_builder_1(char *buf, ...) {
  va_list args;
  va_start(args, buf);
  vsnprintf(buf, 100U, "%d", args);
  va_end(args);
}

testonly void vsnprintf_builder_3(char *buf, ...) {
  va_list args;
  va_start(args, buf);
  vsnprintf(buf, 100U, "%d %d %s", args);
  va_end(args);
}

TEST(sprintf, test_vsnprintf) {
  vsnprintf_builder_1(buffer, -1);
  EXPECT_STREQ("-1", buffer);
  vsnprintf_builder_3(buffer, 3, -1000, "test");
  EXPECT_STREQ("3 -1000 test", buffer);
}

TEST(xasprintf, test) {
  void *pp;
  ASSERT_STREQ("hello 123", (pp = xasprintf("hello %d", 123)));
  free(pp);
}

TEST(xasprintf, nullPointer) {
  ASSERT_STREQ("000000000000", gc(xasprintf("%p", NULL)));
}

TEST(xasprintf, pointer_doesntShowNonCanonicalZeroes) {
  ASSERT_STREQ("100000000010", gc(xasprintf("%p", 0x0000100000000010)));
  ASSERT_STREQ("0x100000000010", gc(xasprintf("%#p", 0x0000100000000010)));
}

TEST(xasprintf, nonCanonicalPointer_discardsHighBits_ratherThanSaturate) {
  ASSERT_STREQ("100000000010", gc(xasprintf("%p", 0x1000100000000010)));
  ASSERT_STREQ("0x100000000010", gc(xasprintf("%#p", 0x1000100000000010)));
  ASSERT_STREQ("7fffffffffff", gc(xasprintf("%p", 0x7fffffffffff)));
  ASSERT_STREQ("0x7fffffffffff", gc(xasprintf("%#p", 0x7fffffffffff)));
}

TEST(xasprintf, hugeNtoa) {
  ASSERT_STREQ(
      "0b1111111111111111111111111111111111111111111111111111111111111111111111"
      "1111111111111111111111111111111111111111111111111111111111",
      gc(xasprintf("%#jb", UINT128_MAX)));
}

TEST(xasprintf, twosBane) {
  ASSERT_STREQ("-2147483648", gc(xasprintf("%d", 0x80000000)));
  ASSERT_STREQ("-9223372036854775808",
               gc(xasprintf("%ld", 0x8000000000000000)));
}

TEST(snprintf, testFixedWidthString_wontOverrunInput) {
  const int N = 2;
  char *buf = tmalloc(N + 1);
  char *inp = memcpy(tmalloc(N), "hi", N);
  EXPECT_EQ(2, snprintf(buf, N + 1, "%.*s", N, inp));
  EXPECT_BINEQ(u"hi ", buf);
  tfree(inp);
  tfree(buf);
}

TEST(snprintf, testFixedWidthStringIsNull_wontOverrunBuffer) {
  int N = 3;
  char *buf = tmalloc(N + 1);
  EXPECT_EQ(6, snprintf(buf, N + 1, "%.*s", pushpop(N), pushpop(NULL)));
  EXPECT_BINEQ(u"(nu ", buf);
  EXPECT_EQ(6, snprintf(buf, N + 1, "%#.*s", pushpop(N), pushpop(NULL)));
  EXPECT_BINEQ(u"(nu ", buf);
  EXPECT_EQ(4, snprintf(buf, N + 1, "%`.*s", pushpop(N), pushpop(NULL)));
  EXPECT_BINEQ(u"NUL ", buf);
  EXPECT_EQ(4, snprintf(buf, N + 1, "%`#.*s", pushpop(N), pushpop(NULL)));
  EXPECT_BINEQ(u"NUL ", buf);
  tfree(buf);
}

TEST(snprintf, testFixedWidthStringIsNull_wontLeakMemory) {
  int N = 16;
  char *buf = tmalloc(N + 1);
  memset(buf, 0, N + 1);
  EXPECT_EQ(6, snprintf(buf, N + 1, "%.*s", pushpop(N), pushpop(NULL)));
  EXPECT_BINEQ(u"(null)           ", buf);
  memset(buf, 0, N + 1);
  EXPECT_EQ(6, snprintf(buf, N + 1, "%#.*s", pushpop(N), pushpop(NULL)));
  EXPECT_BINEQ(u"(null)           ", buf);
  memset(buf, 0, N + 1);
  EXPECT_EQ(4, snprintf(buf, N + 1, "%`.*s", pushpop(N), pushpop(NULL)));
  EXPECT_BINEQ(u"NULL             ", buf);
  memset(buf, 0, N + 1);
  EXPECT_EQ(4, snprintf(buf, N + 1, "%`#.*s", pushpop(N), pushpop(NULL)));
  EXPECT_BINEQ(u"NULL             ", buf);
  tfree(buf);
}

TEST(snprintf, twosBaneWithTypePromotion) {
  int16_t x = 0x8000;
  EXPECT_STREQ("-32768", Format("%hd", x));
}

TEST(snprintf, formatStringLiteral) {
  EXPECT_EQ('\\' | 'n' << 8, cescapec('\n'));
  EXPECT_EQ('\\' | '3' << 8 | '7' << 16 | '7' << 24, cescapec('\377'));
  EXPECT_STREQ("\"hi\\n\"", Format("%`'s", "hi\n"));
  EXPECT_STREQ("\"\\000\"", Format("%`'.*s", 1, "\0"));
}

TEST(palandprintf, precisionStillRespectsNulTerminatorIfNotEscOrRepr) {
  EXPECT_STREQ("Makefile - 25 lines ",
               Format("%.20s - %d lines %s", "Makefile", 25, ""));
}

BENCH(palandprintf, bench) {
  EZBENCH2("ascii %s", donothing, Format("%s", VEIL("r", "hiuhcreohucreo")));
  EZBENCH2("utf8 %s", donothing, Format("%s", VEIL("r", "hi (╯°□°)╯")));
  EZBENCH2("snprintf %hs", donothing, Format("%hs", VEIL("r", u"hi (╯°□°)╯")));
  EZBENCH2("snprintf %ls", donothing, Format("%ls", VEIL("r", L"hi (╯°□°)╯")));
  EZBENCH2("23 %x", donothing, Format("%x", VEIL("r", 23)));
  EZBENCH2("23 %d", donothing, Format("%d", VEIL("r", 23)));
  EZBENCH2("INT_MIN %x", donothing, Format("%x", VEIL("r", INT_MIN)));
  EZBENCH2("INT_MIN %d", donothing, Format("%d", VEIL("r", INT_MIN)));
  EZBENCH2("23 int64toarray", donothing, int64toarray_radix10(23, buffer));
  EZBENCH2("INT_MIN int64toarray", donothing,
           int64toarray_radix10(INT_MIN, buffer));
}
