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

template<typename T, typename D = ctl::default_delete<T>>
using Ptr = ctl::unique_ptr<T, D>;

template<typename T, typename... Args>
Ptr<T>
Mk(Args&&... args)
{
    return ctl::make_unique<T, Args...>(ctl::forward<Args>(args)...);
}

template<typename T>
Ptr<T>
MkRaw()
{
    return ctl::make_unique_for_overwrite<T>();
}

// #undef ctl

static int g = 0;

struct SetsGDeleter
{
    void operator()(auto* x) const noexcept
    {
        ++g;
        delete x;
    }
};

struct StatefulDeleter
{
    char state;
    void operator()(auto* x) const noexcept
    {
    }
};

struct FinalDeleter final
{
    void operator()(auto* x) const noexcept
    {
    }
};

static_assert(sizeof(Ptr<int, SetsGDeleter>) == sizeof(int*));

// not everyone uses [[no_unique_address]]...
static_assert(!ctl::is_same_v<Ptr<int>, ctl::unique_ptr<int>> ||
              sizeof(Ptr<int, FinalDeleter>) == sizeof(int*));

struct SetsGCtor
{
    SetsGCtor()
    {
        ++g;
    }
};

struct SetsGDtor
{
    ~SetsGDtor()
    {
        ++g;
    }
};

struct Base
{};

struct Derived : Base
{};

int
main()
{

    {
        Ptr<int> x(new int(5));
    }

    {
        Ptr<int, SetsGDeleter> x(new int());
        x.reset();
        if (g != 1)
            return 1;
    }

    {
        g = 0;
        Ptr<int, SetsGDeleter> x(new int());
        delete x.release();
        x.reset();
        if (g)
            return 17;
    }

    {
        Ptr<int> x(new int(5)), y(new int(6));
        x.swap(y);
        if (*x != 6 || *y != 5)
            return 2;
    }

    {
        Ptr<int> x;
        if (x)
            return 3;
        x.reset(new int(5));
        if (!x)
            return 4;
    }

    {
        g = 0;
        Ptr<SetsGCtor> x;
        if (g)
            return 5;
        x = Mk<SetsGCtor>();
        if (g != 1)
            return 6;
    }

    {
        g = 0;
        auto x = Mk<SetsGDtor>();
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
        Ptr<SetsGDtor> x, y;
        x = Mk<SetsGDtor>();
        y = Mk<SetsGDtor>();
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
            auto x = Mk<SetsGDtor>();
        }
        if (g != 1)
            return 12;
    }

    {
        g = 0;
        {
            auto x = Mk<SetsGDtor>();
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
        auto x = MkRaw<DefaultInitialized>();
        if (g)
            return 14;
        x.reset();
        if (g)
            return 15;
        x = Mk<DefaultInitialized>();
        if (g != 1)
            return 16;
    }
#endif

    {
        int a;
        // Should compile.
        Ptr<int, FinalDeleter> x(&a);
        Ptr<int, StatefulDeleter> y(&a);
    }

    {
        Ptr<Base> x(new Base);
        x.reset(new Derived);

        Ptr<Derived> y(new Derived);
        Ptr<Base> z(ctl::move(y));
    }

    CheckForMemoryLeaks();
}
