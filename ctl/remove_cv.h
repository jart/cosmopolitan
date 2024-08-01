// -*-mode:c++;indent-tabs-mode:nil;c-basic-offset:4;tab-width:8;coding:utf-8-*-
// vi: set et ft=cpp ts=4 sts=4 sw=4 fenc=utf-8 :vi
#ifndef CTL_REMOVE_CV_H_
#define CTL_REMOVE_CV_H_

namespace ctl {

template<class T>
struct remove_const
{
    typedef T type;
};

template<class T>
struct remove_const<const T>
{
    typedef T type;
};

template<class T>
struct remove_volatile
{
    typedef T type;
};

template<class T>
struct remove_volatile<volatile T>
{
    typedef T type;
};

template<class T>
struct remove_cv
{
    typedef typename remove_volatile<typename remove_const<T>::type>::type type;
};

template<class T>
using remove_const_t = typename remove_const<T>::type;

template<class T>
using remove_volatile_t = typename remove_volatile<T>::type;

template<class T>
using remove_cv_t = typename remove_cv<T>::type;

} // namespace ctl

#endif // CTL_REMOVE_CV_H_
