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
#include "third_party/libcxx/__undef_macros"

// On Linux, wint_t and wchar_t have different signed-ness, and this causes
// lots of noise in the build log, but no bugs that I know of. 
#if defined(__clang__)
#pragma clang diagnostic ignored "-Wsign-conversion"
#endif

_LIBCPP_BEGIN_NAMESPACE_STD

#ifdef __cloc_defined
locale_t __cloc() {
  // In theory this could create a race condition. In practice
  // the race condition is non-fatal since it will just create
  // a little resource leak. Better approach would be appreciated.
  static locale_t result = newlocale(LC_ALL_MASK, "C", 0);
  return result;
}
#endif // __cloc_defined

namespace {

struct release
{
    void operator()(locale::facet* p) {p->__release_shared();}
};

template <class T, class A0>
inline
T&
make(A0 a0)
{
    static typename aligned_storage<sizeof(T)>::type buf;
    auto *obj = ::new (&buf) T(a0);
    return *obj;
}

template <class T, class A0, class A1>
inline
T&
make(A0 a0, A1 a1)
{
    static typename aligned_storage<sizeof(T)>::type buf;
    ::new (&buf) T(a0, a1);
    return *reinterpret_cast<T*>(&buf);
}

template <class T, class A0, class A1, class A2>
inline
T&
make(A0 a0, A1 a1, A2 a2)
{
    static typename aligned_storage<sizeof(T)>::type buf;
    auto *obj = ::new (&buf) T(a0, a1, a2);
    return *obj;
}

_LIBCPP_NORETURN static void __throw_runtime_error(const string &msg)
{
#ifndef _LIBCPP_NO_EXCEPTIONS
    throw runtime_error(msg);
#else
    (void)msg;
    _VSTD::abort();
#endif
}

}

#if defined(_AIX)
// Set priority to INT_MIN + 256 + 150
# pragma priority ( -2147483242 )
#endif

const locale::category locale::none;
const locale::category locale::collate;
const locale::category locale::ctype;
const locale::category locale::monetary;
const locale::category locale::numeric;
const locale::category locale::time;
const locale::category locale::messages;
const locale::category locale::all;

class _LIBCPP_HIDDEN locale::__imp
    : public facet
{
    enum {N = 28};
#if defined(_LIBCPP_COMPILER_MSVC)
// FIXME: MSVC doesn't support aligned parameters by value.
// I can't get the __sso_allocator to work here
// for MSVC I think for this reason.
    vector<facet*> facets_;
#else
    vector<facet*, __sso_allocator<facet*, N> > facets_;
#endif
    string         name_;
public:
    explicit __imp(size_t refs = 0);
    explicit __imp(const string& name, size_t refs = 0);
    __imp(const __imp&);
    __imp(const __imp&, const string&, locale::category c);
    __imp(const __imp& other, const __imp& one, locale::category c);
    __imp(const __imp&, facet* f, long id);
    ~__imp();

    const string& name() const {return name_;}
    bool has_facet(long id) const
        {return static_cast<size_t>(id) < facets_.size() && facets_[static_cast<size_t>(id)];}
    const locale::facet* use_facet(long id) const;

    static const locale& make_classic();
    static       locale& make_global();
private:
    void install(facet* f, long id);
    template <class F> void install(F* f) {install(f, f->id.__get());}
    template <class F> void install_from(const __imp& other);
};

locale::__imp::__imp(size_t refs)
    : facet(refs),
      facets_(N),
      name_("C")
{
    facets_.clear();
    install(&make<_VSTD::collate<char> >(1u));
    install(&make<_VSTD::collate<wchar_t> >(1u));
    install(&make<_VSTD::ctype<char> >(nullptr, false, 1u));
    install(&make<_VSTD::ctype<wchar_t> >(1u));
    install(&make<codecvt<char, char, mbstate_t> >(1u));
    install(&make<codecvt<wchar_t, char, mbstate_t> >(1u));
    install(&make<codecvt<char16_t, char, mbstate_t> >(1u));
    install(&make<codecvt<char32_t, char, mbstate_t> >(1u));
    install(&make<numpunct<char> >(1u));
    install(&make<numpunct<wchar_t> >(1u));
    install(&make<num_get<char> >(1u));
    install(&make<num_get<wchar_t> >(1u));
    install(&make<num_put<char> >(1u));
    install(&make<num_put<wchar_t> >(1u));
    install(&make<moneypunct<char, false> >(1u));
    install(&make<moneypunct<char, true> >(1u));
    install(&make<moneypunct<wchar_t, false> >(1u));
    install(&make<moneypunct<wchar_t, true> >(1u));
    install(&make<money_get<char> >(1u));
    install(&make<money_get<wchar_t> >(1u));
    install(&make<money_put<char> >(1u));
    install(&make<money_put<wchar_t> >(1u));
    install(&make<time_get<char> >(1u));
    install(&make<time_get<wchar_t> >(1u));
    install(&make<time_put<char> >(1u));
    install(&make<time_put<wchar_t> >(1u));
    install(&make<_VSTD::messages<char> >(1u));
    install(&make<_VSTD::messages<wchar_t> >(1u));
}

locale::__imp::__imp(const string& name, size_t refs)
    : facet(refs),
      facets_(N),
      name_(name)
{
#ifndef _LIBCPP_NO_EXCEPTIONS
    try
    {
#endif  // _LIBCPP_NO_EXCEPTIONS
        facets_ = locale::classic().__locale_->facets_;
        for (unsigned i = 0; i < facets_.size(); ++i)
            if (facets_[i])
                facets_[i]->__add_shared();
        install(new collate_byname<char>(name_));
        install(new collate_byname<wchar_t>(name_));
        install(new ctype_byname<char>(name_));
        install(new ctype_byname<wchar_t>(name_));
        install(new codecvt_byname<char, char, mbstate_t>(name_));
        install(new codecvt_byname<wchar_t, char, mbstate_t>(name_));
        install(new codecvt_byname<char16_t, char, mbstate_t>(name_));
        install(new codecvt_byname<char32_t, char, mbstate_t>(name_));
        install(new numpunct_byname<char>(name_));
        install(new numpunct_byname<wchar_t>(name_));
        install(new moneypunct_byname<char, false>(name_));
        install(new moneypunct_byname<char, true>(name_));
        install(new moneypunct_byname<wchar_t, false>(name_));
        install(new moneypunct_byname<wchar_t, true>(name_));
        install(new time_get_byname<char>(name_));
        install(new time_get_byname<wchar_t>(name_));
        install(new time_put_byname<char>(name_));
        install(new time_put_byname<wchar_t>(name_));
        install(new messages_byname<char>(name_));
        install(new messages_byname<wchar_t>(name_));
#ifndef _LIBCPP_NO_EXCEPTIONS
    }
    catch (...)
    {
        for (unsigned i = 0; i < facets_.size(); ++i)
            if (facets_[i])
                facets_[i]->__release_shared();
        throw;
    }
#endif  // _LIBCPP_NO_EXCEPTIONS
}

