// -*-mode:c++;indent-tabs-mode:nil;c-basic-offset:4;tab-width:8;coding:utf-8-*-
// vi: set et ft=cpp ts=4 sts=4 sw=4 fenc=utf-8 :vi
#ifndef CTL_IS_SORTED_H_
#define CTL_IS_SORTED_H_

namespace ctl {

template<typename It, typename Compare>
bool
is_sorted(It first, It last, Compare comp)
{
    if (first == last)
        return true;
    It next = first;
    while (++next != last) {
        if (comp(*next, *first))
            return false;
        first = next;
    }
    return true;
}

} // namespace ctl

#endif // CTL_IS_SORTED_H_
