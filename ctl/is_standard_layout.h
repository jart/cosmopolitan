// -*-mode:c++;indent-tabs-mode:nil;c-basic-offset:4;tab-width:8;coding:utf-8-*-
// vi: set et ft=cpp ts=4 sts=4 sw=4 fenc=utf-8 :vi
#ifndef CTL_IS_STANDARD_LAYOUT_H_
#define CTL_IS_STANDARD_LAYOUT_H_
#include "integral_constant.h"

namespace ctl {

template<typename T>
struct is_standard_layout
  : public ctl::integral_constant<bool, __is_standard_layout(T)>
{};

template<typename T>
inline constexpr bool is_standard_layout_v = __is_standard_layout(T);

} // namespace ctl

#endif // CTL_IS_STANDARD_LAYOUT_H_