// NOTE avoid the `base class should be explicitly initialized in the
// copy constructor` warning emitted by GCC
#if defined(__clang__) || _GNUC_VER >= 406
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wextra"
#endif

locale::__imp::__imp(const __imp& other)
    : facets_(max<size_t>(N, other.facets_.size())),
      name_(other.name_)
{
    facets_ = other.facets_;
    for (unsigned i = 0; i < facets_.size(); ++i)
        if (facets_[i])
            facets_[i]->__add_shared();
}

#if defined(__clang__) || _GNUC_VER >= 406
#pragma GCC diagnostic pop
#endif

locale::__imp::__imp(const __imp& other, const string& name, locale::category c)
    : facets_(N),
      name_("*")
{
    facets_ = other.facets_;
    for (unsigned i = 0; i < facets_.size(); ++i)
        if (facets_[i])
            facets_[i]->__add_shared();
#ifndef _LIBCPP_NO_EXCEPTIONS
    try
    {
#endif  // _LIBCPP_NO_EXCEPTIONS
        if (c & locale::collate)
        {
            install(new collate_byname<char>(name));
            install(new collate_byname<wchar_t>(name));
        }
        if (c & locale::ctype)
        {
            install(new ctype_byname<char>(name));
            install(new ctype_byname<wchar_t>(name));
            install(new codecvt_byname<char, char, mbstate_t>(name));
            install(new codecvt_byname<wchar_t, char, mbstate_t>(name));
            install(new codecvt_byname<char16_t, char, mbstate_t>(name));
            install(new codecvt_byname<char32_t, char, mbstate_t>(name));
        }
        if (c & locale::monetary)
        {
            install(new moneypunct_byname<char, false>(name));
            install(new moneypunct_byname<char, true>(name));
            install(new moneypunct_byname<wchar_t, false>(name));
            install(new moneypunct_byname<wchar_t, true>(name));
        }
        if (c & locale::numeric)
        {
            install(new numpunct_byname<char>(name));
            install(new numpunct_byname<wchar_t>(name));
        }
        if (c & locale::time)
        {
            install(new time_get_byname<char>(name));
            install(new time_get_byname<wchar_t>(name));
            install(new time_put_byname<char>(name));
            install(new time_put_byname<wchar_t>(name));
        }
        if (c & locale::messages)
        {
            install(new messages_byname<char>(name));
            install(new messages_byname<wchar_t>(name));
        }
#ifndef _LIBCPP_NO_EXCEPTIONS
    }
    catch (...)
    {
        for (unsigned i = 0; i < facets_.size(); ++i)
            if (facets_[i])
                facets_[i]->__release_shared();
        throw;
    }
#endif  // _LIBCPP_NO_EXCEPTIONS
}

template<class F>
inline
void
locale::__imp::install_from(const locale::__imp& one)
{
    long id = F::id.__get();
    install(const_cast<F*>(static_cast<const F*>(one.use_facet(id))), id);
}

locale::__imp::__imp(const __imp& other, const __imp& one, locale::category c)
    : facets_(N),
      name_("*")
{
    facets_ = other.facets_;
    for (unsigned i = 0; i < facets_.size(); ++i)
        if (facets_[i])
            facets_[i]->__add_shared();
#ifndef _LIBCPP_NO_EXCEPTIONS
    try
    {
#endif  // _LIBCPP_NO_EXCEPTIONS
        if (c & locale::collate)
        {
            install_from<_VSTD::collate<char> >(one);
            install_from<_VSTD::collate<wchar_t> >(one);
        }
        if (c & locale::ctype)
        {
            install_from<_VSTD::ctype<char> >(one);
            install_from<_VSTD::ctype<wchar_t> >(one);
            install_from<_VSTD::codecvt<char, char, mbstate_t> >(one);
            install_from<_VSTD::codecvt<char16_t, char, mbstate_t> >(one);
            install_from<_VSTD::codecvt<char32_t, char, mbstate_t> >(one);
            install_from<_VSTD::codecvt<wchar_t, char, mbstate_t> >(one);
        }
        if (c & locale::monetary)
        {
            install_from<moneypunct<char, false> >(one);
            install_from<moneypunct<char, true> >(one);
            install_from<moneypunct<wchar_t, false> >(one);
            install_from<moneypunct<wchar_t, true> >(one);
            install_from<money_get<char> >(one);
            install_from<money_get<wchar_t> >(one);
            install_from<money_put<char> >(one);
            install_from<money_put<wchar_t> >(one);
        }
        if (c & locale::numeric)
        {
            install_from<numpunct<char> >(one);
            install_from<numpunct<wchar_t> >(one);
            install_from<num_get<char> >(one);
            install_from<num_get<wchar_t> >(one);
            install_from<num_put<char> >(one);
            install_from<num_put<wchar_t> >(one);
        }
        if (c & locale::time)
        {
            install_from<time_get<char> >(one);
            install_from<time_get<wchar_t> >(one);
            install_from<time_put<char> >(one);
            install_from<time_put<wchar_t> >(one);
        }
        if (c & locale::messages)
        {
            install_from<_VSTD::messages<char> >(one);
            install_from<_VSTD::messages<wchar_t> >(one);
        }
#ifndef _LIBCPP_NO_EXCEPTIONS
    }
    catch (...)
    {
        for (unsigned i = 0; i < facets_.size(); ++i)
            if (facets_[i])
                facets_[i]->__release_shared();
        throw;
    }
#endif  // _LIBCPP_NO_EXCEPTIONS
}

locale::__imp::__imp(const __imp& other, facet* f, long id)
    : facets_(max<size_t>(N, other.facets_.size()+1)),
      name_("*")
{
    f->__add_shared();
    unique_ptr<facet, release> hold(f);
    facets_ = other.facets_;
    for (unsigned i = 0; i < other.facets_.size(); ++i)
        if (facets_[i])
            facets_[i]->__add_shared();
    install(hold.get(), id);
}

locale::__imp::~__imp()
{
    for (unsigned i = 0; i < facets_.size(); ++i)
        if (facets_[i])
            facets_[i]->__release_shared();
}

void
locale::__imp::install(facet* f, long id)
{
    f->__add_shared();
    unique_ptr<facet, release> hold(f);
    if (static_cast<size_t>(id) >= facets_.size())
        facets_.resize(static_cast<size_t>(id+1));
    if (facets_[static_cast<size_t>(id)])
        facets_[static_cast<size_t>(id)]->__release_shared();
    facets_[static_cast<size_t>(id)] = hold.release();
}

