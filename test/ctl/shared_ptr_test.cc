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
#include "ctl/shared_ptr.h"
#include "ctl/vector.h"
#include "libc/mem/leaks.h"

// #include <memory>
// #include <vector>
// #define ctl std

using ctl::bad_weak_ptr;
using ctl::enable_shared_from_this;
using ctl::make_shared;
using ctl::move;
using ctl::shared_ptr;
using ctl::unique_ptr;
using ctl::vector;
using ctl::weak_ptr;

#undef ctl

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
    void operator()(auto*) const noexcept
    {
        ++g;
    }
};

struct Base
{};

struct Derived : Base
{};

class SharedThis : public enable_shared_from_this<SharedThis>
{
    struct Private
    {
        explicit Private() = default;
    };

  public:
    SharedThis(Private)
    {
    }

    static shared_ptr<SharedThis> create()
    {
        return make_shared<SharedThis>(Private());
    }
};

class CanShareThis : public enable_shared_from_this<CanShareThis>
{};

// Sample class used to demonstrate the CTL shared_ptr's weak_self feature.
struct Tree : ctl::weak_self_base
{
    ctl::shared_ptr<Tree> l, r;
    ctl::weak_ptr<Tree> p;
    Tree(ctl::weak_ptr<Tree> const& self, auto&& l2, auto&& r2)
      : l(ctl::forward<decltype(l2)>(l2)), r(ctl::forward<decltype(r2)>(r2))
    {
        if (l)
            l->p = self;
        if (r)
            r->p = self;
    }
};

int
main()
{
    int a, b;

    {
        // Shouldn't cause memory leaks.
        shared_ptr<int> x(new int(5));
    }

    {
        // Objects get destroyed when the last shared_ptr is reset.
        shared_ptr<int> x(&a, CallG());
        shared_ptr<int> y(x);
        x.reset();
        if (g)
            return 1;
        y.reset();
        if (g != 1)
            return 2;
    }

    {
        g = 0;
        // Weak pointers don't prevent object destruction.
        shared_ptr<int> x(&a, CallG());
        weak_ptr<int> y(x);
        x.reset();
        if (g != 1)
            return 3;
    }

    {
        g = 0;
        // Weak pointers can be promoted to shared pointers.
        shared_ptr<int> x(&a, CallG());
        weak_ptr<int> y(x);
        auto z = y.lock();
        x.reset();
        if (g)
            return 4;
        y.reset();
        if (g)
            return 5;
        z.reset();
        if (g != 1)
            return 6;
    }

    {
        // Shared null pointers are falsey.
        shared_ptr<int> x;
        if (x)
            return 7;
        x.reset(new int);
        if (!x)
            return 8;
    }

    {
        // You can cast a shared pointer validly.
        shared_ptr<Derived> x(new Derived);
        shared_ptr<Base> y(x);
        // But not invalidly:
        // shared_ptr<Base> x(new Derived);
        // shared_ptr<Derived> y(x);
    }

    {
        // You can cast a shared pointer to void to retain a reference.
        shared_ptr<int> x(new int);
        shared_ptr<void> y(x);
    }

    {
        // You can also create a shared pointer to void in the first place.
        shared_ptr<void> x(new int);
    }

    {
        // You can take a shared pointer to a subobject, and it will free the
        // base object.
        shared_ptr<vector<int>> x(new vector<int>);
        x->push_back(5);
        shared_ptr<int> y(x, &x->at(0));
        x.reset();
        if (*y != 5)
            return 9;
    }

    {
        g = 0;
        // You can create a shared_ptr from a unique_ptr.
        unique_ptr<int, CallG> x(&a, CallG());
        shared_ptr<int> y(move(x));
        if (x)
            return 10;
        y.reset();
        if (g != 1)
            return 11;
    }

    {
        g = 0;
        // You can reassign shared_ptrs.
        shared_ptr<int> x(&a, CallG());
        shared_ptr<int> y;
        y = x;
        x.reset();
        if (g)
            return 12;
        y.reset();
        if (g != 1)
            return 13;
    }

    {
        // owner_before shows equivalence only for equivalent objects.
        shared_ptr<int> x(&a, CallG());
        shared_ptr<int> y(&b, CallG());
        shared_ptr<void> z(x, &b);
        if (z.owner_before(x) || x.owner_before(z))
            return 14;
        if (!z.owner_before(y) && !y.owner_before(z))
            return 15;
    }

    {
        // Use counts work like you'd expect
        shared_ptr<int> x(new int);
        if (x.use_count() != 1)
            return 16;
        shared_ptr<int> y(x);
        if (x.use_count() != 2 || y.use_count() != 2)
            return 17;
        x.reset();
        if (x.use_count() != 0 || y.use_count() != 1)
            return 18;
    }

    {
        // There is a make_shared that will allocate an object for you safely.
        auto x = make_shared<int>(5);
        if (!x)
            return 19;
        if (*x != 5)
            return 20;
    }

    {
        // Expired weak pointers lock to nullptr, and throw when promoted to
        // shared pointer by constructor.
        auto x = make_shared<int>();
        weak_ptr<int> y(x);
        x.reset();
        if (y.lock())
            return 21;
        int caught = 0;
        try {
            shared_ptr<int> z(y);
        } catch (bad_weak_ptr& e) {
            caught = 1;
        }
        if (!caught)
            return 22;
    }

    {
        // nullptr is always expired.
        shared_ptr<int> x(nullptr);
        weak_ptr<int> y(x);
        if (!y.expired())
            return 23;
    }

    {
        // enable_shared_from_this allows shared pointers to self.
        auto x = SharedThis::create();
        auto y = x->shared_from_this();
        if (x.use_count() != 2 || x.get() != y.get())
            return 24;
        auto z = new CanShareThis();
        auto w = shared_ptr<CanShareThis>(z);
        auto v = w->shared_from_this();
        if (w.use_count() != 2 || w.get() != v.get())
            return 25;
    }

    if constexpr (ctl::is_same_v<shared_ptr<Tree>, ctl::shared_ptr<Tree>>) {
        // Exercise our off-STL make_shared with weak self support.
        auto t = ctl::make_shared<Tree>(
          ctl::make_shared<Tree>(ctl::make_shared<Tree>(nullptr, nullptr),
                                 nullptr),
          ctl::make_shared<Tree>(nullptr, nullptr));
        auto t2 = t->l->l->p.lock()->p.lock();
        if (t.owner_before(t2) || t2.owner_before(t))
            return 26;
        if (!t.owner_before(t->l) && !t->l.owner_before(t))
            return 27;
    }

    // TODO(mrdomino): exercise threads / races. The reference count should be
    // atomically maintained.

    CheckForMemoryLeaks();
    return 0;
}
