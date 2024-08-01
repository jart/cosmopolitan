// -*-mode:c++;indent-tabs-mode:nil;c-basic-offset:4;tab-width:8;coding:utf-8-*-
// vi: set et ft=cpp ts=4 sts=4 sw=4 fenc=utf-8 :vi
#ifndef CTL_REMOVE_REFERENCE_H_
#define CTL_REMOVE_REFERENCE_H_

namespace ctl {

template<class T>
struct remove_reference
{
    typedef T type;
};

template<class T>
struct remove_reference<T&>
{
    typedef T type;
};

template<class T>
struct remove_reference<T&&>
{
    typedef T type;
};

template<class T>
using remove_reference_t = typename remove_reference<T>::type;

} // namespace ctl

#endif // CTL_REMOVE_REFERENCE_H_
