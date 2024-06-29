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

#include "ctl/istringstream.h"
#include "libc/mem/leaks.h"

// #include <sstream>
// #define ctl std

int
main()
{

    // Test default constructor
    {
        ctl::istringstream iss;
        if (iss.eof())
            return 1;
        if (!iss.good())
            return 1;
    }

    // Test constructor with initial string
    {
        ctl::istringstream iss("Hello, world!");
        if (iss.str() != "Hello, world!")
            return 2;
    }

    // Test reading a char
    {
        ctl::istringstream iss("A");
        char c;
        iss >> c;
        if (c != 'A' || !iss.good())
            return 3;
    }

    // Test reading a string
    {
        ctl::istringstream iss("Hello World");
        ctl::string s;
        iss >> s;
        if (s != "Hello" || !iss.good())
            return 4;
    }

    // Test reading multiple strings
    {
        ctl::istringstream iss("One Two Three");
        ctl::string s1, s2, s3;
        iss >> s1 >> s2 >> s3;
        if (s1 != "One" || s2 != "Two" || s3 != "Three" || iss.good() ||
            !iss.eof())
            return 5;
    }

    // Test reading integers
    {
        ctl::istringstream iss("123 -456 789");
        int a, b, c;
        iss >> a >> b >> c;
        if (a != 123 || b != -456 || c != 789 || iss.good() || !iss.eof())
            return 6;
    }

    // Test reading floating-point numbers
    {
        ctl::istringstream iss("3.14 -2.718");
        float f;
        double d;
        iss >> f >> d;
        if (f != 3.14f || d != -2.718 || iss.good() || !iss.eof())
            return 7;
    }

    // Test reading past the end of the stream
    {
        ctl::istringstream iss("42");
        int n;
        iss >> n;
        if (n != 42 || iss.good() || !iss.eof())
            return 8;
        iss >> n;
        if (iss.good())
            return 9;
        if (!iss.eof())
            return 10;
    }

    // Test reading invalid data
    {
        ctl::istringstream iss("not_a_number");
        int n;
        iss >> n;
        if (iss.good() || !iss.fail())
            return 11;
    }

    // Test str() setter
    {
        ctl::istringstream iss;
        iss.str("New content");
        if (iss.str() != "New content" || !iss.good())
            return 12;
    }

    // Test reading after setting a new string
    {
        ctl::istringstream iss("Old content");
        iss.str("New content");
        ctl::string s;
        iss >> s;
        if (s != "New" || !iss.good())
            return 13;
    }

    // Test reading a mixture of types
    {
        ctl::istringstream iss("42 3.14 Hello");
        int i;
        double d;
        ctl::string s;
        iss >> i >> d >> s;
        if (i != 42 || d != 3.14 || s != "Hello" || iss.good() || !iss.eof())
            return 14;
    }

    // Test reading with leading whitespace
    {
        ctl::istringstream iss("   42");
        int n;
        iss >> n;
        if (n != 42 || iss.good() || !iss.eof())
            return 15;
    }

    // Test clear() only affects error state, not content
    {
        ctl::istringstream iss("Test");
        iss.setstate(ctl::ios_base::failbit);
        iss.clear();
        if (!iss.good() || iss.str() != "Test")
            return 16;
    }

    CheckForMemoryLeaks();
}