const locale::facet*
locale::__imp::use_facet(long id) const
{
    if (!has_facet(id))
        __throw_bad_cast();
    return facets_[static_cast<size_t>(id)];
}

// locale

const locale&
locale::__imp::make_classic()
{
    // only one thread can get in here and it only gets in once
    static aligned_storage<sizeof(locale)>::type buf;
    locale* c = reinterpret_cast<locale*>(&buf);
    c->__locale_ = &make<__imp>(1u);
    return *c;
}

const locale&
locale::classic()
{
    static const locale& c = __imp::make_classic();
    return c;
}

locale&
locale::__imp::make_global()
{
    // only one thread can get in here and it only gets in once
    static aligned_storage<sizeof(locale)>::type buf;
    auto *obj = ::new (&buf) locale(locale::classic());
    return *obj;
}

locale&
locale::__global()
{
    static locale& g = __imp::make_global();
    return g;
}

locale::locale()  _NOEXCEPT
    : __locale_(__global().__locale_)
{
    __locale_->__add_shared();
}

locale::locale(const locale& l)  _NOEXCEPT
    : __locale_(l.__locale_)
{
    __locale_->__add_shared();
}

locale::~locale()
{
    __locale_->__release_shared();
}

const locale&
locale::operator=(const locale& other)  _NOEXCEPT
{
    other.__locale_->__add_shared();
    __locale_->__release_shared();
    __locale_ = other.__locale_;
    return *this;
}

locale::locale(const char* name)
    : __locale_(name ? new __imp(name)
                     : (__throw_runtime_error("locale constructed with null"), (__imp*)0))
{
    __locale_->__add_shared();
}

locale::locale(const string& name)
    : __locale_(new __imp(name))
{
    __locale_->__add_shared();
}

locale::locale(const locale& other, const char* name, category c)
    : __locale_(name ? new __imp(*other.__locale_, name, c)
                     : (__throw_runtime_error("locale constructed with null"), (__imp*)0))
{
    __locale_->__add_shared();
}

locale::locale(const locale& other, const string& name, category c)
    : __locale_(new __imp(*other.__locale_, name, c))
{
    __locale_->__add_shared();
}

locale::locale(const locale& other, const locale& one, category c)
    : __locale_(new __imp(*other.__locale_, *one.__locale_, c))
{
    __locale_->__add_shared();
}

string
locale::name() const
{
    return __locale_->name();
}

void
locale::__install_ctor(const locale& other, facet* f, long id)
{
    if (f)
        __locale_ = new __imp(*other.__locale_, f, id);
    else
        __locale_ = other.__locale_;
    __locale_->__add_shared();
}

locale
locale::global(const locale& loc)
{
    locale& g = __global();
    locale r = g;
    g = loc;
    if (g.name() != "*")
        setlocale(LC_ALL, g.name().c_str());
    return r;
}

bool
locale::has_facet(id& x) const
{
    return __locale_->has_facet(x.__get());
}

const locale::facet*
locale::use_facet(id& x) const
{
    return __locale_->use_facet(x.__get());
}

bool
locale::operator==(const locale& y) const
{
    return (__locale_ == y.__locale_)
        || (__locale_->name() != "*" && __locale_->name() == y.__locale_->name());
}

// locale::facet

locale::facet::~facet()
{
}

void
locale::facet::__on_zero_shared() _NOEXCEPT
{
    delete this;
}

// locale::id

int32_t locale::id::__next_id = 0;

namespace
{

class __fake_bind
{
    locale::id* id_;
    void (locale::id::* pmf_)();
public:
    __fake_bind(void (locale::id::* pmf)(), locale::id* id)
        : id_(id), pmf_(pmf) {}

    void operator()() const
    {
        (id_->*pmf_)();
    }
};

}

long
locale::id::__get()
{
    call_once(__flag_, __fake_bind(&locale::id::__init, this));
    return __id_ - 1;
}

void
locale::id::__init()
{
    __id_ = __libcpp_atomic_add(&__next_id, 1);
}

// template <> class collate_byname<char>

collate_byname<char>::collate_byname(const char* n, size_t refs)
    : collate<char>(refs),
      __l(newlocale(LC_ALL_MASK, n, 0))
{
    if (__l == 0)
        __throw_runtime_error("collate_byname<char>::collate_byname"
                            " failed to construct for " + string(n));
}

collate_byname<char>::collate_byname(const string& name, size_t refs)
    : collate<char>(refs),
      __l(newlocale(LC_ALL_MASK, name.c_str(), 0))
{
    if (__l == 0)
        __throw_runtime_error("collate_byname<char>::collate_byname"
                            " failed to construct for " + name);
}

collate_byname<char>::~collate_byname()
{
    freelocale(__l);
}

int
collate_byname<char>::do_compare(const char_type* __lo1, const char_type* __hi1,
                                 const char_type* __lo2, const char_type* __hi2) const
{
    string_type lhs(__lo1, __hi1);
    string_type rhs(__lo2, __hi2);
    int r = strcoll_l(lhs.c_str(), rhs.c_str(), __l);
    if (r < 0)
        return -1;
    if (r > 0)
        return 1;
    return r;
}

collate_byname<char>::string_type
collate_byname<char>::do_transform(const char_type* lo, const char_type* hi) const
{
    const string_type in(lo, hi);
    string_type out(strxfrm_l(0, in.c_str(), 0, __l), char());
    strxfrm_l(const_cast<char*>(out.c_str()), in.c_str(), out.size()+1, __l);
    return out;
}

// template <> class collate_byname<wchar_t>

collate_byname<wchar_t>::collate_byname(const char* n, size_t refs)
    : collate<wchar_t>(refs),
      __l(newlocale(LC_ALL_MASK, n, 0))
{
    if (__l == 0)
        __throw_runtime_error("collate_byname<wchar_t>::collate_byname(size_t refs)"
                            " failed to construct for " + string(n));
}

collate_byname<wchar_t>::collate_byname(const string& name, size_t refs)
    : collate<wchar_t>(refs),
      __l(newlocale(LC_ALL_MASK, name.c_str(), 0))
{
    if (__l == 0)
        __throw_runtime_error("collate_byname<wchar_t>::collate_byname(size_t refs)"
                            " failed to construct for " + name);
}

collate_byname<wchar_t>::~collate_byname()
{
    freelocale(__l);
}

int
collate_byname<wchar_t>::do_compare(const char_type* __lo1, const char_type* __hi1,
                                 const char_type* __lo2, const char_type* __hi2) const
{
    string_type lhs(__lo1, __hi1);
    string_type rhs(__lo2, __hi2);
    int r = wcscoll_l(lhs.c_str(), rhs.c_str(), __l);
    if (r < 0)
        return -1;
    if (r > 0)
        return 1;
    return r;
}

