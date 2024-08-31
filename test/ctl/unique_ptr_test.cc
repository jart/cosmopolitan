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
#include "ctl/unique_ptr.h"
#include "libc/mem/leaks.h"

// #include <memory>
// #include <type_traits>
// #define ctl std

using ctl::unique_ptr;
using ctl::make_unique;
using ctl::make_unique_for_overwrite;

#undef ctl

// The following few definitions are used to get observability into aspects of
// an object's lifecycle, to make sure that e.g. constructing a unique_ptr of a
// type does not construct an object, and that make_unique does construct an
// object.
static int g = 0;

struct ConstructG
{
    ConstructG()
    {
        ++g;
    }
};

struct DestructG
{
    ~DestructG()
    {
        ++g;
    }
};

struct CallG
{
    void operator()(auto* x) const noexcept
    {
        ++g;
    }
};

// A unique_ptr with an empty deleter should be the same size as a raw pointer.
static_assert(sizeof(unique_ptr<int, decltype([] {})>) == sizeof(int*));

struct FinalDeleter final
{
    void operator()(auto* x) const noexcept
    {
    }
};

// ctl::unique_ptr does not need to inherit from its deleter for this property;
// the STL often does, though, so we don't hold them to the following.
static_assert(!ctl::is_same_v<unique_ptr<int>, ctl::unique_ptr<int>> ||
              sizeof(unique_ptr<int, FinalDeleter>) == sizeof(int*));

struct Base
{};

struct Derived : Base
{};

int
main()
{
    int a;

    {
        // Shouldn't cause any memory leaks.
        unique_ptr<int> x(new int(5));
    }

    {
        // Deleter is called if the pointer is non-null when reset.
        unique_ptr<int, CallG> x(&a);
        x.reset();
        if (g != 1)
            return 1;
    }

    {
        g = 0;
        // Deleter is not called if the pointer is null when reset.
        unique_ptr<int, CallG> x(&a);
        x.release();
        x.reset();
        if (g)
            return 17;
    }

    {
        g = 0;
        // Deleter is called when the pointer goes out of scope.
        {
            unique_ptr<int, CallG> x(&a);
        }
        if (!g)
            return 18;
    }

    {
        g = 0;
        // Deleter is called if scope ends exceptionally.
        try {
            unique_ptr<int, CallG> x(&a);
            throw 'a';
        } catch (char) {
        }
        if (!g)
            return 19;
    }

    {
        unique_ptr<int> x(new int(5)), y(new int(6));
        x.swap(y);
        if (*x != 6 || *y != 5)
            return 2;
    }

    {
        unique_ptr<int> x;
        if (x)
            return 3;
        x.reset(new int(5));
        if (!x)
            return 4;
    }

    {
        g = 0;
        unique_ptr<ConstructG> x;
        if (g)
            return 5;
        x = make_unique<ConstructG>();
        if (g != 1)
            return 6;
    }

    {
        g = 0;
        auto x = make_unique<DestructG>();
        if (g)
            return 7;
        x.reset();
        if (g != 1)
            return 8;
        if (x)
            return 9;
    }

    {
        g = 0;
        unique_ptr<DestructG> x, y;
        x = make_unique<DestructG>();
        y = make_unique<DestructG>();
#if 0
        // shouldn't compile
        x = y;
#endif
        x = ctl::move(y);
        if (g != 1)
            return 10;
        if (y)
            return 11;
    }

    {
        g = 0;
        {
            auto x = make_unique<DestructG>();
        }
        if (g != 1)
            return 12;
    }

    {
        g = 0;
        {
            auto x = make_unique<DestructG>();
            delete x.release();
        }
        if (g != 1)
            return 13;
    }

#if 0
    // I could not figure out how to test make_unique_for_overwrite. The only
    // side effects it has are illegal to detect?
    {
        g = 0;
        auto x = make_unique_for_overwrite<DefaultInitialized>();
        if (g)
            return 14;
        x.reset();
        if (g)
            return 15;
        x = make_unique<DefaultInitialized>();
        if (g != 1)
            return 16;
    }
#endif

    {
        int a;
        // Should compile.
        unique_ptr<int, FinalDeleter> x(&a);
    }

    {
        unique_ptr<Base> x(new Base);
        x.reset(new Derived);

        unique_ptr<Derived> y(new Derived);
        unique_ptr<Base> z(ctl::move(y));
    }

    CheckForMemoryLeaks();
}
