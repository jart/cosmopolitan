// -*-mode:c++;indent-tabs-mode:nil;c-basic-offset:4;tab-width:8;coding:utf-8-*-
// vi: set et ft=cpp ts=4 sts=4 sw=4 fenc=utf-8 :vi
#ifndef CTL_UNINITIALIZED_FILL_H_
#define CTL_UNINITIALIZED_FILL_H_
#include "addressof.h"
#include "iterator_traits.h"

namespace ctl {

template<typename ForwardIt, typename T>
void
uninitialized_fill(ForwardIt first, ForwardIt last, const T& value)
{
    using ValueType = typename ctl::iterator_traits<ForwardIt>::value_type;
    ForwardIt current = first;
    try {
        for (; current != last; ++current)
            ::new (static_cast<void*>(ctl::addressof(*current)))
              ValueType(value);
    } catch (...) {
        for (; first != current; ++first)
            first->~ValueType();
        throw;
    }
}

} // namespace ctl

#endif // CTL_UNINITIALIZED_FILL_H_