collate_byname<wchar_t>::string_type
collate_byname<wchar_t>::do_transform(const char_type* lo, const char_type* hi) const
{
    const string_type in(lo, hi);
    string_type out(wcsxfrm_l(0, in.c_str(), 0, __l), wchar_t());
    wcsxfrm_l(const_cast<wchar_t*>(out.c_str()), in.c_str(), out.size()+1, __l);
    return out;
}

// template <> class ctype<wchar_t>;

const ctype_base::mask ctype_base::space;
const ctype_base::mask ctype_base::print;
const ctype_base::mask ctype_base::cntrl;
const ctype_base::mask ctype_base::upper;
const ctype_base::mask ctype_base::lower;
const ctype_base::mask ctype_base::alpha;
const ctype_base::mask ctype_base::digit;
const ctype_base::mask ctype_base::punct;
const ctype_base::mask ctype_base::xdigit;
const ctype_base::mask ctype_base::blank;
const ctype_base::mask ctype_base::alnum;
const ctype_base::mask ctype_base::graph;
    
locale::id ctype<wchar_t>::id;

ctype<wchar_t>::~ctype()
{
}

bool
ctype<wchar_t>::do_is(mask m, char_type c) const
{
    return isascii(c) ? (ctype<char>::classic_table()[c] & m) != 0 : false;
}

const wchar_t*
ctype<wchar_t>::do_is(const char_type* low, const char_type* high, mask* vec) const
{
    for (; low != high; ++low, ++vec)
        *vec = static_cast<mask>(isascii(*low) ?
                                   ctype<char>::classic_table()[*low] : 0);
    return low;
}

const wchar_t*
ctype<wchar_t>::do_scan_is(mask m, const char_type* low, const char_type* high) const
{
    for (; low != high; ++low)
        if (isascii(*low) && (ctype<char>::classic_table()[*low] & m))
            break;
    return low;
}

const wchar_t*
ctype<wchar_t>::do_scan_not(mask m, const char_type* low, const char_type* high) const
{
    for (; low != high; ++low)
        if (!(isascii(*low) && (ctype<char>::classic_table()[*low] & m)))
            break;
    return low;
}

wchar_t
ctype<wchar_t>::do_toupper(char_type c) const
{
#ifdef _LIBCPP_HAS_DEFAULTRUNELOCALE
    return isascii(c) ? _DefaultRuneLocale.__mapupper[c] : c;
#elif defined(__GLIBC__) || defined(__EMSCRIPTEN__) || \
      defined(__NetBSD__)
    return isascii(c) ? ctype<char>::__classic_upper_table()[c] : c;
#else
    return (isascii(c) && iswlower_l(c, _LIBCPP_GET_C_LOCALE)) ? c-L'a'+L'A' : c;
#endif
}

const wchar_t*
ctype<wchar_t>::do_toupper(char_type* low, const char_type* high) const
{
    for (; low != high; ++low)
#ifdef _LIBCPP_HAS_DEFAULTRUNELOCALE
        *low = isascii(*low) ? _DefaultRuneLocale.__mapupper[*low] : *low;
#elif defined(__GLIBC__) || defined(__EMSCRIPTEN__) || \
      defined(__NetBSD__)
        *low = isascii(*low) ? ctype<char>::__classic_upper_table()[*low]
                             : *low;
#else
        *low = (isascii(*low) && islower_l(*low, _LIBCPP_GET_C_LOCALE)) ? (*low-L'a'+L'A') : *low;
#endif
    return low;
}

wchar_t
ctype<wchar_t>::do_tolower(char_type c) const
{
#ifdef _LIBCPP_HAS_DEFAULTRUNELOCALE
    return isascii(c) ? _DefaultRuneLocale.__maplower[c] : c;
#elif defined(__GLIBC__) || defined(__EMSCRIPTEN__) || \
      defined(__NetBSD__)
    return isascii(c) ? ctype<char>::__classic_lower_table()[c] : c;
#else
    return (isascii(c) && isupper_l(c, _LIBCPP_GET_C_LOCALE)) ? c-L'A'+'a' : c;
#endif
}

const wchar_t*
ctype<wchar_t>::do_tolower(char_type* low, const char_type* high) const
{
    for (; low != high; ++low)
#ifdef _LIBCPP_HAS_DEFAULTRUNELOCALE
        *low = isascii(*low) ? _DefaultRuneLocale.__maplower[*low] : *low;
#elif defined(__GLIBC__) || defined(__EMSCRIPTEN__) || \
      defined(__NetBSD__)
        *low = isascii(*low) ? ctype<char>::__classic_lower_table()[*low]
                             : *low;
#else
        *low = (isascii(*low) && isupper_l(*low, _LIBCPP_GET_C_LOCALE)) ? *low-L'A'+L'a' : *low;
#endif
    return low;
}

wchar_t
ctype<wchar_t>::do_widen(char c) const
{
    return c;
}

const char*
ctype<wchar_t>::do_widen(const char* low, const char* high, char_type* dest) const
{
    for (; low != high; ++low, ++dest)
        *dest = *low;
    return low;
}

char
ctype<wchar_t>::do_narrow(char_type c, char dfault) const
{
    if (isascii(c))
        return static_cast<char>(c);
    return dfault;
}

const wchar_t*
ctype<wchar_t>::do_narrow(const char_type* low, const char_type* high, char dfault, char* dest) const
{
    for (; low != high; ++low, ++dest)
        if (isascii(*low))
            *dest = static_cast<char>(*low);
        else
            *dest = dfault;
    return low;
}

// template <> class ctype<char>;

locale::id ctype<char>::id;

ctype<char>::ctype(const mask* tab, bool del, size_t refs)
    : locale::facet(refs),
      __tab_(tab),
      __del_(del)
{
  if (__tab_ == 0)
      __tab_ = classic_table();
}

ctype<char>::~ctype()
{
    if (__tab_ && __del_)
        delete [] __tab_;
}

char
ctype<char>::do_toupper(char_type c) const
{
#ifdef _LIBCPP_HAS_DEFAULTRUNELOCALE
    return isascii(c) ?
      static_cast<char>(_DefaultRuneLocale.__mapupper[static_cast<ptrdiff_t>(c)]) : c;
#elif defined(__NetBSD__)
    return static_cast<char>(__classic_upper_table()[static_cast<unsigned char>(c)]);
#elif defined(__GLIBC__) || defined(__EMSCRIPTEN__)
    return isascii(c) ?
      static_cast<char>(__classic_upper_table()[static_cast<unsigned char>(c)]) : c;
#else
    return (isascii(c) && islower_l(c, _LIBCPP_GET_C_LOCALE)) ? c-'a'+'A' : c;
#endif
}

