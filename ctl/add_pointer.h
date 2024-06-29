// -*-mode:c++;indent-tabs-mode:nil;c-basic-offset:4;tab-width:8;coding:utf-8-*-
// vi: set et ft=cpp ts=4 sts=4 sw=4 fenc=utf-8 :vi
#ifndef CTL_ADD_POINTER_H_
#define CTL_ADD_POINTER_H_
#include "remove_reference.h"

namespace ctl {

// Primary template
template<typename T>
struct add_pointer
{
    typedef typename remove_reference<T>::type* type;
};

// Specialization for functions
template<typename R, typename... Args>
struct add_pointer<R(Args...)>
{
    typedef R (*type)(Args...);
};

// Specialization for function references
template<typename R, typename... Args>
struct add_pointer<R (&)(Args...)>
{
    typedef R (*type)(Args...);
};

// Specialization for rvalue references to functions
template<typename R, typename... Args>
struct add_pointer<R (&&)(Args...)>
{
    typedef R (*type)(Args...);
};

// Helper alias template (C++14 and later)
template<typename T>
using add_pointer_t = typename add_pointer<T>::type;

} // namespace ctl

#endif // CTL_ADD_POINTER_H_
