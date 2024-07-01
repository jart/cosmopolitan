// -*-mode:c++;indent-tabs-mode:nil;c-basic-offset:4;tab-width:8;coding:utf-8-*-
// vi: set et ft=cpp ts=4 sts=4 sw=4 fenc=utf-8 :vi
#ifndef CTL_IS_ARRAY_H_
#define CTL_IS_ARRAY_H_
#include "integral_constant.h"

namespace ctl {

template<typename T>
struct is_array : ctl::false_type
{};

template<typename T, size_t N>
struct is_array<T[N]> : ctl::true_type
{};

template<typename T>
struct is_array<T[]> : ctl::true_type
{};

template<typename T>
inline constexpr bool is_array_v = is_array<T>::value;

} // namespace ctl

#endif // CTL_IS_ARRAY_H_
