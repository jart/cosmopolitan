// -*-mode:c++;indent-tabs-mode:nil;c-basic-offset:4;tab-width:8;coding:utf-8-*-
// vi: set et ft=cpp ts=4 sts=4 sw=4 fenc=utf-8 :vi
#ifndef COSMOPOLITAN_CTL_UNIQUE_PTR_H_
#define COSMOPOLITAN_CTL_UNIQUE_PTR_H_
#include <__utility/forward.h>
#include <__utility/move.h>
#include <__utility/swap.h>

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

    pointer p;
    [[no_unique_address]] deleter_type d;

    constexpr unique_ptr(nullptr_t = nullptr) noexcept : p(nullptr)
    {
    }

    constexpr unique_ptr(pointer p) noexcept : p(p)
    {
    }

    constexpr unique_ptr(pointer p, auto&& d) noexcept
      : p(p), d(std::forward<decltype(d)>(d))
    {
    }

    constexpr unique_ptr(unique_ptr&& u) noexcept : p(u.p), d(std::move(u.d))
    {
        u.p = nullptr;
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
        pointer r = p;
        p = nullptr;
        return r;
    }

    inline void reset(nullptr_t = nullptr) noexcept
    {
        if (p)
            d(p);
        p = nullptr;
    }

    template<typename U>
    // TODO(mrdomino):
    /* requires is_convertible_v<U, T> */
    inline void reset(U* p2)
    {
        if (p) {
            d(p);
        }
        p = static_cast<pointer>(p2);
    }

    inline void swap(unique_ptr& r) noexcept
    {
        using std::swap;
        swap(p, r.p);
        swap(d, r.d);
    }

    inline pointer get() const noexcept
    {
        return p;
    }

    inline deleter_type& get_deleter() noexcept
    {
        return d;
    }

    inline const deleter_type& get_deleter() const noexcept
    {
        return d;
    }

    inline explicit operator bool() const noexcept
    {
        return p;
    }

    inline element_type& operator*() const
      noexcept(noexcept(*std::declval<pointer>()))
    {
        if (!p)
            __builtin_trap();
        return *p;
    }

    inline pointer operator->() const noexcept
    {
        if (!p)
            __builtin_trap();
        return p;
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
