// -*-mode:c++;indent-tabs-mode:nil;c-basic-offset:4;tab-width:8;coding:utf-8-*-
// vi: set et ft=cpp ts=4 sts=4 sw=4 fenc=utf-8 :vi
#ifndef CTL_FILL_N_H_
#define CTL_FILL_N_H_

namespace ctl {

template<typename OutputIt, typename Size, typename T>
OutputIt
fill_n(OutputIt first, Size count, const T& value)
{
    for (; count > 0; --count, ++first)
        *first = value;
    return first;
}

} // namespace ctl

#endif // CTL_FILL_N_H_
