// -*-mode:c++;indent-tabs-mode:nil;c-basic-offset:4;tab-width:8;coding:utf-8-*-
// vi: set et ft=cpp ts=4 sts=4 sw=4 fenc=utf-8 :vi
#ifndef CTL_ITERATOR_TRAITS_H_
#define CTL_ITERATOR_TRAITS_H_
#include "utility.h"

namespace ctl {

template<class Iterator>
struct iterator_traits
{
    using iterator_category = typename Iterator::iterator_category;
    using value_type = typename Iterator::value_type;
    using difference_type = typename Iterator::difference_type;
    using pointer = typename Iterator::pointer;
    using reference = typename Iterator::reference;
};

template<class T>
struct iterator_traits<T*>
{
    using iterator_category = void*; // We don't actually use this
    using value_type = T;
    using difference_type = ptrdiff_t;
    using pointer = T*;
    using reference = T&;
};

} // namespace ctl

#endif // CTL_ITERATOR_TRAITS_H_
