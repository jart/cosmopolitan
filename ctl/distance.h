// -*-mode:c++;indent-tabs-mode:nil;c-basic-offset:4;tab-width:8;coding:utf-8-*-
// vi: set et ft=cpp ts=4 sts=4 sw=4 fenc=utf-8 :vi
#ifndef CTL_DISTANCE_H_
#define CTL_DISTANCE_H_
#include "iterator.h"
#include "iterator_traits.h"

namespace ctl {

template<class InputIter>
constexpr typename ctl::iterator_traits<InputIter>::difference_type
distance_impl(InputIter first, InputIter last, input_iterator_tag)
{
    typename ctl::iterator_traits<InputIter>::difference_type res(0);
    for (; first != last; ++first)
        ++res;
    return res;
}

template<class RandIter>
constexpr typename ctl::iterator_traits<RandIter>::difference_type
distance_impl(RandIter first, RandIter last, random_access_iterator_tag)
{
    return last - first;
}

template<class InputIter>
constexpr typename ctl::iterator_traits<InputIter>::difference_type
distance(InputIter first, InputIter last)
{
    return distance_impl(
      first,
      last,
      typename ctl::iterator_traits<InputIter>::iterator_category());
}

} // namespace ctl

#endif // CTL_DISTANCE_H_
