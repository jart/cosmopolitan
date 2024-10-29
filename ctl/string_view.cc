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

size_t
string_view::find_last_of(char c, size_t pos) const noexcept
{
    const char* b = data();
    size_t n = size();
    if (pos > n)
        pos = n;
    const char* p = (const char*)memrchr(b, c, pos);
    return p ? p - b : npos;
}

size_t
string_view::find_last_of(ctl::string_view set, size_t pos) const noexcept
{
    if (empty() || set.empty())
        return npos;
    bool lut[256] = {};
    for (char c : set)
        lut[c & 255] = true;
    const char* b = data();
    size_t last = size() - 1;
    if (pos > last)
        pos = last;
    for (;;) {
        if (lut[b[pos] & 255])
            return pos;
        if (!pos)
            return npos;
        --pos;
    }
}

size_t
string_view::find_first_of(char c, size_t pos) const noexcept
{
    size_t n = size();
    if (pos >= n)
        return npos;
    const char* b = data();
    const char* p = (const char*)memchr(b + pos, c, n - pos);
    return p ? p - b : npos;
}

size_t
string_view::find_first_of(ctl::string_view set, size_t pos) const noexcept
{
    if (set.empty())
        return npos;
    bool lut[256] = {};
    for (char c : set)
        lut[c & 255] = true;
    const char* b = data();
    size_t n = size();
    for (;;) {
        if (pos >= n)
            return npos;
        if (lut[b[pos] & 255])
            return pos;
        ++pos;
    }
}

} // namespace ctl
