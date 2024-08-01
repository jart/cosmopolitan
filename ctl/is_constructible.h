// -*-mode:c++;indent-tabs-mode:nil;c-basic-offset:4;tab-width:8;coding:utf-8-*-
// vi: set et ft=cpp ts=4 sts=4 sw=4 fenc=utf-8 :vi
#ifndef CTL_IS_CONSTRUCTIBLE_H_
#define CTL_IS_CONSTRUCTIBLE_H_
#include "integral_constant.h"

namespace ctl {

template<class _Tp, class... _Args>
struct is_constructible
  : public ctl::integral_constant<bool, __is_constructible(_Tp, _Args...)>
{};

template<class _Tp, class... _Args>
inline constexpr bool is_constructible_v = __is_constructible(_Tp, _Args...);

} // namespace ctl

#endif // CTL_IS_CONSTRUCTIBLE_H_
