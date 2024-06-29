// -*-mode:c++;indent-tabs-mode:nil;c-basic-offset:4;tab-width:8;coding:utf-8-*-
// vi: set et ft=cpp ts=4 sts=4 sw=4 fenc=utf-8 :vi
#ifndef CTL_REMOVE_EXTENT_H_
#define CTL_REMOVE_EXTENT_H_

namespace ctl {

template<typename T>
struct remove_extent
{
    typedef T type;
};

template<typename T, size_t N>
struct remove_extent<T[N]>
{
    typedef T type;
};

template<typename T>
struct remove_extent<T[]>
{
    typedef T type;
};

template<typename T>
using remove_extent_t = typename remove_extent<T>::type;

} // namespace ctl

#endif // CTL_REMOVE_EXTENT_H_
