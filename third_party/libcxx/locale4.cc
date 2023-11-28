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

// time_get

static
string*
init_weeks()
{
    static string weeks[14];
    weeks[0]  = "Sunday";
    weeks[1]  = "Monday";
    weeks[2]  = "Tuesday";
    weeks[3]  = "Wednesday";
    weeks[4]  = "Thursday";
    weeks[5]  = "Friday";
    weeks[6]  = "Saturday";
    weeks[7]  = "Sun";
    weeks[8]  = "Mon";
    weeks[9]  = "Tue";
    weeks[10] = "Wed";
    weeks[11] = "Thu";
    weeks[12] = "Fri";
    weeks[13] = "Sat";
    return weeks;
}

static
wstring*
init_wweeks()
{
    static wstring weeks[14];
    weeks[0]  = L"Sunday";
    weeks[1]  = L"Monday";
    weeks[2]  = L"Tuesday";
    weeks[3]  = L"Wednesday";
    weeks[4]  = L"Thursday";
    weeks[5]  = L"Friday";
    weeks[6]  = L"Saturday";
    weeks[7]  = L"Sun";
    weeks[8]  = L"Mon";
    weeks[9]  = L"Tue";
    weeks[10] = L"Wed";
    weeks[11] = L"Thu";
    weeks[12] = L"Fri";
    weeks[13] = L"Sat";
    return weeks;
}

template <>
const string*
__time_get_c_storage<char>::__weeks() const
{
    static const string* weeks = init_weeks();
    return weeks;
}

template <>
const wstring*
__time_get_c_storage<wchar_t>::__weeks() const
{
    static const wstring* weeks = init_wweeks();
    return weeks;
}

static
string*
init_months()
{
    static string months[24];
    months[0]  = "January";
    months[1]  = "February";
    months[2]  = "March";
    months[3]  = "April";
    months[4]  = "May";
    months[5]  = "June";
    months[6]  = "July";
    months[7]  = "August";
    months[8]  = "September";
    months[9]  = "October";
    months[10] = "November";
    months[11] = "December";
    months[12] = "Jan";
    months[13] = "Feb";
    months[14] = "Mar";
    months[15] = "Apr";
    months[16] = "May";
    months[17] = "Jun";
    months[18] = "Jul";
    months[19] = "Aug";
    months[20] = "Sep";
    months[21] = "Oct";
    months[22] = "Nov";
    months[23] = "Dec";
    return months;
}

static
wstring*
init_wmonths()
{
    static wstring months[24];
    months[0]  = L"January";
    months[1]  = L"February";
    months[2]  = L"March";
    months[3]  = L"April";
    months[4]  = L"May";
    months[5]  = L"June";
    months[6]  = L"July";
    months[7]  = L"August";
    months[8]  = L"September";
    months[9]  = L"October";
    months[10] = L"November";
    months[11] = L"December";
    months[12] = L"Jan";
    months[13] = L"Feb";
    months[14] = L"Mar";
    months[15] = L"Apr";
    months[16] = L"May";
    months[17] = L"Jun";
    months[18] = L"Jul";
    months[19] = L"Aug";
    months[20] = L"Sep";
    months[21] = L"Oct";
    months[22] = L"Nov";
    months[23] = L"Dec";
    return months;
}

template <>
const string*
__time_get_c_storage<char>::__months() const
{
    static const string* months = init_months();
    return months;
}

template <>
const wstring*
__time_get_c_storage<wchar_t>::__months() const
{
    static const wstring* months = init_wmonths();
    return months;
}

static
string*
init_am_pm()
{
    static string am_pm[2];
    am_pm[0]  = "AM";
    am_pm[1]  = "PM";
    return am_pm;
}

static
wstring*
init_wam_pm()
{
    static wstring am_pm[2];
    am_pm[0]  = L"AM";
    am_pm[1]  = L"PM";
    return am_pm;
}

template <>
const string*
__time_get_c_storage<char>::__am_pm() const
{
    static const string* am_pm = init_am_pm();
    return am_pm;
}

template <>
const wstring*
__time_get_c_storage<wchar_t>::__am_pm() const
{
    static const wstring* am_pm = init_wam_pm();
    return am_pm;
}

template <>
const string&
__time_get_c_storage<char>::__x() const
{
    static string s("%m/%d/%y");
    return s;
}

