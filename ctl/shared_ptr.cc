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

#include "shared_ptr.h"

#include "libc/intrin/atomic.h"

namespace {

inline void
incref(_Atomic(size_t)* r)
{
    size_t r2 = atomic_fetch_add_explicit(r, 1, memory_order_relaxed);
    if (r2 > ((size_t)-1) >> 1)
        __builtin_trap();
}

inline int
decref(_Atomic(size_t)* r)
{
    if (!atomic_fetch_sub_explicit(r, 1, memory_order_release)) {
        atomic_thread_fence(memory_order_acquire);
        return 1;
    }
    return 0;
}

inline size_t
getref(const _Atomic(size_t)* r)
{
    return atomic_load_explicit(r, memory_order_relaxed);
}

} // namespace

namespace ctl {

namespace __ {

void
shared_control::keep_shared() noexcept
{
    incref(&shared);
}

void
shared_control::drop_shared() noexcept
{
    if (decref(&shared)) {
        on_zero_shared();
        drop_weak();
    }
}

void
shared_control::keep_weak() noexcept
{
    incref(&weak);
}

void
shared_control::drop_weak() noexcept
{
    if (decref(&weak))
        on_zero_weak();
}

size_t
shared_control::use_count() const noexcept
{
    return 1 + getref(&shared);
}

size_t
shared_control::weak_count() const noexcept
{
    return getref(&weak);
}

} // namespace __

} // namespace ctl
