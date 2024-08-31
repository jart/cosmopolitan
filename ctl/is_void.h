// -*-mode:c++;indent-tabs-mode:nil;c-basic-offset:4;tab-width:8;coding:utf-8-*-
// vi: set et ft=cpp ts=4 sts=4 sw=4 fenc=utf-8 :vi
#ifndef CTL_IS_VOID_H_
#define CTL_IS_VOID_H_
#include "integral_constant.h"
#include "remove_cv.h"

namespace ctl {

template<typename>
struct is_void_ : public ctl::false_type
{};

template<>
struct is_void_<void> : public ctl::true_type
{};

template<typename _Tp>
struct is_void : public is_void_<typename ctl::remove_cv<_Tp>::type>::type
{};

template<typename T>
inline constexpr bool is_void_v = is_void<T>::value;

} // namespace ctl

#endif // CTL_IS_VOID_H_