template <>
const wstring&
__time_get_c_storage<wchar_t>::__x() const
{
    static wstring s(L"%m/%d/%y");
    return s;
}

template <>
const string&
__time_get_c_storage<char>::__X() const
{
    static string s("%H:%M:%S");
    return s;
}

template <>
const wstring&
__time_get_c_storage<wchar_t>::__X() const
{
    static wstring s(L"%H:%M:%S");
    return s;
}

template <>
const string&
__time_get_c_storage<char>::__c() const
{
    static string s("%a %b %d %H:%M:%S %Y");
    return s;
}

template <>
const wstring&
__time_get_c_storage<wchar_t>::__c() const
{
    static wstring s(L"%a %b %d %H:%M:%S %Y");
    return s;
}

template <>
const string&
__time_get_c_storage<char>::__r() const
{
    static string s("%I:%M:%S %p");
    return s;
}

template <>
const wstring&
__time_get_c_storage<wchar_t>::__r() const
{
    static wstring s(L"%I:%M:%S %p");
    return s;
}

// time_get_byname

__time_get::__time_get(const char* nm)
    : __loc_(newlocale(LC_ALL_MASK, nm, 0))
{
    if (__loc_ == 0)
        __throw_runtime_error("time_get_byname"
                            " failed to construct for " + string(nm));
}

__time_get::__time_get(const string& nm)
    : __loc_(newlocale(LC_ALL_MASK, nm.c_str(), 0))
{
    if (__loc_ == 0)
        __throw_runtime_error("time_get_byname"
                            " failed to construct for " + nm);
}

__time_get::~__time_get()
{
    freelocale(__loc_);
}
#if defined(__clang__)
#pragma clang diagnostic ignored "-Wmissing-field-initializers"
#endif
#if defined(__GNUG__)
#pragma GCC   diagnostic ignored "-Wmissing-field-initializers"
#endif

template <>
string
__time_get_storage<char>::__analyze(char fmt, const ctype<char>& ct)
{
    tm t = {0};
    t.tm_sec = 59;
    t.tm_min = 55;
    t.tm_hour = 23;
    t.tm_mday = 31;
    t.tm_mon = 11;
    t.tm_year = 161;
    t.tm_wday = 6;
    t.tm_yday = 364;
    t.tm_isdst = -1;
    char buf[100];
    char f[3] = {0};
    f[0] = '%';
    f[1] = fmt;
    size_t n = strftime_l(buf, countof(buf), f, &t, __loc_);
    char* bb = buf;
    char* be = buf + n;
    string result;
    while (bb != be)
    {
        if (ct.is(ctype_base::space, *bb))
        {
            result.push_back(' ');
            for (++bb; bb != be && ct.is(ctype_base::space, *bb); ++bb)
                ;
            continue;
        }
        char* w = bb;
        ios_base::iostate err = ios_base::goodbit;
        ptrdiff_t i = __scan_keyword(w, be, this->__weeks_, this->__weeks_+14,
                               ct, err, false)
                               - this->__weeks_;
        if (i < 14)
        {
            result.push_back('%');
            if (i < 7)
                result.push_back('A');
            else
                result.push_back('a');
            bb = w;
            continue;
        }
        w = bb;
        i = __scan_keyword(w, be, this->__months_, this->__months_+24,
                           ct, err, false)
                           - this->__months_;
        if (i < 24)
        {
            result.push_back('%');
            if (i < 12)
                result.push_back('B');
            else
                result.push_back('b');
            if (fmt == 'x' && ct.is(ctype_base::digit, this->__months_[i][0]))
                result.back() = 'm';
            bb = w;
            continue;
        }
        if (this->__am_pm_[0].size() + this->__am_pm_[1].size() > 0)
        {
            w = bb;
            i = __scan_keyword(w, be, this->__am_pm_, this->__am_pm_+2,
                               ct, err, false) - this->__am_pm_;
            if (i < 2)
            {
                result.push_back('%');
                result.push_back('p');
                bb = w;
                continue;
            }
        }
        w = bb;
        if (ct.is(ctype_base::digit, *bb))
        {
            switch(__get_up_to_n_digits(bb, be, err, ct, 4))
            {
            case 6:
                result.push_back('%');
                result.push_back('w');
                break;
            case 7:
                result.push_back('%');
                result.push_back('u');
                break;
            case 11:
                result.push_back('%');
                result.push_back('I');
                break;
            case 12:
                result.push_back('%');
                result.push_back('m');
                break;
            case 23:
                result.push_back('%');
                result.push_back('H');
                break;
            case 31:
                result.push_back('%');
                result.push_back('d');
                break;
            case 55:
                result.push_back('%');
                result.push_back('M');
                break;
            case 59:
                result.push_back('%');
                result.push_back('S');
                break;
            case 61:
                result.push_back('%');
                result.push_back('y');
                break;
            case 364:
                result.push_back('%');
                result.push_back('j');
                break;
            case 2061:
                result.push_back('%');
                result.push_back('Y');
                break;
            default:
                for (; w != bb; ++w)
                    result.push_back(*w);
                break;
            }
            continue;
        }
        if (*bb == '%')
        {
            result.push_back('%');
            result.push_back('%');
            ++bb;
            continue;
        }
        result.push_back(*bb);
        ++bb;
    }
    return result;
}

