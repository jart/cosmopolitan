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

#include "ostream.h"
#include "dubble.h"
#include "libc/fmt/itoa.h"
#include "libc/stdio/stdio.h"
#include "string_view.h"

namespace ctl {

extern const double_conversion::DoubleToStringConverter kDoubleToPrintfG;

ostream cout(stdout);
ostream cerr(stderr);

ostream::~ostream() = default;

ostream::ostream(FILE* file) : ios(file)
{
}

ostream::ostream(ostream&& other) noexcept : ios(other.file_)
{
    other.file_ = nullptr;
}

ostream&
ostream::operator=(ostream&& other) noexcept
{
    if (this != &other) {
        file_ = other.file_;
        other.file_ = nullptr;
    }
    return *this;
}

ostream&
ostream::operator<<(const char* str)
{
    if (good() && str)
        if (fputs(str, file_) < 0)
            setstate(badbit);
    return *this;
}

ostream&
ostream::operator<<(char c)
{
    if (good())
        if (fputc(c, file_) == EOF)
            setstate(badbit);
    return *this;
}

ostream&
ostream::operator<<(int n)
{
    if (good()) {
        char buf[12];
        FormatInt32(buf, n);
        if (fputs(buf, file_) < 0)
            setstate(badbit);
    }
    return *this;
}

ostream&
ostream::operator<<(unsigned n)
{
    if (good()) {
        char buf[12];
        FormatUint32(buf, n);
        if (fputs(buf, file_) < 0)
            setstate(badbit);
    }
    return *this;
}

ostream&
ostream::operator<<(long n)
{
    if (good()) {
        char buf[21];
        FormatInt64(buf, n);
        if (fputs(buf, file_) < 0)
            setstate(badbit);
    }
    return *this;
}

ostream&
ostream::operator<<(unsigned long n)
{
    if (good()) {
        char buf[21];
        FormatUint64(buf, n);
        if (fputs(buf, file_) < 0)
            setstate(badbit);
    }
    return *this;
}

ostream&
ostream::operator<<(float f)
{
    if (good()) {
        char buf[128];
        double_conversion::StringBuilder b(buf, sizeof(buf));
        kDoubleToPrintfG.ToShortestSingle(f, &b);
        b.Finalize();
        if (fputs(buf, file_) < 0)
            setstate(badbit);
    }
    return *this;
}

ostream&
ostream::operator<<(double d)
{
    if (good()) {
        char buf[128];
        double_conversion::StringBuilder b(buf, sizeof(buf));
        kDoubleToPrintfG.ToShortest(d, &b);
        b.Finalize();
        if (fputs(buf, file_) < 0)
            setstate(badbit);
    }
    return *this;
}

ostream&
ostream::operator<<(const string_view& s)
{
    if (good() && s.size())
        if (!fwrite(s.data(), s.size(), 1, file_))
            setstate(badbit);
    return *this;
}

ostream&
ostream::operator<<(bool b)
{
    if (good()) {
        const char* value =
          (flags() & boolalpha) ? (b ? "true" : "false") : (b ? "1" : "0");
        if (fputs(value, file_) < 0)
            setstate(badbit);
    }
    return *this;
}

ostream&
ostream::operator<<(ostream& (*manip)(ostream&))
{
    return manip(*this);
}

ostream&
ostream::put(char c)
{
    if (good())
        if (fputc(c, file_) == EOF)
            setstate(badbit);
    return *this;
}

ostream&
ostream::write(const char* s, streamsize n)
{
    if (good())
        if (fwrite(s, 1, n, file_) != static_cast<size_t>(n))
            setstate(badbit);
    return *this;
}

ostream&
ostream::flush()
{
    if (good())
        if (fflush(file_) != 0)
            setstate(badbit);
    return *this;
}

ostream&
endl(ostream& os)
{
    return os.put('\n').flush();
}

ostream&
ends(ostream& os)
{
    return os.put('\0');
}

ostream&
flush(ostream& os)
{
    return os.flush();
}

} // namespace ctl
