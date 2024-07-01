// -*-mode:c++;indent-tabs-mode:nil;c-basic-offset:4;tab-width:8;coding:utf-8-*-
// vi: set et ft=cpp ts=4 sts=4 sw=4 fenc=utf-8 :vi
#ifndef CTL_IS_INTEGRAL_H_
#define CTL_IS_INTEGRAL_H_
#include "integral_constant.h"

namespace ctl {

template<typename T>
struct is_integral : ctl::false_type
{};

template<>
struct is_integral<bool> : ctl::true_type
{};

template<>
struct is_integral<char> : ctl::true_type
{};

template<>
struct is_integral<signed char> : ctl::true_type
{};

template<>
struct is_integral<unsigned char> : ctl::true_type
{};

template<>
struct is_integral<short> : ctl::true_type
{};

template<>
struct is_integral<unsigned short> : ctl::true_type
{};

template<>
struct is_integral<int> : ctl::true_type
{};

template<>
struct is_integral<unsigned int> : ctl::true_type
{};

template<>
struct is_integral<long> : ctl::true_type
{};

template<>
struct is_integral<unsigned long> : ctl::true_type
{};

template<>
struct is_integral<long long> : ctl::true_type
{};

template<>
struct is_integral<unsigned long long> : ctl::true_type
{};

template<>
struct is_integral<char16_t> : ctl::true_type
{};

template<>
struct is_integral<char32_t> : ctl::true_type
{};

template<>
struct is_integral<wchar_t> : ctl::true_type
{};

template<typename T>
inline constexpr bool is_integral_v = is_integral<T>::value;

} // namespace ctl

#endif // CTL_IS_INTEGRAL_H_
