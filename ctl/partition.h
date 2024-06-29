// -*-mode:c++;indent-tabs-mode:nil;c-basic-offset:4;tab-width:8;coding:utf-8-*-
// vi: set et ft=cpp ts=4 sts=4 sw=4 fenc=utf-8 :vi
#ifndef CTL_PARTITION_H_
#define CTL_PARTITION_H_
#include "utility.h"

namespace ctl {

template<typename RandomIt, typename Compare>
RandomIt
partition(RandomIt first, RandomIt last, Compare comp)
{
    auto pivot = *ctl::move(last - 1);
    auto i = first - 1;

    for (auto j = first; j < last - 1; ++j) {
        if (comp(*j, pivot)) {
            ++i;
            ctl::swap(*i, *j);
        }
    }

    ctl::swap(*(i + 1), *(last - 1));
    return i + 1;
}

} // namespace ctl

#endif // CTL_PARTITION_H_
