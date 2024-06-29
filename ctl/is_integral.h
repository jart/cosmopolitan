// -*-mode:c++;indent-tabs-mode:nil;c-basic-offset:4;tab-width:8;coding:utf-8-*-
// vi: set et ft=cpp ts=4 sts=4 sw=4 fenc=utf-8 :vi
#ifndef CTL_IS_INTEGRAL_H_
#define CTL_IS_INTEGRAL_H_
#include "integral_constant.h"

namespace ctl {

template<typename T>
struct is_integral : false_type
{};

template<>
struct is_integral<bool> : true_type
{};

template<>
struct is_integral<char> : true_type
{};

template<>
struct is_integral<signed char> : true_type
{};

template<>
struct is_integral<unsigned char> : true_type
{};

template<>
struct is_integral<short> : true_type
{};

template<>
struct is_integral<unsigned short> : true_type
{};

template<>
struct is_integral<int> : true_type
{};

template<>
struct is_integral<unsigned int> : true_type
{};

template<>
struct is_integral<long> : true_type
{};

template<>
struct is_integral<unsigned long> : true_type
{};

template<>
struct is_integral<long long> : true_type
{};

template<>
struct is_integral<unsigned long long> : true_type
{};

template<>
struct is_integral<char16_t> : true_type
{};

template<>
struct is_integral<char32_t> : true_type
{};

template<>
struct is_integral<wchar_t> : true_type
{};

template<typename T>
inline constexpr bool is_integral_v = is_integral<T>::value;

} // namespace ctl

#endif // CTL_IS_INTEGRAL_H_
