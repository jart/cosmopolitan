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

#include "type_traits.h"

// #include <type_traits>
// #define ctl std

using namespace ctl;

#undef ctl

static_assert(is_void_v<void>);
static_assert(is_void_v<const void>);
static_assert(is_void_v<volatile const void>);

static_assert(is_same_v<int, int>);
static_assert(!is_same_v<int, bool>);

struct A
{
};

using B = A;

struct C
{
};

struct Base
{
};

struct Derived : Base
{
};

static_assert(is_same_v<A, A>);
static_assert(is_same_v<A, B>);
static_assert(!is_same_v<A, C>);
static_assert(!is_same_v<Base, Derived>);

static_assert(is_convertible_v<int, bool>);
static_assert(is_convertible_v<Derived, Base>);
static_assert(!is_convertible_v<Base, Derived>);
static_assert(is_convertible_v<Derived*, Base*>);
static_assert(!is_convertible_v<Base*, Derived*>);

static_assert(is_convertible_v<A&, const A&>);
static_assert(is_convertible_v<A&&, const A&>);
static_assert(!is_convertible_v<const A&, A&>);
static_assert(!is_convertible_v<A&, A&&>);
static_assert(!is_convertible_v<A&&, A&>);

static_assert(is_same_v<int, remove_extent_t<int[]>>);
static_assert(is_same_v<int, remove_extent_t<int[5]>>);