const char*
ctype<char>::do_toupper(char_type* low, const char_type* high) const
{
    for (; low != high; ++low)
#ifdef _LIBCPP_HAS_DEFAULTRUNELOCALE
        *low = isascii(*low) ?
          static_cast<char>(_DefaultRuneLocale.__mapupper[static_cast<ptrdiff_t>(*low)]) : *low;
#elif defined(__NetBSD__)
        *low = static_cast<char>(__classic_upper_table()[static_cast<unsigned char>(*low)]);
#elif defined(__GLIBC__) || defined(__EMSCRIPTEN__)
        *low = isascii(*low) ?
          static_cast<char>(__classic_upper_table()[static_cast<size_t>(*low)]) : *low;
#else
        *low = (isascii(*low) && islower_l(*low, _LIBCPP_GET_C_LOCALE)) ? *low-'a'+'A' : *low;
#endif
    return low;
}

char
ctype<char>::do_tolower(char_type c) const
{
#ifdef _LIBCPP_HAS_DEFAULTRUNELOCALE
    return isascii(c) ?
      static_cast<char>(_DefaultRuneLocale.__maplower[static_cast<ptrdiff_t>(c)]) : c;
#elif defined(__NetBSD__)
    return static_cast<char>(__classic_lower_table()[static_cast<unsigned char>(c)]);
#elif defined(__GLIBC__) || defined(__EMSCRIPTEN__)
    return isascii(c) ?
      static_cast<char>(__classic_lower_table()[static_cast<size_t>(c)]) : c;
#else
    return (isascii(c) && isupper_l(c, _LIBCPP_GET_C_LOCALE)) ? c-'A'+'a' : c;
#endif
}

const char*
ctype<char>::do_tolower(char_type* low, const char_type* high) const
{
    for (; low != high; ++low)
#ifdef _LIBCPP_HAS_DEFAULTRUNELOCALE
        *low = isascii(*low) ? static_cast<char>(_DefaultRuneLocale.__maplower[static_cast<ptrdiff_t>(*low)]) : *low;
#elif defined(__NetBSD__)
        *low = static_cast<char>(__classic_lower_table()[static_cast<unsigned char>(*low)]);
#elif defined(__GLIBC__) || defined(__EMSCRIPTEN__)
        *low = isascii(*low) ? static_cast<char>(__classic_lower_table()[static_cast<size_t>(*low)]) : *low;
#else
        *low = (isascii(*low) && isupper_l(*low, _LIBCPP_GET_C_LOCALE)) ? *low-'A'+'a' : *low;
#endif
    return low;
}

char
ctype<char>::do_widen(char c) const
{
    return c;
}

const char*
ctype<char>::do_widen(const char* low, const char* high, char_type* dest) const
{
    for (; low != high; ++low, ++dest)
        *dest = *low;
    return low;
}

char
ctype<char>::do_narrow(char_type c, char dfault) const
{
    if (isascii(c))
        return static_cast<char>(c);
    return dfault;
}

const char*
ctype<char>::do_narrow(const char_type* low, const char_type* high, char dfault, char* dest) const
{
    for (; low != high; ++low, ++dest)
        if (isascii(*low))
            *dest = *low;
        else
            *dest = dfault;
    return low;
}

#if defined(__EMSCRIPTEN__)
extern "C" const unsigned short ** __ctype_b_loc();
extern "C" const int ** __ctype_tolower_loc();
extern "C" const int ** __ctype_toupper_loc();
#endif

#ifdef _LIBCPP_PROVIDES_DEFAULT_RUNE_TABLE
const ctype<char>::mask*
ctype<char>::classic_table()  _NOEXCEPT
{
    static _LIBCPP_CONSTEXPR const ctype<char>::mask builtin_table[table_size] = {
        cntrl,                          cntrl,
        cntrl,                          cntrl,
        cntrl,                          cntrl,
        cntrl,                          cntrl,
        cntrl,                          cntrl | space | blank,
        cntrl | space,                  cntrl | space,
        cntrl | space,                  cntrl | space,
        cntrl,                          cntrl,
        cntrl,                          cntrl,
        cntrl,                          cntrl,
        cntrl,                          cntrl,
        cntrl,                          cntrl,
        cntrl,                          cntrl,
        cntrl,                          cntrl,
        cntrl,                          cntrl,
        cntrl,                          cntrl,
        space | blank | print,          punct | print,
        punct | print,                  punct | print,
        punct | print,                  punct | print,
        punct | print,                  punct | print,
        punct | print,                  punct | print,
        punct | print,                  punct | print,
        punct | print,                  punct | print,
        punct | print,                  punct | print,
        digit | print | xdigit,         digit | print | xdigit,
        digit | print | xdigit,         digit | print | xdigit,
        digit | print | xdigit,         digit | print | xdigit,
        digit | print | xdigit,         digit | print | xdigit,
        digit | print | xdigit,         digit | print | xdigit,
        punct | print,                  punct | print,
        punct | print,                  punct | print,
        punct | print,                  punct | print,
        punct | print,                  upper | xdigit | print | alpha,
        upper | xdigit | print | alpha, upper | xdigit | print | alpha,
        upper | xdigit | print | alpha, upper | xdigit | print | alpha,
        upper | xdigit | print | alpha, upper | print | alpha,
        upper | print | alpha,          upper | print | alpha,
        upper | print | alpha,          upper | print | alpha,
        upper | print | alpha,          upper | print | alpha,
        upper | print | alpha,          upper | print | alpha,
        upper | print | alpha,          upper | print | alpha,
        upper | print | alpha,          upper | print | alpha,
        upper | print | alpha,          upper | print | alpha,
        upper | print | alpha,          upper | print | alpha,
        upper | print | alpha,          upper | print | alpha,
        upper | print | alpha,          punct | print,
        punct | print,                  punct | print,
        punct | print,                  punct | print,
        punct | print,                  lower | xdigit | print | alpha,
        lower | xdigit | print | alpha, lower | xdigit | print | alpha,
        lower | xdigit | print | alpha, lower | xdigit | print | alpha,
        lower | xdigit | print | alpha, lower | print | alpha,
        lower | print | alpha,          lower | print | alpha,
        lower | print | alpha,          lower | print | alpha,
        lower | print | alpha,          lower | print | alpha,
        lower | print | alpha,          lower | print | alpha,
        lower | print | alpha,          lower | print | alpha,
        lower | print | alpha,          lower | print | alpha,
        lower | print | alpha,          lower | print | alpha,
        lower | print | alpha,          lower | print | alpha,
        lower | print | alpha,          lower | print | alpha,
        lower | print | alpha,          punct | print,
        punct | print,                  punct | print,
        punct | print,                  cntrl,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    };
    return builtin_table;
}
#else
const ctype<char>::mask*
ctype<char>::classic_table()  _NOEXCEPT
{
#if defined(__APPLE__) || defined(__FreeBSD__)
    return _DefaultRuneLocale.__runetype;
#elif defined(__NetBSD__)
    return _C_ctype_tab_ + 1;
#elif defined(__GLIBC__)
    return _LIBCPP_GET_C_LOCALE->__ctype_b;
#elif __sun__
    return __ctype_mask;
#elif defined(_LIBCPP_MSVCRT) || defined(__MINGW32__)
    return __pctype_func();
#elif defined(__EMSCRIPTEN__)
    return *__ctype_b_loc();
#elif defined(_NEWLIB_VERSION)
    // Newlib has a 257-entry table in ctype_.c, where (char)0 starts at [1].
    return _ctype_ + 1;
#elif defined(_AIX)
    return (const unsigned int *)__lc_ctype_ptr->obj->mask;
#else
    // Platform not supported: abort so the person doing the port knows what to
    // fix
# warning  ctype<char>::classic_table() is not implemented
    printf("ctype<char>::classic_table() is not implemented\n");
    abort();
    return NULL;
#endif
}
#endif

