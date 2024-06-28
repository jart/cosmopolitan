// -*-mode:c++;indent-tabs-mode:nil;c-basic-offset:4;tab-width:8;coding:utf-8-*-
// vi: set et ft=cpp ts=4 sts=4 sw=4 fenc=utf-8 :vi
#ifndef CTL_TYPE_TRAITS_H_
#define CTL_TYPE_TRAITS_H_

namespace ctl {

template<class T, T v>
struct integral_constant
{
    static constexpr T value = v;
    using value_type = T;
    using type = integral_constant;
    constexpr operator value_type() const noexcept
    {
        return value;
    }
    constexpr value_type operator()() const noexcept
    {
        return value;
    }
};

using true_type = integral_constant<bool, true>;
using false_type = integral_constant<bool, false>;

template<typename T>
struct is_integral : false_type
{};

template<>
struct is_integral<bool> : true_type
{};

template<>
struct is_integral<char> : true_type
{};

template<>
struct is_integral<signed char> : true_type
{};

template<>
struct is_integral<unsigned char> : true_type
{};

template<>
struct is_integral<short> : true_type
{};

template<>
struct is_integral<unsigned short> : true_type
{};

template<>
struct is_integral<int> : true_type
{};

template<>
struct is_integral<unsigned int> : true_type
{};

template<>
struct is_integral<long> : true_type
{};

template<>
struct is_integral<unsigned long> : true_type
{};

template<>
struct is_integral<long long> : true_type
{};

template<>
struct is_integral<unsigned long long> : true_type
{};

template<>
struct is_integral<char16_t> : true_type
{};

template<>
struct is_integral<char32_t> : true_type
{};

template<>
struct is_integral<wchar_t> : true_type
{};

template<typename T>
inline constexpr bool is_integral_v = is_integral<T>::value;

template<typename T>
struct is_signed
{
    static constexpr bool value = T(0) > T(-1);
};

template<typename T>
struct is_unsigned
{
    static constexpr bool value = T(0) < T(-1);
};

template<typename T, typename U>
struct is_same
{
    static constexpr bool value = false;
};

template<typename T>
struct is_same<T, T>
{
    static constexpr bool value = true;
};

template<class T, class U>
inline constexpr bool is_same_v = is_same<T, U>::value;

} // namespace ctl

#endif // CTL_TYPE_TRAITS_H_
