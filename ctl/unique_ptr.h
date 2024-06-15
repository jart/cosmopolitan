// -*-mode:c++;indent-tabs-mode:nil;c-basic-offset:4;tab-width:8;coding:utf-8-*-
// vi: set et ft=cpp ts=4 sts=4 sw=4 fenc=utf-8 :vi
#ifndef COSMOPOLITAN_CTL_UNIQUE_PTR_H_
#define COSMOPOLITAN_CTL_UNIQUE_PTR_H_
#include "compressed_pair.h"

namespace ctl {

template<typename T>
struct default_delete
{
    constexpr void operator()(T* p) const noexcept
    {
        delete p;
    }
};

template<typename T, typename D = default_delete<T>>
struct unique_ptr
{
    using pointer = T*;
    using element_type = T;
    using deleter_type = D;

    compressed_pair<pointer, D> d;

    constexpr unique_ptr(nullptr_t = nullptr) noexcept : d(nullptr, D{})
    {
    }

    constexpr unique_ptr(pointer p) noexcept : d(p, D{})
    {
    }

    constexpr unique_ptr(pointer p, auto&& d) noexcept
      : d(p, std::forward<decltype(d)>(d))
    {
    }

    constexpr unique_ptr(unique_ptr&& u) noexcept
      : d(u.d.first(), std::move(u.d.second()))
    {
        u.d.first() = nullptr;
    }

    // TODO(mrdomino):
    // template <typename U, typename E>
    // unique_ptr(unique_ptr<U, E>&& u) noexcept;

    unique_ptr(const unique_ptr&) = delete;

    inline ~unique_ptr() /* noexcept */
    {
        reset();
    }

    inline unique_ptr& operator=(unique_ptr r) noexcept
    {
        swap(r);
        return *this;
    }

    inline pointer release() noexcept
    {
        pointer r = d.first();
        d.first() = nullptr;
        return r;
    }

    inline void reset(nullptr_t = nullptr) noexcept
    {
        if (d.first())
            d.second()(d.first());
        d.first() = nullptr;
    }

    template<typename U>
    // TODO(mrdomino):
    /* requires is_convertible_v<U, T> */
    inline void reset(U* p2)
    {
        if (d.first()) {
            d.second()(d.first());
        }
        d.first() = static_cast<pointer>(p2);
    }

    inline void swap(unique_ptr& r) noexcept
    {
        using std::swap;
        swap(d, r.d);
    }

    inline pointer get() const noexcept
    {
        return d.first();
    }

    inline deleter_type& get_deleter() noexcept
    {
        return d.second();
    }

    inline const deleter_type& get_deleter() const noexcept
    {
        return d.second();
    }

    inline explicit operator bool() const noexcept
    {
        return d.first();
    }

    inline element_type& operator*() const
      noexcept(noexcept(*std::declval<pointer>()))
    {
        if (!*this)
            __builtin_trap();
        return *d.first();
    }

    inline pointer operator->() const noexcept
    {
        if (!*this)
            __builtin_trap();
        return d.first();
    }
};

template<typename T, typename... Args>
inline unique_ptr<T>
make_unique(Args&&... args)
{
    return unique_ptr<T>(new T(std::forward<Args>(args)...));
}

template<typename T>
inline unique_ptr<T>
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
