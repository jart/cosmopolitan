//===------------------------- locale.cpp ---------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "third_party/libcxx/string"
#include "third_party/libcxx/locale"
#include "third_party/libcxx/codecvt"
#include "third_party/libcxx/vector"
#include "third_party/libcxx/algorithm"
#include "third_party/libcxx/typeinfo"
#ifndef _LIBCPP_NO_EXCEPTIONS
#include "third_party/libcxx/type_traits"
#endif
#include "third_party/libcxx/clocale"
#include "third_party/libcxx/cstring"
#include "third_party/libcxx/cwctype"
#include "third_party/libcxx/__sso_allocator"
#include "third_party/libcxx/include/atomic_support.hh"
#include "libc/str/locale.h"
#include "third_party/libcxx/countof.internal.hh"
#include "third_party/libcxx/__undef_macros"

// On Linux, wint_t and wchar_t have different signed-ness, and this causes
// lots of noise in the build log, but no bugs that I know of. 
#if defined(__clang__)
#pragma clang diagnostic ignored "-Wsign-conversion"
#endif

_LIBCPP_BEGIN_NAMESPACE_STD

namespace {

_LIBCPP_NORETURN static void __throw_runtime_error(const string &msg)
{
#ifndef _LIBCPP_NO_EXCEPTIONS
    throw runtime_error(msg);
#else
    (void)msg;
    _VSTD::abort();
#endif
}

} // namespace

bool __checked_string_to_wchar_convert(wchar_t& dest,
                                       const char* ptr,
                                       locale_t loc);
bool __checked_string_to_char_convert(char& dest,
                                      const char* ptr,
                                      locale_t __loc);

struct __libcpp_unique_locale {
  __libcpp_unique_locale(const char* nm) : __loc_(newlocale(LC_ALL_MASK, nm, 0)) {}

  ~__libcpp_unique_locale() {
    if (__loc_)
      freelocale(__loc_);
  }

  explicit operator bool() const { return __loc_; }

  locale_t& get() { return __loc_; }

  locale_t __loc_;
private:
  __libcpp_unique_locale(__libcpp_unique_locale const&);
  __libcpp_unique_locale& operator=(__libcpp_unique_locale const&);
};

// moneypunct_byname

