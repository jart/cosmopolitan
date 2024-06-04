// -*- mode:c++; indent-tabs-mode:nil; c-basic-offset:4; coding:utf-8 -*-
// vi: set et ft=c++ ts=4 sts=4 sw=4 fenc=utf-8
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

#include <__utility/move.h>

#include "libc/runtime/runtime.h"
#include "libc/str/str.h"

// #include <string>
// #define ctl std

int
main()
{

    {
        ctl::string s;
        s += 'h';
        s += 'i';
        if (s != "hi")
            return 1;
    }

    {
        ctl::string s;
        if (!s.empty())
            return 6;
        s.reserve(32);
        if (!s.empty())
            return 7;
        if (!s.starts_with(""))
            return 8;
        if (s.starts_with("a"))
            return 9;
        s += "abc";
        if (!s.starts_with("a"))
            return 10;
    }

    {
        ctl::string s;
        s += "hello world how are you";
        s.reserve(3);
        if (s != "hello world how are you")
            return 11;
    }

    {
        ctl::string s(4, 'x');
        if (s != "xxxx")
            return 12;
        s.resize(3);
        if (s != "xxx")
            return 13;
        s.resize(4, 'y');
        if (s != "xxxy")
            return 14;
    }

    {
        ctl::string a = "a";
        ctl::string b = "a";
        if (a.compare(b) != 0)
            return 17;
    }

    {
        ctl::string a = "a";
        ctl::string b = "b";
        if (a.compare(b) >= 0)
            return 18;
    }

    {
        ctl::string a = "a";
        ctl::string b = "ab";
        if (a.compare(b) >= 0)
            return 19;
    }

    {
        ctl::string a = "ab";
        ctl::string b = "a";
        if (a.compare(b) <= 0)
            return 20;
    }

    {
        ctl::string a = "";
        ctl::string b = "";
        if (a.compare(b) != 0)
            return 21;
    }

    {
        ctl::string a = "fooBARbaz";
        if (a.substr(3, 3) != "BAR")
            return 22;
        if (a.replace(3, 3, "MOO") != "fooMOObaz")
            return 23;
    }

    {
        ctl::string a = "fooBAR";
        if (a.substr(3, 3) != "BAR")
            return 24;
        if (a.replace(3, 3, "MOO") != "fooMOO")
            return 25;
    }

    {
        ctl::string a = "fooBAR";
        if (a.substr(1, 0) != "")
            return 26;
        if (a.replace(1, 0, "MOO") != "fMOOooBAR")
            return 27;
        if (!a.starts_with("fMOOooBAR"))
            return 28;
        if (!a.ends_with(""))
            return 29;
        if (!a.ends_with("BAR"))
            return 30;
        if (a.ends_with("bar"))
            return 31;
    }

    {
        ctl::string s1 = "hello";
        ctl::string s2 = "world";
        ctl::string s3 = s1 + " " + s2;
        if (s3 != "hello world")
            return 32;
    }

    {
        ctl::string s = "hello";
        if (s.size() != 5)
            return 33;
        if (s.length() != 5)
            return 34;
        if (s.capacity() < 5)
            return 35;
    }

    {
        ctl::string s = "hello";
        if (s[0] != 'h' || s[1] != 'e' || s[2] != 'l' || s[3] != 'l' ||
            s[4] != 'o')
            return 36;
        s[0] = 'H';
        if (s != "Hello")
            return 37;
    }

    {
        ctl::string s = "hello";
        if (s.find('e') != 1)
            return 38;
        if (s.find('l') != 2)
            return 39;
        if (s.find('x') != ctl::string::npos)
            return 40;
    }

    {
        ctl::string s = "hello";
        if (!s.ends_with("lo"))
            return 41;
        if (s.ends_with("el"))
            return 42;
    }

    {
        ctl::string s = "hello";
        ctl::string sub = s.substr(1, 3);
        if (sub != "ell")
            return 43;
        sub = s.substr(2);
        if (sub != "llo")
            return 44;
    }

    {
        ctl::string s = "hello";
        ctl::string s2 = s;
        if (s != s2)
            return 45;
        s2[0] = 'H';
        if (s == s2)
            return 46;
    }

    {
        ctl::string s = "hello";
        ctl::string s2 = std::move(s);
        if (s2 != "hello")
            return 47;
        if (!s.empty())
            return 48;
    }

    {
        ctl::string s = "hello";
        const char* cstr = s.c_str();
        if (strcmp(cstr, "hello") != 0)
            return 49;
    }

    // {
    //     ctl::string s = "hello";
    //     char buffer[10];
    //     s.copy(buffer, sizeof(buffer));
    //     if (strcmp(buffer, "hello") != 0)
    //         return 50;
    // }

    {
        ctl::string s = "hello";
        s.resize(3);
        if (s != "hel")
            return 51;
        s.resize(10, 'x');
        if (s != "helxxxxxxx")
            return 52;
    }

    {
        ctl::string s = "hello";
        s.clear();
        if (!s.empty())
            return 53;
    }

    {
        ctl::string s = "hello";
        auto it = s.begin();
        if (*it != 'h')
            return 54;
        ++it;
        if (*it != 'e')
            return 55;
    }

    // {
    //     ctl::string s = "hello";
    //     ctl::string s2 = "world";
    //     s.swap(s2);
    //     if (s != "world" || s2 != "hello")
    //         return 56;
    // }

    {
        ctl::string s = "hello";
        if (s.front() != 'h' || s.back() != 'o')
            return 57;
    }

    {
        ctl::string s = "hello";
        s.push_back('!');
        if (s != "hello!")
            return 58;
        s.pop_back();
        if (s != "hello")
            return 59;
    }

    {
        ctl::string s = "hello";
        s.insert(2, "XYZ");
        if (s != "heXYZllo")
            return 60;
    }

    {
        ctl::string s = "hello";
        s.erase(1, 2);
        if (s != "hlo")
            return 61;
    }

    {
        ctl::string s = "hello";
        s.replace(1, 2, "XYZ");
        if (s != "hXYZlo")
            return 62;
    }

    {
        ctl::string s = "hello";
        s.append(" world");
        if (s != "hello world")
            return 63;
    }

    // {
    //     ctl::string s = "hello";
    //     s.assign("world");
    //     if (s != "world")
    //         return 64;
    // }

    {
        ctl::string s = "hello";
        if (s.compare("world") >= 0)
            return 65;
        if (s.compare("hello") != 0)
            return 66;
        if (s.compare("hallo") <= 0)
            return 67;
    }

    {
        ctl::string s = "hello";
        if (s == "world")
            return 68;
        if (s != "hello")
            return 69;
        if (s < "hallo")
            return 70;
        if (s > "world")
            return 71;
    }

    {
        if ("hello"s != "hello")
            return 77;
        if ("hell"s + "o" != "hello")
            return 78;
    }

    CheckForMemoryLeaks();
    return 0;
}
