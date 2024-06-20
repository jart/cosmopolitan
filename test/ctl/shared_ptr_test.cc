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

#include "ctl/shared_ptr.h"

#include "libc/runtime/runtime.h"

// #include <memory>
// #define ctl std

template<typename T>
using Ptr = ctl::shared_ptr<T>;

template<typename T, typename... Args>
Ptr<T>
Mk(Args&&... args)
{
    return ctl::make_shared<T>(ctl::forward<Args>(args)...);
}

#undef ctl

int
main()
{
    {
        Ptr<int> x;
    }

    {
        Ptr<int> x(new int());
        if (x.use_count() != 1)
            return 1;
        Ptr<int> y(x);
        if (x.use_count() != 2 || y.use_count() != 2)
            return 2;
        x.reset();
        if (x.use_count() || y.use_count() != 1)
            return 3;
    }

    {
        Ptr<int> x(new int());
        x.reset(new int(2));
        if (x.use_count() != 1)
            return 5;
    }

    // TODO(mrdomino):
#if 0
    {
        Ptr<int> x(new int);
        Ptr<void> y(x, nullptr);
        if (x.use_count() != 2)
            return 7;
    }
#endif

    {
        auto x = Mk<int>(5);
        if (x.use_count() != 1)
            return 8;
    }

    // TODO(mrdomino): exercise more of API
    // TODO(mrdomino): threading stress-test

    CheckForMemoryLeaks();
    return 0;
}
