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

#include "ctl/any_of.h"
#include "ctl/array.h"
#include "libc/mem/leaks.h"

// #include <algorithm>
// #include <array>
// #define ctl std

int
main()
{
    ctl::array<int, 5> arr1 = { 1, 3, 5, 7, 9 };
    ctl::array<int, 5> arr2 = { 2, 4, 6, 8, 10 };

    // Test when at least one element satisfies the condition
    if (!ctl::any_of(arr1.begin(), arr1.end(), [](int n) { return n == 7; }))
        return 1;

    // Test when no elements satisfy the condition
    if (ctl::any_of(arr1.begin(), arr1.end(), [](int n) { return n == 11; }))
        return 2;

    // Test with empty range
    if (ctl::any_of(arr1.end(), arr1.end(), [](int n) { return true; }))
        return 3;

    // Test when all elements satisfy the condition
    if (!ctl::any_of(arr2.begin(), arr2.end(), [](int n) { return true; }))
        return 4;

    // Test with a different condition
    if (!ctl::any_of(arr1.begin(), arr1.end(), [](int n) { return n > 5; }))
        return 5;

    CheckForMemoryLeaks();
}