template <class charT>
static
void
__init_pat(money_base::pattern& pat, basic_string<charT>& __curr_symbol_,
           bool intl, char cs_precedes, char sep_by_space, char sign_posn,
           charT space_char)
{
    const char sign = static_cast<char>(money_base::sign);
    const char space = static_cast<char>(money_base::space);
    const char none = static_cast<char>(money_base::none);
    const char symbol = static_cast<char>(money_base::symbol);
    const char value = static_cast<char>(money_base::value);
    const bool symbol_contains_sep = intl && __curr_symbol_.size() == 4;

    // Comments on case branches reflect 'C11 7.11.2.1 The localeconv
    // function'. "Space between sign and symbol or value" means that
    // if the sign is adjacent to the symbol, there's a space between
    // them, and otherwise there's a space between the sign and value.
    //
    // C11's localeconv specifies that the fourth character of an
    // international curr_symbol is used to separate the sign and
    // value when sep_by_space says to do so. C++ can't represent
    // that, so we just use a space.  When sep_by_space says to
    // separate the symbol and value-or-sign with a space, we rearrange the
    // curr_symbol to put its spacing character on the correct side of
    // the symbol.
    //
    // We also need to avoid adding an extra space between the sign
    // and value when the currency symbol is suppressed (by not
    // setting showbase).  We match glibc's strfmon by interpreting
    // sep_by_space==1 as "omit the space when the currency symbol is
    // absent".
    //
    // Users who want to get this right should use ICU instead.

    switch (cs_precedes)
    {
    case 0:  // value before curr_symbol
        if (symbol_contains_sep) {
            // Move the separator to before the symbol, to place it
            // between the value and symbol.
            rotate(__curr_symbol_.begin(), __curr_symbol_.begin() + 3,
                   __curr_symbol_.end());
        }
        switch (sign_posn)
        {
        case 0:  // Parentheses surround the quantity and currency symbol.
            pat.field[0] = sign;
            pat.field[1] = value;
            pat.field[2] = none;  // Any space appears in the symbol.
            pat.field[3] = symbol;
            switch (sep_by_space)
            {
            case 0:  // No space separates the currency symbol and value.
                // This case may have changed between C99 and C11;
                // assume the currency symbol matches the intention.
            case 2:  // Space between sign and currency or value.
                // The "sign" is two parentheses, so no space here either.
                return;
            case 1:  // Space between currency-and-sign or currency and value.
                if (!symbol_contains_sep) {
                    // We insert the space into the symbol instead of
                    // setting pat.field[2]=space so that when
                    // showbase is not set, the space goes away too.
                    __curr_symbol_.insert(0, 1, space_char);
                }
                return;
            default:
                break;
            }
            break;
        case 1:  // The sign string precedes the quantity and currency symbol.
            pat.field[0] = sign;
            pat.field[3] = symbol;
            switch (sep_by_space)
            {
            case 0:  // No space separates the currency symbol and value.
                pat.field[1] = value;
                pat.field[2] = none;
                return;
            case 1:  // Space between currency-and-sign or currency and value.
                pat.field[1] = value;
                pat.field[2] = none;
                if (!symbol_contains_sep) {
                    // We insert the space into the symbol instead of
                    // setting pat.field[2]=space so that when
                    // showbase is not set, the space goes away too.
                    __curr_symbol_.insert(0, 1, space_char);
                }
                return;
            case 2:  // Space between sign and currency or value.
                pat.field[1] = space;
                pat.field[2] = value;
                if (symbol_contains_sep) {
                    // Remove the separator from the symbol, since it
                    // has already appeared after the sign.
                    __curr_symbol_.erase(__curr_symbol_.begin());
                }
                return;
            default:
                break;
            }
            break;
        case 2:  // The sign string succeeds the quantity and currency symbol.
            pat.field[0] = value;
            pat.field[3] = sign;
            switch (sep_by_space)
            {
            case 0:  // No space separates the currency symbol and value.
                pat.field[1] = none;
                pat.field[2] = symbol;
                return;
            case 1:  // Space between currency-and-sign or currency and value.
                if (!symbol_contains_sep) {
                    // We insert the space into the symbol instead of
                    // setting pat.field[1]=space so that when
                    // showbase is not set, the space goes away too.
                    __curr_symbol_.insert(0, 1, space_char);
                }
                pat.field[1] = none;
                pat.field[2] = symbol;
                return;
            case 2:  // Space between sign and currency or value.
                pat.field[1] = symbol;
                pat.field[2] = space;
                if (symbol_contains_sep) {
                    // Remove the separator from the symbol, since it
                    // should not be removed if showbase is absent.
                    __curr_symbol_.erase(__curr_symbol_.begin());
                }
                return;
            default:
                break;
            }
            break;
        case 3:  // The sign string immediately precedes the currency symbol.
            pat.field[0] = value;
            pat.field[3] = symbol;
            switch (sep_by_space)
            {
            case 0:  // No space separates the currency symbol and value.
                pat.field[1] = none;
                pat.field[2] = sign;
                return;
            case 1:  // Space between currency-and-sign or currency and value.
                pat.field[1] = space;
                pat.field[2] = sign;
                if (symbol_contains_sep) {
                    // Remove the separator from the symbol, since it
                    // has already appeared before the sign.
                    __curr_symbol_.erase(__curr_symbol_.begin());
                }
                return;
            case 2:  // Space between sign and currency or value.
                pat.field[1] = sign;
                pat.field[2] = none;
                if (!symbol_contains_sep) {
                    // We insert the space into the symbol instead of
                    // setting pat.field[2]=space so that when
                    // showbase is not set, the space goes away too.
                    __curr_symbol_.insert(0, 1, space_char);
                }
                return;
            default:
                break;
            }
            break;
        case 4:  // The sign string immediately succeeds the currency symbol.
            pat.field[0] = value;
            pat.field[3] = sign;
            switch (sep_by_space)
            {
            case 0:  // No space separates the currency symbol and value.
                pat.field[1] = none;
                pat.field[2] = symbol;
                return;
            case 1:  // Space between currency-and-sign or currency and value.
                pat.field[1] = none;
                pat.field[2] = symbol;
                if (!symbol_contains_sep) {
                    // We insert the space into the symbol instead of
                    // setting pat.field[1]=space so that when
                    // showbase is not set, the space goes away too.
                    __curr_symbol_.insert(0, 1, space_char);
                }
                return;
            case 2:  // Space between sign and currency or value.
                pat.field[1] = symbol;
                pat.field[2] = space;
                if (symbol_contains_sep) {
                    // Remove the separator from the symbol, since it
                    // should not disappear when showbase is absent.
                    __curr_symbol_.erase(__curr_symbol_.begin());
                }
                return;
            default:
                break;
            }
            break;
        default:
            break;
        }
        break;
    case 1:  // curr_symbol before value
        switch (sign_posn)
        {
        case 0:  // Parentheses surround the quantity and currency symbol.
            pat.field[0] = sign;
            pat.field[1] = symbol;
            pat.field[2] = none;  // Any space appears in the symbol.
            pat.field[3] = value;
            switch (sep_by_space)
            {
            case 0:  // No space separates the currency symbol and value.
                // This case may have changed between C99 and C11;
                // assume the currency symbol matches the intention.
            case 2:  // Space between sign and currency or value.
                // The "sign" is two parentheses, so no space here either.
                return;
            case 1:  // Space between currency-and-sign or currency and value.
                if (!symbol_contains_sep) {
                    // We insert the space into the symbol instead of
                    // setting pat.field[2]=space so that when
                    // showbase is not set, the space goes away too.
                    __curr_symbol_.insert(0, 1, space_char);
                }
                return;
            default:
                break;
            }
            break;
        case 1:  // The sign string precedes the quantity and currency symbol.
            pat.field[0] = sign;
            pat.field[3] = value;
            switch (sep_by_space)
            {
            case 0:  // No space separates the currency symbol and value.
                pat.field[1] = symbol;
                pat.field[2] = none;
                return;
            case 1:  // Space between currency-and-sign or currency and value.
                pat.field[1] = symbol;
                pat.field[2] = none;
                if (!symbol_contains_sep) {
                    // We insert the space into the symbol instead of
                    // setting pat.field[2]=space so that when
                    // showbase is not set, the space goes away too.
                    __curr_symbol_.push_back(space_char);
                }
                return;
            case 2:  // Space between sign and currency or value.
                pat.field[1] = space;
                pat.field[2] = symbol;
                if (symbol_contains_sep) {
                    // Remove the separator from the symbol, since it
                    // has already appeared after the sign.
                    __curr_symbol_.pop_back();
                }
                return;
            default:
                break;
            }
            break;
        case 2:  // The sign string succeeds the quantity and currency symbol.
            pat.field[0] = symbol;
            pat.field[3] = sign;
            switch (sep_by_space)
            {
            case 0:  // No space separates the currency symbol and value.
                pat.field[1] = none;
                pat.field[2] = value;
                return;
            case 1:  // Space between currency-and-sign or currency and value.
                pat.field[1] = none;
                pat.field[2] = value;
                if (!symbol_contains_sep) {
                    // We insert the space into the symbol instead of
                    // setting pat.field[1]=space so that when
                    // showbase is not set, the space goes away too.
                    __curr_symbol_.push_back(space_char);
                }
                return;
            case 2:  // Space between sign and currency or value.
                pat.field[1] = value;
                pat.field[2] = space;
                if (symbol_contains_sep) {
                    // Remove the separator from the symbol, since it
                    // will appear before the sign.
                    __curr_symbol_.pop_back();
                }
                return;
            default:
                break;
            }
            break;
        case 3:  // The sign string immediately precedes the currency symbol.
            pat.field[0] = sign;
            pat.field[3] = value;
            switch (sep_by_space)
            {
            case 0:  // No space separates the currency symbol and value.
                pat.field[1] = symbol;
                pat.field[2] = none;
                return;
            case 1:  // Space between currency-and-sign or currency and value.
                pat.field[1] = symbol;
                pat.field[2] = none;
                if (!symbol_contains_sep) {
                    // We insert the space into the symbol instead of
                    // setting pat.field[2]=space so that when
                    // showbase is not set, the space goes away too.
                    __curr_symbol_.push_back(space_char);
                }
                return;
            case 2:  // Space between sign and currency or value.
                pat.field[1] = space;
                pat.field[2] = symbol;
                if (symbol_contains_sep) {
                    // Remove the separator from the symbol, since it
                    // has already appeared after the sign.
                    __curr_symbol_.pop_back();
                }
                return;
            default:
                break;
            }
            break;
        case 4:  // The sign string immediately succeeds the currency symbol.
            pat.field[0] = symbol;
            pat.field[3] = value;
            switch (sep_by_space)
            {
            case 0:  // No space separates the currency symbol and value.
                pat.field[1] = sign;
                pat.field[2] = none;
                return;
            case 1:  // Space between currency-and-sign or currency and value.
                pat.field[1] = sign;
                pat.field[2] = space;
                if (symbol_contains_sep) {
                    // Remove the separator from the symbol, since it
                    // should not disappear when showbase is absent.
                    __curr_symbol_.pop_back();
                }
                return;
            case 2:  // Space between sign and currency or value.
                pat.field[1] = none;
                pat.field[2] = sign;
                if (!symbol_contains_sep) {
                    // We insert the space into the symbol instead of
                    // setting pat.field[1]=space so that when
                    // showbase is not set, the space goes away too.
                    __curr_symbol_.push_back(space_char);
                }
                return;
           default:
                break;
            }
            break;
        default:
            break;
        }
        break;
    default:
        break;
    }
    pat.field[0] = symbol;
    pat.field[1] = sign;
    pat.field[2] = none;
    pat.field[3] = value;
}

