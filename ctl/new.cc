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

#include "new.h"

#include "libc/mem/mem.h"

using ctl::align_val_t;

namespace {

constexpr auto a1 = align_val_t(1);

} // namespace

void*
operator new(size_t n, align_val_t a)
{
    void* p;
    if (!(p = memalign(static_cast<size_t>(a), n))) {
        __builtin_trap();
    }
    return p;
}

void*
operator new[](size_t n, align_val_t a)
{
    return operator new(n, a);
}
void*
operator new(size_t n)
{
    return operator new(n, a1);
}
void*
operator new[](size_t n)
{
    return operator new(n, a1);
}

void*
operator new(size_t, void* p)
{
    return p;
}
void*
operator new[](size_t, void* p)
{
    return p;
}

void
operator delete(void* p) noexcept
{
    free(p);
}
void
operator delete[](void* p) noexcept
{
    free(p);
}
void
operator delete(void* p, align_val_t) noexcept
{
    free(p);
}
void
operator delete[](void* p, align_val_t) noexcept
{
    free(p);
}
void
operator delete(void* p, size_t) noexcept
{
    free(p);
}
void
operator delete[](void* p, size_t) noexcept
{
    free(p);
}
void
operator delete(void* p, size_t, align_val_t) noexcept
{
    free(p);
}
void
operator delete[](void* p, size_t, align_val_t) noexcept
{
    free(p);
}

void
operator delete(void*, void*) noexcept
{
}
void
operator delete[](void*, void*) noexcept
{
}
