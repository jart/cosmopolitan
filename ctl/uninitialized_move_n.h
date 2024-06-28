// -*-mode:c++;indent-tabs-mode:nil;c-basic-offset:4;tab-width:8;coding:utf-8-*-
// vi: set et ft=cpp ts=4 sts=4 sw=4 fenc=utf-8 :vi
#ifndef CTL_UNINITIALIZED_MOVE_N_H_
#define CTL_UNINITIALIZED_MOVE_N_H_
#include "addressof.h"
#include "allocator_traits.h"
#include "iterator_traits.h"
#include "utility.h"

namespace ctl {

template<typename InputIt, typename Size, typename ForwardIt>
ForwardIt
uninitialized_move_n(InputIt first, Size n, ForwardIt d_first)
{
    using T = typename ctl::iterator_traits<ForwardIt>::value_type;
    ForwardIt current = d_first;
    try {
        for (; n > 0; ++first, (void)++current, --n) {
            ::new (static_cast<void*>(ctl::addressof(*current)))
              T(ctl::move(*first));
        }
    } catch (...) {
        for (; d_first != current; ++d_first) {
            d_first->~T();
        }
        throw;
    }
    return current;
}

template<typename InputIt, typename Size, typename ForwardIt, typename Alloc>
ForwardIt
uninitialized_move_n(InputIt first, Size n, ForwardIt d_first, Alloc& alloc)
{
    using AllocTraits = ctl::allocator_traits<Alloc>;
    ForwardIt current = d_first;
    try {
        for (; n > 0; ++first, (void)++current, --n)
            AllocTraits::construct(
              alloc, ctl::addressof(*current), ctl::move(*first));
    } catch (...) {
        for (; d_first != current; ++d_first)
            AllocTraits::destroy(alloc, ctl::addressof(*d_first));
        throw;
    }
    return current;
}

} // namespace ctl

#endif // CTL_UNINITIALIZED_MOVE_N_H_