template<>
void
moneypunct_byname<char, false>::init(const char* nm)
{
    typedef moneypunct<char, false> base;
    __libcpp_unique_locale loc(nm);
    if (!loc)
        __throw_runtime_error("moneypunct_byname"
                            " failed to construct for " + string(nm));

    lconv* lc = __libcpp_localeconv_l(loc.get());
    if (!__checked_string_to_char_convert(__decimal_point_,
                                          lc->mon_decimal_point,
                                          loc.get()))
      __decimal_point_ = base::do_decimal_point();
    if (!__checked_string_to_char_convert(__thousands_sep_,
                                          lc->mon_thousands_sep,
                                          loc.get()))
      __thousands_sep_ = base::do_thousands_sep();

    __grouping_ = lc->mon_grouping;
    __curr_symbol_ = lc->currency_symbol;
    if (lc->frac_digits != CHAR_MAX)
        __frac_digits_ = lc->frac_digits;
    else
        __frac_digits_ = base::do_frac_digits();
    if (lc->p_sign_posn == 0)
        __positive_sign_ = "()";
    else
        __positive_sign_ = lc->positive_sign;
    if (lc->n_sign_posn == 0)
        __negative_sign_ = "()";
    else
        __negative_sign_ = lc->negative_sign;
    // Assume the positive and negative formats will want spaces in
    // the same places in curr_symbol since there's no way to
    // represent anything else.
    string_type __dummy_curr_symbol = __curr_symbol_;
    __init_pat(__pos_format_, __dummy_curr_symbol, false,
               lc->p_cs_precedes, lc->p_sep_by_space, lc->p_sign_posn, ' ');
    __init_pat(__neg_format_, __curr_symbol_, false,
               lc->n_cs_precedes, lc->n_sep_by_space, lc->n_sign_posn, ' ');
}

