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

#include "ios_base.h"

namespace ctl {

ios_base::ios_base() : state_(goodbit), flags_(skipws | dec)
{
}

ios_base::~ios_base() = default;

ios_base::fmtflags
ios_base::flags() const
{
    return static_cast<ios_base::fmtflags>(flags_);
}

ios_base::fmtflags
ios_base::flags(fmtflags fmtfl)
{
    int old = flags_;
    flags_ = fmtfl;
    return static_cast<ios_base::fmtflags>(old);
}

ios_base::fmtflags
ios_base::setf(fmtflags fmtfl)
{
    int old = flags_;
    flags_ |= fmtfl;
    return static_cast<ios_base::fmtflags>(old);
}

ios_base::fmtflags
ios_base::setf(fmtflags fmtfl, fmtflags mask)
{
    int old = flags_;
    flags_ = (flags_ & ~mask) | (fmtfl & mask);
    return static_cast<ios_base::fmtflags>(old);
}

void
ios_base::unsetf(fmtflags mask)
{
    flags_ &= ~mask;
}

ios_base::iostate
ios_base::rdstate() const
{
    return static_cast<ios_base::iostate>(state_);
}

void
ios_base::clear(int state)
{
    state_ = state;
}

void
ios_base::setstate(int state)
{
    state_ |= state;
}

bool
ios_base::good() const
{
    return state_ == goodbit;
}

bool
ios_base::eof() const
{
    return (state_ & eofbit) != 0;
}

bool
ios_base::fail() const
{
    return (state_ & (failbit | badbit)) != 0;
}

bool
ios_base::bad() const
{
    return (state_ & badbit) != 0;
}

bool
ios_base::operator!() const
{
    return fail();
}

ios_base::operator bool() const
{
    return !fail();
}

} // namespace ctl