#if defined(__clang__)
#pragma clang diagnostic ignored "-Wmissing-braces"
#endif

template <>
wstring
__time_get_storage<wchar_t>::__analyze(char fmt, const ctype<wchar_t>& ct)
{
    tm t = {0};
    t.tm_sec = 59;
    t.tm_min = 55;
    t.tm_hour = 23;
    t.tm_mday = 31;
    t.tm_mon = 11;
    t.tm_year = 161;
    t.tm_wday = 6;
    t.tm_yday = 364;
    t.tm_isdst = -1;
    char buf[100];
    char f[3] = {0};
    f[0] = '%';
    f[1] = fmt;
    strftime_l(buf, countof(buf), f, &t, __loc_);
    wchar_t wbuf[100];
    wchar_t* wbb = wbuf;
    mbstate_t mb = {0};
    const char* bb = buf;
    size_t j = __libcpp_mbsrtowcs_l( wbb, &bb, countof(wbuf), &mb, __loc_);
    if (j == size_t(-1))
        __throw_runtime_error("locale not supported");
    wchar_t* wbe = wbb + j;
    wstring result;
    while (wbb != wbe)
    {
        if (ct.is(ctype_base::space, *wbb))
        {
            result.push_back(L' ');
            for (++wbb; wbb != wbe && ct.is(ctype_base::space, *wbb); ++wbb)
                ;
            continue;
        }
        wchar_t* w = wbb;
        ios_base::iostate err = ios_base::goodbit;
        ptrdiff_t i = __scan_keyword(w, wbe, this->__weeks_, this->__weeks_+14,
                               ct, err, false)
                               - this->__weeks_;
        if (i < 14)
        {
            result.push_back(L'%');
            if (i < 7)
                result.push_back(L'A');
            else
                result.push_back(L'a');
            wbb = w;
            continue;
        }
        w = wbb;
        i = __scan_keyword(w, wbe, this->__months_, this->__months_+24,
                           ct, err, false)
                           - this->__months_;
        if (i < 24)
        {
            result.push_back(L'%');
            if (i < 12)
                result.push_back(L'B');
            else
                result.push_back(L'b');
            if (fmt == 'x' && ct.is(ctype_base::digit, this->__months_[i][0]))
                result.back() = L'm';
            wbb = w;
            continue;
        }
        if (this->__am_pm_[0].size() + this->__am_pm_[1].size() > 0)
        {
            w = wbb;
            i = __scan_keyword(w, wbe, this->__am_pm_, this->__am_pm_+2,
                               ct, err, false) - this->__am_pm_;
            if (i < 2)
            {
                result.push_back(L'%');
                result.push_back(L'p');
                wbb = w;
                continue;
            }
        }
        w = wbb;
        if (ct.is(ctype_base::digit, *wbb))
        {
            switch(__get_up_to_n_digits(wbb, wbe, err, ct, 4))
            {
            case 6:
                result.push_back(L'%');
                result.push_back(L'w');
                break;
            case 7:
                result.push_back(L'%');
                result.push_back(L'u');
                break;
            case 11:
                result.push_back(L'%');
                result.push_back(L'I');
                break;
            case 12:
                result.push_back(L'%');
                result.push_back(L'm');
                break;
            case 23:
                result.push_back(L'%');
                result.push_back(L'H');
                break;
            case 31:
                result.push_back(L'%');
                result.push_back(L'd');
                break;
            case 55:
                result.push_back(L'%');
                result.push_back(L'M');
                break;
            case 59:
                result.push_back(L'%');
                result.push_back(L'S');
                break;
            case 61:
                result.push_back(L'%');
                result.push_back(L'y');
                break;
            case 364:
                result.push_back(L'%');
                result.push_back(L'j');
                break;
            case 2061:
                result.push_back(L'%');
                result.push_back(L'Y');
                break;
            default:
                for (; w != wbb; ++w)
                    result.push_back(*w);
                break;
            }
            continue;
        }
        if (ct.narrow(*wbb, 0) == '%')
        {
            result.push_back(L'%');
            result.push_back(L'%');
            ++wbb;
            continue;
        }
        result.push_back(*wbb);
        ++wbb;
    }
    return result;
}

