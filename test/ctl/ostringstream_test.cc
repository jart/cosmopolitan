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

#include "ctl/ostringstream.h"
#include "libc/mem/leaks.h"

// #include <sstream>
// #define ctl std

int
main()
{

    // Test default constructor and basic string insertion
    {
        ctl::ostringstream oss;
        oss << "Hello, world!";
        if (oss.str() != "Hello, world!")
            return 1;
    }

    // Test constructor with initial string (overwrite mode)
    {
        ctl::ostringstream oss("Initial content");
        oss << "New";
        if (oss.str() != "Newtial content")
            return 2;
    }

    // Test overwrite and append behavior
    {
        ctl::ostringstream oss("Hello, world!");
        oss << "Hi";
        if (oss.str() != "Hillo, world!")
            return 3;
        oss << " Earth";
        if (oss.str() != "Hi Earthorld!")
            return 4;
    }

    // Test multiple insertions of different types
    {
        ctl::ostringstream oss;
        oss << "Int: " << 42 << ", Float: " << 3.14f << ", Double: " << 2.718;
        if (oss.str() != "Int: 42, Float: 3.14, Double: 2.718")
            return 5;
    }

    // Test char insertion
    {
        ctl::ostringstream oss("ABCDEF");
        oss << 'X' << 'Y' << 'Z';
        if (oss.str() != "XYZDEF")
            return 6;
    }

    // Test unsigned types
    {
        ctl::ostringstream oss("Numbers: ");
        unsigned int ui = 12345;
        unsigned long ul = 67890UL;
        oss << ui << " " << ul;
        if (oss.str() != "12345 67890")
            return 7;
    }

    // Test long type
    {
        ctl::ostringstream oss("Long: ");
        long l = -9876543210L;
        oss << l;
        if (oss.str() != "-9876543210")
            return 8;
    }

    // Test clear() method
    {
        ctl::ostringstream oss("not that kind of clear");
        oss.clear();
        if (!oss.good() || oss.str() != "not that kind of clear")
            return 9;
    }

    // Test str() setter method
    {
        ctl::ostringstream oss;
        oss.str("New content");
        oss << "Old";
        if (oss.str() != "Old content")
            return 10;
    }

    // Test chaining of insertion operators
    {
        ctl::ostringstream oss("Start: ");
        oss << "Chain " << 1 << " " << 2.0 << " " << 'Z';
        if (oss.str() != "Chain 1 2 Z")
            return 11;
    }

    // Test insertion when stream is not in good state
    {
        ctl::ostringstream oss("Original");
        oss.setstate(ctl::ios_base::failbit);
        oss << "This should not be inserted";
        if (oss.str() != "Original")
            return 12;
    }

    // Test with larger amounts of data
    {
        ctl::ostringstream oss("Prefix: ");
        for (int i = 0; i < 1000; ++i) {
            oss << "Line " << i << "\n";
        }
        ctl::string result = oss.str();
        if (result.substr(0, 14) != "Line 0\nLine 1\n")
            return 13;
        if (result.substr(result.length() - 8) != "ine 999\n")
            return 14;
    }

    CheckForMemoryLeaks();
    return 0;
}
