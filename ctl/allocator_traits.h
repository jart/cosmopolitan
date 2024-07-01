// -*-mode:c++;indent-tabs-mode:nil;c-basic-offset:4;tab-width:8;coding:utf-8-*-
// vi: set et ft=cpp ts=4 sts=4 sw=4 fenc=utf-8 :vi
#ifndef CTL_ALLOCATOR_TRAITS_H_
#define CTL_ALLOCATOR_TRAITS_H_
#include "integral_constant.h"

namespace ctl {

template<typename Alloc>
struct allocator_traits
{
    using allocator_type = Alloc;
    using value_type = typename Alloc::value_type;
    using pointer = typename Alloc::pointer;
    using const_pointer = typename Alloc::const_pointer;
    using void_pointer = void*;
    using const_void_pointer = const void*;
    using difference_type = typename Alloc::difference_type;
    using size_type = typename Alloc::size_type;

    using propagate_on_container_copy_assignment = ctl::false_type;
    using propagate_on_container_move_assignment = ctl::true_type;
    using propagate_on_container_swap = ctl::false_type;
    using is_always_equal = ctl::true_type;

    template<typename T>
    using rebind_alloc = typename Alloc::template rebind<T>::other;

    template<typename T>
    using rebind_traits = allocator_traits<rebind_alloc<T>>;

    static pointer allocate(Alloc& a, size_type n)
    {
        return a.allocate(n);
    }

    static void deallocate(Alloc& a, pointer p, size_type n)
    {
        a.deallocate(p, n);
    }

    template<typename T, typename... Args>
    static void construct(Alloc& a, T* p, Args&&... args)
    {
        ::new ((void*)p) T(static_cast<Args&&>(args)...);
    }

    template<typename T>
    static void destroy(Alloc& a, T* p)
    {
        p->~T();
    }

    static size_type max_size(const Alloc& a) noexcept
    {
        return __PTRDIFF_MAX__ / sizeof(value_type);
    }

    static Alloc select_on_container_copy_construction(const Alloc& a)
    {
        return a;
    }
};

} // namespace ctl

#endif // CTL_ALLOCATOR_TRAITS_H_
