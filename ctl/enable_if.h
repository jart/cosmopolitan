// -*-mode:c++;indent-tabs-mode:nil;c-basic-offset:4;tab-width:8;coding:utf-8-*-
// vi: set et ft=cpp ts=4 sts=4 sw=4 fenc=utf-8 :vi
#ifndef CTL_ENABLE_IF_H_
#define CTL_ENABLE_IF_H_

namespace ctl {

template<bool B, class T = void>
struct enable_if
{};

template<class T>
struct enable_if<true, T>
{
    typedef T type;
};

template<bool B, class T = void>
using enable_if_t = typename enable_if<B, T>::type;

} // namespace ctl

#endif // CTL_ENABLE_IF_H_
