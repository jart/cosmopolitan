// -*-mode:c++;indent-tabs-mode:nil;c-basic-offset:4;tab-width:8;coding:utf-8-*-
// vi: set et ft=cpp ts=4 sts=4 sw=4 fenc=utf-8 :vi
#ifndef CTL_IS_UNSIGNED_H_
#define CTL_IS_UNSIGNED_H_

namespace ctl {

template<typename T>
struct is_unsigned
{
    static constexpr bool value = T(0) < T(-1);
};

} // namespace ctl

#endif // CTL_IS_UNSIGNED_H_
