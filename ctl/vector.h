// -*-mode:c++;indent-tabs-mode:nil;c-basic-offset:4;tab-width:8;coding:utf-8-*-
// vi: set et ft=cpp ts=4 sts=4 sw=4 fenc=utf-8 :vi
#ifndef COSMOPOLITAN_CTL_OPTIONAL_H_
#define COSMOPOLITAN_CTL_OPTIONAL_H_
#include "new.h"
#include <__utility/forward.h>
#include <__utility/move.h>
#include <__utility/swap.h>

namespace ctl {

template<typename T>
struct vector
{
    size_t n = 0;
    size_t c = 0;
    T* p = nullptr;

    using iterator = T*;
    using const_iterator = const T*;

    vector() = default;

    ~vector()
    {
        delete[] p;
    }

    vector(const vector& other)
    {
        n = other.n;
        c = other.c;
        p = new T[c];
        for (size_t i = 0; i < n; ++i)
            new (&p[i]) T(other.p[i]);
    }

    vector(vector&& other) noexcept
    {
        n = other.n;
        c = other.c;
        p = other.p;
        other.n = 0;
        other.c = 0;
        other.p = nullptr;
    }

    explicit vector(size_t count, const T& value = T())
    {
        n = count;
        c = count;
        p = new T[c];
        for (size_t i = 0; i < n; ++i)
            new (&p[i]) T(value);
    }

    vector& operator=(const vector& other)
    {
        if (this != &other) {
            T* newData = new T[other.c];
            for (size_t i = 0; i < other.n; ++i) {
                newData[i] = other.p[i];
            }
            delete[] p;
            p = newData;
            n = other.n;
            c = other.c;
        }
        return *this;
    }

    vector& operator=(vector&& other) noexcept
    {
        if (this != &other) {
            delete[] p;
            p = other.p;
            n = other.n;
            c = other.c;
            other.p = nullptr;
            other.n = 0;
            other.c = 0;
        }
        return *this;
    }

    bool empty() const
    {
        return !n;
    }

    size_t size() const
    {
        return n;
    }

    size_t capacity() const
    {
        return c;
    }

    T& operator[](size_t i)
    {
        if (i >= n)
            __builtin_trap();
        return p[i];
    }

    const T& operator[](size_t i) const
    {
        if (i >= n)
            __builtin_trap();
        return p[i];
    }

    iterator begin()
    {
        return p;
    }

    iterator end()
    {
        return p + n;
    }

    const_iterator cbegin() const
    {
        return p;
    }

    const_iterator cend() const
    {
        return p + n;
    }

    T& front()
    {
        if (!n)
            __builtin_trap();
        return p[0];
    }

    const T& front() const
    {
        if (!n)
            __builtin_trap();
        return p[0];
    }

    T& back()
    {
        if (!n)
            __builtin_trap();
        return p[n - 1];
    }

    const T& back() const
    {
        if (!n)
            __builtin_trap();
        return p[n - 1];
    }

    void clear()
    {
        for (size_t i = 0; i < n; ++i)
            p[i].~T();
        n = 0;
    }

    void reserve(size_t c2)
    {
        if (c2 <= c)
            return;
        T* newP = new T[c2];
        for (size_t i = 0; i < n; ++i)
            newP[i] = std::move(p[i]);
        delete[] p;
        p = newP;
        c = c2;
    }

    void push_back(const T& e)
    {
        if (n == c) {
            size_t c2 = c + 1;
            c2 += c2 >> 1;
            reserve(c2);
        }
        new (&p[n]) T(e);
        ++n;
    }

    void push_back(T&& e)
    {
        if (n == c) {
            size_t c2 = c + 1;
            c2 += c2 >> 1;
            reserve(c2);
        }
        new (&p[n]) T(std::forward<T>(e));
        ++n;
    }

    template<typename... Args>
    void emplace_back(Args&&... args)
    {
        if (n == c) {
            size_t c2 = c + 1;
            c2 += c2 >> 1;
            reserve(c2);
        }
        new (&p[n]) T(std::forward<Args>(args)...);
        ++n;
    }

    void pop_back()
    {
        if (n > 0) {
            --n;
            p[n].~T();
        }
    }

    void resize(size_t n2)
    {
        if (n2 > n) {
            reserve(n2);
            for (size_t i = n; i < n2; ++i)
                new (&p[i]) T();
        } else if (n2 < n) {
            for (size_t i = n2; i < n; ++i)
                p[i].~T();
        }
        n = n2;
    }

    void swap(vector& other) noexcept
    {
        std::swap(n, other.n);
        std::swap(c, other.c);
        std::swap(p, other.p);
    }
};

} // namespace ctl

#endif // COSMOPOLITAN_CTL_OPTIONAL_H_
