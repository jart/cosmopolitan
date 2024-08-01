// -*- mode:c++; indent-tabs-mode:nil; c-basic-offset:4; coding:utf-8 -*-
// vi: set et ft=cpp ts=4 sts=4 sw=4 fenc=utf-8 :vi
//
// Copyright 2024 Justine Alexandra Roberts Tunney
//
// Permission to use, copy, modify, and/or distribute this software for
// any purpose with or without fee is hereby granted, provided that the
// above copyright notice and this permission notice appear in all copies.
//
// THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL
// WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE
// AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
// DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
// PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
// TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
// PERFORMANCE OF THIS SOFTWARE.

#include "ctl/string.h"
#include "libc/limits.h"
#include "libc/math.h"
#include "libc/mem/leaks.h"

int
main()
{

    // test integer conversion
    {
        if (ctl::to_string(0) != "0")
            return 1;
        if (ctl::to_string(3) != "3")
            return 1;
        if (ctl::to_string(INT32_MAX) != "2147483647")
            return 2;
        if (ctl::to_string(INT32_MIN) != "-2147483648")
            return 3;
        if (ctl::to_string(UINT32_MAX) != "4294967295")
            return 4;
        if (ctl::to_string(INT64_MAX) != "9223372036854775807")
            return 5;
        if (ctl::to_string(INT64_MIN) != "-9223372036854775808")
            return 6;
        if (ctl::to_string(UINT64_MAX) != "18446744073709551615")
            return 7;
    }

    // test float conversion
    // we diverge from std::to_string(float) because it's garbage
    {
        if (ctl::to_string(0.f) != "0") // 0.000000
            return 8;
        if (ctl::to_string(-0.f) != "-0") // 0.000000
            return 9;
        if (ctl::to_string(3.f) != "3") // 3.000000
            return 10;
        if (ctl::to_string(3.14f) != "3.14") // 3.140000
            return 11;
        if (ctl::to_string(3.140001f) != "3.140001")
            return 12;
        if (ctl::to_string(1000000000.f) != "1000000000") // 1000000000.000000
            return 12;
        if (ctl::to_string(10000000000.f) != "1e+10") // 10000000000.000000
            return 12;
        if (ctl::to_string(NAN) != "nan")
            return 13;
        if (ctl::to_string(INFINITY) != "inf")
            return 14;
        if (ctl::to_string(-INFINITY) != "-inf")
            return 15;
        if (ctl::to_string(FLT_MIN) != "1.1754944e-38") // 0.000000 lool
            return 16;
        if (ctl::to_string(-FLT_MIN) != "-1.1754944e-38")
            return 17;
        if (ctl::to_string(FLT_MAX) != "3.4028235e+38")
            return 18;
        if (ctl::to_string(-FLT_MAX) != "-3.4028235e+38")
            return 19;
    }

    // test double conversion
    {
        if (ctl::to_string(0.) != "0")
            return 20;
        if (ctl::to_string(-0.) != "-0")
            return 21;
        if (ctl::to_string(3.) != "3")
            return 22;
        if (ctl::to_string(2147483647.) != "2147483647")
            return 23;
        if (ctl::to_string(-2147483648.) != "-2147483648")
            return 23;
        if (ctl::to_string(10000000000.) != "1e+10")
            return 23;
        if (ctl::to_string(3.14) != "3.14")
            return 23;
        if (ctl::to_string(3.140001) != "3.140001")
            return 24;
        if (ctl::to_string(DBL_MIN) != "2.2250738585072014e-308")
            return 25;
        if (ctl::to_string(-DBL_MIN) != "-2.2250738585072014e-308")
            return 26;
        if (ctl::to_string(DBL_MAX) != "1.7976931348623157e+308")
            return 27;
        if (ctl::to_string(-DBL_MAX) != "-1.7976931348623157e+308")
            return 28;
    }

    // test long double conversion
    {
        if (ctl::to_string(0.L) != "0")
            return 29;
        if (ctl::to_string(-0.L) != "-0")
            return 30;
        if (ctl::to_string(3.L) != "3")
            return 31;
        if (ctl::to_string(3.14L) != "3.14")
            return 32;
#if LDBL_MANT_DIG > 64
        if (ctl::to_string(LDBL_MAX) != "1.189731495357232e+4932")
            return 33;
        if (ctl::to_string(-LDBL_MAX) != "-1.189731495357232e+4932")
            return 34;
#endif
    }

    CheckForMemoryLeaks();
}
