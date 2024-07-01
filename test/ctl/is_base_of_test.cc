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

#include "ctl/is_base_of.h"

// #include <type_traits>
// #define ctl std

// Test classes
class A
{};
class B : public A
{};
class C : public B
{};
class D
{};

struct Empty
{};

template<typename T>
class TemplateClass
{};

class VirtualBase
{};
class VirtualDerived : virtual public VirtualBase
{};

int
main()
{
    // Test basic inheritance
    if (!ctl::is_base_of_v<A, B>)
        return 1;
    if (!ctl::is_base_of_v<A, C>)
        return 2;
    if (ctl::is_base_of_v<B, A>)
        return 3;
    if (ctl::is_base_of_v<C, A>)
        return 4;
    if (ctl::is_base_of_v<A, D>)
        return 5;

    // Test with same type
    if (!ctl::is_base_of_v<A, A>)
        return 6;
    if (!ctl::is_base_of_v<B, B>)
        return 7;

    // Test with void
    if (ctl::is_base_of_v<void, void>)
        return 8;
    if (ctl::is_base_of_v<A, void>)
        return 9;
    if (ctl::is_base_of_v<void, A>)
        return 10;

    // Test with fundamental types
    if (ctl::is_base_of_v<int, int>)
        return 11;
    if (ctl::is_base_of_v<int, double>)
        return 12;
    if (ctl::is_base_of_v<double, int>)
        return 13;

    // Test with empty class
    if (ctl::is_base_of_v<Empty, A>)
        return 14;
    if (ctl::is_base_of_v<A, Empty>)
        return 15;

    // Test with template class
    if (ctl::is_base_of_v<TemplateClass<int>, A>)
        return 16;
    if (ctl::is_base_of_v<A, TemplateClass<int>>)
        return 17;

    // Test with virtual inheritance
    if (!ctl::is_base_of_v<VirtualBase, VirtualDerived>)
        return 18;
    if (ctl::is_base_of_v<VirtualDerived, VirtualBase>)
        return 19;

    return 0; // All tests passed
}
