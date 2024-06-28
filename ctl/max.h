// -*-mode:c++;indent-tabs-mode:nil;c-basic-offset:4;tab-width:8;coding:utf-8-*-
// vi: set et ft=cpp ts=4 sts=4 sw=4 fenc=utf-8 :vi
#ifndef CTL_MAX_H_
#define CTL_MAX_H_

namespace ctl {

template<class T>
inline constexpr const T&
max(const T& a, const T& b)
{
    return a < b ? b : a;
}

} // namespace ctl

#endif /* CTL_MAX_H_ */
