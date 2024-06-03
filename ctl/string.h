// -*-mode:c++;indent-tabs-mode:nil;c-basic-offset:4;tab-width:8;coding:utf-8-*-
// vi: set et ft=c++ ts=4 sts=4 sw=4 fenc=utf-8 :vi
#ifndef COSMOPOLITAN_CTL_STRING_H_
#define COSMOPOLITAN_CTL_STRING_H_

struct String;

struct StringView
{
    const char* p;
    size_t n;

    static constexpr size_t npos = -1;

    constexpr StringView(const char* s) noexcept
      : p(s), n(s ? __builtin_strlen(s) : 0)
    {
    }

    constexpr StringView(const char* s, size_t n) noexcept : p(s), n(n)
    {
    }

    inline constexpr ~StringView() noexcept
    {
    }

    int compare(const StringView) const noexcept;
    bool operator==(const StringView) const noexcept;
    bool operator!=(const StringView) const noexcept;
    bool contains(const StringView) const noexcept;
    String operator+(const StringView) const noexcept;
    bool ends_with(const StringView) const noexcept;
    bool starts_with(const StringView) const noexcept;
    StringView substr(size_t = 0, size_t = npos) const noexcept;
    size_t find(char, size_t = 0) const noexcept;
    size_t find(const StringView, size_t = 0) const noexcept;

    constexpr StringView& operator=(const StringView& s) noexcept
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

    constexpr const char& operator[](size_t i) const noexcept
    {
        if (i >= n)
            __builtin_trap();
        return p[i];
    }

    constexpr void remove_prefix(size_t count)
    {
        if (count > n)
            __builtin_trap();
        p += count;
        n -= count;
    }

    constexpr void remove_suffix(size_t count)
    {
        if (count > n)
            __builtin_trap();
        n -= count;
    }

    bool operator<(const StringView& s) const noexcept
    {
        return compare(s) < 0;
    }

    bool operator<=(const StringView& s) const noexcept
    {
        return compare(s) <= 0;
    }

    bool operator>(const StringView& s) const noexcept
    {
        return compare(s) > 0;
    }

    bool operator>=(const StringView& s) const noexcept
    {
        return compare(s) >= 0;
    }
};

struct String
{
    char* p = nullptr;
    size_t n = 0;
    size_t c = 0;

    using iterator = char*;
    using const_iterator = const char*;
    static constexpr size_t npos = -1;

    ~String() noexcept;
    String() = default;
    String(const StringView) noexcept;
    String(const char*) noexcept;
    String(const String&) noexcept;
    String(const char*, size_t) noexcept;
    explicit String(size_t, char = 0) noexcept;
    String& operator=(String&&) noexcept;
    const char* c_str() const noexcept;

    void pop_back() noexcept;
    void grow(size_t) noexcept;
    void reserve(size_t) noexcept;
    void resize(size_t, char = 0) noexcept;
    void append(char) noexcept;
    void append(char, size_t) noexcept;
    void append(unsigned long) noexcept;
    void append(const void*, size_t) noexcept;
    String& insert(size_t, const StringView) noexcept;
    String& erase(size_t = 0, size_t = npos) noexcept;
    String operator+(const StringView) const noexcept;
    String substr(size_t = 0, size_t = npos) const noexcept;
    String& replace(size_t, size_t, const StringView&) noexcept;
    bool operator==(const StringView) const noexcept;
    bool operator!=(const StringView) const noexcept;
    bool contains(const StringView) const noexcept;
    bool ends_with(const StringView) const noexcept;
    bool starts_with(const StringView) const noexcept;
    int compare(const StringView) const noexcept;
    size_t find(char, size_t = 0) const noexcept;
    size_t find(const StringView, size_t = 0) const noexcept;

    String(String&& s) noexcept : p(s.p), n(s.n), c(s.c)
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

    void append(const StringView s) noexcept
    {
        append(s.p, s.n);
    }

    inline constexpr operator StringView() const noexcept
    {
        return StringView(p, n);
    }

    String& operator=(const char* s) noexcept
    {
        clear();
        append(s);
        return *this;
    }

    String& operator=(const StringView s) noexcept
    {
        clear();
        append(s);
        return *this;
    }

    String& operator+=(char x) noexcept
    {
        append(x);
        return *this;
    }

    String& operator+=(const StringView s) noexcept
    {
        append(s);
        return *this;
    }

    bool operator<(const StringView s) const noexcept
    {
        return compare(s) < 0;
    }

    bool operator<=(const StringView s) const noexcept
    {
        return compare(s) <= 0;
    }

    bool operator>(const StringView s) const noexcept
    {
        return compare(s) > 0;
    }

    bool operator>=(const StringView s) const noexcept
    {
        return compare(s) >= 0;
    }
};

#pragma GCC diagnostic ignored "-Wliteral-suffix"

inline String
operator"" s(const char* s, size_t n)
{
    return String(s, n);
}

#endif // COSMOPOLITAN_CTL_STRING_H_
