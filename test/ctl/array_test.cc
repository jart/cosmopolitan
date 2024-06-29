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
#include "ctl/string.h"
#include "libc/mem/leaks.h"

// #include <array>
// #include <string>
// #define ctl std

int
main()
{

    // Test construction and basic properties
    {
        ctl::array<int, 5> arr = { 1, 2, 3, 4, 5 };
        if (arr.size() != 5)
            return 2;
        if (arr.max_size() != 5)
            return 3;
        if (arr.empty())
            return 4;
    }

    // Test element access
    {
        ctl::array<int, 3> arr = { 10, 20, 30 };
        if (arr[0] != 10 || arr[1] != 20 || arr[2] != 30)
            return 5;
        if (arr.front() != 10)
            return 6;
        if (arr.back() != 30)
            return 7;
    }

    // Test data() method
    {
        ctl::array<int, 3> arr = { 1, 2, 3 };
        int* data = arr.data();
        if (data[0] != 1 || data[1] != 2 || data[2] != 3)
            return 9;
    }

    // Test iterators
    {
        ctl::array<int, 3> arr = { 1, 2, 3 };
        int sum = 0;
        for (auto it = arr.begin(); it != arr.end(); ++it) {
            sum += *it;
        }
        if (sum != 6)
            return 10;

        sum = 0;
        for (auto it = arr.rbegin(); it != arr.rend(); ++it) {
            sum += *it;
        }
        if (sum != 6)
            return 11;
    }

    // Test const iterators
    {
        const ctl::array<int, 3> arr = { 1, 2, 3 };
        int sum = 0;
        for (auto it = arr.cbegin(); it != arr.cend(); ++it) {
            sum += *it;
        }
        if (sum != 6)
            return 12;

        sum = 0;
        for (auto it = arr.crbegin(); it != arr.crend(); ++it) {
            sum += *it;
        }
        if (sum != 6)
            return 13;
    }

    // Test fill method
    {
        ctl::array<int, 5> arr;
        arr.fill(42);
        for (int i = 0; i < 5; ++i) {
            if (arr[i] != 42)
                return 14;
        }
    }

    // Test swap method
    {
        ctl::array<int, 3> arr1 = { 1, 2, 3 };
        ctl::array<int, 3> arr2 = { 4, 5, 6 };
        arr1.swap(arr2);
        if (arr1[0] != 4 || arr1[1] != 5 || arr1[2] != 6)
            return 15;
        if (arr2[0] != 1 || arr2[1] != 2 || arr2[2] != 3)
            return 16;
    }

    // Test comparison operators
    {
        ctl::array<int, 3> arr1 = { 1, 2, 3 };
        ctl::array<int, 3> arr2 = { 1, 2, 3 };
        ctl::array<int, 3> arr3 = { 1, 2, 4 };

        if (!(arr1 == arr2))
            return 17;
        if (arr1 != arr2)
            return 18;
        if (!(arr1 < arr3))
            return 19;
        if (arr3 <= arr1)
            return 20;
        if (!(arr3 > arr1))
            return 21;
        if (arr1 >= arr3)
            return 22;
    }

    // Test non-member swap function
    {
        ctl::array<int, 3> arr1 = { 1, 2, 3 };
        ctl::array<int, 3> arr2 = { 4, 5, 6 };
        swap(arr1, arr2);
        if (arr1[0] != 4 || arr1[1] != 5 || arr1[2] != 6)
            return 23;
        if (arr2[0] != 1 || arr2[1] != 2 || arr2[2] != 3)
            return 24;
    }

    // Test with non-trivial type
    {
        struct NonTrivial
        {
            int value;

            NonTrivial(int v = 0) : value(v)
            {
            }

            bool operator==(const NonTrivial& other) const
            {
                return value == other.value;
            }
        };

        ctl::array<NonTrivial, 3> arr = { 1, 2, 3 };
        if (arr[0].value != 1 || arr[1].value != 2 || arr[2].value != 3)
            return 25;
    }

    // Test empty array
    {
        ctl::array<int, 0> arr;
        if (!arr.empty())
            return 26;
        if (arr.size() != 0)
            return 27;
        if (arr.begin() != arr.end())
            return 28;
    }

    // Test basic array functionality
    {
        ctl::array<int, 5> arr = { 1, 2, 3, 4, 5 };
        if (arr.size() != 5)
            return 2;
        if (arr[0] != 1 || arr[4] != 5)
            return 3;
    }

    // Test reverse iterator basics
    {
        ctl::array<int, 5> arr = { 1, 2, 3, 4, 5 };
        auto rit = arr.rbegin();
        if (*rit != 5)
            return 4;
        ++rit;
        if (*rit != 4)
            return 5;
        if (*(arr.rbegin() + 2) != 3)
            return 6;
    }

    // Test reverse iterator traversal
    {
        ctl::array<int, 5> arr = { 1, 2, 3, 4, 5 };
        int expected = 5;
        for (auto rit = arr.rbegin(); rit != arr.rend(); ++rit) {
            if (*rit != expected)
                return 7;
            --expected;
        }
    }

    // Test const reverse iterator
    {
        const ctl::array<int, 5> arr = { 1, 2, 3, 4, 5 };
        auto crit = arr.crbegin();
        if (*crit != 5)
            return 8;
        ++crit;
        if (*crit != 4)
            return 9;
    }

    // Test reverse iterator arithmetic
    {
        ctl::array<int, 5> arr = { 1, 2, 3, 4, 5 };
        auto rit = arr.rbegin();
        rit += 2;
        if (*rit != 3)
            return 10;
        rit -= 1;
        if (*rit != 4)
            return 11;
        if (*(rit + 2) != 2)
            return 12;
        if (*(rit - 1) != 5)
            return 13;
    }

    // Test reverse iterator comparison
    {
        ctl::array<int, 5> arr = { 1, 2, 3, 4, 5 };
        auto rit1 = arr.rbegin();
        auto rit2 = arr.rbegin() + 2;
        if (rit1 >= rit2)
            return 14;
        if (!(rit1 < rit2))
            return 15;
        if (rit1 == rit2)
            return 16;
    }

    // Test it seems legit
    {
        ctl::array<int, 3> arr = { 1, 2, 3 };
        auto rit = arr.rbegin();
        if (*rit != 3)
            return 1;
        ++rit;
        if (*rit != 2)
            return 2;
        ++rit;
        if (*rit != 1)
            return 3;
        ++rit;
        if (rit != arr.rend())
            return 4;
    }

    {
        ctl::array<ctl::string, 2> A = { "hi", "theretheretheretherethere" };
        if (A.size() != 2)
            return 76;
        if (A[0] != "hi")
            return 77;
        if (A[1] != "theretheretheretherethere")
            return 78;
        A = { "theretheretheretherethere", "hi" };
        if (A[0] != "theretheretheretherethere")
            return 79;
        if (A[1] != "hi")
            return 80;
    }

    CheckForMemoryLeaks();
}
