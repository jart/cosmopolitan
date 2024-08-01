// -*-mode:c++;indent-tabs-mode:nil;c-basic-offset:4;tab-width:8;coding:utf-8-*-
// vi: set et ft=cpp ts=4 sts=4 sw=4 fenc=utf-8 :vi
#ifndef CTL_IS_CONVERTIBLE_H_
#define CTL_IS_CONVERTIBLE_H_
#include "ctl/integral_constant.h"
#include "ctl/void_t.h"

namespace ctl {

// Declaration of declval
template<typename T>
T&&
declval() noexcept;

namespace detail {

template<typename From, typename To, typename = void>
struct is_convertible_impl : ctl::false_type
{};

template<typename From, typename To>
struct is_convertible_impl<From,
                           To,
                           void_t<decltype(static_cast<To>(declval<From>()))>>
  : true_type
{};

// Handle void types separately
template<>
struct is_convertible_impl<void, void> : ctl::true_type
{};

template<typename To>
struct is_convertible_impl<void, To> : ctl::false_type
{};

template<typename From>
struct is_convertible_impl<From, void> : ctl::false_type
{};

} // namespace detail

template<typename From, typename To>
struct is_convertible : detail::is_convertible_impl<From, To>
{};

// Helper variable template (C++17 and later)
template<typename From, typename To>
inline constexpr bool is_convertible_v = is_convertible<From, To>::value;

} // namespace ctl

#endif // CTL_IS_CONVERTIBLE_H_
