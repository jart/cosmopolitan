// -*-mode:c++;indent-tabs-mode:nil;c-basic-offset:4;tab-width:8;coding:utf-8-*-
// vi: set et ft=cpp ts=4 sts=4 sw=4 fenc=utf-8 :vi
#ifndef CTL_LEXICOGRAPHICAL_COMPARE_H_
#define CTL_LEXICOGRAPHICAL_COMPARE_H_

namespace ctl {

template<class InputIt1, class InputIt2>
bool
lexicographical_compare(InputIt1 first1,
                        InputIt1 last1,
                        InputIt2 first2,
                        InputIt2 last2)
{
    for (; (first1 != last1) && (first2 != last2); ++first1, ++first2) {
        if (*first1 < *first2)
            return true;
        if (*first2 < *first1)
            return false;
    }
    return (first1 == last1) && (first2 != last2);
}

template<class InputIt1, class InputIt2, class Compare>
bool
lexicographical_compare(InputIt1 first1,
                        InputIt1 last1,
                        InputIt2 first2,
                        InputIt2 last2,
                        Compare comp)
{
    for (; (first1 != last1) && (first2 != last2); ++first1, ++first2) {
        if (comp(*first1, *first2))
            return true;
        if (comp(*first2, *first1))
            return false;
    }
    return (first1 == last1) && (first2 != last2);
}

} // namespace ctl

#endif /* CTL_LEXICOGRAPHICAL_COMPARE_H_ */
