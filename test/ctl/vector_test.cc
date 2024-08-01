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

#include "ctl/string.h"
#include "ctl/vector.h"
#include "libc/mem/leaks.h"

// #include <string>
// #include <vector>
// #define ctl std

static int counter;

// Test with non-trivial type
struct NonTrivial
{
    int value;

    NonTrivial(int v) : value(v)
    {
        ++counter;
    }

    NonTrivial(const NonTrivial& other) : value(other.value)
    {
        ++counter;
    }

    NonTrivial(NonTrivial&& other) noexcept : value(other.value)
    {
        ++counter;
    }

    ~NonTrivial()
    {
        --counter;
    }

    NonTrivial& operator=(const NonTrivial& other)
    {
        value = other.value;
        return *this;
    }

    NonTrivial& operator=(NonTrivial&& other) noexcept
    {
        value = other.value;
        return *this;
    }
};

int
main()
{

    {
        int x = 3;
        ctl::vector<int> A;
        A.push_back(1);
        A.push_back(2);
        A.push_back(x);
        if (A[0] != 1)
            return 1;
        if (A[1] != 2)
            return 2;
        if (A[2] != 3)
            return 3;
        if (A.size() != 3)
            return 4;
    }

    {
        ctl::string yo = "foo";
        ctl::vector<ctl::string> A;
        A.push_back("fun");
        A.push_back(ctl::move(yo));
        if (yo != "")
            return 5;
        A.emplace_back("bar");
        if (A[0] != "fun")
            return 7;
        if (A[1] != "foo")
            return 8;
        if (A[2] != "bar")
            return 9;
        if (A.size() != 3)
            return 10;
    }

    {
        ctl::vector<int> A;
        if (!A.empty())
            return 11;
        A.push_back(5);
        if (A.empty())
            return 12;
        if (A.front() != 5)
            return 13;
        if (A.back() != 5)
            return 14;
    }

    {
        ctl::vector<int> A;
        A.push_back(1);
        A.push_back(2);
        A.push_back(3);
        ctl::vector<int> B(A);
        if (B.size() != 3)
            return 15;
        if (B[0] != 1 || B[1] != 2 || B[2] != 3)
            return 16;
    }

    {
        ctl::vector<int> A;
        A.push_back(1);
        A.push_back(2);
        A.push_back(3);
        ctl::vector<int> B(ctl::move(A));
        if (A.size() != 0)
            return 17;
        if (B.size() != 3)
            return 18;
        if (B[0] != 1 || B[1] != 2 || B[2] != 3)
            return 19;
    }

    {
        ctl::vector<int> A;
        A.push_back(1);
        A.push_back(2);
        A.push_back(3);
        ctl::vector<int> B;
        B = A;
        if (B.size() != 3)
            return 20;
        if (B[0] != 1 || B[1] != 2 || B[2] != 3)
            return 21;
    }

    {
        ctl::vector<int> A;
        A.push_back(1);
        A.push_back(2);
        A.push_back(3);
        ctl::vector<int> B;
        B = ctl::move(A);
        if (A.size() != 0)
            return 22;
        if (B.size() != 3)
            return 23;
        if (B[0] != 1 || B[1] != 2 || B[2] != 3)
            return 24;
    }

    {
        ctl::vector<int> A;
        A.push_back(1);
        A.push_back(2);
        A.push_back(3);
        A.pop_back();
        if (A.size() != 2)
            return 25;
        if (A[0] != 1 || A[1] != 2)
            return 26;
    }

    {
        ctl::vector<int> A;
        A.resize(5);
        if (A.size() != 5)
            return 27;
        A.resize(3);
        if (A.size() != 3)
            return 28;
    }

    {
        ctl::vector<int> A;
        A.push_back(1);
        A.push_back(2);
        A.push_back(3);
        ctl::vector<int> B;
        B.push_back(4);
        B.push_back(5);
        A.swap(B);
        if (A.size() != 2)
            return 29;
        if (B.size() != 3)
            return 30;
        if (A[0] != 4 || A[1] != 5)
            return 31;
        if (B[0] != 1 || B[1] != 2 || B[2] != 3)
            return 32;
    }

    {
        ctl::vector<int> A;
        A.push_back(1);
        A.push_back(2);
        A.push_back(3);
        A.clear();
        if (A.size() != 0)
            return 33;
        if (!A.empty())
            return 34;
    }

    {
        ctl::vector<int> A;
        A.push_back(1);
        A.push_back(2);
        A.push_back(3);
        ctl::vector<int>::iterator it = A.begin();
        if (*it != 1)
            return 35;
        ++it;
        if (*it != 2)
            return 36;
        ++it;
        if (*it != 3)
            return 37;
        ++it;
        if (it != A.end())
            return 38;
    }

    {
        ctl::vector<int> A;
        A.push_back(1);
        A.push_back(2);
        A.push_back(3);
        ctl::vector<int>::const_iterator cit = A.cbegin();
        if (*cit != 1)
            return 39;
        ++cit;
        if (*cit != 2)
            return 40;
        ++cit;
        if (*cit != 3)
            return 41;
        ++cit;
        if (cit != A.cend())
            return 42;
    }

    {
        ctl::vector<int> A;
        for (int i = 0; i < 100; ++i) {
            A.push_back(i);
        }
        if (A.size() != 100)
            return 51;
        for (int i = 0; i < 100; ++i) {
            if (A[i] != i)
                return 52;
        }
    }

    {
        ctl::vector<int> A;
        A.push_back(1);
        A.push_back(2);
        A.push_back(3);
        ctl::vector<int> B(A);
        if (B.size() != 3)
            return 53;
        B.push_back(4);
        if (A.size() != 3)
            return 54;
        if (B.size() != 4)
            return 55;
    }

    {
        ctl::vector<int> A;
        A.reserve(100);
        if (A.size() != 0)
            return 56;
        if (A.capacity() != 100)
            return 57;
        A.push_back(1);
        if (A.size() != 1)
            return 58;
        if (A.capacity() != 100)
            return 59;
    }

    {
        ctl::vector<int> A;
        A.push_back(1);
        A.push_back(2);
        A.push_back(3);
        ctl::vector<int> B;
        B = A;
        if (B.size() != 3)
            return 60;
        B.push_back(4);
        if (A.size() != 3)
            return 61;
        if (B.size() != 4)
            return 62;
    }

    {
        ctl::vector<int> A;
        A.push_back(1);
        A.push_back(2);
        A.push_back(3);
        ctl::vector<int> B;
        B = ctl::move(A);
        if (A.size() != 0)
            return 63;
        if (B.size() != 3)
            return 64;
        if (B[0] != 1 || B[1] != 2 || B[2] != 3)
            return 65;
    }

    {
        ctl::vector<int> A;
        A.push_back(1);
        A.push_back(2);
        A.push_back(3);
        ctl::vector<int> B;
        B.push_back(4);
        B.push_back(5);
        A.swap(B);
        if (A.size() != 2)
            return 66;
        if (B.size() != 3)
            return 67;
        if (A[0] != 4 || A[1] != 5)
            return 68;
        if (B[0] != 1 || B[1] != 2 || B[2] != 3)
            return 69;
    }

    {
        ctl::vector<int> A = { 1, 2, 3 };
        if (A[1] != 2)
            return 70;
        A = { 4, 5, 6 };
        if (A[1] != 5)
            return 71;
    }

    {
        ctl::vector<int> arr = { 1, 2, 3 };
        auto rit = arr.rbegin();
        if (*rit != 3)
            return 72;
        ++rit;
        if (*rit != 2)
            return 73;
        ++rit;
        if (*rit != 1)
            return 74;
        ++rit;
        if (rit != arr.rend())
            return 75;
    }

    {
        ctl::vector<ctl::string> A = { "hi", "theretheretheretherethere" };
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

    {
        ctl::vector<int> dog(8, 0);
        if (dog.size() != 8)
            return 81;
        if (dog[0] != 0)
            return 82;
    }

    // Test erase(const_iterator first, const_iterator last)
    {
        ctl::vector<int> v = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };

        // Test erasing from the middle
        auto it = v.erase(v.begin() + 3, v.begin() + 7);
        if (v.size() != 6 || v != ctl::vector<int>{ 1, 2, 3, 8, 9, 10 } ||
            it != v.begin() + 3)
            return 83;

        // Test erasing from the beginning
        it = v.erase(v.begin(), v.begin() + 2);
        if (v.size() != 4 || v != ctl::vector<int>{ 3, 8, 9, 10 } ||
            it != v.begin())
            return 84;

        // Test erasing to the end
        it = v.erase(v.begin() + 2, v.end());
        if (v.size() != 2 || v != ctl::vector<int>{ 3, 8 } || it != v.end())
            return 85;

        // Test erasing all elements
        it = v.erase(v.begin(), v.end());
        if (!v.empty() || it != v.end())
            return 86;

        // Test erasing empty range
        v = { 1, 2, 3, 4, 5 };
        it = v.erase(v.begin() + 2, v.begin() + 2);
        if (v.size() != 5 || v != ctl::vector<int>{ 1, 2, 3, 4, 5 } ||
            it != v.begin() + 2)
            return 87;

        counter = 0;

        {
            ctl::vector<NonTrivial> v2;
            for (int i = 0; i < 10; ++i)
                v2.emplace_back(i);
            v2.erase(v2.begin() + 3, v2.begin() + 7);
            if (v2.size() != 6 || counter != 6)
                return 89;
            for (int i = 0; i < (int)v2.size(); ++i)
                if (v2[i].value != (i < 3 ? i : i + 4))
                    return 90;
        }

        if (counter != 0)
            return 91;
    }

    CheckForMemoryLeaks();
}
