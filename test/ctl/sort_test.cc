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

#include "ctl/is_sorted.h"
#include "ctl/sort.h"
#include "ctl/string.h"
#include "ctl/vector.h"
#include "libc/mem/leaks.h"
#include "libc/stdio/rand.h"

// #include <algorithm>
// #include <string>
// #include <vector>
// #define ctl std

// Test sorting integers
int
test_sort_integers()
{
    ctl::vector<int> v = { 5, 2, 8, 1, 9, 3, 7, 6, 4 };
    ctl::sort(v.begin(), v.end());
    if (!ctl::is_sorted(v.begin(), v.end(), ctl::less<int>()))
        return 1;
    return 0;
}

// Test sorting with custom comparator
int
test_sort_custom_compare()
{
    ctl::vector<int> v = { 5, 2, 8, 1, 9, 3, 7, 6, 4 };
    ctl::sort(v.begin(), v.end(), [](int a, int b) { return a > b; });
    if (!ctl::is_sorted(v.begin(), v.end(), [](int a, int b) { return a > b; }))
        return 2;
    return 0;
}

// Test sorting strings
int
test_sort_strings()
{
    ctl::vector<ctl::string> v = { "banana", "apple", "cherry", "date" };
    ctl::sort(v.begin(), v.end());
    if (!ctl::is_sorted(v.begin(), v.end(), ctl::less<ctl::string>()))
        return 3;
    return 0;
}

// Test sorting with large number of elements
int
test_sort_large()
{
    const int SIZE = 10000;
    ctl::vector<int> v(SIZE);
    for (int i = 0; i < SIZE; ++i)
        v[i] = rand() % SIZE;
    ctl::sort(v.begin(), v.end());
    if (!is_sorted(v.begin(), v.end(), ctl::less<int>()))
        return 4;
    return 0;
}

// Test sorting already sorted vector
int
test_sort_sorted()
{
    ctl::vector<int> v = { 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    ctl::sort(v.begin(), v.end());
    if (!is_sorted(v.begin(), v.end(), ctl::less<int>()))
        return 5;
    return 0;
}

// Test sorting reverse sorted vector
int
test_sort_reverse()
{
    ctl::vector<int> v = { 9, 8, 7, 6, 5, 4, 3, 2, 1 };
    ctl::sort(v.begin(), v.end());
    if (!is_sorted(v.begin(), v.end(), ctl::less<int>()))
        return 6;
    return 0;
}

int
main()
{
    int result;

    result = test_sort_integers();
    if (result != 0)
        return result;

    result = test_sort_custom_compare();
    if (result != 0)
        return result;

    result = test_sort_strings();
    if (result != 0)
        return result;

    result = test_sort_large();
    if (result != 0)
        return result;

    result = test_sort_sorted();
    if (result != 0)
        return result;

    result = test_sort_reverse();
    if (result != 0)
        return result;

    CheckForMemoryLeaks();
}
