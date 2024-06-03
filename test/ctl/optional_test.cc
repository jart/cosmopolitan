// -*- mode:c++;indent-tabs-mode:nil;c-basic-offset:4;coding:utf-8 -*-
// vi: set et ft=cpp ts=4 sts=4 sw=4 fenc=utf-8 :vi
//
// Copyright 2024 Mozilla Foundation
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "ctl/optional.h"

#include <new>

#include "ctl/string.h"

int
main()
{

    {
        Optional<int> x;
        if (x)
            return 1;
        if (x.has_value())
            return 2;
    }

    {
        Optional<int> x(42);
        if (!x)
            return 3;
        if (!x.has_value())
            return 4;
        if (x.value() != 42)
            return 5;
    }

    {
        Optional<String> x("hello");
        Optional<String> y(x);
        if (!y)
            return 6;
        if (!y.has_value())
            return 7;
        if (y.value() != "hello")
            return 8;
    }

    {
        Optional<String> x("world");
        Optional<String> y(std::move(x));
        if (!y)
            return 9;
        if (!y.has_value())
            return 10;
        if (y.value() != "world")
            return 11;
    }

    {
        Optional<int> x(42);
        Optional<int> y;
        y = x;
        if (!y)
            return 13;
        if (!y.has_value())
            return 14;
        if (y.value() != 42)
            return 15;
    }

    {
        Optional<String> x("hello");
        Optional<String> y;
        y = std::move(x);
        if (!y)
            return 16;
        if (!y.has_value())
            return 17;
        if (y.value() != "hello")
            return 18;
    }

    {
        Optional<int> x(42);
        x.reset();
        if (x)
            return 20;
        if (x.has_value())
            return 21;
    }

    {
        Optional<String> x;
        x.emplace("hello");
        if (!x)
            return 22;
        if (!x.has_value())
            return 23;
        if (x.value() != "hello")
            return 24;
    }

    CheckForMemoryLeaks();
    return 0;
}
