// -*-mode:c++;indent-tabs-mode:nil;c-basic-offset:4;tab-width:8;coding:utf-8-*-
// vi: set et ft=cpp ts=4 sts=4 sw=4 fenc=utf-8 :vi
#ifndef CTL_UNIQUE_PTR_H_
#define CTL_UNIQUE_PTR_H_
#include "add_lvalue_reference.h"
#include "default_delete.h"
#include "is_convertible.h"
#include "is_reference.h"
#include "is_same.h"
#include "utility.h"

namespace ctl {

template<typename T, typename D = ctl::default_delete<T>>
struct unique_ptr
{
    using pointer = T*;
    using element_type = T;
    using deleter_type = D;

    pointer p;
    [[no_unique_address]] deleter_type d;

    constexpr unique_ptr(const nullptr_t = nullptr) noexcept : p(nullptr), d()
    {
    }

    constexpr explicit unique_ptr(pointer p) noexcept : p(p), d()
    {
    }

    constexpr unique_ptr(pointer p, const D& d) noexcept : p(p), d(d)
    {
    }

    constexpr unique_ptr(pointer p, D&& d) noexcept : p(p), d(ctl::move(d))
    {
    }

    template<typename U, typename E>
    constexpr unique_ptr(unique_ptr<U, E>&& u) noexcept
      : p(u.release()), d(ctl::forward<E>(u.get_deleter()))
    {
        static_assert(ctl::is_convertible<typename unique_ptr<U, E>::pointer,
                                          pointer>::value,
                      "U* must be implicitly convertible to T*");
        static_assert(
          (ctl::is_reference<D>::value && ctl::is_same<D, E>::value) ||
            (!ctl::is_reference<D>::value && ctl::is_convertible<E, D>::value),
          "The deleter must be convertible to the target deleter type");
    }

    unique_ptr(const unique_ptr&) = delete;

    constexpr ~unique_ptr() noexcept
    {
        if (p)
            d(p);
    }

    constexpr unique_ptr& operator=(unique_ptr r) noexcept
    {
        swap(r);
        return *this;
    }

    template<typename U, typename E>
    constexpr unique_ptr& operator=(unique_ptr<U, E>&& r) noexcept
    {
        reset(r.release());
        d = ctl::forward<E>(r.get_deleter());
        return *this;
    }

    constexpr unique_ptr& operator=(nullptr_t) noexcept
    {
        reset();
        return *this;
    }

    constexpr pointer release() noexcept
    {
        pointer r = p;
        p = nullptr;
        return r;
    }

    constexpr void reset(pointer p2 = pointer()) noexcept
    {
        pointer old = p;
        p = p2;
        if (old)
            d(old);
    }

    constexpr void swap(unique_ptr& r) noexcept
    {
        using ctl::swap;
        swap(p, r.p);
        swap(d, r.d);
    }

    constexpr pointer get() const noexcept
    {
        return p;
    }

    constexpr deleter_type& get_deleter() noexcept
    {
        return d;
    }

    constexpr const deleter_type& get_deleter() const noexcept
    {
        return d;
    }

    constexpr explicit operator bool() const noexcept
    {
        return p != nullptr;
    }

    constexpr typename ctl::add_lvalue_reference<T>::type operator*() const
    {
        return *p;
    }

    constexpr pointer operator->() const noexcept
    {
        return p;
    }
};

template<typename T, typename... Args>
constexpr unique_ptr<T>
make_unique(Args&&... args)
{
    return unique_ptr<T>(new T(ctl::forward<Args>(args)...));
}

template<typename T>
constexpr unique_ptr<T>
make_unique_for_overwrite()
{
    return unique_ptr<T>(new T);
}

} // namespace ctl

#endif // CTL_UNIQUE_PTR_H_
