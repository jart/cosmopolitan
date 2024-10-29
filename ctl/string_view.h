// -*-mode:c++;indent-tabs-mode:nil;c-basic-offset:4;tab-width:8;coding:utf-8-*-
// vi: set et ft=cpp ts=4 sts=4 sw=4 fenc=utf-8 :vi
#ifndef CTL_STRINGVIEW_H_
#define CTL_STRINGVIEW_H_
#include "utility.h"

namespace ctl {

struct string_view;

int
strcmp(const string_view, const string_view) noexcept;

struct string_view
{
    const char* p;
    size_t n;

    using iterator = const char*;
    using const_iterator = const char*;
    static constexpr size_t npos = -1;

    constexpr string_view() noexcept : p(nullptr), n(0)
    {
    }

    constexpr string_view(const char* s) noexcept
      : p(s), n(s ? __builtin_strlen(s) : 0)
    {
    }

    constexpr string_view(const char* s, const size_t n) noexcept : p(s), n(n)
    {
    }

    inline constexpr ~string_view() noexcept
    {
    }

    bool operator==(string_view) const noexcept;
    bool operator!=(string_view) const noexcept;
    bool contains(string_view) const noexcept;
    bool ends_with(string_view) const noexcept;
    bool starts_with(string_view) const noexcept;
    string_view substr(size_t = 0, size_t = npos) const noexcept;
    size_t find(char, size_t = 0) const noexcept;
    size_t find(string_view, size_t = 0) const noexcept;
    size_t find_first_of(char, size_t = 0) const noexcept;
    size_t find_first_of(ctl::string_view, size_t = 0) const noexcept;
    size_t find_last_of(char, size_t = npos) const noexcept;
    size_t find_last_of(ctl::string_view, size_t = npos) const noexcept;

    constexpr string_view& operator=(const string_view s) noexcept
    {
        p = s.p;
        n = s.n;
        return *this;
    }

    constexpr bool empty() const noexcept
    {
        return !n;
    }

    constexpr const char* data() const noexcept
    {
        return p;
    }

    constexpr size_t size() const noexcept
    {
        return n;
    }

    constexpr size_t length() const noexcept
    {
        return n;
    }

    constexpr const char& operator[](const size_t i) const noexcept
    {
        if (i >= n)
            __builtin_trap();
        return p[i];
    }

    constexpr void remove_prefix(const size_t count)
    {
        if (count > n)
            __builtin_trap();
        p += count;
        n -= count;
    }

    constexpr void remove_suffix(const size_t count)
    {
        if (count > n)
            __builtin_trap();
        n -= count;
    }

    constexpr const char& front() const
    {
        if (!n)
            __builtin_trap();
        return p[0];
    }

    constexpr const char& back() const
    {
        if (!n)
            __builtin_trap();
        return p[n - 1];
    }

    constexpr const_iterator begin() const noexcept
    {
        return p;
    }

    constexpr const_iterator end() const noexcept
    {
        return p + n;
    }

    constexpr const_iterator cbegin() const noexcept
    {
        return p;
    }

    constexpr const_iterator cend() const noexcept
    {
        return p + n;
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

#endif // CTL_STRINGVIEW_H_
