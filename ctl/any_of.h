// -*-mode:c++;indent-tabs-mode:nil;c-basic-offset:4;tab-width:8;coding:utf-8-*-
// vi: set et ft=cpp ts=4 sts=4 sw=4 fenc=utf-8 :vi
#ifndef CTL_ANY_OF_H_
#define CTL_ANY_OF_H_

namespace ctl {

template<class InputIt, class UnaryPredicate>
constexpr bool
any_of(InputIt first, InputIt last, UnaryPredicate p)
{
    for (; first != last; ++first)
        if (p(*first))
            return true;
    return false;
}

} // namespace ctl

#endif // CTL_ANY_OF_H_