template<>
void
moneypunct_byname<char, true>::init(const char* nm)
{
    typedef moneypunct<char, true> base;
    __libcpp_unique_locale loc(nm);
    if (!loc)
        __throw_runtime_error("moneypunct_byname"
                            " failed to construct for " + string(nm));

    lconv* lc = __libcpp_localeconv_l(loc.get());
    if (!__checked_string_to_char_convert(__decimal_point_,
                                          lc->mon_decimal_point,
                                          loc.get()))
      __decimal_point_ = base::do_decimal_point();
    if (!__checked_string_to_char_convert(__thousands_sep_,
                                          lc->mon_thousands_sep,
                                          loc.get()))
      __thousands_sep_ = base::do_thousands_sep();
    __grouping_ = lc->mon_grouping;
    __curr_symbol_ = lc->int_curr_symbol;
    if (lc->int_frac_digits != CHAR_MAX)
        __frac_digits_ = lc->int_frac_digits;
    else
        __frac_digits_ = base::do_frac_digits();
#if defined(_LIBCPP_MSVCRT) || defined(__MINGW32__)
    if (lc->p_sign_posn == 0)
#else // _LIBCPP_MSVCRT
    if (lc->int_p_sign_posn == 0)
#endif // !_LIBCPP_MSVCRT
        __positive_sign_ = "()";
    else
        __positive_sign_ = lc->positive_sign;
#if defined(_LIBCPP_MSVCRT) || defined(__MINGW32__)
    if(lc->n_sign_posn == 0)
#else // _LIBCPP_MSVCRT
    if (lc->int_n_sign_posn == 0)
#endif // !_LIBCPP_MSVCRT
        __negative_sign_ = "()";
    else
        __negative_sign_ = lc->negative_sign;
    // Assume the positive and negative formats will want spaces in
    // the same places in curr_symbol since there's no way to
    // represent anything else.
    string_type __dummy_curr_symbol = __curr_symbol_;
#if defined(_LIBCPP_MSVCRT) || defined(__MINGW32__)
    __init_pat(__pos_format_, __dummy_curr_symbol, true,
               lc->p_cs_precedes, lc->p_sep_by_space, lc->p_sign_posn, ' ');
    __init_pat(__neg_format_, __curr_symbol_, true,
               lc->n_cs_precedes, lc->n_sep_by_space, lc->n_sign_posn, ' ');
#else // _LIBCPP_MSVCRT
    __init_pat(__pos_format_, __dummy_curr_symbol, true,
               lc->int_p_cs_precedes, lc->int_p_sep_by_space,
               lc->int_p_sign_posn, ' ');
    __init_pat(__neg_format_, __curr_symbol_, true,
               lc->int_n_cs_precedes, lc->int_n_sep_by_space,
               lc->int_n_sign_posn, ' ');
#endif // !_LIBCPP_MSVCRT
}

