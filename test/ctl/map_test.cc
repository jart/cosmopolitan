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

#include "ctl/map.h"
#include "ctl/string.h"
#include "libc/mem/leaks.h"

// #include <map>
// #include <string>
// #define ctl std

int
main()
{

    {
        ctl::map<int, double> s;
        if (!s.empty())
            return 1;
        if (s.size())
            return 2;
        s[1] = 10;
        s[2] = 20;
        s[3] = 3.14;
        if (s[1] != 10)
            return 3;
        if (s[2] != 20)
            return 4;
        if (s[3] != 3.14)
            return 5;
    }

    {
        ctl::map<int, double> m;
        if (!m.empty())
            return 1;
        if (m.size() != 0)
            return 2;

        m[1] = 10.5;
        m[2] = 20.7;
        m[3] = 3.14;

        if (m.empty())
            return 3;
        if (m.size() != 3)
            return 4;
        if (m[1] != 10.5)
            return 5;
        if (m[2] != 20.7)
            return 6;
        if (m[3] != 3.14)
            return 7;
    }

    {
        ctl::map<ctl::string, int> m;
        m["one"] = 1;
        m["two"] = 2;
        m["three"] = 3;

        int sum = 0;
        for (const auto& pair : m) {
            sum += pair.second;
        }
        if (sum != 6)
            return 8;

        auto it = m.find("two");
        if (it == m.end())
            return 9;
        if (it->second != 2)
            return 10;

        it = m.find("four");
        if (it != m.end())
            return 11;
    }

    {
        ctl::map<int, ctl::string> m = { { 1, "one" },
                                         { 2, "two" },
                                         { 3, "three" } };
        if (m.size() != 3)
            return 12;
        if (m[2] != "two")
            return 13;

        auto result = m.insert({ 4, "four" });
        if (!result.second)
            return 14;
        if (result.first->second != "four")
            return 15;

        result = m.insert({ 2, "deux" });
        if (result.second)
            return 16;
        if (result.first->second != "two")
            return 17;
    }

    {
        ctl::map<int, double> m1 = { { 1, 1.1 }, { 2, 2.2 }, { 3, 3.3 } };
        ctl::map<int, double> m2 = { { 1, 1.1 }, { 2, 2.2 }, { 3, 3.3 } };
        ctl::map<int, double> m3 = { { 1, 1.1 }, { 2, 2.2 }, { 4, 4.4 } };

        if (!(m1 == m2))
            return 18;
        if (m1 != m2)
            return 19;
        if (m1 == m3)
            return 20;
        if (!(m1 != m3))
            return 21;
    }

    {
        ctl::map<int, ctl::string> m;
        m[1] = "one";
        m[2] = "two";
        m[3] = "three";

        if (m.count(2) != 1)
            return 22;
        if (m.count(4) != 0)
            return 23;

        m.erase(2);
        if (m.size() != 2)
            return 24;
        if (m.count(2) != 0)
            return 25;
    }

    {
        ctl::map<int, double> m = { { 1, 1.1 }, { 3, 3.3 }, { 5, 5.5 } };

        auto lower = m.lower_bound(2);
        if (lower->first != 3)
            return 26;

        auto upper = m.upper_bound(3);
        if (upper->first != 5)
            return 27;

        auto range = m.equal_range(3);
        if (range.first->first != 3 || range.second->first != 5)
            return 28;
    }

    {
        ctl::map<int, ctl::string> m = {
            { 1, "one" }, { 2, "two" }, { 3, "three" }, { 4, "four" }
        };

        // Test reverse iteration
        auto rit = m.rbegin();
        if (rit->first != 4 || rit->second != "four")
            return 29;

        ++rit;
        if (rit->first != 3 || rit->second != "three")
            return 30;

        // Test reverse iteration with const_reverse_iterator
        auto crit = m.crbegin();
        if (crit->first != 4 || crit->second != "four")
            return 31;

        ++crit;
        ++crit;
        if (crit->first != 2 || crit->second != "two")
            return 32;

        // Test rend()
        rit = m.rbegin();
        int count = 0;
        while (rit != m.rend()) {
            ++rit;
            ++count;
        }
        if (count != 4)
            return 33;

        // Test reverse iteration with range-based for loop
        ctl::string result;
        for (auto it = m.rbegin(); it != m.rend(); ++it) {
            result += it->second;
        }
        if (result != "fourthreetwoone")
            return 34;

        // Test modification through reverse iterator
        rit = m.rbegin();
        rit->second = "quatre";
        if (m[4] != "quatre")
            return 35;

        // Test comparison of reverse iterators
        auto rit1 = m.rbegin();
        auto rit2 = m.rbegin();
        ++rit2;
        if (!(*rit1 > *rit2))
            return 36;
    }

    CheckForMemoryLeaks();
}
