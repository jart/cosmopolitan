//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include <__utility/no_destroy.h>
#include <algorithm>
#include <clocale>
#include <codecvt>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <locale>
#include <new>
#include <string>
#include <type_traits>
#include <typeinfo>
#include <utility>
#include <vector>

#ifndef _LIBCPP_HAS_NO_WIDE_CHARACTERS
#  include <cwctype>
#endif

#if defined(_AIX)
#  include <sys/localedef.h> // for __lc_ctype_ptr
#endif

#if defined(_LIBCPP_MSVCRT)
#  define _CTYPE_DISABLE_MACROS
#endif

#if !defined(_LIBCPP_MSVCRT) && !defined(__MINGW32__) && !defined(__BIONIC__) && !defined(__NuttX__)
#  include <langinfo.h>
#endif

#include "atomic_support.h"
#include "sso_allocator.h"

// On Linux, wint_t and wchar_t have different signed-ness, and this causes
// lots of noise in the build log, but no bugs that I know of.
_LIBCPP_CLANG_DIAGNOSTIC_IGNORED("-Wsign-conversion")

_LIBCPP_PUSH_MACROS
#include <__undef_macros>

_LIBCPP_BEGIN_NAMESPACE_STD

template class _LIBCPP_CLASS_TEMPLATE_INSTANTIATION_VIS moneypunct<char, false>;
template class _LIBCPP_CLASS_TEMPLATE_INSTANTIATION_VIS moneypunct<char, true>;
_LIBCPP_IF_WIDE_CHARACTERS(template class _LIBCPP_CLASS_TEMPLATE_INSTANTIATION_VIS moneypunct<wchar_t, false>;)
_LIBCPP_IF_WIDE_CHARACTERS(template class _LIBCPP_CLASS_TEMPLATE_INSTANTIATION_VIS moneypunct<wchar_t, true>;)

template class _LIBCPP_CLASS_TEMPLATE_INSTANTIATION_VIS moneypunct_byname<char, false>;
template class _LIBCPP_CLASS_TEMPLATE_INSTANTIATION_VIS moneypunct_byname<char, true>;
_LIBCPP_IF_WIDE_CHARACTERS(template class _LIBCPP_CLASS_TEMPLATE_INSTANTIATION_VIS moneypunct_byname<wchar_t, false>;)
_LIBCPP_IF_WIDE_CHARACTERS(template class _LIBCPP_CLASS_TEMPLATE_INSTANTIATION_VIS moneypunct_byname<wchar_t, true>;)

template class _LIBCPP_CLASS_TEMPLATE_INSTANTIATION_VIS money_get<char>;
_LIBCPP_IF_WIDE_CHARACTERS(template class _LIBCPP_CLASS_TEMPLATE_INSTANTIATION_VIS money_get<wchar_t>;)

template class _LIBCPP_CLASS_TEMPLATE_INSTANTIATION_VIS __money_get<char>;
_LIBCPP_IF_WIDE_CHARACTERS(template class _LIBCPP_CLASS_TEMPLATE_INSTANTIATION_VIS __money_get<wchar_t>;)

template class _LIBCPP_CLASS_TEMPLATE_INSTANTIATION_VIS money_put<char>;
_LIBCPP_IF_WIDE_CHARACTERS(template class _LIBCPP_CLASS_TEMPLATE_INSTANTIATION_VIS money_put<wchar_t>;)

template class _LIBCPP_CLASS_TEMPLATE_INSTANTIATION_VIS __money_put<char>;
_LIBCPP_IF_WIDE_CHARACTERS(template class _LIBCPP_CLASS_TEMPLATE_INSTANTIATION_VIS __money_put<wchar_t>;)

template class _LIBCPP_CLASS_TEMPLATE_INSTANTIATION_VIS messages<char>;
_LIBCPP_IF_WIDE_CHARACTERS(template class _LIBCPP_CLASS_TEMPLATE_INSTANTIATION_VIS messages<wchar_t>;)

template class _LIBCPP_CLASS_TEMPLATE_INSTANTIATION_VIS messages_byname<char>;
_LIBCPP_IF_WIDE_CHARACTERS(template class _LIBCPP_CLASS_TEMPLATE_INSTANTIATION_VIS messages_byname<wchar_t>;)

template class _LIBCPP_CLASS_TEMPLATE_INSTANTIATION_VIS codecvt_byname<char, char, mbstate_t>;
_LIBCPP_IF_WIDE_CHARACTERS(
    template class _LIBCPP_CLASS_TEMPLATE_INSTANTIATION_VIS codecvt_byname<wchar_t, char, mbstate_t>;)
template class _LIBCPP_DEPRECATED_IN_CXX20 _LIBCPP_CLASS_TEMPLATE_INSTANTIATION_VIS
    codecvt_byname<char16_t, char, mbstate_t>;
template class _LIBCPP_DEPRECATED_IN_CXX20 _LIBCPP_CLASS_TEMPLATE_INSTANTIATION_VIS
    codecvt_byname<char32_t, char, mbstate_t>;
#ifndef _LIBCPP_HAS_NO_CHAR8_T
template class _LIBCPP_CLASS_TEMPLATE_INSTANTIATION_VIS codecvt_byname<char16_t, char8_t, mbstate_t>;
template class _LIBCPP_CLASS_TEMPLATE_INSTANTIATION_VIS codecvt_byname<char32_t, char8_t, mbstate_t>;
#endif

_LIBCPP_END_NAMESPACE_STD

_LIBCPP_POP_MACROS