#if defined(__GLIBC__)
const int*
ctype<char>::__classic_lower_table() _NOEXCEPT
{
    return _LIBCPP_GET_C_LOCALE->__ctype_tolower;
}

const int*
ctype<char>::__classic_upper_table() _NOEXCEPT
{
    return _LIBCPP_GET_C_LOCALE->__ctype_toupper;
}
#elif __NetBSD__
const short*
ctype<char>::__classic_lower_table() _NOEXCEPT
{
    return _C_tolower_tab_ + 1;
}

const short*
ctype<char>::__classic_upper_table() _NOEXCEPT
{
    return _C_toupper_tab_ + 1;
}

#elif defined(__EMSCRIPTEN__)
const int*
ctype<char>::__classic_lower_table() _NOEXCEPT
{
    return *__ctype_tolower_loc();
}

const int*
ctype<char>::__classic_upper_table() _NOEXCEPT
{
    return *__ctype_toupper_loc();
}
#endif // __GLIBC__ || __NETBSD__ || __EMSCRIPTEN__

// template <> class ctype_byname<char>

ctype_byname<char>::ctype_byname(const char* name, size_t refs)
    : ctype<char>(0, false, refs),
      __l(newlocale(LC_ALL_MASK, name, 0))
{
    if (__l == 0)
        __throw_runtime_error("ctype_byname<char>::ctype_byname"
                            " failed to construct for " + string(name));
}

ctype_byname<char>::ctype_byname(const string& name, size_t refs)
    : ctype<char>(0, false, refs),
      __l(newlocale(LC_ALL_MASK, name.c_str(), 0))
{
    if (__l == 0)
        __throw_runtime_error("ctype_byname<char>::ctype_byname"
                            " failed to construct for " + name);
}

ctype_byname<char>::~ctype_byname()
{
    freelocale(__l);
}

char
ctype_byname<char>::do_toupper(char_type c) const
{
    return static_cast<char>(toupper_l(static_cast<unsigned char>(c), __l));
}

const char*
ctype_byname<char>::do_toupper(char_type* low, const char_type* high) const
{
    for (; low != high; ++low)
        *low = static_cast<char>(toupper_l(static_cast<unsigned char>(*low), __l));
    return low;
}

char
ctype_byname<char>::do_tolower(char_type c) const
{
    return static_cast<char>(tolower_l(static_cast<unsigned char>(c), __l));
}

const char*
ctype_byname<char>::do_tolower(char_type* low, const char_type* high) const
{
    for (; low != high; ++low)
        *low = static_cast<char>(tolower_l(static_cast<unsigned char>(*low), __l));
    return low;
}

// template <> class ctype_byname<wchar_t>

ctype_byname<wchar_t>::ctype_byname(const char* name, size_t refs)
    : ctype<wchar_t>(refs),
      __l(newlocale(LC_ALL_MASK, name, 0))
{
    if (__l == 0)
        __throw_runtime_error("ctype_byname<wchar_t>::ctype_byname"
                            " failed to construct for " + string(name));
}

ctype_byname<wchar_t>::ctype_byname(const string& name, size_t refs)
    : ctype<wchar_t>(refs),
      __l(newlocale(LC_ALL_MASK, name.c_str(), 0))
{
    if (__l == 0)
        __throw_runtime_error("ctype_byname<wchar_t>::ctype_byname"
                            " failed to construct for " + name);
}

ctype_byname<wchar_t>::~ctype_byname()
{
    freelocale(__l);
}

bool
ctype_byname<wchar_t>::do_is(mask m, char_type c) const
{
#ifdef _LIBCPP_WCTYPE_IS_MASK
    return static_cast<bool>(iswctype_l(c, m, __l));
#else
    bool result = false;
    wint_t ch = static_cast<wint_t>(c);
    if ((m & space) == space) result |= (iswspace_l(ch, __l) != 0);
    if ((m & print) == print) result |= (iswprint_l(ch, __l) != 0);
    if ((m & cntrl) == cntrl) result |= (iswcntrl_l(ch, __l) != 0);
    if ((m & upper) == upper) result |= (iswupper_l(ch, __l) != 0);
    if ((m & lower) == lower) result |= (iswlower_l(ch, __l) != 0);
    if ((m & alpha) == alpha) result |= (iswalpha_l(ch, __l) != 0);
    if ((m & digit) == digit) result |= (iswdigit_l(ch, __l) != 0);
    if ((m & punct) == punct) result |= (iswpunct_l(ch, __l) != 0);
    if ((m & xdigit) == xdigit) result |= (iswxdigit_l(ch, __l) != 0);
    if ((m & blank) == blank) result |= (iswblank_l(ch, __l) != 0);
    return result;
#endif
}

const wchar_t*
ctype_byname<wchar_t>::do_is(const char_type* low, const char_type* high, mask* vec) const
{
    for (; low != high; ++low, ++vec)
    {
        if (isascii(*low))
            *vec = static_cast<mask>(ctype<char>::classic_table()[*low]);
        else
        {
            *vec = 0;
            wint_t ch = static_cast<wint_t>(*low);
            if (iswspace_l(ch, __l))
                *vec |= space;
#ifndef _LIBCPP_CTYPE_MASK_IS_COMPOSITE_PRINT
            if (iswprint_l(ch, __l))
                *vec |= print;
#endif
            if (iswcntrl_l(ch, __l))
                *vec |= cntrl;
            if (iswupper_l(ch, __l))
                *vec |= upper;
            if (iswlower_l(ch, __l))
                *vec |= lower;
#ifndef _LIBCPP_CTYPE_MASK_IS_COMPOSITE_ALPHA
            if (iswalpha_l(ch, __l))
                *vec |= alpha;
#endif
            if (iswdigit_l(ch, __l))
                *vec |= digit;
            if (iswpunct_l(ch, __l))
                *vec |= punct;
#ifndef _LIBCPP_CTYPE_MASK_IS_COMPOSITE_XDIGIT
            if (iswxdigit_l(ch, __l))
                *vec |= xdigit;
#endif
#if !defined(__sun__)
            if (iswblank_l(ch, __l))
                *vec |= blank;
#endif
        }
    }
    return low;
}

