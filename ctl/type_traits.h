// -*-mode:c++;indent-tabs-mode:nil;c-basic-offset:4;tab-width:8;coding:utf-8-*-
// vi: set et ft=cpp ts=4 sts=4 sw=4 fenc=utf-8 :vi
#ifndef COSMOPOLITAN_CTL_TYPE_TRAITS_H_
#define COSMOPOLITAN_CTL_TYPE_TRAITS_H_

#include "utility.h"

namespace ctl {

template <typename T, T V>
struct integral_constant
{
    using value_type = T;
    using type = integral_constant<T, V>;

    static constexpr T value = V;

    constexpr operator value_type() const noexcept
    {
        return V;
    }

    constexpr value_type operator()() const noexcept
    {
        return V;
    }
};

using true_type = integral_constant<bool, true>;
using false_type = integral_constant<bool, false>;

template <typename T, typename U>
struct is_same : false_type
{
};

template <typename T>
struct is_same<T, T> : true_type
{
};

template <typename T, typename U>
inline constexpr bool is_same_v = is_same<T, U>::value;

template <typename T>
struct remove_extent
{
    using type = T;
};

template <typename T>
struct remove_extent<T[]>
{
    using type = T;
};

template <typename T, size_t N>
struct remove_extent<T[N]>
{
    using type = T;
};

template <typename T>
using remove_extent_t = typename remove_extent<T>::type;

template <typename T>
struct remove_cv
{
    using type = T;
};

template <typename T>
struct remove_cv<const T>
{
    using type = T;
};

template <typename T>
struct remove_cv<volatile T>
{
    using type = T;
};

template <typename T>
struct remove_cv<const volatile T>
{
    using type = T;
};

template <typename T>
using remove_cv_t = typename remove_cv<T>::type;

template <typename T>
inline constexpr bool is_void_v = is_same_v<void, remove_cv_t<T>>;

namespace __ {

template <typename T>
auto test_returnable(int) -> decltype(
    void(static_cast<T(*)()>(nullptr)), true_type{});

template <typename>
auto test_returnable(...) -> false_type;

template <typename T, typename U>
auto test_implicitly_convertible(int) -> decltype(
    void(declval<void(&)(U)>()(declval<T>())), true_type{});

template <typename, typename>
auto test_implicitly_convertible(...) -> false_type;

} // namespace __

template <typename T, typename U>
struct is_convertible : integral_constant<bool,
    (decltype(__::test_returnable<U>(0))::value &&
     decltype(__::test_implicitly_convertible<T, U>(0))::value) ||
    (is_void_v<T> && is_void_v<U>)
>
{
};

template <typename T, typename U>
inline constexpr bool is_convertible_v = is_convertible<T, U>::value;

} // namespace ctl

#endif // COSMOPOLITAN_CTL_TYPE_TRAITS_H_
