// -*-mode:c++;indent-tabs-mode:nil;c-basic-offset:4;tab-width:8;coding:utf-8-*-
// vi: set et ft=cpp ts=4 sts=4 sw=4 fenc=utf-8 :vi
#ifndef CTL_COPY_H_
#define CTL_COPY_H_

namespace ctl {

template<class InputIt, class OutputIt>
OutputIt
copy(InputIt first, InputIt last, OutputIt d_first)
{
    while (first != last) {
        *d_first = *first;
        ++d_first;
        ++first;
    }
    return d_first;
}

} // namespace ctl

#endif // CTL_COPY_H_
