// -*-mode:c++;indent-tabs-mode:nil;c-basic-offset:4;tab-width:8;coding:utf-8-*-
// vi: set et ft=c++ ts=4 sts=4 sw=4 fenc=utf-8 :vi
#ifndef COSMOPOLITAN_CTL_STRING_H_
#define COSMOPOLITAN_CTL_STRING_H_
#include "string_view.h"

namespace ctl {

struct string;

string
strcat(const string_view, const string_view) noexcept __wur;

struct string
{
    char* p = nullptr;
    size_t n = 0;
    size_t c = 0;

    using iterator = char*;
    using const_iterator = const char*;
    static constexpr size_t npos = -1;

    ~string() noexcept;
    string() = default;
    string(const string_view) noexcept;
    string(const char*) noexcept;
    string(const string&) noexcept;
    string(const char*, size_t) noexcept;
    explicit string(size_t, char = 0) noexcept;
    string& operator=(string&&) noexcept;
    const char* c_str() const noexcept;

    void pop_back() noexcept;
    void grow(size_t) noexcept;
    void reserve(size_t) noexcept;
    void resize(size_t, char = 0) noexcept;
    void append(char) noexcept;
    void append(char, size_t) noexcept;
    void append(unsigned long) noexcept;
    void append(const void*, size_t) noexcept;
    string& insert(size_t, const string_view) noexcept;
    string& erase(size_t = 0, size_t = npos) noexcept;
    string substr(size_t = 0, size_t = npos) const noexcept;
    string& replace(size_t, size_t, const string_view&) noexcept;
    bool operator==(const string_view) const noexcept;
    bool operator!=(const string_view) const noexcept;
    bool contains(const string_view) const noexcept;
    bool ends_with(const string_view) const noexcept;
    bool starts_with(const string_view) const noexcept;
    size_t find(char, size_t = 0) const noexcept;
    size_t find(const string_view, size_t = 0) const noexcept;

    string(string&& s) noexcept : p(s.p), n(s.n), c(s.c)
    {
        s.p = nullptr;
        s.n = 0;
        s.c = 0;
    }

    void clear() noexcept
    {
        n = 0;
    }

    bool empty() const noexcept
    {
        return !n;
    }

    char* data() const noexcept
    {
        return p;
    }

    size_t size() const noexcept
    {
        return n;
    }

    size_t length() const noexcept
    {
        return n;
    }

    size_t capacity() const noexcept
    {
        return c;
    }

    iterator begin() noexcept
    {
        return p;
    }

    iterator end() noexcept
    {
        return p + n;
    }

    const_iterator cbegin() const noexcept
    {
        return p;
    }

    const_iterator cend() const noexcept
    {
        return p + n;
    }

    char& front()
    {
        if (!n)
            __builtin_trap();
        return p[0];
    }

    const char& front() const
    {
        if (!n)
            __builtin_trap();
        return p[0];
    }

    char& back()
    {
        if (!n)
            __builtin_trap();
        return p[n - 1];
    }

    const char& back() const
    {
        if (!n)
            __builtin_trap();
        return p[n - 1];
    }

    char& operator[](size_t i) noexcept
    {
        if (i >= n)
            __builtin_trap();
        return p[i];
    }

    const char& operator[](size_t i) const noexcept
    {
        if (i >= n)
            __builtin_trap();
        return p[i];
    }

    void push_back(char ch) noexcept
    {
        append(ch);
    }

    void append(const string_view s) noexcept
    {
        append(s.p, s.n);
    }

    inline constexpr operator string_view() const noexcept
    {
        return string_view(p, n);
    }

    string& operator=(const char* s) noexcept
    {
        clear();
        append(s);
        return *this;
    }

    string& operator=(const string_view s) noexcept
    {
        clear();
        append(s);
        return *this;
    }

    string& operator+=(char x) noexcept
    {
        append(x);
        return *this;
    }

    string& operator+=(const string_view s) noexcept
    {
        append(s);
        return *this;
    }

    string operator+(const string_view s) const noexcept
    {
        return strcat(*this, s);
    }

    int compare(const string_view s) const noexcept
    {
        return strcmp(*this, s);
    }

    bool operator<(const string_view s) const noexcept
    {
        return compare(s) < 0;
    }

    bool operator<=(const string_view s) const noexcept
    {
        return compare(s) <= 0;
    }

    bool operator>(const string_view s) const noexcept
    {
        return compare(s) > 0;
    }

    bool operator>=(const string_view s) const noexcept
    {
        return compare(s) >= 0;
    }
};

} // namespace ctl

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wliteral-suffix"
inline ctl::string
operator"" s(const char* s, size_t n)
{
    return ctl::string(s, n);
}
#pragma GCC diagnostic pop

#endif // COSMOPOLITAN_CTL_STRING_H_
