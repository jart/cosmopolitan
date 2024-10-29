// -*-mode:c++;indent-tabs-mode:nil;c-basic-offset:4;tab-width:8;coding:utf-8-*-
// vi: set et ft=cpp ts=4 sts=4 sw=4 fenc=utf-8 :vi
#ifndef CTL_STRING_H_
#define CTL_STRING_H_
#include "reverse_iterator.h"
#include "string_view.h"

namespace ctl {

class string;

ctl::string strcat(ctl::string_view, ctl::string_view) noexcept __wur;

namespace __ {

constexpr size_t string_size = 3 * sizeof(size_t);
constexpr size_t sso_max = string_size - 1;
constexpr size_t big_mask = ~(1ull << (8ull * sizeof(size_t) - 1ull));

struct small_string
{
    char buf[sso_max];
    // interpretation is: size == sso_max - rem
    unsigned char rem;
#if 0
    size_t rem : 7;
    size_t big : 1 /* = 0 */;
#endif
};

struct big_string
{
    char* p;
    size_t n;
    // interpretation is: capacity == c & big_mask
    size_t c;
#if 0
    size_t c : sizeof(size_t) * 8 - 1;
    size_t big : 1 /* = 1 */;
#endif
};

} // namespace __

class string
{
  public:
    using value_type = char;
    using size_type = size_t;
    using difference_type = ptrdiff_t;
    using pointer = value_type*;
    using const_pointer = const value_type*;
    using reference = value_type&;
    using const_reference = const value_type&;
    using iterator = pointer;
    using const_iterator = const_pointer;
    using reverse_iterator = ctl::reverse_iterator<iterator>;
    using const_reverse_iterator = ctl::reverse_iterator<const_iterator>;

    static constexpr size_t npos = -1;

    string() noexcept
    {
        __builtin_memset(blob, 0, sizeof(size_t) * 2);
        // equivalent to set_small_size(0) but also zeroes memory
        *(((size_t*)blob) + 2) = __::sso_max << (sizeof(size_t) - 1) * 8;
    }

    string(const ctl::string_view s) noexcept
    {
        if (s.n <= __::sso_max) {
            __builtin_memcpy(blob, s.p, s.n);
            __builtin_memset(blob + s.n, 0, __::sso_max - s.n);
            set_small_size(s.n);
        } else {
            init_big(s);
        }
    }

    explicit string(const size_t n, const char ch = 0) noexcept
    {
        if (n <= __::sso_max) {
            __builtin_memset(blob, ch, n);
            __builtin_memset(blob + n, 0, __::sso_max - n);
            set_small_size(n);
        } else {
            init_big(n, ch);
        }
    }

    string(const char* const p) noexcept
      : string(ctl::string_view(p, __builtin_strlen(p)))
    {
    }

    string(const string& r) noexcept
    {
        if (r.size() <= __::sso_max) {
            __builtin_memcpy(blob, r.data(), __::string_size);
            set_small_size(r.size());
        } else {
            init_big(r);
        }
    }

    string(const char* const p, const size_t n) noexcept
      : string(ctl::string_view(p, n))
    {
    }

    ~string() /* noexcept */
    {
        if (isbig())
            destroy_big();
    }

    string& operator=(string) noexcept;
    const char* c_str() const noexcept;

    void pop_back() noexcept;
    void grow(size_t) noexcept;
    void reserve(size_t) noexcept;
    void resize(size_t, char = 0) noexcept;
    void append(char) noexcept;
    void append(char, size_t) noexcept;
    void append(unsigned long) noexcept;
    void append(const void*, size_t) noexcept;
    void append(const ctl::string_view&, size_t, size_t = npos) noexcept;
    string& insert(size_t, ctl::string_view) noexcept;
    string& erase(size_t = 0, size_t = npos) noexcept;
    string substr(size_t = 0, size_t = npos) const noexcept;
    string& replace(size_t, size_t, ctl::string_view) noexcept;
    bool operator==(ctl::string_view) const noexcept;
    bool operator!=(ctl::string_view) const noexcept;
    bool contains(ctl::string_view) const noexcept;
    bool ends_with(ctl::string_view) const noexcept;
    bool starts_with(ctl::string_view) const noexcept;
    size_t find(char, size_t = 0) const noexcept;
    size_t find(ctl::string_view, size_t = 0) const noexcept;
    size_t find_first_of(char, size_t = 0) const noexcept;
    size_t find_first_of(ctl::string_view, size_t = 0) const noexcept;
    size_t find_last_of(char, size_t = npos) const noexcept;
    size_t find_last_of(ctl::string_view, size_t = npos) const noexcept;

    void swap(string& s) noexcept
    {
        ctl::swap(blob, s.blob);
    }

    string(string&& s) noexcept
    {
        __builtin_memcpy(blob, s.blob, __::string_size);
        s.set_small_size(0);
    }

    void clear() noexcept
    {
        if (isbig()) {
            __b.n = 0;
        } else {
            set_small_size(0);
        }
    }

    bool empty() const noexcept
    {
        return isbig() ? !__b.n : __s.rem >= __::sso_max;
    }

    __attribute__((__always_inline__)) char* data() noexcept
    {
        return isbig() ? __b.p : __s.buf;
    }

    __attribute__((__always_inline__)) const char* data() const noexcept
    {
        return isbig() ? __b.p : __s.buf;
    }

    __attribute__((__always_inline__)) size_t size() const noexcept
    {
#if 0
        if (!isbig() && __s.rem > __::sso_max)
            __builtin_trap();
#endif
        return isbig() ? __b.n : __::sso_max - __s.rem;
    }

