// -*-mode:c++;indent-tabs-mode:nil;c-basic-offset:4;tab-width:8;coding:utf-8-*-
// vi: set et ft=cpp ts=4 sts=4 sw=4 fenc=utf-8 :vi
#ifndef CTL_CONDITIONAL_H_
#define CTL_CONDITIONAL_H_

namespace ctl {

template<bool B, class T, class F>
struct conditional
{
    typedef T type;
};

template<class T, class F>
struct conditional<false, T, F>
{
    typedef F type;
};

template<bool B, typename T, typename F>
using conditional_t = typename conditional<B, T, F>::type;

} // namespace ctl

#endif // CTL_CONDITIONAL_H_
