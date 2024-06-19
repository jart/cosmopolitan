// -*-mode:c++;indent-tabs-mode:nil;c-basic-offset:4;tab-width:8;coding:utf-8-*-
// vi: set et ft=cpp ts=4 sts=4 sw=4 fenc=utf-8 :vi
#ifndef COSMOPOLITAN_CTL_UTILITY_H_
#define COSMOPOLITAN_CTL_UTILITY_H_

namespace ctl {

namespace __ {

template<typename T>
struct no_infer_
{
    typedef T type;
};

template<typename T>
using no_infer = typename no_infer_<T>::type;

} // namespace __

template<typename T>
constexpr T&&
move(T& t) noexcept
{
    return static_cast<T&&>(t);
}

template<typename T>
constexpr T&&
forward(__::no_infer<T>& t) noexcept
{
    return static_cast<T&&>(t);
}

template<typename T>
// TODO(mrdomino): requires move_constructible<T> && move_assignable<T>
constexpr void
swap(T& a, T& b) noexcept
{
    T t(ctl::move(a));
    a = ctl::move(b);
    b = ctl::move(t);
}

template<typename T, size_t N>
// TODO(mrdomino): requires is_swappable
constexpr void
swap(T (&a)[N], T (&b)[N]) noexcept
{
    for (size_t i = 0; i < N; ++i) {
        swap(a[i], b[i]);
    }
}

template<typename T>
T&&
declval() noexcept;

} // namespace ctl

#endif // COSMOPOLITAN_CTL_UTILITY_H_