template<>
void
moneypunct_byname<wchar_t, false>::init(const char* nm)
{
    typedef moneypunct<wchar_t, false> base;
    __libcpp_unique_locale loc(nm);
    if (!loc)
        __throw_runtime_error("moneypunct_byname"
                            " failed to construct for " + string(nm));
    lconv* lc = __libcpp_localeconv_l(loc.get());
    if (!__checked_string_to_wchar_convert(__decimal_point_,
                                           lc->mon_decimal_point,
                                           loc.get()))
      __decimal_point_ = base::do_decimal_point();
    if (!__checked_string_to_wchar_convert(__thousands_sep_,
                                           lc->mon_thousands_sep,
                                           loc.get()))
      __thousands_sep_ = base::do_thousands_sep();
    __grouping_ = lc->mon_grouping;
    wchar_t wbuf[100];
    mbstate_t mb = {0};
    const char* bb = lc->currency_symbol;
    size_t j = __libcpp_mbsrtowcs_l(wbuf, &bb, countof(wbuf), &mb, loc.get());
    if (j == size_t(-1))
        __throw_runtime_error("locale not supported");
    wchar_t* wbe = wbuf + j;
    __curr_symbol_.assign(wbuf, wbe);
    if (lc->frac_digits != CHAR_MAX)
        __frac_digits_ = lc->frac_digits;
    else
        __frac_digits_ = base::do_frac_digits();
    if (lc->p_sign_posn == 0)
        __positive_sign_ = L"()";
    else
    {
        mb = mbstate_t();
        bb = lc->positive_sign;
        j = __libcpp_mbsrtowcs_l(wbuf, &bb, countof(wbuf), &mb, loc.get());
        if (j == size_t(-1))
            __throw_runtime_error("locale not supported");
        wbe = wbuf + j;
        __positive_sign_.assign(wbuf, wbe);
    }
    if (lc->n_sign_posn == 0)
        __negative_sign_ = L"()";
    else
    {
        mb = mbstate_t();
        bb = lc->negative_sign;
        j = __libcpp_mbsrtowcs_l(wbuf, &bb, countof(wbuf), &mb, loc.get());
        if (j == size_t(-1))
            __throw_runtime_error("locale not supported");
        wbe = wbuf + j;
        __negative_sign_.assign(wbuf, wbe);
    }
    // Assume the positive and negative formats will want spaces in
    // the same places in curr_symbol since there's no way to
    // represent anything else.
    string_type __dummy_curr_symbol = __curr_symbol_;
    __init_pat(__pos_format_, __dummy_curr_symbol, false,
               lc->p_cs_precedes, lc->p_sep_by_space, lc->p_sign_posn, L' ');
    __init_pat(__neg_format_, __curr_symbol_, false,
               lc->n_cs_precedes, lc->n_sep_by_space, lc->n_sign_posn, L' ');
}

