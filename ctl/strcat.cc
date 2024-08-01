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

#include "string.h"

#include <stdckdint.h>
#include <string.h>

namespace ctl {

string
strcat(const string_view lhs, const string_view rhs) noexcept
{
    string res;
    size_t need;
    if (ckd_add(&need, lhs.n, rhs.n))
        __builtin_trap();
    if (ckd_add(&need, need, 1))
        __builtin_trap();
    res.reserve(need);
    if (lhs.n)
        memcpy(res.data(), lhs.p, lhs.n);
    if (rhs.n)
        memcpy(res.data() + lhs.n, rhs.p, rhs.n);
    if (res.isbig()) {
        res.__b.n = lhs.n + rhs.n;
    } else {
        res.__s.rem = __::sso_max - lhs.n - rhs.n;
    }
    res.data()[res.size()] = 0;
    return res;
}

} // namespace ctl
