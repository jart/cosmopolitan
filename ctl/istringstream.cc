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

#include "istringstream.h"
#include "libc/ctype.h"
#include "libc/fmt/conv.h"
#include "libc/str/str.h"

namespace ctl {

istringstream::istringstream() : buffer_(), read_pos_(0)
{
}

istringstream::istringstream(const ctl::string_view& str)
  : buffer_(str), read_pos_(0)
{
}

ctl::string
istringstream::str() const
{
    return buffer_;
}

void
istringstream::str(const ctl::string& s)
{
    buffer_ = s;
    read_pos_ = 0;
    clear();
}

istringstream&
istringstream::operator>>(char& c)
{
    if (good() && read_pos_ < buffer_.size()) {
        c = buffer_[read_pos_++];
    } else {
        setstate(ios_base::failbit);
    }
    return *this;
}

istringstream&
istringstream::operator>>(char* s)
{
    if (!good())
        return *this;

    while (read_pos_ < buffer_.size() && isspace(buffer_[read_pos_]))
        ++read_pos_;

    size_t start = read_pos_;
    while (read_pos_ < buffer_.size() && !isspace(buffer_[read_pos_])) {
        s[read_pos_ - start] = buffer_[read_pos_];
        ++read_pos_;
    }
    s[read_pos_ - start] = '\0';

    if (start == read_pos_) {
        setstate(ios_base::failbit);
    } else if (read_pos_ == buffer_.size()) {
        setstate(ios_base::eofbit);
    }

    return *this;
}

istringstream&
istringstream::operator>>(ctl::string& s)
{
    if (!good())
        return *this;

    s.clear();
    while (read_pos_ < buffer_.size() && isspace(buffer_[read_pos_]))
        ++read_pos_;

    while (read_pos_ < buffer_.size() && !isspace(buffer_[read_pos_])) {
        s.push_back(buffer_[read_pos_]);
        ++read_pos_;
    }

    if (s.empty()) {
        setstate(ios_base::failbit);
    } else if (read_pos_ == buffer_.size()) {
        setstate(ios_base::eofbit);
    }

    return *this;
}

template<typename T>
istringstream&
istringstream::read_numeric(T& value)
{
    if (!good())
        return *this;

    while (read_pos_ < buffer_.size() && isspace(buffer_[read_pos_]))
        ++read_pos_;

    // size_t start = read_pos_;
    bool is_negative = false;
    if (read_pos_ < buffer_.size() &&
        (buffer_[read_pos_] == '-' || buffer_[read_pos_] == '+')) {
        is_negative = (buffer_[read_pos_] == '-');
        ++read_pos_;
    }

    T result = 0;
    bool valid = false;
    while (read_pos_ < buffer_.size() && isdigit(buffer_[read_pos_])) {
        result = result * 10 + (buffer_[read_pos_] - '0');
        ++read_pos_;
        valid = true;
    }

    if (valid) {
        value = is_negative ? -result : result;
        if (read_pos_ == buffer_.size())
            setstate(ios_base::eofbit);
    } else {
        setstate(ios_base::failbit);
    }

    return *this;
}

istringstream&
istringstream::operator>>(short& n)
{
    return read_numeric(n);
}

istringstream&
istringstream::operator>>(unsigned short& n)
{
    return read_numeric(n);
}

istringstream&
istringstream::operator>>(int& n)
{
    return read_numeric(n);
}

istringstream&
istringstream::operator>>(unsigned int& n)
{
    return read_numeric(n);
}

istringstream&
istringstream::operator>>(long& n)
{
    return read_numeric(n);
}

istringstream&
istringstream::operator>>(unsigned long& n)
{
    return read_numeric(n);
}

istringstream&
istringstream::operator>>(float& f)
{
    if (!good())
        return *this;

    char* end;
    f = strtof(buffer_.c_str() + read_pos_, &end);

    if (end == buffer_.c_str() + read_pos_) {
        setstate(ios_base::failbit);
    } else {
        read_pos_ = end - buffer_.c_str();
        if (read_pos_ == buffer_.size())
            setstate(ios_base::eofbit);
    }

    return *this;
}

istringstream&
istringstream::operator>>(double& d)
{
    if (!good())
        return *this;

    char* end;
    d = strtod(buffer_.c_str() + read_pos_, &end);

    if (end == buffer_.c_str() + read_pos_) {
        setstate(ios_base::failbit);
    } else {
        read_pos_ = end - buffer_.c_str();
        if (read_pos_ == buffer_.size())
            setstate(ios_base::eofbit);
    }

    return *this;
}

} // namespace ctl
