// -*-mode:c++;indent-tabs-mode:nil;c-basic-offset:4;tab-width:8;coding:utf-8-*-
// vi: set et ft=cpp ts=4 sts=4 sw=4 fenc=utf-8 :vi
#ifndef CTL_IS_REFERENCE_H_
#define CTL_IS_REFERENCE_H_

namespace ctl {

// Primary template
template<class T>
struct is_reference
{
    static constexpr bool value = false;
};

// Specialization for lvalue reference
template<class T>
struct is_reference<T&>
{
    static constexpr bool value = true;
};

// Specialization for rvalue reference
template<class T>
struct is_reference<T&&>
{
    static constexpr bool value = true;
};

// Helper variable template (C++14 and later)
template<class T>
inline constexpr bool is_reference_v = is_reference<T>::value;

} // namespace ctl

#endif // CTL_IS_REFERENCE_H_
