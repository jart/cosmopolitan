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

#include "ctl/is_same.h"
#include "ctl/string.h"
#include "libc/mem/leaks.h"

#include "libc/str/str.h"

// #include <string>
// #include <utility>
// #define ctl std

using String = ctl::string;

#undef ctl

inline bool
issmall(const String& s)
{
    return s.capacity() == sizeof(s) &&
           s.data() == reinterpret_cast<const char*>(&s);
}

int
main()
{

    {
        String s;
        s += 'h';
        s += 'i';
        if (s != "hi")
            return 1;
    }

    {
        String s;
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
        String s;
        s += "hello world how are you";
        s.reserve(3);
        if (s != "hello world how are you")
            return 11;
    }

    {
        String s(4, 'x');
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
        String a = "a";
        String b = "a";
        if (a.compare(b) != 0)
            return 17;
    }

    {
        String a = "a";
        String b = "b";
        if (a.compare(b) >= 0)
            return 18;
    }

    {
        String a = "a";
        String b = "ab";
        if (a.compare(b) >= 0)
            return 19;
    }

    {
        String a = "ab";
        String b = "a";
        if (a.compare(b) <= 0)
            return 20;
    }

    {
        String a = "";
        String b = "";
        if (a.compare(b) != 0)
            return 21;
    }

    {
        String a = "fooBARbaz";
        if (a.substr(3, 3) != "BAR")
            return 22;
        if (a.replace(3, 3, "MOO") != "fooMOObaz")
            return 23;
    }

    {
        String a = "fooBAR";
        if (a.substr(3, 3) != "BAR")
            return 24;
        if (a.replace(3, 3, "MOO") != "fooMOO")
            return 25;
    }

    {
        String a = "fooBAR";
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
        String s1 = "hello";
        String s2 = "world";
        String s3 = s1 + " " + s2;
        if (s3 != "hello world")
            return 32;
    }

    {
        String s = "hello";
        if (s.size() != 5)
            return 33;
        if (s.length() != 5)
            return 34;
        if (s.capacity() < 5)
            return 35;
    }

    {
        String s = "hello";
        if (s[0] != 'h' || s[1] != 'e' || s[2] != 'l' || s[3] != 'l' ||
            s[4] != 'o')
            return 36;
        s[0] = 'H';
        if (s != "Hello")
            return 37;
    }

    {
        String s = "hello";
        if (s.find('e') != 1)
            return 38;
        if (s.find('l') != 2)
            return 39;
        if (s.find('x') != s.npos)
            return 40;
    }

    {
        String s = "hello";
        if (!s.ends_with("lo"))
            return 41;
        if (s.ends_with("el"))
            return 42;
    }

    {
        String s = "hello";
        String sub = s.substr(1, 3);
        if (sub != "ell")
            return 43;
        sub = s.substr(2);
        if (sub != "llo")
            return 44;
    }

    {
        String s = "hello";
        String s2 = s;
        if (s != s2)
            return 45;
        s2[0] = 'H';
        if (s == s2)
            return 46;
    }

    {
        String s = "hello";
        String s2 = ctl::move(s);
        if (s2 != "hello")
            return 47;
        if (!s.empty())
            return 48;
    }

    {
        String s = "hello";
        const char* cstr = s.c_str();
        if (strcmp(cstr, "hello") != 0)
            return 49;
    }

    // {
    //     String s = "hello";
    //     char buffer[10];
    //     s.copy(buffer, sizeof(buffer));
    //     if (strcmp(buffer, "hello") != 0)
    //         return 50;
    // }

    {
        String s = "hello";
        s.resize(3);
        if (s != "hel")
            return 51;
        s.resize(10, 'x');
        if (s != "helxxxxxxx")
            return 52;
    }

    {
        String s = "hello";
        s.clear();
        if (!s.empty())
            return 53;
    }

    {
        String s = "hello";
        auto it = s.begin();
        if (*it != 'h')
            return 54;
        ++it;
        if (*it != 'e')
            return 55;
    }

    {
        String s = "hello";
        String s2 = "world";
        s.swap(s2);
        if (s != "world" || s2 != "hello")
            return 56;
    }

    {
        String s = "hello";
        if (s.front() != 'h' || s.back() != 'o')
            return 57;
    }

    {
        String s = "hello";
        s.push_back('!');
        if (s != "hello!")
            return 58;
        s.pop_back();
        if (s != "hello")
            return 59;
    }

    {
        String s = "hello";
        s.insert(2, "XYZ");
        if (s != "heXYZllo")
            return 60;
    }

    {
        String s = "hello";
        s.erase(1, 2);
        if (s != "hlo")
            return 61;
    }

    {
        String s = "hello";
        s.replace(1, 2, "XYZ");
        if (s != "hXYZlo")
            return 62;
    }

    {
        String s = "hello";
        s.append(" world");
        if (s != "hello world")
            return 63;
        for (int i = 0; i < 10; ++i) {
            s.append(" world");
        }
        if (s != "hello world world world world world world world world world "
                 "world world") {
            return 64;
        }
    }

    // {
    //     String s = "hello";
    //     s.assign("world");
    //     if (s != "world")
    //         return 64;
    // }

    {
        String s = "hello";
        if (s.compare("world") >= 0)
            return 65;
        if (s.compare("hello") != 0)
            return 66;
        if (s.compare("hallo") <= 0)
            return 67;
    }

    {
        String s = "hello";
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

    {
        String s;
        if constexpr (ctl::is_same_v<ctl::string, decltype(s)>) {
            // tests the small-string optimization on ctl::string
            for (int i = 0; i < 23; ++i) {
                s.append("a");
                if (!issmall(s)) {
                    return 79 + i;
                }
            }
            s.append("a");
            if (issmall(s)) {
                return 103;
            }
        } else {
            // just check that append in a loop works
            for (int i = 0; i < 24; ++i) {
                s.append("a");
            }
        }
        if (s != "aaaaaaaaaaaaaaaaaaaaaaaa") {
            return 104;
        }
    }

    {
        String s("arst", 4);
        for (int i = 0; i < 30; ++i) {
            s.append("a");
        }
        s.resize(4);
        if (s != "arst")
            return 105;
        if constexpr (ctl::is_same_v<ctl::string, decltype(s)>) {
            String r(s);
            if (issmall(s) || !issmall(r))
                return 106;
        }
    }

    {
        String s = "love";
        String b;
        b.append(s, 1, 2);
        if (b != "ov")
            return 107;
    }

    {
        String s = "ee";
        if (s.find_last_of('E') != String::npos)
            return 108;
        if (s.find_last_of('e') != 1)
            return 109;
    }

    {
        String e = "";
        String s = "ee";
        if (e.find_last_of("") != String::npos)
            return 110;
        if (s.find_last_of("") != String::npos)
            return 111;
        if (s.find_last_of("AE") != String::npos)
            return 112;
        if (s.find_last_of("ae") != 1)
            return 113;
        if (s.find_last_of("ae", 1) != 1)
            return 114;
        if (s.find_last_of("ae", 0) != 0)
            return 115;
        if (s.find_last_of("ae", 10) != 1)
            return 116;
    }

    {
        String s = "ee";
        if (s.find_first_of('E') != String::npos)
            return 117;
        if (s.find_first_of('e') != 0)
            return 118;
        if (s.find_first_of('e', 1) != 1)
            return 119;
    }

    {
        String e = "";
        String s = "ee";
        if (e.find_first_of("") != String::npos)
            return 120;
        if (s.find_first_of("") != String::npos)
            return 121;
        if (s.find_first_of("AE") != String::npos)
            return 122;
        if (s.find_first_of("ae") != 0)
            return 123;
        if (s.find_first_of("ae", 1) != 1)
            return 124;
        if (s.find_first_of("ae", 0) != 0)
            return 125;
        if (s.find_first_of("ae", 10) != String::npos)
            return 126;
    }

    CheckForMemoryLeaks();
}
