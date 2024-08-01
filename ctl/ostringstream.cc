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
#include "dubble.h"
#include "libc/fmt/itoa.h"

namespace ctl {

extern const double_conversion::DoubleToStringConverter kDoubleToPrintfG;

ostringstream::ostringstream() : buffer_(), write_pos_(0)
{
}

ostringstream::ostringstream(const string_view& str)
  : buffer_(str), write_pos_(0)
{
}

string
ostringstream::str() const
{
    return buffer_;
}

void
ostringstream::str(const string& s)
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
ostringstream::operator<<(const string_view& s)
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
    if (good()) {
        char buf[12];
        *this << string_view(buf, FormatInt32(buf, n) - buf);
    }
    return *this;
}

ostringstream&
ostringstream::operator<<(unsigned n)
{
    if (good()) {
        char buf[12];
        *this << string_view(buf, FormatUint32(buf, n) - buf);
    }
    return *this;
}

ostringstream&
ostringstream::operator<<(long n)
{
    if (good()) {
        char buf[21];
        *this << string_view(buf, FormatInt64(buf, n) - buf);
    }
    return *this;
}

ostringstream&
ostringstream::operator<<(unsigned long n)
{
    if (good()) {
        char buf[21];
        *this << string_view(buf, FormatUint64(buf, n) - buf);
    }
    return *this;
}

ostringstream&
ostringstream::operator<<(float f)
{
    if (good()) {
        char buf[128];
        double_conversion::StringBuilder b(buf, sizeof(buf));
        kDoubleToPrintfG.ToShortestSingle(f, &b);
        b.Finalize();
        *this << string_view(buf);
    }
    return *this;
}

ostringstream&
ostringstream::operator<<(double d)
{
    if (good()) {
        char buf[128];
        double_conversion::StringBuilder b(buf, sizeof(buf));
        kDoubleToPrintfG.ToShortest(d, &b);
        b.Finalize();
        *this << string_view(buf);
    }
    return *this;
}

} // namespace ctl
