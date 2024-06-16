// -*-mode:c++;indent-tabs-mode:nil;c-basic-offset:4;tab-width:8;coding:utf-8-*-
// vi: set et ft=cpp ts=4 sts=4 sw=4 fenc=utf-8 :vi
#ifndef COSMOPOLITAN_CTL_COMPRESSED_PAIR_H_
#define COSMOPOLITAN_CTL_COMPRESSED_PAIR_H_
#include <__type_traits/is_empty.h>
#include <__type_traits/is_final.h>
#include <__utility/forward.h>
#include <__utility/move.h>
#include <__utility/swap.h>

namespace ctl {

namespace __ {

template<typename T>
concept EmptyBase = std::is_empty_v<T> && !std::is_final_v<T>;

template<typename T, int I>
struct pair_elem
{
    using value_type = T;

    constexpr pair_elem(auto&& u) : t(std::forward<decltype(u)>(u))
    {
    }

    constexpr T& get() noexcept
    {
        return t;
    }

    constexpr const T& get() const noexcept
    {
        return t;
    }

  private:
    T t;
};

template<EmptyBase T, int I>
struct pair_elem<T, I> : private T
{
    using value_type = T;

    constexpr pair_elem(auto&& u) : T(std::forward<decltype(u)>(u))
    {
    }

    constexpr T& get() noexcept
    {
        return static_cast<T&>(*this);
    }

    constexpr const T& get() const noexcept
    {
        return static_cast<const T&>(*this);
    }

  protected:
};

} // namespace __

template<typename A, typename B>
struct compressed_pair
  : private __::pair_elem<A, 0>
  , private __::pair_elem<B, 1>
{
    using first_type = __::pair_elem<A, 0>;
    using second_type = __::pair_elem<B, 1>;

    constexpr compressed_pair(auto&& a, auto&& b)
      : first_type(std::forward<decltype(a)>(a))
      , second_type(std::forward<decltype(b)>(b))
    {
    }

    constexpr typename first_type::value_type& first() noexcept
    {
        return static_cast<first_type&>(*this).get();
    }

    constexpr const typename first_type::value_type& first() const noexcept
    {
        return static_cast<const first_type&>(*this).get();
    }

    constexpr typename second_type::value_type& second() noexcept
    {
        return static_cast<second_type&>(*this).get();
    }

    constexpr const typename second_type::value_type& second() const noexcept
    {
        return static_cast<const second_type&>(*this).get();
    }
};

} // namespace ctl
#endif // COSMOPOLITAN_CTL_COMPRESSED_PAIR_H_
