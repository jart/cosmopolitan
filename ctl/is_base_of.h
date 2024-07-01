// -*-mode:c++;indent-tabs-mode:nil;c-basic-offset:4;tab-width:8;coding:utf-8-*-
// vi: set et ft=cpp ts=4 sts=4 sw=4 fenc=utf-8 :vi
#ifndef CTL_IS_BASE_OF_H_
#define CTL_IS_BASE_OF_H_
#include "integral_constant.h"

namespace ctl {

template<typename Base, typename Derived>
struct is_base_of
  : public ctl::integral_constant<bool, __is_base_of(Base, Derived)>
{};

template<typename Base, typename Derived>
inline constexpr bool is_base_of_v = __is_base_of(Base, Derived);

} // namespace ctl

#endif // CTL_IS_BASE_OF_H_
