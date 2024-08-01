// -*-mode:c++;indent-tabs-mode:nil;c-basic-offset:4;tab-width:8;coding:utf-8-*-
// vi: set et ft=cpp ts=4 sts=4 sw=4 fenc=utf-8 :vi
#ifndef CTL_IS_SAME_H_
#define CTL_IS_SAME_H_

namespace ctl {

template<typename T, typename U>
struct is_same
{
    static constexpr bool value = false;
};

template<typename T>
struct is_same<T, T>
{
    static constexpr bool value = true;
};

template<class T, class U>
inline constexpr bool is_same_v = is_same<T, U>::value;

} // namespace ctl

#endif // CTL_IS_SAME_H_