const wchar_t*
ctype_byname<wchar_t>::do_scan_is(mask m, const char_type* low, const char_type* high) const
{
    for (; low != high; ++low)
    {
#ifdef _LIBCPP_WCTYPE_IS_MASK
        if (iswctype_l(*low, m, __l))
            break;
#else
        wint_t ch = static_cast<wint_t>(*low);
        if ((m & space) == space && iswspace_l(ch, __l)) break;
        if ((m & print) == print && iswprint_l(ch, __l)) break;
        if ((m & cntrl) == cntrl && iswcntrl_l(ch, __l)) break;
        if ((m & upper) == upper && iswupper_l(ch, __l)) break;
        if ((m & lower) == lower && iswlower_l(ch, __l)) break;
        if ((m & alpha) == alpha && iswalpha_l(ch, __l)) break;
        if ((m & digit) == digit && iswdigit_l(ch, __l)) break;
        if ((m & punct) == punct && iswpunct_l(ch, __l)) break;
        if ((m & xdigit) == xdigit && iswxdigit_l(ch, __l)) break;
        if ((m & blank) == blank && iswblank_l(ch, __l)) break;
#endif
    }
    return low;
}

const wchar_t*
ctype_byname<wchar_t>::do_scan_not(mask m, const char_type* low, const char_type* high) const
{
    for (; low != high; ++low)
    {
#ifdef _LIBCPP_WCTYPE_IS_MASK
        if (!iswctype_l(*low, m, __l))
            break;
#else
        wint_t ch = static_cast<wint_t>(*low);
        if ((m & space) == space && iswspace_l(ch, __l)) continue;
        if ((m & print) == print && iswprint_l(ch, __l)) continue;
        if ((m & cntrl) == cntrl && iswcntrl_l(ch, __l)) continue;
        if ((m & upper) == upper && iswupper_l(ch, __l)) continue;
        if ((m & lower) == lower && iswlower_l(ch, __l)) continue;
        if ((m & alpha) == alpha && iswalpha_l(ch, __l)) continue;
        if ((m & digit) == digit && iswdigit_l(ch, __l)) continue;
        if ((m & punct) == punct && iswpunct_l(ch, __l)) continue;
        if ((m & xdigit) == xdigit && iswxdigit_l(ch, __l)) continue;
        if ((m & blank) == blank && iswblank_l(ch, __l)) continue;
        break;
#endif
    }
    return low;
}

wchar_t
ctype_byname<wchar_t>::do_toupper(char_type c) const
{
    return towupper_l(c, __l);
}

const wchar_t*
ctype_byname<wchar_t>::do_toupper(char_type* low, const char_type* high) const
{
    for (; low != high; ++low)
        *low = towupper_l(*low, __l);
    return low;
}

wchar_t
ctype_byname<wchar_t>::do_tolower(char_type c) const
{
    return towlower_l(c, __l);
}

const wchar_t*
ctype_byname<wchar_t>::do_tolower(char_type* low, const char_type* high) const
{
    for (; low != high; ++low)
        *low = towlower_l(*low, __l);
    return low;
}

wchar_t
ctype_byname<wchar_t>::do_widen(char c) const
{
    return __libcpp_btowc_l(c, __l);
}

const char*
ctype_byname<wchar_t>::do_widen(const char* low, const char* high, char_type* dest) const
{
    for (; low != high; ++low, ++dest)
        *dest = __libcpp_btowc_l(*low, __l);
    return low;
}

char
ctype_byname<wchar_t>::do_narrow(char_type c, char dfault) const
{
    int r = __libcpp_wctob_l(c, __l);
    return r != static_cast<int>(WEOF) ? static_cast<char>(r) : dfault;
}

const wchar_t*
ctype_byname<wchar_t>::do_narrow(const char_type* low, const char_type* high, char dfault, char* dest) const
{
    for (; low != high; ++low, ++dest)
    {
        int r = __libcpp_wctob_l(*low, __l);
        *dest = r != static_cast<int>(WEOF) ? static_cast<char>(r) : dfault;
    }
    return low;
}

// template <> class codecvt<char, char, mbstate_t>

locale::id codecvt<char, char, mbstate_t>::id;

codecvt<char, char, mbstate_t>::~codecvt()
{
}

codecvt<char, char, mbstate_t>::result
codecvt<char, char, mbstate_t>::do_out(state_type&,
    const intern_type* frm, const intern_type*, const intern_type*& frm_nxt,
    extern_type* to, extern_type*, extern_type*& to_nxt) const
{
    frm_nxt = frm;
    to_nxt = to;
    return noconv;
}

codecvt<char, char, mbstate_t>::result
codecvt<char, char, mbstate_t>::do_in(state_type&,
    const extern_type* frm, const extern_type*, const extern_type*& frm_nxt,
    intern_type* to, intern_type*, intern_type*& to_nxt) const
{
    frm_nxt = frm;
    to_nxt = to;
    return noconv;
}

codecvt<char, char, mbstate_t>::result
codecvt<char, char, mbstate_t>::do_unshift(state_type&,
    extern_type* to, extern_type*, extern_type*& to_nxt) const
{
    to_nxt = to;
    return noconv;
}

int
codecvt<char, char, mbstate_t>::do_encoding() const  _NOEXCEPT
{
    return 1;
}

bool
codecvt<char, char, mbstate_t>::do_always_noconv() const  _NOEXCEPT
{
    return true;
}

int
codecvt<char, char, mbstate_t>::do_length(state_type&,
    const extern_type* frm, const extern_type* end, size_t mx) const
{
    return static_cast<int>(min<size_t>(mx, static_cast<size_t>(end-frm)));
}

int
codecvt<char, char, mbstate_t>::do_max_length() const  _NOEXCEPT
{
    return 1;
}

// template <> class codecvt<wchar_t, char, mbstate_t>

locale::id codecvt<wchar_t, char, mbstate_t>::id;

codecvt<wchar_t, char, mbstate_t>::codecvt(size_t refs)
    : locale::facet(refs),
      __l(_LIBCPP_GET_C_LOCALE)
{
}

codecvt<wchar_t, char, mbstate_t>::codecvt(const char* nm, size_t refs)
    : locale::facet(refs),
      __l(newlocale(LC_ALL_MASK, nm, 0))
{
    if (__l == 0)
        __throw_runtime_error("codecvt_byname<wchar_t, char, mbstate_t>::codecvt_byname"
                            " failed to construct for " + string(nm));
}

