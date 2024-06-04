// -*- mode:c++; indent-tabs-mode:nil; c-basic-offset:4; coding:utf-8 -*-
// vi: set et ft=c++ ts=4 sts=4 sw=4 fenc=utf-8
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

#include <__atomic/fence.h>
#include <stdckdint.h>

namespace ctl {

string::~string() noexcept
{
    if (n) {
        if (n >= c)
            __builtin_trap();
        if (p[n])
            __builtin_trap();
    }
    if (c && !p)
        __builtin_trap();
    free(p);
}

string::string(const char* s) noexcept
{
    append(s, strlen(s));
}

string::string(const string& s) noexcept
{
    append(s.p, s.n);
}

string::string(const string_view s) noexcept
{
    append(s.p, s.n);
}

string::string(size_t size, char ch) noexcept
{
    resize(size, ch);
}

string::string(const char* s, size_t size) noexcept
{
    append(s, size);
}

const char*
string::c_str() const noexcept
{
    if (!n)
        return "";
    if (n >= c)
        __builtin_trap();
    if (p[n])
        __builtin_trap();
    return p;
}

void
string::reserve(size_t c2) noexcept
{
    char* p2;
    if (c2 < n)
        c2 = n;
    if (ckd_add(&c2, c2, 15))
        __builtin_trap();
    c2 &= -16;
    if (!(p2 = (char*)realloc(p, c2)))
        __builtin_trap();
    std::atomic_signal_fence(std::memory_order_seq_cst);
    c = c2;
    p = p2;
}

void
string::resize(size_t n2, char ch) noexcept
{
    size_t c2;
    if (ckd_add(&c2, n2, 1))
        __builtin_trap();
    reserve(c2);
    if (n2 > n)
        memset(p + n, ch, n2 - n);
    p[n = n2] = 0;
}

void
string::append(char ch) noexcept
{
    if (n + 2 > c) {
        size_t c2 = c + 2;
        c2 += c2 >> 1;
        reserve(c2);
    }
    p[n++] = ch;
    p[n] = 0;
}

void
string::grow(size_t size) noexcept
{
    size_t need;
    if (ckd_add(&need, n, size))
        __builtin_trap();
    if (ckd_add(&need, need, 1))
        __builtin_trap();
    if (need <= c)
        return;
    size_t c2 = c;
    if (!c2) {
        c2 = need;
    } else {
        while (c2 < need)
            if (ckd_add(&c2, c2, c2 >> 1))
                __builtin_trap();
    }
    reserve(c2);
}

void
string::append(char ch, size_t size) noexcept
{
    grow(size);
    if (size)
        memset(p + n, ch, size);
    p[n += size] = 0;
}

void
string::append(const void* data, size_t size) noexcept
{
    grow(size);
    if (size)
        memcpy(p + n, data, size);
    p[n += size] = 0;
}

void
string::pop_back() noexcept
{
    if (!n)
        __builtin_trap();
    p[--n] = 0;
}

string&
string::operator=(string&& s) noexcept
{
    if (p != s.p) {
        if (p) {
            clear();
            append(s.p, s.n);
        } else {
            p = s.p;
            n = s.n;
            c = s.c;
            s.p = nullptr;
            s.n = 0;
            s.c = 0;
        }
    }
    return *this;
}

bool
string::operator==(const string_view s) const noexcept
{
    if (n != s.n)
        return false;
    if (!n)
        return true;
    return !memcmp(p, s.p, n);
}

bool
string::operator!=(const string_view s) const noexcept
{
    if (n != s.n)
        return true;
    if (!n)
        return false;
    return !!memcmp(p, s.p, n);
}

bool
string::contains(const string_view s) const noexcept
{
    if (!s.n)
        return true;
    return !!memmem(p, n, s.p, s.n);
}

bool
string::ends_with(const string_view s) const noexcept
{
    if (n < s.n)
        return false;
    if (!s.n)
        return true;
    return !memcmp(p + n - s.n, s.p, s.n);
}

bool
string::starts_with(const string_view s) const noexcept
{
    if (n < s.n)
        return false;
    if (!s.n)
        return true;
    return !memcmp(p, s.p, s.n);
}

size_t
string::find(char ch, size_t pos) const noexcept
{
    char* q;
    if ((q = (char*)memchr(p, ch, n)))
        return q - p;
    return npos;
}

size_t
string::find(const string_view s, size_t pos) const noexcept
{
    char* q;
    if (pos > n)
        __builtin_trap();
    if ((q = (char*)memmem(p + pos, n - pos, s.p, s.n)))
        return q - p;
    return npos;
}

string
string::substr(size_t pos, size_t count) const noexcept
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
    return string(p + pos, count);
}

string&
string::replace(size_t pos, size_t count, const string_view& s) noexcept
{
    size_t last;
    if (ckd_add(&last, pos, count))
        __builtin_trap();
    if (last > n)
        __builtin_trap();
    size_t need;
    if (ckd_add(&need, pos, s.n))
        __builtin_trap();
    size_t extra = n - last;
    if (ckd_add(&need, need, extra))
        __builtin_trap();
    size_t c2;
    if (ckd_add(&c2, need, 1))
        __builtin_trap();
    reserve(c2);
    if (extra)
        memmove(p + pos + s.n, p + last, extra);
    memcpy(p + pos, s.p, s.n);
    p[n = need] = 0;
    return *this;
}

string&
string::insert(size_t i, const string_view s) noexcept
{
    if (i > n)
        __builtin_trap();
    size_t extra = n - i;
    size_t need;
    if (ckd_add(&need, n, s.n))
        __builtin_trap();
    if (ckd_add(&need, need, 1))
        __builtin_trap();
    reserve(need);
    if (extra)
        memmove(p + i + s.n, p + i, extra);
    memcpy(p + i, s.p, s.n);
    p[n += s.n] = 0;
    return *this;
}

string&
string::erase(size_t pos, size_t count) noexcept
{
    if (pos > n)
        __builtin_trap();
    if (count > n - pos)
        count = n - pos;
    size_t extra = n - (pos + count);
    if (extra)
        memmove(p + pos, p + pos + count, extra);
    p[n = pos + extra] = 0;
    return *this;
}

} // namespace ctl
