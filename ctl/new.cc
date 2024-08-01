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

COSMOPOLITAN_C_START_

static void*
_ctl_alloc(size_t n, size_t a) noexcept
{
    void* p;
    if (!(p = memalign(a, n)))
        __builtin_trap();
    return p;
}

static void*
_ctl_alloc_nothrow(size_t n, size_t a, const ctl::nothrow_t&)
{
    void* p;
    if (!(p = memalign(a, n)))
        __builtin_trap();
    return p;
}

static void*
_ctl_alloc1(size_t n) noexcept
{
    void* p;
    if (!(p = malloc(n)))
        __builtin_trap();
    return p;
}

static void*
_ctl_alloc1_nothrow(size_t n, const ctl::nothrow_t&) noexcept
{
    void* p;
    if (!(p = malloc(n)))
        __builtin_trap();
    return p;
}

static void*
_ctl_ret(size_t, void* p) noexcept
{
    return p;
}

static void
_ctl_free(void* p) noexcept
{
    free(p);
}

static void
_ctl_nop(void*, void*) noexcept
{
}

COSMOPOLITAN_C_END_

#undef __weak_reference
#define __weak_reference(P, A) P __attribute__((weak, alias(#A)))

/* The ISO says that these should be replaceable by user code. It also says
   that the declarations for the first four (i.e. non placement-) operators
   new are implicitly available in each translation unit, including the std
   align_val_t parameter. (?) However, <new> also _defines_ the align_val_t
   type so you can’t just write your own. Our way through this morass is to
   supply ours as ctl::align_val_t and not implicitly declare anything, for
   now. If you have any brain cells left after reading this comment then go
   look at the eight operator delete weak references to free in the below. */

__weak_reference(void*
                 operator new(size_t, ctl::align_val_t),
                 _ctl_alloc);

__weak_reference(void*
                 operator new(size_t,
                              ctl::align_val_t,
                              const ctl::nothrow_t&) noexcept,
                 _ctl_alloc_nothrow);

__weak_reference(void*
                 operator new[](size_t, ctl::align_val_t),
                 _ctl_alloc);

__weak_reference(void*
                 operator new[](size_t,
                                ctl::align_val_t,
                                const ctl::nothrow_t&) noexcept,
                 _ctl_alloc_nothrow);

__weak_reference(void*
                 operator new(size_t),
                 _ctl_alloc1);

__weak_reference(void*
                 operator new(size_t, const ctl::nothrow_t&) noexcept,
                 _ctl_alloc1_nothrow);

__weak_reference(void*
                 operator new[](size_t),
                 _ctl_alloc1);

__weak_reference(void*
                 operator new[](size_t, const ctl::nothrow_t&) noexcept,
                 _ctl_alloc1_nothrow);

// XXX clang-format currently mutilates these for some reason.
// clang-format off

__weak_reference(void* operator new(size_t, void*) noexcept, _ctl_ret);
__weak_reference(void* operator new[](size_t, void*) noexcept, _ctl_ret);

__weak_reference(void operator delete(void*) noexcept, _ctl_free);
__weak_reference(void operator delete[](void*) noexcept, _ctl_free);
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wattribute-alias="
__weak_reference(void operator delete(void*, ctl::align_val_t) noexcept,
                 _ctl_free);
__weak_reference(void operator delete[](void*, ctl::align_val_t) noexcept,
                 _ctl_free);
__weak_reference(void operator delete(void*, size_t) noexcept, _ctl_free);
__weak_reference(void operator delete[](void*, size_t) noexcept, _ctl_free);
__weak_reference(void operator delete(void*, size_t, ctl::align_val_t) noexcept,
                 _ctl_free);
__weak_reference(void operator delete[](void*, size_t, ctl::align_val_t)
                 noexcept, _ctl_free);
#pragma GCC diagnostic pop

__weak_reference(void operator delete(void*, void*) noexcept, _ctl_nop);
__weak_reference(void operator delete[](void*, void*) noexcept, _ctl_nop);
