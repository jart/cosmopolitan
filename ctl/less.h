// -*-mode:c++;indent-tabs-mode:nil;c-basic-offset:4;tab-width:8;coding:utf-8-*-
// vi: set et ft=cpp ts=4 sts=4 sw=4 fenc=utf-8 :vi
#ifndef CTL_LESS_H_
#define CTL_LESS_H_
#include "utility.h"

namespace ctl {

template<class T = void>
struct less
{
    constexpr bool operator()(const T& lhs, const T& rhs) const
    {
        return lhs < rhs;
    }

    typedef T first_argument_type;
    typedef T second_argument_type;
    typedef bool result_type;
};

template<>
struct less<void>
{
    template<class T, class U>
    constexpr auto operator()(T&& lhs,
                              U&& rhs) const -> decltype(ctl::forward<T>(lhs) <
                                                         ctl::forward<U>(rhs))
    {
        return ctl::forward<T>(lhs) < ctl::forward<U>(rhs);
    }

    typedef void is_transparent;
};

} // namespace ctl

#endif /* CTL_LESS_H_ */
