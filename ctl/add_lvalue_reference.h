// -*-mode:c++;indent-tabs-mode:nil;c-basic-offset:4;tab-width:8;coding:utf-8-*-
// vi: set et ft=cpp ts=4 sts=4 sw=4 fenc=utf-8 :vi
#ifndef CTL_ADD_LVALUE_REFERENCE_H_
#define CTL_ADD_LVALUE_REFERENCE_H_

namespace ctl {

template<typename T>
struct add_lvalue_reference
{
    typedef T& type;
};

template<typename T>
struct add_lvalue_reference<T&>
{
    typedef T& type;
};

template<typename T>
struct add_lvalue_reference<T&&>
{
    typedef T& type;
};

template<>
struct add_lvalue_reference<void>
{
    typedef void type;
};

template<>
struct add_lvalue_reference<const void>
{
    typedef const void type;
};

template<>
struct add_lvalue_reference<volatile void>
{
    typedef volatile void type;
};

template<>
struct add_lvalue_reference<const volatile void>
{
    typedef const volatile void type;
};

template<typename T>
using add_lvalue_reference_t = typename add_lvalue_reference<T>::type;

} // namespace ctl

#endif // CTL_ADD_LVALUE_REFERENCE_H_
