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
#include "ctl/tuple.h"
#include "libc/mem/leaks.h"

// #include <string>
// #include <tuple>
// #define ctl std

int
test_tuple_creation()
{
    ctl::tuple<int, double, ctl::string> t(1, 2.5, "hello");
    if (ctl::get<0>(t) != 1 || ctl::get<1>(t) != 2.5 ||
        ctl::get<2>(t) != "hello") {
        return 1;
    }
    return 0;
}

int
test_make_tuple()
{
    auto t = ctl::make_tuple(1, 2.5, ctl::string("hello"));
    if (ctl::get<0>(t) != 1 || ctl::get<1>(t) != 2.5 ||
        ctl::get<2>(t) != "hello") {
        return 2;
    }
    return 0;
}

int
test_tuple_get()
{
    ctl::tuple<int, double, ctl::string> t(1, 2.5, "hello");
    if (ctl::get<0>(t) != 1)
        return 3;
    if (ctl::get<1>(t) != 2.5)
        return 4;
    if (ctl::get<2>(t) != "hello")
        return 5;
    return 0;
}

int
test_tuple_comparison()
{
    auto t1 = ctl::make_tuple(1, 2.5, "hello");
    auto t2 = ctl::make_tuple(1, 2.5, "hello");
    auto t3 = ctl::make_tuple(2, 3.5, "world");

    if (!(t1 == t2))
        return 6;
    if (t1 != t2)
        return 7;
    if (t1 == t3)
        return 8;
    if (!(t1 != t3))
        return 9;

    return 0;
}

int
test_tuple_assignment()
{
    ctl::tuple<int, double, ctl::string> t1(1, 2.5, "hello");
    ctl::tuple<int, double, ctl::string> t2;
    t2 = t1;
    if (!(t1 == t2))
        return 10;
    return 0;
}

int
test_tuple_move()
{
    ctl::tuple<int, double, ctl::string> t1(1, 2.5, "hello");
    ctl::tuple<int, double, ctl::string> t2(ctl::move(t1));
    if (ctl::get<0>(t2) != 1 || ctl::get<1>(t2) != 2.5 ||
        ctl::get<2>(t2) != "hello") {
        return 11;
    }
    return 0;
}

int
test_empty_tuple()
{
    ctl::tuple<> t;
    ctl::tuple<> t2;
    if (!(t == t2))
        return 12;
    return 0;
}

int
test_single_element_tuple()
{
    ctl::tuple<int> t(42);
    if (ctl::get<0>(t) != 42)
        return 13;
    return 0;
}

int
main()
{
    int result;

    result = test_tuple_creation();
    if (result != 0)
        return result;

    result = test_make_tuple();
    if (result != 0)
        return result;

    result = test_tuple_get();
    if (result != 0)
        return result;

    result = test_tuple_comparison();
    if (result != 0)
        return result;

    result = test_tuple_assignment();
    if (result != 0)
        return result;

    result = test_tuple_move();
    if (result != 0)
        return result;

    result = test_empty_tuple();
    if (result != 0)
        return result;

    result = test_single_element_tuple();
    if (result != 0)
        return result;

    CheckForMemoryLeaks();
}
