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

#include "string_view.h"

#include <stdckdint.h>
#include <string.h>

#include "string.h"

namespace ctl {

size_t
string_view::find(const char ch, const size_t pos) const noexcept
{
    char* q;
    if (n && (q = (char*)memchr(p, ch, n)))
        return q - p;
    return npos;
}

size_t
string_view::find(const string_view s, const size_t pos) const noexcept
{
    char* q;
    if (pos > n)
        __builtin_trap();
    if ((q = (char*)memmem(p + pos, n - pos, s.p, s.n)))
        return q - p;
    return npos;
}

string_view
string_view::substr(const size_t pos, size_t count) const noexcept
{
    size_t last;
    if (pos > n)
        __builtin_trap();
    if (count > n - pos)
        count = n - pos;
    if (ckd_add(&last, pos, count))
        last = n;
    if (last > n)
        __builtin_trap();
    return string_view(p + pos, count);
}

bool
string_view::operator==(const string_view s) const noexcept
{
    if (n != s.n)
        return false;
    if (!n)
        return true;
    return !memcmp(p, s.p, n);
}

bool
string_view::operator!=(const string_view s) const noexcept
{
    if (n != s.n)
        return true;
    if (!n)
        return false;
    return !!memcmp(p, s.p, n);
}

bool
string_view::contains(const string_view s) const noexcept
{
    if (!s.n)
        return true;
    return !!memmem(p, n, s.p, s.n);
}

bool
string_view::ends_with(const string_view s) const noexcept
{
    if (n < s.n)
        return false;
    if (!s.n)
        return true;
    return !memcmp(p + n - s.n, s.p, s.n);
}

bool
string_view::starts_with(const string_view s) const noexcept
{
    if (n < s.n)
        return false;
    if (!s.n)
        return true;
    return !memcmp(p, s.p, s.n);
}

} // namespace ctl
