// -*-mode:c++;indent-tabs-mode:nil;c-basic-offset:4;tab-width:8;coding:utf-8-*-
// vi: set et ft=cpp ts=4 sts=4 sw=4 fenc=utf-8 :vi
#ifndef CTL_ARRAY_H_
#define CTL_ARRAY_H_
#include "initializer_list.h"
#include "out_of_range.h"
#include "reverse_iterator.h"

namespace ctl {

template<typename T, size_t N>
struct array
{
    using value_type = T;
    using size_type = size_t;
    using difference_type = ptrdiff_t;
    using reference = value_type&;
    using const_reference = const value_type&;
    using pointer = value_type*;
    using const_pointer = const value_type*;
    using iterator = value_type*;
    using const_iterator = const value_type*;
    using reverse_iterator = ctl::reverse_iterator<iterator>;
    using const_reverse_iterator = ctl::reverse_iterator<const_iterator>;

    T elems[N];

    constexpr array() = default;

    constexpr array(std::initializer_list<T> init)
    {
        auto it = init.begin();
        for (size_t i = 0; i < N && it != init.end(); ++i, ++it)
            elems[i] = *it;
    }

    constexpr reference at(size_type pos)
    {
        if (pos >= N)
            throw ctl::out_of_range();
        return elems[pos];
    }

    constexpr const_reference at(size_type pos) const
    {
        if (pos >= N)
            throw ctl::out_of_range();
        return elems[pos];
    }

    constexpr reference operator[](size_type pos)
    {
        if (pos >= N)
            __builtin_trap();
        return elems[pos];
    }

    constexpr const_reference operator[](size_type pos) const
    {
        if (pos >= N)
            __builtin_trap();
        return elems[pos];
    }

    constexpr reference front()
    {
        return elems[0];
    }

    constexpr const_reference front() const
    {
        return elems[0];
    }

    constexpr reference back()
    {
        return elems[N - 1];
    }

    constexpr const_reference back() const
    {
        return elems[N - 1];
    }

    constexpr T* data() noexcept
    {
        return elems;
    }

    constexpr const T* data() const noexcept
    {
        return elems;
    }

    constexpr iterator begin() noexcept
    {
        return elems;
    }

    constexpr const_iterator begin() const noexcept
    {
        return elems;
    }

    constexpr iterator end() noexcept
    {
        return elems + N;
    }

    constexpr const_iterator end() const noexcept
    {
        return elems + N;
    }

    constexpr reverse_iterator rbegin() noexcept
    {
        return reverse_iterator(end());
    }

    constexpr const_reverse_iterator rbegin() const noexcept
    {
        return const_reverse_iterator(end());
    }

    constexpr reverse_iterator rend() noexcept
    {
        return reverse_iterator(begin());
    }

    constexpr const_reverse_iterator rend() const noexcept
    {
        return const_reverse_iterator(begin());
    }

    constexpr const_iterator cbegin() const noexcept
    {
        return begin();
    }

    constexpr const_iterator cend() const noexcept
    {
        return end();
    }

    constexpr const_reverse_iterator crbegin() const noexcept
    {
        return rbegin();
    }

    constexpr const_reverse_iterator crend() const noexcept
    {
        return rend();
    }

    constexpr bool empty() const noexcept
    {
        return N == 0;
    }

    constexpr size_type size() const noexcept
    {
        return N;
    }

    constexpr size_type max_size() const noexcept
    {
        return N;
    }

    void fill(const T& value)
    {
        for (size_type i = 0; i < N; ++i) {
            elems[i] = value;
        }
    }

    void swap(array& other) noexcept
    {
        for (size_type i = 0; i < N; ++i) {
            T temp = elems[i];
            elems[i] = other.elems[i];
            other.elems[i] = temp;
        }
    }
};

template<typename T, size_t N>
bool
operator==(const array<T, N>& lhs, const array<T, N>& rhs)
{
    for (size_t i = 0; i < N; ++i) {
        if (!(lhs[i] == rhs[i]))
            return false;
    }
    return true;
}

template<typename T, size_t N>
bool
operator!=(const array<T, N>& lhs, const array<T, N>& rhs)
{
    return !(lhs == rhs);
}

template<typename T, size_t N>
bool
operator<(const array<T, N>& lhs, const array<T, N>& rhs)
{
    for (size_t i = 0; i < N; ++i) {
        if (lhs[i] < rhs[i])
            return true;
        if (rhs[i] < lhs[i])
            return false;
    }
    return false;
}

template<typename T, size_t N>
bool
operator<=(const array<T, N>& lhs, const array<T, N>& rhs)
{
    return !(rhs < lhs);
}

template<typename T, size_t N>
bool
operator>(const array<T, N>& lhs, const array<T, N>& rhs)
{
    return rhs < lhs;
}

template<typename T, size_t N>
bool
operator>=(const array<T, N>& lhs, const array<T, N>& rhs)
{
    return !(lhs < rhs);
}

template<typename T, size_t N>
void
swap(array<T, N>& lhs, array<T, N>& rhs) noexcept
{
    lhs.swap(rhs);
}

} // namespace ctl

#endif // CTL_ARRAY_H_
