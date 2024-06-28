// -*-mode:c++;indent-tabs-mode:nil;c-basic-offset:4;tab-width:8;coding:utf-8-*-
// vi: set et ft=cpp ts=4 sts=4 sw=4 fenc=utf-8 :vi
#ifndef CTL_ADDRESSOF_H_
#define CTL_ADDRESSOF_H_

namespace ctl {

template<typename T>
T*
addressof(T& arg) noexcept
{
    return reinterpret_cast<T*>(
      &const_cast<char&>(reinterpret_cast<const volatile char&>(arg)));
}

template<typename R, typename... Args>
R (*addressof(R (*&arg)(Args...)) noexcept)
(Args...)
{
    return arg;
}

template<typename T>
T*
addressof(T&&) = delete;

} // namespace ctl

#endif // CTL_ADDRESSOF_H_
