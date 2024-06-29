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

#include "ctl/accumulate.h"
#include "ctl/array.h"
#include "libc/mem/leaks.h"

// #include <array>
// #include <numeric>
// #define ctl std

int
main()
{
    ctl::array<int, 5> arr = { 1, 2, 3, 4, 5 };

    // Test basic accumulation with addition
    if (ctl::accumulate(arr.begin(), arr.end(), 0) != 15)
        return 1;

    // Test accumulation with initial value
    if (ctl::accumulate(arr.begin(), arr.end(), 10) != 25)
        return 2;

    // Test accumulation with custom operation (multiplication)
    if (ctl::accumulate(
          arr.begin(), arr.end(), 1, [](int a, int b) { return a * b; }) != 120)
        return 3;

    // Test accumulation with empty range
    if (ctl::accumulate(arr.end(), arr.end(), 0) != 0)
        return 4;

    // Test accumulation with single element
    if (ctl::accumulate(arr.begin(), arr.begin() + 1, 0) != 1)
        return 5;

    CheckForMemoryLeaks();
}
