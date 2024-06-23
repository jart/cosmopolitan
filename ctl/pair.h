// -*-mode:c++;indent-tabs-mode:nil;c-basic-offset:4;tab-width:8;coding:utf-8-*-
// vi: set et ft=cpp ts=4 sts=4 sw=4 fenc=utf-8 :vi
#ifndef CTL_PAIR_H_
#define CTL_PAIR_H_
#include "utility.h"

namespace ctl {

template<typename T1, typename T2>
struct pair
{
    using first_type = T1;
    using second_type = T2;

    T1 first;
    T2 second;

    constexpr pair() : first(), second()
    {
    }

    constexpr pair(const T1& x, const T2& y) : first(x), second(y)
    {
    }

    constexpr pair(const pair<T1, T2>& p) : first(p.first), second(p.second)
    {
    }

    template<class U1, class U2>
    constexpr pair(U1&& x, U2&& y)
      : first(ctl::forward<U1>(x)), second(ctl::forward<U2>(y))
    {
    }

    template<class U1, class U2>
    constexpr pair(const pair<U1, U2>& p) : first(p.first), second(p.second)
    {
    }

    constexpr pair(pair<T1, T2>&& p)
      : first(ctl::move(p.first)), second(ctl::move(p.second))
    {
    }

    template<class U1, class U2>
    constexpr pair(pair<U1, U2>&& p)
      : first(ctl::move(p.first)), second(ctl::move(p.second))
    {
    }

    pair& operator=(const pair& other)
    {
        first = other.first;
        second = other.second;
        return *this;
    }

    pair& operator=(pair&& other) noexcept
    {
        first = ctl::move(other.first);
        second = ctl::move(other.second);
        return *this;
    }

    template<class U1, class U2>
    pair& operator=(const pair<U1, U2>& other)
    {
        first = other.first;
        second = other.second;
        return *this;
    }

    template<class U1, class U2>
    pair& operator=(pair<U1, U2>&& other)
    {
        first = ctl::move(other.first);
        second = ctl::move(other.second);
        return *this;
    }

    void swap(pair& other) noexcept
    {
        using ctl::swap;
        swap(first, other.first);
        swap(second, other.second);
    }
};

template<class T1, class T2>
constexpr pair<T1, T2>
make_pair(T1&& t, T2&& u)
{
    return pair<T1, T2>(ctl::forward<T1>(t), ctl::forward<T2>(u));
}

// Comparison operators
template<class T1, class T2>
constexpr bool
operator==(const pair<T1, T2>& lhs, const pair<T1, T2>& rhs)
{
    return lhs.first == rhs.first && lhs.second == rhs.second;
}

template<class T1, class T2>
constexpr bool
operator!=(const pair<T1, T2>& lhs, const pair<T1, T2>& rhs)
{
    return !(lhs == rhs);
}

template<class T1, class T2>
constexpr bool
operator<(const pair<T1, T2>& lhs, const pair<T1, T2>& rhs)
{
    return lhs.first < rhs.first ||
           (!(rhs.first < lhs.first) && lhs.second < rhs.second);
}

template<class T1, class T2>
constexpr bool
operator<=(const pair<T1, T2>& lhs, const pair<T1, T2>& rhs)
{
    return !(rhs < lhs);
}

template<class T1, class T2>
constexpr bool
operator>(const pair<T1, T2>& lhs, const pair<T1, T2>& rhs)
{
    return rhs < lhs;
}

template<class T1, class T2>
constexpr bool
operator>=(const pair<T1, T2>& lhs, const pair<T1, T2>& rhs)
{
    return !(lhs < rhs);
}

template<class T1, class T2>
void
swap(pair<T1, T2>& lhs, pair<T1, T2>& rhs) noexcept(noexcept(lhs.swap(rhs)))
{
    lhs.swap(rhs);
}

} // namespace ctl

#endif // CTL_PAIR_H_
