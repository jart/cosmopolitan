// -*-mode:c++;indent-tabs-mode:nil;c-basic-offset:4;tab-width:8;coding:utf-8-*-
// vi: set et ft=cpp ts=4 sts=4 sw=4 fenc=utf-8 :vi
#ifndef COSMOPOLITAN_CTL_UNIQUE_PTR_H_
#define COSMOPOLITAN_CTL_UNIQUE_PTR_H_
#include "utility.h"
#include <__type_traits/is_convertible.h>

namespace ctl {

template<typename T>
struct default_delete
{
    constexpr default_delete() noexcept = default;
    template<typename U>
    constexpr default_delete(default_delete<U>&&) noexcept
    {
    }
    constexpr void operator()(T* const p) const noexcept
    {
        delete p;
    }
};

template<typename T>
struct default_delete<T[]>
{
    constexpr default_delete() noexcept = default;
    template<typename U>
    constexpr default_delete(default_delete<U>&&) noexcept
    {
    }
    constexpr void operator()(T* const p) const noexcept
    {
        delete[] p;
    }
};

template<typename T, typename D = default_delete<T>>
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

    constexpr unique_ptr(const pointer p) noexcept : p(p), d()
    {
    }

    constexpr unique_ptr(const pointer p, auto&& d) noexcept
      : p(p), d(ctl::forward<decltype(d)>(d))
    {
    }

    template<typename U, typename E>
        requires std::is_convertible_v<U, T> && std::is_convertible_v<E, D>
    constexpr unique_ptr(unique_ptr<U, E>&& u) noexcept
      : p(u.p), d(ctl::move(u.d))
    {
        u.p = nullptr;
    }

    unique_ptr(const unique_ptr&) = delete;

    constexpr ~unique_ptr() /* noexcept */
    {
        if (p)
            d(p);
    }

    constexpr unique_ptr& operator=(unique_ptr r) noexcept
    {
        swap(r);
        return *this;
    }

    constexpr pointer release() noexcept
    {
        pointer r = p;
        p = nullptr;
        return r;
    }

    constexpr void reset(const pointer p2 = pointer()) noexcept
    {
        const pointer r = p;
        p = p2;
        if (r)
            d(r);
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
        return p;
    }

    element_type& operator*() const noexcept(noexcept(*ctl::declval<pointer>()))
    {
        if (!p)
            __builtin_trap();
        return *p;
    }

    pointer operator->() const noexcept
    {
        if (!p)
            __builtin_trap();
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
#if 0
    // You'd think that it'd work like this, but std::unique_ptr does not.
    return unique_ptr<T>(
      static_cast<T*>(::operator new(sizeof(T), align_val_t(alignof(T)))));
#else
    return unique_ptr<T>(new T);
#endif
}

// TODO(mrdomino): specializations for T[]

} // namespace ctl
#endif // COSMOPOLITAN_CTL_UNIQUE_PTR_H_
