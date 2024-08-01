// -*-mode:c++;indent-tabs-mode:nil;c-basic-offset:4;tab-width:8;coding:utf-8-*-
// vi: set et ft=cpp ts=4 sts=4 sw=4 fenc=utf-8 :vi
#ifndef CTL_MOVE_BACKWARD_H_
#define CTL_MOVE_BACKWARD_H_
#include "utility.h"

namespace ctl {

template<typename BidirIt1, typename BidirIt2>
BidirIt2
move_backward(BidirIt1 first, BidirIt1 last, BidirIt2 d_last)
{
    while (first != last)
        *(--d_last) = ctl::move(*(--last));
    return d_last;
}

} // namespace ctl

#endif // CTL_MOVE_BACKWARD_H_
