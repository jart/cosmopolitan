// -*-mode:c++;indent-tabs-mode:nil;c-basic-offset:4;tab-width:8;coding:utf-8-*-
// vi: set et ft=cpp ts=4 sts=4 sw=4 fenc=utf-8 :vi
#ifndef CTL_ADVANCE_H_
#define CTL_ADVANCE_H_

namespace ctl {

template<class InputIt, class Distance>
constexpr void
advance(InputIt& it, Distance n)
{
    while (n > 0) {
        --n;
        ++it;
    }
    while (n < 0) {
        ++n;
        --it;
    }
}

} // namespace ctl

#endif // CTL_ADVANCE_H_
