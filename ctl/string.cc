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

#include <__atomic/fence.h>
#include <stdckdint.h>

namespace ctl {

string::~string() noexcept
{
    if (isbig()) {
        auto* b = big();
        if (b->n) {
            if (b->n >= b->c)
                __builtin_trap();
            if (b->p[b->n])
                __builtin_trap();
        }
        if (b->c && !b->p)
            __builtin_trap();
        free(b->p);
    }
}

string::string(const char* s) noexcept : string()
{
    append(s, strlen(s));
}

string::string(const string& s) noexcept : string()
{
    append(s.data(), s.size());
}

string::string(const string_view s) noexcept : string()
{
    append(s.p, s.n);
}

string::string(size_t size, char ch) noexcept : string()
{
    resize(size, ch);
}

string::string(const char* s, size_t size) noexcept : string()
{
    append(s, size);
}

const char*
string::c_str() const noexcept
{
    if (!size())
        return "";
    if (size() >= capacity())
        __builtin_trap();
    if (data()[size()])
        __builtin_trap();
    return data();
}

void
string::reserve(size_t c2) noexcept
{
    char* p2;
    size_t n = size();
    if (c2 < n + 1)
        c2 = n + 1;
    if (c2 <= __::string_size)
        return;
    if (ckd_add(&c2, c2, 15))
        __builtin_trap();
    c2 &= -16;
    if (!isbig()) {
        if (!(p2 = (char*)malloc(c2)))
            __builtin_trap();
        memcpy(p2, data(), size());
        p2[size()] = 0;
    } else {
        if (!(p2 = (char*)realloc(big()->p, c2)))
            __builtin_trap();
    }
    std::atomic_signal_fence(std::memory_order_seq_cst);
    set_big_capacity(c2);
    big()->n = n;
    big()->p = p2;
}

void
string::resize(size_t n2, char ch) noexcept
{
    size_t c2;
    if (ckd_add(&c2, n2, 1))
        __builtin_trap();
    reserve(c2);
    if (n2 > size())
        memset(data() + size(), ch, n2 - size());
    if (isbig()) {
        big()->p[big()->n = n2] = 0;
    } else {
        set_small_size(n2);
        data()[size()] = 0;
    }
}

void
string::append(char ch) noexcept
{
    size_t n2;
    if (ckd_add(&n2, size(), 2))
        __builtin_trap();
    if (n2 > capacity()) {
        size_t c2 = capacity();
        if (ckd_add(&c2, c2, c2 >> 1))
            __builtin_trap();
        reserve(c2);
    }
    data()[size()] = ch;
    if (isbig()) {
        ++big()->n;
    } else {
        --small()->rem;
    }
    data()[size()] = 0;
}

void
string::grow(size_t size) noexcept
{
    size_t need;
    if (ckd_add(&need, this->size(), size))
        __builtin_trap();
    if (ckd_add(&need, need, 1))
        __builtin_trap();
    if (need <= capacity())
        return;
    size_t c2 = capacity();
    if (!c2)
        __builtin_trap();
    while (c2 < need)
        if (ckd_add(&c2, c2, c2 >> 1))
            __builtin_trap();
    reserve(c2);
}

void
string::append(char ch, size_t size) noexcept
{
    grow(size);
    if (size)
        memset(data() + this->size(), ch, size);
    if (isbig()) {
        big()->n += size;
    } else {
        small()->rem -= size;
    }
    data()[this->size()] = 0;
}

void
string::append(const void* data, size_t size) noexcept
{
    grow(size);
    if (size)
        memcpy(this->data() + this->size(), data, size);
    if (isbig()) {
        big()->n += size;
    } else {
        small()->rem -= size;
    }
    this->data()[this->size()] = 0;
}

void
string::pop_back() noexcept
{
    if (!size())
        __builtin_trap();
    if (isbig()) {
        --big()->n;
    } else {
        ++small()->rem;
    }
    data()[size()] = 0;
}

string&
string::operator=(string s) noexcept
{
    swap(s);
    return *this;
}

bool
string::operator==(const string_view s) const noexcept
{
    if (size() != s.n)
        return false;
    if (!s.n)
        return true;
    return !memcmp(data(), s.p, s.n);
}

bool
string::operator!=(const string_view s) const noexcept
{
    if (size() != s.n)
        return true;
    if (!s.n)
        return false;
    return !!memcmp(data(), s.p, s.n);
}

bool
string::contains(const string_view s) const noexcept
{
    if (!s.n)
        return true;
    return !!memmem(data(), size(), s.p, s.n);
}

bool
string::ends_with(const string_view s) const noexcept
{
    if (size() < s.n)
        return false;
    if (!s.n)
        return true;
    return !memcmp(data() + size() - s.n, s.p, s.n);
}

bool
string::starts_with(const string_view s) const noexcept
{
    if (size() < s.n)
        return false;
    if (!s.n)
        return true;
    return !memcmp(data(), s.p, s.n);
}

size_t
string::find(char ch, size_t pos) const noexcept
{
    char* q;
    if ((q = (char*)memchr(data(), ch, size())))
        return q - data();
    return npos;
}

size_t
string::find(const string_view s, size_t pos) const noexcept
{
    char* q;
    if (pos > size())
        __builtin_trap();
    if ((q = (char*)memmem(data() + pos, size() - pos, s.p, s.n)))
        return q - data();
    return npos;
}

string
string::substr(size_t pos, size_t count) const noexcept
{
    size_t last;
    if (pos > size())
        __builtin_trap();
    if (count > size() - pos)
        count = size() - pos;
    if (ckd_add(&last, pos, count))
        last = size();
    if (last > size())
        __builtin_trap();
    return string(data() + pos, count);
}

string&
string::replace(size_t pos, size_t count, const string_view& s) noexcept
{
    size_t last;
    if (ckd_add(&last, pos, count))
        __builtin_trap();
    if (last > size())
        __builtin_trap();
    size_t need;
    if (ckd_add(&need, pos, s.n))
        __builtin_trap();
    size_t extra = size() - last;
    if (ckd_add(&need, need, extra))
        __builtin_trap();
    size_t c2;
    if (ckd_add(&c2, need, 1))
        __builtin_trap();
    reserve(c2);
    if (extra)
        memmove(data() + pos + s.n, data() + last, extra);
    memcpy(data() + pos, s.p, s.n);
    if (isbig()) {
        big()->p[big()->n = need] = 0;
    } else {
        set_small_size(need);
        data()[size()] = 0;
    }
    return *this;
}

string&
string::insert(size_t i, const string_view s) noexcept
{
    if (i > size())
        __builtin_trap();
    size_t extra = size() - i;
    size_t need;
    if (ckd_add(&need, size(), s.n))
        __builtin_trap();
    if (ckd_add(&need, need, 1))
        __builtin_trap();
    reserve(need);
    if (extra)
        memmove(data() + i + s.n, data() + i, extra);
    memcpy(data() + i, s.p, s.n);
    if (isbig()) {
        big()->n += s.n;
    } else {
        small()->rem -= s.n;
    }
    data()[size()] = 0;
    return *this;
}

string&
string::erase(size_t pos, size_t count) noexcept
{
    if (pos > size())
        __builtin_trap();
    if (count > size() - pos)
        count = size() - pos;
    size_t extra = size() - (pos + count);
    if (extra)
        memmove(data() + pos, data() + pos + count, extra);
    if (isbig()) {
        big()->n = pos + extra;
    } else {
        set_small_size(pos + extra);
    }
    data()[size()] = 0;
    return *this;
}

} // namespace ctl
