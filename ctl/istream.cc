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

#include "istream.h"
#include "libc/ctype.h"
#include "libc/fmt/conv.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "numeric_limits.h"
#include "string.h"
#include "utility.h"

namespace ctl {

istream cin(stdin);

istream::~istream() = default;

istream::istream(FILE* file) : ios(file), gcount_(0)
{
}

istream::istream(istream&& other) noexcept
  : ios(other.file_), gcount_(other.gcount_)
{
    other.file_ = nullptr;
    other.gcount_ = 0;
}

istream&
istream::operator=(istream&& other) noexcept
{
    if (this != &other) {
        ios::operator=(ctl::move(other));
        gcount_ = other.gcount_;
        other.gcount_ = 0;
    }
    return *this;
}

istream&
istream::operator>>(char& c)
{
    gcount_ = 0;
    int ch = fgetc(file_);
    if (ch == EOF) {
        setstate(eofbit | failbit);
    } else {
        c = static_cast<char>(ch);
        gcount_ = 1;
    }
    return *this;
}

istream&
istream::operator>>(int& n)
{
    if (fscanf(file_, "%d", &n) != 1)
        setstate(failbit);
    return *this;
}

istream&
istream::operator>>(long& n)
{
    if (fscanf(file_, "%ld", &n) != 1)
        setstate(failbit);
    return *this;
}

istream&
istream::operator>>(double& d)
{
    if (fscanf(file_, "%f", &d) != 1)
        setstate(failbit);
    return *this;
}

istream&
istream::operator>>(ctl::string& s)
{
    char buffer[1024];
    if (fscanf(file_, "%1023s", buffer) == 1) {
        s = buffer;
    } else {
        setstate(failbit);
    }
    return *this;
}

istream&
istream::operator>>(bool& b)
{
    char buffer[6];
    if (fscanf(file_, "%5s", buffer) == 1) {
        if (strcmp(buffer, "true") == 0 || strcmp(buffer, "1") == 0) {
            b = true;
        } else if (strcmp(buffer, "false") == 0 || strcmp(buffer, "0") == 0) {
            b = false;
        } else {
            setstate(failbit);
        }
    } else {
        setstate(failbit);
    }
    return *this;
}

istream&
istream::operator>>(istream& (*manip)(istream&))
{
    return manip(*this);
}

int
istream::get()
{
    gcount_ = 0;
    int ch = fgetc(file_);
    if (ch == EOF) {
        setstate(eofbit);
    } else {
        gcount_ = 1;
    }
    return ch;
}

istream&
istream::get(char& c)
{
    int ch = get();
    if (ch != EOF)
        c = static_cast<char>(ch);
    return *this;
}

istream&
istream::getline(char* s, streamsize n)
{
    return getline(s, n, '\n');
}

istream&
istream::getline(char* s, streamsize n, char delim)
{
    gcount_ = 0;
    if (n <= 0) {
        setstate(failbit);
        return *this;
    }
    while (gcount_ < n - 1) {
        int ch = fgetc(file_);
        if (ch == EOF) {
            setstate(eofbit);
            break;
        }
        if (ch == delim)
            break;
        s[gcount_++] = static_cast<char>(ch);
    }
    s[gcount_] = '\0';
    if (gcount_ == 0)
        setstate(failbit);
    return *this;
}

istream&
istream::read(char* s, streamsize n)
{
    gcount_ = fread(s, 1, n, file_);
    if (gcount_ < n) {
        setstate(eofbit);
        if (gcount_ == 0)
            setstate(failbit);
    }
    return *this;
}

istream::streamsize
istream::gcount() const
{
    return gcount_;
}

int
istream::peek()
{
    int ch = fgetc(file_);
    if (ch != EOF) {
        ungetc(ch, file_);
    } else {
        setstate(eofbit);
    }
    return ch;
}

istream&
istream::ignore(streamsize n, int delim)
{
    gcount_ = 0;
    while (gcount_ < n) {
        int ch = fgetc(file_);
        if (ch == EOF) {
            setstate(eofbit);
            break;
        }
        ++gcount_;
        if (ch == delim)
            break;
    }
    return *this;
}

istream&
ws(istream& is)
{
    int ch;
    while ((ch = is.peek()) != EOF && isspace(ch))
        is.get();
    return is;
}

} // namespace ctl
