// -*-mode:c++;indent-tabs-mode:nil;c-basic-offset:4;tab-width:8;coding:utf-8-*-
// vi: set et ft=cpp ts=4 sts=4 sw=4 fenc=utf-8 :vi
#ifndef CTL_SORT_H_
#define CTL_SORT_H_
#include "iterator_traits.h"
#include "less.h"
#include "partition.h"

namespace ctl {

namespace detail {

template<typename RandomIt, typename Compare>
void
quicksort(RandomIt first, RandomIt last, Compare comp)
{
    if (first < last) {
        auto pivot = ctl::partition(first, last, comp);
        quicksort(first, pivot, comp);
        quicksort(pivot + 1, last, comp);
    }
}

} // namespace detail

template<typename RandomIt, typename Compare>
void
sort(RandomIt first, RandomIt last, Compare comp)
{
    detail::quicksort(first, last, comp);
}

template<typename RandomIt>
void
sort(RandomIt first, RandomIt last)
{
    sort(first,
         last,
         ctl::less<typename ctl::iterator_traits<RandomIt>::value_type>());
}

} // namespace ctl

#endif // CTL_SORT_H_
