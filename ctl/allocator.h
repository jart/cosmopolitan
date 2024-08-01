// -*-mode:c++;indent-tabs-mode:nil;c-basic-offset:4;tab-width:8;coding:utf-8-*-
// vi: set et ft=cpp ts=4 sts=4 sw=4 fenc=utf-8 :vi
#ifndef CTL_ALLOCATOR_H_
#define CTL_ALLOCATOR_H_
#include "bad_alloc.h"
#include "integral_constant.h"
#include "new.h"
#include "utility.h"

namespace ctl {

template<typename T>
class allocator
{
  public:
    using value_type = T;
    using size_type = size_t;
    using difference_type = ptrdiff_t;
    using propagate_on_container_move_assignment = ctl::true_type;
    using is_always_equal = ctl::true_type;
    using pointer = T*;
    using const_pointer = const T*;
    using reference = T&;
    using const_reference = const T&;

    constexpr allocator() noexcept = default;

    constexpr allocator(const allocator&) noexcept = default;

    template<class U>
    constexpr allocator(const allocator<U>&) noexcept
    {
    }

    constexpr ~allocator() = default;

    [[nodiscard]] T* allocate(size_type n)
    {
        if (n > __SIZE_MAX__ / sizeof(T))
            throw ctl::bad_alloc();
        if (auto p = static_cast<T*>(::operator new(
              n * sizeof(T), ctl::align_val_t(alignof(T)), ctl::nothrow)))
            return p;
        throw ctl::bad_alloc();
    }

    void deallocate(T* p, size_type n) noexcept
    {
        ::operator delete(p, n * sizeof(T), ctl::align_val_t(alignof(T)));
    }

    template<typename U, typename... Args>
    void construct(U* p, Args&&... args)
    {
        ::new (static_cast<void*>(p)) U(ctl::forward<Args>(args)...);
    }

    template<typename U>
    void destroy(U* p)
    {
        p->~U();
    }

    size_type max_size() const noexcept
    {
        return __SIZE_MAX__ / sizeof(T);
    }

    allocator& operator=(const allocator&) = default;

    template<typename U>
    struct rebind
    {
        using other = allocator<U>;
    };
};

template<class T, class U>
bool
operator==(const allocator<T>&, const allocator<U>&) noexcept
{
    return true;
}

template<class T, class U>
bool
operator!=(const allocator<T>&, const allocator<U>&) noexcept
{
    return false;
}

} // namespace ctl

#endif // CTL_ALLOCATOR_H_