template<>
void
moneypunct_byname<wchar_t, true>::init(const char* nm)
{
    typedef moneypunct<wchar_t, true> base;
    __libcpp_unique_locale loc(nm);
    if (!loc)
        __throw_runtime_error("moneypunct_byname"
                            " failed to construct for " + string(nm));

    lconv* lc = __libcpp_localeconv_l(loc.get());
    if (!__checked_string_to_wchar_convert(__decimal_point_,
                                           lc->mon_decimal_point,
                                           loc.get()))
      __decimal_point_ = base::do_decimal_point();
    if (!__checked_string_to_wchar_convert(__thousands_sep_,
                                           lc->mon_thousands_sep,
                                           loc.get()))
      __thousands_sep_ = base::do_thousands_sep();
    __grouping_ = lc->mon_grouping;
    wchar_t wbuf[100];
    mbstate_t mb = {0};
    const char* bb = lc->int_curr_symbol;
    size_t j = __libcpp_mbsrtowcs_l(wbuf, &bb, countof(wbuf), &mb, loc.get());
    if (j == size_t(-1))
        __throw_runtime_error("locale not supported");
    wchar_t* wbe = wbuf + j;
    __curr_symbol_.assign(wbuf, wbe);
    if (lc->int_frac_digits != CHAR_MAX)
        __frac_digits_ = lc->int_frac_digits;
    else
        __frac_digits_ = base::do_frac_digits();
#if defined(_LIBCPP_MSVCRT) || defined(__MINGW32__)
    if (lc->p_sign_posn == 0)
#else // _LIBCPP_MSVCRT
    if (lc->int_p_sign_posn == 0)
#endif // !_LIBCPP_MSVCRT
        __positive_sign_ = L"()";
    else
    {
        mb = mbstate_t();
        bb = lc->positive_sign;
        j = __libcpp_mbsrtowcs_l(wbuf, &bb, countof(wbuf), &mb, loc.get());
        if (j == size_t(-1))
            __throw_runtime_error("locale not supported");
        wbe = wbuf + j;
        __positive_sign_.assign(wbuf, wbe);
    }
#if defined(_LIBCPP_MSVCRT) || defined(__MINGW32__)
    if (lc->n_sign_posn == 0)
#else // _LIBCPP_MSVCRT
    if (lc->int_n_sign_posn == 0)
#endif // !_LIBCPP_MSVCRT
        __negative_sign_ = L"()";
    else
    {
        mb = mbstate_t();
        bb = lc->negative_sign;
        j = __libcpp_mbsrtowcs_l(wbuf, &bb, countof(wbuf), &mb, loc.get());
        if (j == size_t(-1))
            __throw_runtime_error("locale not supported");
        wbe = wbuf + j;
        __negative_sign_.assign(wbuf, wbe);
    }
    // Assume the positive and negative formats will want spaces in
    // the same places in curr_symbol since there's no way to
    // represent anything else.
    string_type __dummy_curr_symbol = __curr_symbol_;
#if defined(_LIBCPP_MSVCRT) || defined(__MINGW32__)
    __init_pat(__pos_format_, __dummy_curr_symbol, true,
               lc->p_cs_precedes, lc->p_sep_by_space, lc->p_sign_posn, L' ');
    __init_pat(__neg_format_, __curr_symbol_, true,
               lc->n_cs_precedes, lc->n_sep_by_space, lc->n_sign_posn, L' ');
#else // _LIBCPP_MSVCRT
    __init_pat(__pos_format_, __dummy_curr_symbol, true,
               lc->int_p_cs_precedes, lc->int_p_sep_by_space,
               lc->int_p_sign_posn, L' ');
    __init_pat(__neg_format_, __curr_symbol_, true,
               lc->int_n_cs_precedes, lc->int_n_sep_by_space,
               lc->int_n_sign_posn, L' ');
#endif // !_LIBCPP_MSVCRT
}