codecvt<wchar_t, char, mbstate_t>::~codecvt()
{
    if (__l != _LIBCPP_GET_C_LOCALE)
        freelocale(__l);
}

codecvt<wchar_t, char, mbstate_t>::result
codecvt<wchar_t, char, mbstate_t>::do_out(state_type& st,
    const intern_type* frm, const intern_type* frm_end, const intern_type*& frm_nxt,
    extern_type* to, extern_type* to_end, extern_type*& to_nxt) const
{
    // look for first internal null in frm
    const intern_type* fend = frm;
    for (; fend != frm_end; ++fend)
        if (*fend == 0)
            break;
    // loop over all null-terminated sequences in frm
    to_nxt = to;
    for (frm_nxt = frm; frm != frm_end && to != to_end; frm = frm_nxt, to = to_nxt)
    {
        // save state in case it is needed to recover to_nxt on error
        mbstate_t save_state = st;
        size_t n = __libcpp_wcsnrtombs_l(to, &frm_nxt, static_cast<size_t>(fend-frm),
                                     static_cast<size_t>(to_end-to), &st, __l);
        if (n == size_t(-1))
        {
            // need to recover to_nxt
            for (to_nxt = to; frm != frm_nxt; ++frm)
            {
                n = __libcpp_wcrtomb_l(to_nxt, *frm, &save_state, __l);
                if (n == size_t(-1))
                    break;
                to_nxt += n;
            }
            frm_nxt = frm;
            return error;
        }
        if (n == 0)
            return partial;
        to_nxt += n;
        if (to_nxt == to_end)
            break;
        if (fend != frm_end)  // set up next null terminated sequence
        {
            // Try to write the terminating null
            extern_type tmp[MB_LEN_MAX];
            n = __libcpp_wcrtomb_l(tmp, intern_type(), &st, __l);
            if (n == size_t(-1))  // on error
                return error;
            if (n > static_cast<size_t>(to_end-to_nxt))  // is there room?
                return partial;
            for (extern_type* p = tmp; n; --n)  // write it
                *to_nxt++ = *p++;
            ++frm_nxt;
            // look for next null in frm
            for (fend = frm_nxt; fend != frm_end; ++fend)
                if (*fend == 0)
                    break;
        }
    }
    return frm_nxt == frm_end ? ok : partial;
}

codecvt<wchar_t, char, mbstate_t>::result
codecvt<wchar_t, char, mbstate_t>::do_in(state_type& st,
    const extern_type* frm, const extern_type* frm_end, const extern_type*& frm_nxt,
    intern_type* to, intern_type* to_end, intern_type*& to_nxt) const
{
    // look for first internal null in frm
    const extern_type* fend = frm;
    for (; fend != frm_end; ++fend)
        if (*fend == 0)
            break;
    // loop over all null-terminated sequences in frm
    to_nxt = to;
    for (frm_nxt = frm; frm != frm_end && to != to_end; frm = frm_nxt, to = to_nxt)
    {
        // save state in case it is needed to recover to_nxt on error
        mbstate_t save_state = st;
        size_t n = __libcpp_mbsnrtowcs_l(to, &frm_nxt, static_cast<size_t>(fend-frm),
                                     static_cast<size_t>(to_end-to), &st, __l);
        if (n == size_t(-1))
        {
            // need to recover to_nxt
            for (to_nxt = to; frm != frm_nxt; ++to_nxt)
            {
                n = __libcpp_mbrtowc_l(to_nxt, frm, static_cast<size_t>(fend-frm),
                                   &save_state, __l);
                switch (n)
                {
                case 0:
                    ++frm;
                    break;
                case size_t(-1):
                    frm_nxt = frm;
                    return error;
                case size_t(-2):
                    frm_nxt = frm;
                    return partial;
                default:
                    frm += n;
                    break;
                }
            }
            frm_nxt = frm;
            return frm_nxt == frm_end ? ok : partial;
        }
        if (n == size_t(-1))
            return error;
        to_nxt += n;
        if (to_nxt == to_end)
            break;
        if (fend != frm_end)  // set up next null terminated sequence
        {
            // Try to write the terminating null
            n = __libcpp_mbrtowc_l(to_nxt, frm_nxt, 1, &st, __l);
            if (n != 0)  // on error
                return error;
            ++to_nxt;
            ++frm_nxt;
            // look for next null in frm
            for (fend = frm_nxt; fend != frm_end; ++fend)
                if (*fend == 0)
                    break;
        }
    }
    return frm_nxt == frm_end ? ok : partial;
}

codecvt<wchar_t, char, mbstate_t>::result
codecvt<wchar_t, char, mbstate_t>::do_unshift(state_type& st,
    extern_type* to, extern_type* to_end, extern_type*& to_nxt) const
{
    to_nxt = to;
    extern_type tmp[MB_LEN_MAX];
    size_t n = __libcpp_wcrtomb_l(tmp, intern_type(), &st, __l);
    if (n == size_t(-1) || n == 0)  // on error
        return error;
    --n;
    if (n > static_cast<size_t>(to_end-to_nxt))  // is there room?
        return partial;
    for (extern_type* p = tmp; n; --n)  // write it
        *to_nxt++ = *p++;
    return ok;
}

int
codecvt<wchar_t, char, mbstate_t>::do_encoding() const  _NOEXCEPT
{
    if (__libcpp_mbtowc_l(nullptr, nullptr, MB_LEN_MAX, __l) != 0)
        return -1;

    // stateless encoding
    if (__l == 0 || __libcpp_mb_cur_max_l(__l) == 1)  // there are no known constant length encodings
        return 1;                // which take more than 1 char to form a wchar_t
    return 0;
}

bool
codecvt<wchar_t, char, mbstate_t>::do_always_noconv() const  _NOEXCEPT
{
    return false;
}

int
codecvt<wchar_t, char, mbstate_t>::do_length(state_type& st,
    const extern_type* frm, const extern_type* frm_end, size_t mx) const
{
    int nbytes = 0;
    for (size_t nwchar_t = 0; nwchar_t < mx && frm != frm_end; ++nwchar_t)
    {
        size_t n = __libcpp_mbrlen_l(frm, static_cast<size_t>(frm_end-frm), &st, __l);
        switch (n)
        {
        case 0:
            ++nbytes;
            ++frm;
            break;
        case size_t(-1):
        case size_t(-2):
            return nbytes;
        default:
            nbytes += n;
            frm += n;
            break;
        }
    }
    return nbytes;
}

int
codecvt<wchar_t, char, mbstate_t>::do_max_length() const  _NOEXCEPT
{
    return __l == 0 ? 1 : static_cast<int>(__libcpp_mb_cur_max_l(__l));
}

_LIBCPP_END_NAMESPACE_STD
