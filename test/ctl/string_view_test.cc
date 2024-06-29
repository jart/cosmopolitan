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

#include "ctl/string_view.h"
#include "libc/mem/leaks.h"
#include "libc/str/str.h"

// #include <string_view>
// #define ctl std

int
main(int argc, char* argv[])
{

    {
        ctl::string_view s = "hello there";
        s.remove_prefix(6);
        if (s != "there")
            return 1;
        s.remove_suffix(1);
        if (s != "ther")
            return 2;
    }

    {
        ctl::string_view s = "hello";
        if (s.front() != 'h' || s.back() != 'o')
            return 3;
    }

    {
        ctl::string_view s = "hello";
        if (s.compare("world") >= 0)
            return 4;
        if (s.compare("hello") != 0)
            return 5;
        if (s.compare("hallo") <= 0)
            return 6;
    }

    {
        ctl::string_view s = "hello";
        if (s.find('e') != 1)
            return 7;
        if (s.find('l') != 2)
            return 8;
        if (s.find('x') != ctl::string_view::npos)
            return 9;
    }

    {
        ctl::string_view s = "hello";
        if (s == "world")
            return 10;
        if (s != "hello")
            return 11;
        if (s < "hallo")
            return 12;
        if (s > "world")
            return 13;
    }

    {
        ctl::string_view s = "hello";
        auto it = s.begin();
        if (*it != 'h')
            return 14;
        ++it;
        if (*it != 'e')
            return 15;
    }

    {
        ctl::string_view s = "hello";
        ctl::string_view s2 = ctl::move(s);
        if (s2 != "hello")
            return 16;
        if (s.empty())
            return 17;
    }

    {
        ctl::string_view s = "hello";
        ctl::string_view sub = s.substr(1, 3);
        if (sub != "ell")
            return 18;
        sub = s.substr(2);
        if (sub != "llo")
            return 19;
    }

    {
        ctl::string_view s = "hello";
        if (!s.ends_with("lo"))
            return 20;
        if (s.ends_with("el"))
            return 21;
    }

    {
        ctl::string_view s = "hello";
        if (s.find('e') != 1)
            return 22;
        if (s.find('l') != 2)
            return 23;
        if (s.find('x') != ctl::string_view::npos)
            return 24;
    }

    {
        ctl::string_view s = "hello";
        if (s[0] != 'h' || s[1] != 'e' || s[2] != 'l' || s[3] != 'l' ||
            s[4] != 'o')
            return 25;
    }

    {
        ctl::string_view s = "hello";
        if (s.size() != 5)
            return 26;
        if (s.length() != 5)
            return 27;
    }

    {
        ctl::string_view a = "a";
        ctl::string_view b = "a";
        if (a.compare(b) != 0)
            return 28;
    }

    {
        ctl::string_view a = "a";
        ctl::string_view b = "b";
        if (a.compare(b) >= 0)
            return 29;
    }

    {
        ctl::string_view a = "a";
        ctl::string_view b = "ab";
        if (a.compare(b) >= 0)
            return 30;
    }

    {
        ctl::string_view a = "ab";
        ctl::string_view b = "a";
        if (a.compare(b) <= 0)
            return 31;
    }

    {
        ctl::string_view a = "";
        ctl::string_view b = "";
        if (a.compare(b) != 0)
            return 32;
    }

    CheckForMemoryLeaks();
}
