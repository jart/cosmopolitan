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

#include "dubble.h"
#include "libc/fmt/itoa.h"
#include "libc/math.h"
#include "string.h"
#include "third_party/gdtoa/gdtoa.h"

namespace ctl {

extern const double_conversion::DoubleToStringConverter kDoubleToPrintfG;

string
to_string(int value) noexcept
{
    char buf[12];
    return { buf, FormatInt32(buf, value) - buf };
}

string
to_string(unsigned value) noexcept
{
    char buf[12];
    return { buf, FormatUint32(buf, value) - buf };
}

string
to_string(long value) noexcept
{
    char buf[21];
    return { buf, FormatInt64(buf, value) - buf };
}

string
to_string(unsigned long value) noexcept
{
    char buf[21];
    return { buf, FormatUint64(buf, value) - buf };
}

string
to_string(long long value) noexcept
{
    char buf[21];
    return { buf, FormatInt64(buf, value) - buf };
}

string
to_string(unsigned long long value) noexcept
{
    char buf[21];
    return { buf, FormatUint64(buf, value) - buf };
}

string
to_string(float value) noexcept
{
    char buf[128];
    double_conversion::StringBuilder b(buf, sizeof(buf));
    kDoubleToPrintfG.ToShortestSingle(value, &b);
    b.Finalize();
    return string(buf);
}

string
to_string(double value) noexcept
{
    char buf[128];
    double_conversion::StringBuilder b(buf, sizeof(buf));
    kDoubleToPrintfG.ToShortest(value, &b);
    b.Finalize();
    return string(buf);
}

string
to_string(long double value) noexcept
{
#if LDBL_MANT_DIG == 53 && LDBL_MAX_EXP == 1024
    return to_string((double)value);
#else
    char buf[128];
#if LDBL_MANT_DIG == 113
    g_Qfmt_p(buf, &value, 16, 128, NIK(2, 0, 0));
#elif LDBL_MANT_DIG == 64
    g_xfmt_p(buf, &value, 16, 128, NIK(2, 0, 0));
#else
#error "unsupported long double"
#endif
    return string(buf);
#endif
}

} // namespace ctl
