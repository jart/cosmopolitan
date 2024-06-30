// -*-mode:c++;indent-tabs-mode:nil;c-basic-offset:4;tab-width:8;coding:utf-8-*-
// vi: set et ft=cpp ts=4 sts=4 sw=4 fenc=utf-8 :vi
#ifndef CTL_ITERATOR_TRAITS_H_
#define CTL_ITERATOR_TRAITS_H_
#include "iterator.h"
#include "utility.h"
#include "void_t.h"

namespace ctl {

template<typename Iterator, typename = void>
struct iterator_traits
{};

template<typename T>
struct iterator_traits<T*>
{
    using difference_type = ptrdiff_t;
    using value_type = T;
    using pointer = T*;
    using reference = T&;
    using iterator_category = ctl::random_access_iterator_tag;
};

template<typename Iterator>
struct iterator_traits<Iterator,
                       ctl::void_t<typename Iterator::iterator_category,
                                   typename Iterator::value_type,
                                   typename Iterator::difference_type,
                                   typename Iterator::pointer,
                                   typename Iterator::reference>>
{
    using iterator_category = typename Iterator::iterator_category;
    using value_type = typename Iterator::value_type;
    using difference_type = typename Iterator::difference_type;
    using pointer = typename Iterator::pointer;
    using reference = typename Iterator::reference;
};

} // namespace ctl

#endif // CTL_ITERATOR_TRAITS_H_
