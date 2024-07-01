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

#include "ostringstream.h"
#include "libc/fmt/itoa.h"
#include "libc/stdio/stdio.h"

namespace ctl {

ostringstream::ostringstream() : buffer_(), write_pos_(0)
{
}

ostringstream::ostringstream(const ctl::string_view& str)
  : buffer_(str), write_pos_(0)
{
}

ctl::string
ostringstream::str() const
{
    return buffer_;
}

void
ostringstream::str(const ctl::string& s)
{
    buffer_ = s;
    write_pos_ = 0;
}

void
ostringstream::clear()
{
    ios_base::clear();
}

ostringstream&
ostringstream::operator<<(char c)
{
    if (good()) {
        if (write_pos_ < buffer_.size()) {
            buffer_[write_pos_++] = c;
        } else {
            buffer_.push_back(c);
            ++write_pos_;
        }
    }
    return *this;
}

ostringstream&
ostringstream::operator<<(const ctl::string_view& s)
{
    if (good()) {
        if (write_pos_ + s.size() <= buffer_.size()) {
            buffer_.replace(write_pos_, s.size(), s);
        } else {
            buffer_.replace(write_pos_, buffer_.size() - write_pos_, s);
            buffer_.append(s.substr(buffer_.size() - write_pos_));
        }
        write_pos_ += s.size();
    }
    return *this;
}

ostringstream&
ostringstream::operator<<(int n)
{
    char temp[12];
    if (good())
        *this << ctl::string_view(temp, FormatInt32(temp, n) - temp);
    return *this;
}

ostringstream&
ostringstream::operator<<(unsigned int n)
{
    char temp[12];
    if (good())
        *this << ctl::string_view(temp, FormatUint32(temp, n) - temp);
    return *this;
}

ostringstream&
ostringstream::operator<<(long n)
{
    char temp[21];
    if (good())
        *this << ctl::string_view(temp, FormatInt64(temp, n) - temp);
    return *this;
}

ostringstream&
ostringstream::operator<<(unsigned long n)
{
    char temp[21];
    if (good())
        *this << ctl::string_view(temp, FormatUint64(temp, n) - temp);
    return *this;
}

ostringstream&
ostringstream::operator<<(float f)
{
    if (good()) {
        char temp[32];
        int len = snprintf(temp, sizeof(temp), "%g", f);
        *this << ctl::string_view(temp, len);
    }
    return *this;
}

ostringstream&
ostringstream::operator<<(double d)
{
    if (good()) {
        char temp[32];
        int len = snprintf(temp, sizeof(temp), "%g", d);
        *this << ctl::string_view(temp, len);
    }
    return *this;
}

} // namespace ctl
