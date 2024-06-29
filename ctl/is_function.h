// -*-mode:c++;indent-tabs-mode:nil;c-basic-offset:4;tab-width:8;coding:utf-8-*-
// vi: set et ft=cpp ts=4 sts=4 sw=4 fenc=utf-8 :vi
#ifndef CTL_IS_FUNCTION_H_
#define CTL_IS_FUNCTION_H_
#include "integral_constant.h"

namespace ctl {

// Primary template
template<class>
struct is_function : false_type
{};

// Specializations for various function types

// Regular functions
template<class Ret, class... Args>
struct is_function<Ret(Args...)> : true_type
{};

// Functions with cv-qualifiers
template<class Ret, class... Args>
struct is_function<Ret(Args...) const> : true_type
{};

template<class Ret, class... Args>
struct is_function<Ret(Args...) volatile> : true_type
{};

template<class Ret, class... Args>
struct is_function<Ret(Args...) const volatile> : true_type
{};

// Functions with ref-qualifiers
template<class Ret, class... Args>
struct is_function<Ret(Args...)&> : true_type
{};

template<class Ret, class... Args>
struct is_function<Ret(Args...) const&> : true_type
{};

template<class Ret, class... Args>
struct is_function<Ret(Args...) volatile&> : true_type
{};

template<class Ret, class... Args>
struct is_function<Ret(Args...) const volatile&> : true_type
{};

template<class Ret, class... Args>
struct is_function<Ret(Args...) &&> : true_type
{};

template<class Ret, class... Args>
struct is_function<Ret(Args...) const&&> : true_type
{};

template<class Ret, class... Args>
struct is_function<Ret(Args...) volatile&&> : true_type
{};

template<class Ret, class... Args>
struct is_function<Ret(Args...) const volatile&&> : true_type
{};

// Variadic functions
template<class Ret, class... Args>
struct is_function<Ret(Args..., ...)> : true_type
{};

// Variadic functions with cv-qualifiers
template<class Ret, class... Args>
struct is_function<Ret(Args..., ...) const> : true_type
{};

template<class Ret, class... Args>
struct is_function<Ret(Args..., ...) volatile> : true_type
{};

template<class Ret, class... Args>
struct is_function<Ret(Args..., ...) const volatile> : true_type
{};

// Variadic functions with ref-qualifiers
template<class Ret, class... Args>
struct is_function<Ret(Args..., ...)&> : true_type
{};

template<class Ret, class... Args>
struct is_function<Ret(Args..., ...) const&> : true_type
{};

template<class Ret, class... Args>
struct is_function<Ret(Args..., ...) volatile&> : true_type
{};

template<class Ret, class... Args>
struct is_function<Ret(Args..., ...) const volatile&> : true_type
{};

template<class Ret, class... Args>
struct is_function<Ret(Args..., ...) &&> : true_type
{};

template<class Ret, class... Args>
struct is_function<Ret(Args..., ...) const&&> : true_type
{};

template<class Ret, class... Args>
struct is_function<Ret(Args..., ...) volatile&&> : true_type
{};

template<class Ret, class... Args>
struct is_function<Ret(Args..., ...) const volatile&&> : true_type
{};

} // namespace ctl

#endif // CTL_IS_FUNCTION_H_
