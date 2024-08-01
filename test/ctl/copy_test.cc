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

#include "ctl/array.h"
#include "ctl/copy.h"
#include "libc/mem/leaks.h"

// #include <algorithm>
// #include <array>
// #include <iterator>
// #define ctl std

int
main()
{

    {
        ctl::array<int, 5> src = { 1, 2, 3, 4, 5 };
        ctl::array<int, 5> dest = { 0, 0, 0, 0, 0 };

        // Test basic copy
        ctl::copy(src.begin(), src.end(), dest.begin());
        for (size_t i = 0; i < 5; ++i)
            if (dest[i] != src[i])
                return 1;

        // Test partial copy
        ctl::array<int, 5> dest2 = { 0, 0, 0, 0, 0 };
        ctl::copy(src.begin(), src.begin() + 3, dest2.begin());
        if (dest2[0] != 1 || dest2[1] != 2 || dest2[2] != 3 || dest2[3] != 0 ||
            dest2[4] != 0)
            return 2;

        // Test copy to middle of destination
        ctl::array<int, 7> dest3 = { 0, 0, 0, 0, 0, 0, 0 };
        ctl::copy(src.begin(), src.end(), dest3.begin() + 1);
        if (dest3[0] != 0 || dest3[1] != 1 || dest3[2] != 2 || dest3[3] != 3 ||
            dest3[4] != 4 || dest3[5] != 5 || dest3[6] != 0)
            return 3;

        // Test copy with empty range
        ctl::array<int, 5> dest4 = { 0, 0, 0, 0, 0 };
        ctl::copy(src.begin(), src.begin(), dest4.begin());
        for (size_t i = 0; i < 5; ++i)
            if (dest4[i] != 0)
                return 4;

        // Test copy return value
        ctl::array<int, 5> dest5 = { 0, 0, 0, 0, 0 };
        auto result = ctl::copy(src.begin(), src.end(), dest5.begin());
        if (result != dest5.end())
            return 5;
    }

    CheckForMemoryLeaks();
}
