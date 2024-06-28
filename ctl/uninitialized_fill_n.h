// -*-mode:c++;indent-tabs-mode:nil;c-basic-offset:4;tab-width:8;coding:utf-8-*-
// vi: set et ft=cpp ts=4 sts=4 sw=4 fenc=utf-8 :vi
#ifndef CTL_UNINITIALIZED_FILL_N_H_
#define CTL_UNINITIALIZED_FILL_N_H_
#include "addressof.h"
#include "allocator_traits.h"
#include "iterator_traits.h"

namespace ctl {

template<typename ForwardIt, typename Size, typename T>
ForwardIt
uninitialized_fill_n(ForwardIt first, Size n, const T& value)
{
    using ValueType = typename ctl::iterator_traits<ForwardIt>::value_type;
    ForwardIt current = first;
    try {
        for (; n > 0; ++current, --n) {
            ::new (static_cast<void*>(ctl::addressof(*current)))
              ValueType(value);
        }
        return current;
    } catch (...) {
        for (; first != current; ++first) {
            first->~ValueType();
        }
        throw;
    }
}

template<typename ForwardIt, typename Size, typename T, typename Alloc>
ForwardIt
uninitialized_fill_n(ForwardIt first, Size n, const T& value, Alloc& alloc)
{
    using AllocTraits = ctl::allocator_traits<Alloc>;
    ForwardIt current = first;
    try {
        for (; n > 0; ++current, --n) {
            AllocTraits::construct(alloc, ctl::addressof(*current), value);
        }
        return current;
    } catch (...) {
        for (; first != current; ++first) {
            AllocTraits::destroy(alloc, ctl::addressof(*first));
        }
        throw;
    }
}

} // namespace ctl

#endif // CTL_UNINITIALIZED_FILL_N_H_