    size_t length() const noexcept
    {
        return size();
    }

    size_t capacity() const noexcept
    {
#if 0
        if (isbig() && __b.c <= __::sso_max)
            __builtin_trap();
#endif
        return isbig() ? __::big_mask & __b.c : __::string_size;
    }

    iterator begin() noexcept
    {
        return data();
    }

    const_iterator begin() const noexcept
    {
        return data();
    }

    const_iterator cbegin() const noexcept
    {
        return data();
    }

    reverse_iterator rbegin() noexcept
    {
        return reverse_iterator(end());
    }

    const_reverse_iterator rbegin() const noexcept
    {
        return const_reverse_iterator(end());
    }

    const_reverse_iterator crbegin() const noexcept
    {
        return const_reverse_iterator(end());
    }

    iterator end() noexcept
    {
        return data() + size();
    }

    const_iterator end() const noexcept
    {
        return data() + size();
    }

    const_iterator cend() const noexcept
    {
        return data() + size();
    }

    reverse_iterator rend() noexcept
    {
        return reverse_iterator(begin());
    }

    const_reverse_iterator rend() const noexcept
    {
        return const_reverse_iterator(cbegin());
    }

    const_reverse_iterator crend() const noexcept
    {
        return const_reverse_iterator(cbegin());
    }

    char& front()
    {
        if (!size())
            __builtin_trap();
        return data()[0];
    }

    const char& front() const
    {
        if (!size())
            __builtin_trap();
        return data()[0];
    }

    char& back()
    {
        if (!size())
            __builtin_trap();
        return data()[size() - 1];
    }

    const char& back() const
    {
        if (!size())
            __builtin_trap();
        return data()[size() - 1];
    }

    char& operator[](size_t i) noexcept
    {
        if (i >= size())
            __builtin_trap();
        return data()[i];
    }

    const char& operator[](const size_t i) const noexcept
    {
        if (i >= size())
            __builtin_trap();
        return data()[i];
    }

    void push_back(const char ch) noexcept
    {
        append(ch);
    }

    void append(const ctl::string_view& s) noexcept
    {
        append(s.p, s.n);
    }

    operator ctl::string_view() const noexcept
    {
        return ctl::string_view(data(), size());
    }

    string& operator=(const char* s) noexcept
    {
        clear();
        append(s);
        return *this;
    }

    string& operator=(const ctl::string_view s) noexcept
    {
        clear();
        append(s);
        return *this;
    }

    string& operator+=(const char x) noexcept
    {
        append(x);
        return *this;
    }

    string& operator+=(const char* s) noexcept
    {
        append(s);
        return *this;
    }

    string& operator+=(const ctl::string s) noexcept
    {
        append(s);
        return *this;
    }

    string& operator+=(const ctl::string_view s) noexcept
    {
        append(s);
        return *this;
    }

    string operator+(const char c) const noexcept
    {
        char s[2] = { c };
        return strcat(*this, s);
    }

    string operator+(const char* s) const noexcept
    {
        return strcat(*this, s);
    }

    string operator+(const string& s) const noexcept
    {
        return strcat(*this, s);
    }

    string operator+(const ctl::string_view s) const noexcept
    {
        return strcat(*this, s);
    }

    int compare(const ctl::string_view s) const noexcept
    {
        return strcmp(*this, s);
    }

    bool operator<(const ctl::string_view s) const noexcept
    {
        return compare(s) < 0;
    }

    bool operator<=(const ctl::string_view s) const noexcept
    {
        return compare(s) <= 0;
    }

    bool operator>(const ctl::string_view s) const noexcept
    {
        return compare(s) > 0;
    }

    bool operator>=(const ctl::string_view s) const noexcept
    {
        return compare(s) >= 0;
    }

  private:
    void destroy_big() noexcept;
    void init_big(const string&) noexcept;
    void init_big(ctl::string_view) noexcept;
    void init_big(size_t, char) noexcept;

    __attribute__((__always_inline__)) bool isbig() const noexcept
    {
        return *(blob + __::sso_max) & 0x80;
    }

    void set_small_size(const size_t size) noexcept
    {
        if (size > __::sso_max)
            __builtin_trap();
        __s.rem = __::sso_max - size;
    }

    void set_big_string(char* const p, const size_t n, const size_t c2) noexcept
    {
        if (c2 > __::big_mask)
            __builtin_trap();
        __b.p = p;
        __b.n = n;
        __b.c = c2 | ~__::big_mask;
    }

    friend string strcat(ctl::string_view, ctl::string_view) noexcept;

    union
    {
        __::big_string __b;
        __::small_string __s;
        char blob[__::string_size];
    };
};

static_assert(sizeof(string) == __::string_size);
static_assert(sizeof(__::small_string) == __::string_size);
static_assert(sizeof(__::big_string) == __::string_size);

ctl::string
to_string(int) noexcept;

ctl::string
to_string(long) noexcept;

ctl::string
to_string(long long) noexcept;

ctl::string
to_string(unsigned) noexcept;

ctl::string
to_string(unsigned long) noexcept;

ctl::string
to_string(unsigned long long) noexcept;

ctl::string
to_string(float) noexcept;

ctl::string
to_string(double) noexcept;

ctl::string
to_string(long double) noexcept;

} // namespace ctl

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wliteral-suffix"
inline ctl::string
operator"" s(const char* s, size_t n)
{
    return ctl::string(s, n);
}
#pragma GCC diagnostic pop

#endif // CTL_STRING_H_
