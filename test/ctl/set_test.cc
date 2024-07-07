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

#include "ctl/set.h"
#include "libc/mem/leaks.h"

// #include <set>
// #define ctl std
// #define check() size()

int
rand32(void)
{
    /* Knuth, D.E., "The Art of Computer Programming," Vol 2,
       Seminumerical Algorithms, Third Edition, Addison-Wesley, 1998,
       p. 106 (line 26) & p. 108 */
    static unsigned long long lcg = 1;
    lcg *= 6364136223846793005;
    lcg += 1442695040888963407;
    return lcg >> 32;
}

int
main()
{

    {
        // Test construction and basic operations
        ctl::set<int> s;
        if (!s.empty())
            return 1;
        if (s.size() != 0)
            return 2;

        s.insert(1);
        s.insert(2);
        s.insert(3);
        if (s.size() != 3)
            return 3;
        if (s.count(2) != 1)
            return 4;
        if (s.count(4) != 0)
            return 5;
    }

    {
        // Test insertion and find
        ctl::set<int> s;
        auto result = s.insert(5);
        if (!result.second || *result.first != 5)
            return 6;

        result = s.insert(5); // Duplicate insertion
        if (result.second || *result.first != 5)
            return 7;

        auto it = s.find(5);
        if (it == s.end() || *it != 5)
            return 8;

        it = s.find(6);
        if (it != s.end())
            return 9;
        s.check();
    }

    {
        // Test erase
        ctl::set<int> s;
        s.insert(1);
        s.insert(2);
        s.insert(3);
        s.insert(4);
        s.insert(5);
        s.erase(3);
        if (s.size() != 4 || s.count(3) != 0)
            return 10;

        auto it = s.find(2);
        s.erase(it);
        if (s.size() != 3 || s.count(2) != 0)
            return 11;
    }

    {
        // Test clear
        ctl::set<int> s;
        s.insert(1);
        s.insert(2);
        s.insert(3);
        s.insert(4);
        s.insert(5);
        s.clear();
        if (!s.empty() || s.size() != 0)
            return 12;
    }

    {
        // Test comparison operators
        ctl::set<int> s1{ 1, 2, 3 };
        ctl::set<int> s2{ 1, 2, 3 };
        ctl::set<int> s3{ 1, 2, 3, 4 };

        if (!(s1 == s2))
            return 13;
        if (s1 != s2)
            return 14;
        if (!(s1 < s3))
            return 15;
        if (s3 <= s1)
            return 16;
    }

    {
        // Test iterator functionality
        ctl::set<int> s{ 5, 3, 1, 4, 2 };
        int prev = 0;
        for (const auto& value : s) {
            if (value <= prev)
                return 17;
            prev = value;
            s.check();
        }
    }

    {
        // Test lower_bound and upper_bound
        ctl::set<int> s{ 1, 3, 5, 7, 9 };
        auto lower = s.lower_bound(4);
        auto upper = s.upper_bound(4);
        if (*lower != 5 || *upper != 5)
            return 18;
        s.check();
    }

    {
        // Test lower_bound and upper_bound
        ctl::set<int> s{ 1, 3, 4, 5, 7, 9 };
        auto lower = s.lower_bound(4);
        auto upper = s.upper_bound(4);
        if (*lower != 4 || *upper != 5)
            return 18;
        s.check();
    }

    {
        // Test emplace
        ctl::set<ctl::pair<int, int>> s;
        auto result = s.emplace(1, 2);
        if (!result.second || result.first->first != 1 ||
            result.first->second != 2)
            return 19;
        s.check();
    }

    {
        // Test insertion and size
        ctl::set<int> s;
        for (int i = 0; i < 1000; ++i)
            s.insert(i);
        if (s.size() != 1000)
            return 20;
        s.check();
    }

    {
        // Test duplicate insertions
        ctl::set<int> s;
        for (int i = 0; i < 100; ++i) {
            s.insert(i);
            s.insert(i); // Duplicate
        }
        if (s.size() != 100)
            return 21;
        s.check();
    }

    {
        // Test deletion
        ctl::set<int> s;
        for (int i = 0; i < 100; ++i)
            s.insert(i);
        for (int i = 0; i < 50; ++i)
            s.erase(i);
        if (s.size() != 50 || s.find(0) != s.end() || s.find(99) == s.end())
            return 22;
        s.check();
    }

    {
        // Test balance after multiple insertions
        ctl::set<int> s;
        for (int i = 0; i < 1000; ++i) {
            s.insert(i);
            s.check();
        }
    }

    {
        // Test balance after multiple insertions and deletions
        ctl::set<int> s;
        for (int i = 0; i < 20000; ++i) {
            if (s.empty() || rand32() % 2 == 0) {
                s.insert(rand32() % 500);
            } else {
                s.erase(rand32() % s.size());
            }
            s.check();
        }
    }

    {
        // Test iterator functionality
        ctl::set<int> s;
        for (int i = 0; i < 100; ++i)
            s.insert(i);
        int prev = -1;
        for (const auto& val : s) {
            if (val <= prev)
                return 25;
            prev = val;
        }
    }

    {
        // Test clear() function
        ctl::set<int> s;
        for (int i = 0; i < 100; ++i)
            s.insert(i);
        s.clear();
        s.check();
        if (!s.empty() || s.size() != 0)
            return 26;
    }

    {
        // Test extreme case: insert in descending order
        ctl::set<int> s;
        for (int i = 1000; i >= 0; --i)
            s.insert(i);
        if (s.size() != 1001 || *s.begin() != 0 || *s.rbegin() != 1000)
            return 27;
    }

    {
        // Test extreme case: insert in ascending order
        ctl::set<int> s;
        for (int i = 0; i <= 1000; ++i)
            s.insert(i);
        if (s.size() != 1001 || *s.begin() != 0 || *s.rbegin() != 1000)
            return 28;
    }

    {
        // Test lower_bound and upper_bound more extensively
        ctl::set<int> s{ 10, 20, 30, 40, 50 };
        if (s.lower_bound(25) != s.find(30) || s.upper_bound(25) != s.find(30))
            return 29;
        if (s.lower_bound(30) != s.find(30) || s.upper_bound(30) != s.find(40))
            return 30;
        if (s.lower_bound(60) != s.end() || s.upper_bound(60) != s.end())
            return 31;
    }

    {
        // Test emplace with more complex types
        struct TestStruct
        {
            int a;
            double b;

            TestStruct(int a, double b) : a(a), b(b)
            {
            }

            bool operator<(const TestStruct& other) const
            {
                return a < other.a;
            }
        };

        ctl::set<TestStruct> s;
        auto result = s.emplace(5, 3.14);
        if (!result.second || result.first->a != 5 || result.first->b != 3.14)
            return 32;
        s.check();
    }

    {
        ctl::set<int> s;
        if (s.count(6) != 0)
            return 33;
        s.insert(6);
        s.insert(6);
        s.insert(6);
        if (s.count(6) != 1)
            return 34;
    }

    CheckForMemoryLeaks();
}