template <>
void
__time_get_storage<char>::init(const ctype<char>& ct)
{
    tm t = {0};
    char buf[100];
    // __weeks_
    for (int i = 0; i < 7; ++i)
    {
        t.tm_wday = i;
        strftime_l(buf, countof(buf), "%A", &t, __loc_);
        __weeks_[i] = buf;
        strftime_l(buf, countof(buf), "%a", &t, __loc_);
        __weeks_[i+7] = buf;
    }
    // __months_
    for (int i = 0; i < 12; ++i)
    {
        t.tm_mon = i;
        strftime_l(buf, countof(buf), "%B", &t, __loc_);
        __months_[i] = buf;
        strftime_l(buf, countof(buf), "%b", &t, __loc_);
        __months_[i+12] = buf;
    }
    // __am_pm_
    t.tm_hour = 1;
    strftime_l(buf, countof(buf), "%p", &t, __loc_);
    __am_pm_[0] = buf;
    t.tm_hour = 13;
    strftime_l(buf, countof(buf), "%p", &t, __loc_);
    __am_pm_[1] = buf;
    __c_ = __analyze('c', ct);
    __r_ = __analyze('r', ct);
    __x_ = __analyze('x', ct);
    __X_ = __analyze('X', ct);
}

template <>
void
__time_get_storage<wchar_t>::init(const ctype<wchar_t>& ct)
{
    tm t = {0};
    char buf[100];
    wchar_t wbuf[100];
    wchar_t* wbe;
    mbstate_t mb = {0};
    // __weeks_
    for (int i = 0; i < 7; ++i)
    {
        t.tm_wday = i;
        strftime_l(buf, countof(buf), "%A", &t, __loc_);
        mb = mbstate_t();
        const char* bb = buf;
        size_t j = __libcpp_mbsrtowcs_l(wbuf, &bb, countof(wbuf), &mb, __loc_);
        if (j == size_t(-1))
            __throw_runtime_error("locale not supported");
        wbe = wbuf + j;
        __weeks_[i].assign(wbuf, wbe);
        strftime_l(buf, countof(buf), "%a", &t, __loc_);
        mb = mbstate_t();
        bb = buf;
        j = __libcpp_mbsrtowcs_l(wbuf, &bb, countof(wbuf), &mb, __loc_);
        if (j == size_t(-1))
            __throw_runtime_error("locale not supported");
        wbe = wbuf + j;
        __weeks_[i+7].assign(wbuf, wbe);
    }
    // __months_
    for (int i = 0; i < 12; ++i)
    {
        t.tm_mon = i;
        strftime_l(buf, countof(buf), "%B", &t, __loc_);
        mb = mbstate_t();
        const char* bb = buf;
        size_t j = __libcpp_mbsrtowcs_l(wbuf, &bb, countof(wbuf), &mb, __loc_);
        if (j == size_t(-1))
            __throw_runtime_error("locale not supported");
        wbe = wbuf + j;
        __months_[i].assign(wbuf, wbe);
        strftime_l(buf, countof(buf), "%b", &t, __loc_);
        mb = mbstate_t();
        bb = buf;
        j = __libcpp_mbsrtowcs_l(wbuf, &bb, countof(wbuf), &mb, __loc_);
        if (j == size_t(-1))
            __throw_runtime_error("locale not supported");
        wbe = wbuf + j;
        __months_[i+12].assign(wbuf, wbe);
    }
    // __am_pm_
    t.tm_hour = 1;
    strftime_l(buf, countof(buf), "%p", &t, __loc_);
    mb = mbstate_t();
    const char* bb = buf;
    size_t j = __libcpp_mbsrtowcs_l(wbuf, &bb, countof(wbuf), &mb, __loc_);
    if (j == size_t(-1))
        __throw_runtime_error("locale not supported");
    wbe = wbuf + j;
    __am_pm_[0].assign(wbuf, wbe);
    t.tm_hour = 13;
    strftime_l(buf, countof(buf), "%p", &t, __loc_);
    mb = mbstate_t();
    bb = buf;
    j = __libcpp_mbsrtowcs_l(wbuf, &bb, countof(wbuf), &mb, __loc_);
    if (j == size_t(-1))
        __throw_runtime_error("locale not supported");
    wbe = wbuf + j;
    __am_pm_[1].assign(wbuf, wbe);
    __c_ = __analyze('c', ct);
    __r_ = __analyze('r', ct);
    __x_ = __analyze('x', ct);
    __X_ = __analyze('X', ct);
}

