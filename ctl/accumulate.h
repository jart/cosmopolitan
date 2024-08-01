// -*-mode:c++;indent-tabs-mode:nil;c-basic-offset:4;tab-width:8;coding:utf-8-*-
// vi: set et ft=cpp ts=4 sts=4 sw=4 fenc=utf-8 :vi
#ifndef CTL_ACCUMULATE_H_
#define CTL_ACCUMULATE_H_

namespace ctl {

template<class InputIt, class T>
constexpr T
accumulate(InputIt first, InputIt last, T init)
{
    for (; first != last; ++first)
        init = init + *first;
    return init;
}

template<class InputIt, class T, class BinaryOperation>
constexpr T
accumulate(InputIt first, InputIt last, T init, BinaryOperation op)
{
    for (; first != last; ++first)
        init = op(init, *first);
    return init;
}

} // namespace ctl

#endif // CTL_ACCUMULATE_H_
