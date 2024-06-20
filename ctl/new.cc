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

// clang-format off

extern "C" {

void* ctl_alloc(size_t n, size_t a)
{
    void* p;
    if (!(p = memalign(a, n)))
        __builtin_trap();
    return p;
}

void* ctl_alloc1(size_t n)
{
    return ctl_alloc(n, 1);
}

void* ctl_ret(size_t, void* p)
{
    return p;
}

/* The ISO says that these should be replaceable by user code. It also says
   that the declarations for the first four (i.e. non–placement-) operators
   new are implicitly available in each translation unit, including the std
   align_val_t parameter. (?) However, <new> also _defines_ the align_val_t
   type so you can’t just write your own. Our way through this morass is to
   supply ours as ctl::align_val_t and not implicitly declare anything, for
   now. If you have any brain cells left after reading this comment then go
   look at the ten operator delete weak references to free in the below. */

// operator new(size_t, align_val_t)
__weak_reference(ctl_alloc, _ZnwmN3ctl11align_val_tE);

// operator new[](size_t, align_val_t)
__weak_reference(ctl_alloc, _ZnamN3ctl11align_val_tE);

// operator new(size_t)
__weak_reference(ctl_alloc1, _Znwm);

// operator new[](size_t)
__weak_reference(ctl_alloc1, _Znam);

// operator new(size_t, void*)
__weak_reference(ctl_ret, _ZnwmPv);

// operator new[](size_t, void*)
__weak_reference(ctl_ret, _ZnamPv);

// operator delete(void*)
__weak_reference(free, _ZdlPv);

// operator delete[](void*)
__weak_reference(free, _ZdaPv);

// operator delete(void*, align_val_t)
__weak_reference(free, _ZdlPvN3ctl11align_val_tE);

// operator delete[](void*, align_val_t)
__weak_reference(free, _ZdaPvN3ctl11align_val_tE);

// operator delete(void*, size_t)
__weak_reference(free, _ZdlPvm);

// operator delete[](void*, size_t)
__weak_reference(free, _ZdaPvm);

// operator delete(void*, size_t, align_val_t)
__weak_reference(free, _ZdlPvmN3ctl11align_val_tE);

// operator delete[](void*, size_t, align_val_t)
__weak_reference(free, _ZdaPvmN3ctl11align_val_tE);

// operator delete(void*, void*) noexcept
__weak_reference(free, _ZdlPvS_);

// operator delete[](void*, void*)
__weak_reference(free, _ZdaPvS_);

} // extern "C"
