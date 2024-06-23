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

#include "ctl/optional.h"
#include "libc/mem/leaks.h"

#include "ctl/string.h"

// #include <optional>
// #include <string>
// #define ctl std

static int g = 0;

int
main()
{

    {
        ctl::optional<int> x;
        if (x)
            return 1;
        if (x.has_value())
            return 2;
    }

    {
        ctl::optional<int> x(42);
        if (!x)
            return 3;
        if (!x.has_value())
            return 4;
        if (x.value() != 42)
            return 5;
    }

    {
        ctl::optional<ctl::string> x("hello");
        ctl::optional<ctl::string> y(x);
        if (!y)
            return 6;
        if (!y.has_value())
            return 7;
        if (y.value() != "hello")
            return 8;
    }

    {
        ctl::optional<ctl::string> x("world");
        ctl::optional<ctl::string> y(ctl::move(x));
        if (!y)
            return 9;
        if (!y.has_value())
            return 10;
        if (y.value() != "world")
            return 11;
    }

    {
        ctl::optional<int> x(42);
        ctl::optional<int> y;
        y = x;
        if (!y)
            return 13;
        if (!y.has_value())
            return 14;
        if (y.value() != 42)
            return 15;
    }

    {
        ctl::optional<ctl::string> x("hello");
        ctl::optional<ctl::string> y;
        y = ctl::move(x);
        if (!y)
            return 16;
        if (!y.has_value())
            return 17;
        if (y.value() != "hello")
            return 18;
    }

    {
        ctl::optional<int> x(42);
        x.reset();
        if (x)
            return 20;
        if (x.has_value())
            return 21;
    }

    {
        ctl::optional<ctl::string> x;
        x.emplace("hello");
        if (!x)
            return 22;
        if (!x.has_value())
            return 23;
        if (x.value() != "hello")
            return 24;
    }

    {
        struct A
        {
            int* p = &g;
            A()
            {
                ++*p;
            }
        };
        ctl::optional<A> x;
        if (g != 0)
            return 25;
    }

    CheckForMemoryLeaks();
}
