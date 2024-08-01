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
#include "ctl/back_inserter.h"
#include "ctl/copy.h"
#include "ctl/vector.h"
#include "libc/mem/leaks.h"

// #include <array>
// #include <iterator>
// #include <vector>
// #define ctl std

int
main()
{

    {
        ctl::vector<int> vec = { 1, 2, 3 };
        ctl::array<int, 3> arr = { 4, 5, 6 };

        // Use back_inserter to append elements from arr to vec
        ctl::copy(arr.begin(), arr.end(), ctl::back_inserter(vec));

        // Check if vec now contains all elements
        if (vec.size() != 6)
            return 1;
        if (vec[0] != 1 || vec[1] != 2 || vec[2] != 3 || vec[3] != 4 ||
            vec[4] != 5 || vec[5] != 6)
            return 2;

        // Use back_inserter with a single element
        ctl::back_inserter(vec) = 7;

        // Check if the new element was added
        if (vec.size() != 7)
            return 3;
        if (vec[6] != 7)
            return 4;

        // Test with an empty source range
        ctl::array<int, 0> empty_arr;
        ctl::copy(empty_arr.begin(), empty_arr.end(), ctl::back_inserter(vec));

        // Check that no elements were added
        if (vec.size() != 7)
            return 5;
    }

    CheckForMemoryLeaks();
}
