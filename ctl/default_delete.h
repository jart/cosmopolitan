// -*-mode:c++;indent-tabs-mode:nil;c-basic-offset:4;tab-width:8;coding:utf-8-*-
// vi: set et ft=cpp ts=4 sts=4 sw=4 fenc=utf-8 :vi
#ifndef CTL_DEFAULT_DELETE_H_
#define CTL_DEFAULT_DELETE_H_
#include "enable_if.h"
#include "is_convertible.h"

namespace ctl {

template<typename T>
struct default_delete
{
    constexpr default_delete() noexcept = default;
    template<typename U,
             typename = typename ctl::enable_if<
               ctl::is_convertible<U*, T*>::value>::type>
    constexpr default_delete(const default_delete<U>&) noexcept
    {
    }
    constexpr void operator()(T* const p) const noexcept
    {
        delete p;
    }
};

template<typename T>
struct default_delete<T[]>
{
    constexpr default_delete() noexcept = default;
    template<typename U,
             typename = typename ctl::enable_if<
               ctl::is_convertible<U (*)[], T (*)[]>::value>::type>
    constexpr default_delete(const default_delete<U[]>&) noexcept
    {
    }
    constexpr void operator()(T* const p) const noexcept
    {
        delete[] p;
    }
};

} // namespace ctl

#endif // CTL_DEFAULT_DELETE_H_