template <class CharT>
struct _LIBCPP_HIDDEN __time_get_temp
    : public ctype_byname<CharT>
{
    explicit __time_get_temp(const char* nm)
        : ctype_byname<CharT>(nm, 1) {}
    explicit __time_get_temp(const string& nm)
        : ctype_byname<CharT>(nm, 1) {}
};

template <>
__time_get_storage<char>::__time_get_storage(const char* __nm)
    : __time_get(__nm)
{
    const __time_get_temp<char> ct(__nm);
    init(ct);
}

template <>
__time_get_storage<char>::__time_get_storage(const string& __nm)
    : __time_get(__nm)
{
    const __time_get_temp<char> ct(__nm);
    init(ct);
}

template <>
__time_get_storage<wchar_t>::__time_get_storage(const char* __nm)
    : __time_get(__nm)
{
    const __time_get_temp<wchar_t> ct(__nm);
    init(ct);
}

template <>
__time_get_storage<wchar_t>::__time_get_storage(const string& __nm)
    : __time_get(__nm)
{
    const __time_get_temp<wchar_t> ct(__nm);
    init(ct);
}

template <>
time_base::dateorder
__time_get_storage<char>::__do_date_order() const
{
    unsigned i;
    for (i = 0; i < __x_.size(); ++i)
        if (__x_[i] == '%')
            break;
    ++i;
    switch (__x_[i])
    {
    case 'y':
    case 'Y':
        for (++i; i < __x_.size(); ++i)
            if (__x_[i] == '%')
                break;
        if (i == __x_.size())
            break;
        ++i;
        switch (__x_[i])
        {
        case 'm':
            for (++i; i < __x_.size(); ++i)
                if (__x_[i] == '%')
                    break;
            if (i == __x_.size())
                break;
            ++i;
            if (__x_[i] == 'd')
                return time_base::ymd;
            break;
        case 'd':
            for (++i; i < __x_.size(); ++i)
                if (__x_[i] == '%')
                    break;
            if (i == __x_.size())
                break;
            ++i;
            if (__x_[i] == 'm')
                return time_base::ydm;
            break;
        }
        break;
    case 'm':
        for (++i; i < __x_.size(); ++i)
            if (__x_[i] == '%')
                break;
        if (i == __x_.size())
            break;
        ++i;
        if (__x_[i] == 'd')
        {
            for (++i; i < __x_.size(); ++i)
                if (__x_[i] == '%')
                    break;
            if (i == __x_.size())
                break;
            ++i;
            if (__x_[i] == 'y' || __x_[i] == 'Y')
                return time_base::mdy;
            break;
        }
        break;
    case 'd':
        for (++i; i < __x_.size(); ++i)
            if (__x_[i] == '%')
                break;
        if (i == __x_.size())
            break;
        ++i;
        if (__x_[i] == 'm')
        {
            for (++i; i < __x_.size(); ++i)
                if (__x_[i] == '%')
                    break;
            if (i == __x_.size())
                break;
            ++i;
            if (__x_[i] == 'y' || __x_[i] == 'Y')
                return time_base::dmy;
            break;
        }
        break;
    }
    return time_base::no_order;
}

