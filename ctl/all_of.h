// -*-mode:c++;indent-tabs-mode:nil;c-basic-offset:4;tab-width:8;coding:utf-8-*-
// vi: set et ft=cpp ts=4 sts=4 sw=4 fenc=utf-8 :vi
#ifndef CTL_ALL_OF_H_
#define CTL_ALL_OF_H_

namespace ctl {

template<class InputIt, class UnaryPredicate>
constexpr bool
all_of(InputIt first, InputIt last, UnaryPredicate p)
{
    for (; first != last; ++first)
        if (!p(*first))
            return false;
    return true;
}

} // namespace ctl

#endif // CTL_ALL_OF_H_