template class _LIBCPP_CLASS_TEMPLATE_INSTANTIATION_VIS moneypunct<char, false>;
template class _LIBCPP_CLASS_TEMPLATE_INSTANTIATION_VIS moneypunct<char, true>;
template class _LIBCPP_CLASS_TEMPLATE_INSTANTIATION_VIS moneypunct<wchar_t, false>;
template class _LIBCPP_CLASS_TEMPLATE_INSTANTIATION_VIS moneypunct<wchar_t, true>;

template class _LIBCPP_CLASS_TEMPLATE_INSTANTIATION_VIS moneypunct_byname<char, false>;
template class _LIBCPP_CLASS_TEMPLATE_INSTANTIATION_VIS moneypunct_byname<char, true>;
template class _LIBCPP_CLASS_TEMPLATE_INSTANTIATION_VIS moneypunct_byname<wchar_t, false>;
template class _LIBCPP_CLASS_TEMPLATE_INSTANTIATION_VIS moneypunct_byname<wchar_t, true>;

template class _LIBCPP_CLASS_TEMPLATE_INSTANTIATION_VIS money_get<char>;
template class _LIBCPP_CLASS_TEMPLATE_INSTANTIATION_VIS money_get<wchar_t>;

template class _LIBCPP_CLASS_TEMPLATE_INSTANTIATION_VIS __money_get<char>;
template class _LIBCPP_CLASS_TEMPLATE_INSTANTIATION_VIS __money_get<wchar_t>;

template class _LIBCPP_CLASS_TEMPLATE_INSTANTIATION_VIS money_put<char>;
template class _LIBCPP_CLASS_TEMPLATE_INSTANTIATION_VIS money_put<wchar_t>;

template class _LIBCPP_CLASS_TEMPLATE_INSTANTIATION_VIS __money_put<char>;
template class _LIBCPP_CLASS_TEMPLATE_INSTANTIATION_VIS __money_put<wchar_t>;

_LIBCPP_END_NAMESPACE_STD