template <>
time_base::dateorder
__time_get_storage<wchar_t>::__do_date_order() const
{
    unsigned i;
    for (i = 0; i < __x_.size(); ++i)
        if (__x_[i] == L'%')
            break;
    ++i;
    switch (__x_[i])
    {
    case L'y':
    case L'Y':
        for (++i; i < __x_.size(); ++i)
            if (__x_[i] == L'%')
                break;
        if (i == __x_.size())
            break;
        ++i;
        switch (__x_[i])
        {
        case L'm':
            for (++i; i < __x_.size(); ++i)
                if (__x_[i] == L'%')
                    break;
            if (i == __x_.size())
                break;
            ++i;
            if (__x_[i] == L'd')
                return time_base::ymd;
            break;
        case L'd':
            for (++i; i < __x_.size(); ++i)
                if (__x_[i] == L'%')
                    break;
            if (i == __x_.size())
                break;
            ++i;
            if (__x_[i] == L'm')
                return time_base::ydm;
            break;
        }
        break;
    case L'm':
        for (++i; i < __x_.size(); ++i)
            if (__x_[i] == L'%')
                break;
        if (i == __x_.size())
            break;
        ++i;
        if (__x_[i] == L'd')
        {
            for (++i; i < __x_.size(); ++i)
                if (__x_[i] == L'%')
                    break;
            if (i == __x_.size())
                break;
            ++i;
            if (__x_[i] == L'y' || __x_[i] == L'Y')
                return time_base::mdy;
            break;
        }
        break;
    case L'd':
        for (++i; i < __x_.size(); ++i)
            if (__x_[i] == L'%')
                break;
        if (i == __x_.size())
            break;
        ++i;
        if (__x_[i] == L'm')
        {
            for (++i; i < __x_.size(); ++i)
                if (__x_[i] == L'%')
                    break;
            if (i == __x_.size())
                break;
            ++i;
            if (__x_[i] == L'y' || __x_[i] == L'Y')
                return time_base::dmy;
            break;
        }
        break;
    }
    return time_base::no_order;
}

// time_put

__time_put::__time_put(const char* nm)
    : __loc_(newlocale(LC_ALL_MASK, nm, 0))
{
    if (__loc_ == 0)
        __throw_runtime_error("time_put_byname"
                            " failed to construct for " + string(nm));
}

__time_put::__time_put(const string& nm)
    : __loc_(newlocale(LC_ALL_MASK, nm.c_str(), 0))
{
    if (__loc_ == 0)
        __throw_runtime_error("time_put_byname"
                            " failed to construct for " + nm);
}

__time_put::~__time_put()
{
    if (__loc_ != _LIBCPP_GET_C_LOCALE)
        freelocale(__loc_);
}

void
__time_put::__do_put(char* __nb, char*& __ne, const tm* __tm,
                     char __fmt, char __mod) const
{
    char fmt[] = {'%', __fmt, __mod, 0};
    if (__mod != 0)
        swap(fmt[1], fmt[2]);
    size_t n = strftime_l(__nb, countof(__nb, __ne), fmt, __tm, __loc_);
    __ne = __nb + n;
}

void
__time_put::__do_put(wchar_t* __wb, wchar_t*& __we, const tm* __tm,
                     char __fmt, char __mod) const
{
    char __nar[100];
    char* __ne = __nar + 100;
    __do_put(__nar, __ne, __tm, __fmt, __mod);
    mbstate_t mb = {0};
    const char* __nb = __nar;
    size_t j = __libcpp_mbsrtowcs_l(__wb, &__nb, countof(__wb, __we), &mb, __loc_);
    if (j == size_t(-1))
        __throw_runtime_error("locale not supported");
    __we = __wb + j;
}

template class _LIBCPP_CLASS_TEMPLATE_INSTANTIATION_VIS time_get<char>;
template class _LIBCPP_CLASS_TEMPLATE_INSTANTIATION_VIS time_get<wchar_t>;

template class _LIBCPP_CLASS_TEMPLATE_INSTANTIATION_VIS time_get_byname<char>;
template class _LIBCPP_CLASS_TEMPLATE_INSTANTIATION_VIS time_get_byname<wchar_t>;

template class _LIBCPP_CLASS_TEMPLATE_INSTANTIATION_VIS time_put<char>;
template class _LIBCPP_CLASS_TEMPLATE_INSTANTIATION_VIS time_put<wchar_t>;

template class _LIBCPP_CLASS_TEMPLATE_INSTANTIATION_VIS time_put_byname<char>;
template class _LIBCPP_CLASS_TEMPLATE_INSTANTIATION_VIS time_put_byname<wchar_t>;

_LIBCPP_END_NAMESPACE_STD
