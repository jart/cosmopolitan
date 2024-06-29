// -*-mode:c++;indent-tabs-mode:nil;c-basic-offset:4;tab-width:8;coding:utf-8-*-
// vi: set et ft=cpp ts=4 sts=4 sw=4 fenc=utf-8 :vi
#ifndef CTL_NUMERIC_LIMITS_H_
#define CTL_NUMERIC_LIMITS_H_
#include "is_integral.h"
#include "is_unsigned.h"

namespace ctl {

template<typename T>
class numeric_limits
{
    static constexpr T max_signed()
    {
        return T(((uintmax_t)1 << (sizeof(T) * 8 - 1)) - 1);
    }

    static constexpr T max_unsigned()
    {
        return T(~T(0));
    }

  public:
    static constexpr bool is_specialized = ctl::is_integral<T>::value;
    static constexpr bool is_signed = !ctl::is_unsigned<T>::value;
    static constexpr bool is_integer = ctl::is_integral<T>::value;
    static constexpr bool is_exact = ctl::is_integral<T>::value;
    static constexpr int digits = sizeof(T) * 8 - (is_signed ? 1 : 0);

    static constexpr T min() noexcept
    {
        return is_signed ? T(-max_signed() - 1) : T(0);
    }

    static constexpr T max() noexcept
    {
        return is_signed ? max_signed() : max_unsigned();
    }

    static constexpr T lowest() noexcept
    {
        return min();
    }
};

template<>
struct numeric_limits<bool>
{
    static constexpr bool is_specialized = true;
    static constexpr bool is_signed = false;
    static constexpr bool is_integer = true;
    static constexpr bool is_exact = true;
    static constexpr int digits = 1;

    static constexpr bool min() noexcept
    {
        return false;
    }

    static constexpr bool max() noexcept
    {
        return true;
    }

    static constexpr bool lowest() noexcept
    {
        return false;
    }
};

template<>
struct numeric_limits<float>
{
    static constexpr bool is_specialized = true;
    static constexpr bool is_signed = true;
    static constexpr bool is_integer = false;
    static constexpr bool is_exact = false;
    static constexpr int digits = 24;

    static constexpr float min() noexcept
    {
        return __FLT_MIN__;
    }

    static constexpr float max() noexcept
    {
        return __FLT_MAX__;
    }

    static constexpr float lowest() noexcept
    {
        return -max();
    }
};

template<>
struct numeric_limits<double>
{
    static constexpr bool is_specialized = true;
    static constexpr bool is_signed = true;
    static constexpr bool is_integer = false;
    static constexpr bool is_exact = false;
    static constexpr int digits = 53;

    static constexpr double min() noexcept
    {
        return __DBL_MIN__;
    }

    static constexpr double max() noexcept
    {
        return __DBL_MAX__;
    }

    static constexpr double lowest() noexcept
    {
        return -max();
    }
};

} // namespace ctl

#endif // CTL_NUMERIC_LIMITS_H_
