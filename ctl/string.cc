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

String::~String() noexcept
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

String::String(const char* s) noexcept
{
    append(s, strlen(s));
}

String::String(const String& s) noexcept
{
    append(s.p, s.n);
}

String::String(const StringView s) noexcept
{
    append(s.p, s.n);
}

String::String(size_t size, char ch) noexcept
{
    resize(size, ch);
}

String::String(const char* s, size_t size) noexcept
{
    append(s, size);
}

const char*
String::c_str() const noexcept
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
String::reserve(size_t c2) noexcept
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
String::resize(size_t n2, char ch) noexcept
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
String::append(char ch) noexcept
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
String::grow(size_t size) noexcept
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
String::append(char ch, size_t size) noexcept
{
    grow(size);
    if (size)
        memset(p + n, ch, size);
    p[n += size] = 0;
}

void
String::append(const void* data, size_t size) noexcept
{
    grow(size);
    if (size)
        memcpy(p + n, data, size);
    p[n += size] = 0;
}

void
String::pop_back() noexcept
{
    if (!n)
        __builtin_trap();
    p[--n] = 0;
}

String&
String::operator=(String&& s) noexcept
{
    if (p != s.p) {
        free(p);
        p = s.p;
        n = s.n;
        c = s.c;
        s.p = nullptr;
        s.n = 0;
        s.c = 0;
    }
    return *this;
}

static String
StrCat(const StringView lhs, const StringView rhs) noexcept
{
    String res;
    size_t need;
    if (ckd_add(&need, lhs.n, rhs.n))
        __builtin_trap();
    if (ckd_add(&need, need, 1))
        __builtin_trap();
    res.reserve(need);
    if (lhs.n)
        memcpy(res.p, lhs.p, lhs.n);
    if (rhs.n)
        memcpy(res.p + lhs.n, rhs.p, rhs.n);
    res.p[res.n = lhs.n + rhs.n] = 0;
    return res;
}

String
StringView::operator+(const StringView s) const noexcept
{
    return StrCat(*this, s);
}

String
String::operator+(const StringView s) const noexcept
{
    return StrCat(*this, s);
}

bool
String::operator==(const StringView s) const noexcept
{
    if (n != s.n)
        return false;
    return !memcmp(p, s.p, n);
}

bool
String::operator!=(const StringView s) const noexcept
{
    if (n != s.n)
        return true;
    return !!memcmp(p, s.p, n);
}

bool
String::contains(const StringView s) const noexcept
{
    if (!s.n)
        return true;
    return !!memmem(p, n, s.p, s.n);
}

bool
String::ends_with(const StringView s) const noexcept
{
    if (n < s.n)
        return false;
    return !memcmp(p + n - s.n, s.p, s.n);
}

bool
String::starts_with(const StringView s) const noexcept
{
    if (n < s.n)
        return false;
    return !memcmp(p, s.p, s.n);
}

static int
StrCmp(const StringView lhs, const StringView rhs) noexcept
{
    int r;
    size_t m = lhs.n;
    if ((m = rhs.n < m ? rhs.n : m))
        if ((r = memcmp(lhs.p, rhs.p, m)))
            return r;
    if (lhs.n == rhs.n)
        return 0;
    if (m < lhs.n)
        return +1;
    return -1;
}

int
String::compare(const StringView s) const noexcept
{
    return StrCmp(*this, s);
}

size_t
String::find(char ch, size_t pos) const noexcept
{
    char* q;
    if ((q = (char*)memchr(p, ch, n)))
        return q - p;
    return npos;
}

size_t
String::find(const StringView s, size_t pos) const noexcept
{
    char* q;
    if (pos > n)
        __builtin_trap();
    if ((q = (char*)memmem(p + pos, n - pos, s.p, s.n)))
        return q - p;
    return npos;
}

String
String::substr(size_t pos, size_t count) const noexcept
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
    return String(p + pos, count);
}

String&
String::replace(size_t pos, size_t count, const StringView& s) noexcept
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

int
StringView::compare(const StringView s) const noexcept
{
    return StrCmp(*this, s);
}

size_t
StringView::find(char ch, size_t pos) const noexcept
{
    char* q;
    if (n && (q = (char*)memchr(p, ch, n)))
        return q - p;
    return npos;
}

size_t
StringView::find(const StringView s, size_t pos) const noexcept
{
    char* q;
    if (pos > n)
        __builtin_trap();
    if ((q = (char*)memmem(p + pos, n - pos, s.p, s.n)))
        return q - p;
    return npos;
}

StringView
StringView::substr(size_t pos, size_t count) const noexcept
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
    return StringView(p + pos, count);
}

String&
String::insert(size_t i, const StringView s) noexcept
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

String&
String::erase(size_t pos, size_t count) noexcept
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

bool
StringView::operator==(const StringView s) const noexcept
{
    if (n == s.n)
        return true;
    return !memcmp(p, s.p, n);
}

bool
StringView::operator!=(const StringView s) const noexcept
{
    if (n != s.n)
        return true;
    return !!memcmp(p, s.p, n);
}

bool
StringView::contains(const StringView s) const noexcept
{
    if (!s.n)
        return true;
    return !!memmem(p, n, s.p, s.n);
}

bool
StringView::ends_with(const StringView s) const noexcept
{
    if (n < s.n)
        return false;
    return !memcmp(p + n - s.n, s.p, s.n);
}

bool
StringView::starts_with(const StringView s) const noexcept
{
    if (n < s.n)
        return false;
    return !memcmp(p, s.p, s.n);
}
