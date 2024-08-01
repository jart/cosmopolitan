// -*-mode:c++;indent-tabs-mode:nil;c-basic-offset:4;tab-width:8;coding:utf-8-*-
// vi: set et ft=cpp ts=4 sts=4 sw=4 fenc=utf-8 :vi
#ifndef CTL_EQUAL_H_
#define CTL_EQUAL_H_

namespace ctl {

template<class InputIt1, class InputIt2>
bool
equal(InputIt1 first1, InputIt1 last1, InputIt2 first2)
{
    for (; first1 != last1; ++first1, ++first2) {
        if (!(*first1 == *first2)) {
            return false;
        }
    }
    return true;
}

// Overload that takes a predicate
template<class InputIt1, class InputIt2, class BinaryPredicate>
bool
equal(InputIt1 first1, InputIt1 last1, InputIt2 first2, BinaryPredicate pred)
{
    for (; first1 != last1; ++first1, ++first2) {
        if (!pred(*first1, *first2)) {
            return false;
        }
    }
    return true;
}

// Overloads that take two ranges (C++14 and later)
template<class InputIt1, class InputIt2>
bool
equal(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2)
{
    for (; first1 != last1 && first2 != last2; ++first1, ++first2) {
        if (!(*first1 == *first2)) {
            return false;
        }
    }
    return first1 == last1 && first2 == last2;
}

template<class InputIt1, class InputIt2, class BinaryPredicate>
bool
equal(InputIt1 first1,
      InputIt1 last1,
      InputIt2 first2,
      InputIt2 last2,
      BinaryPredicate pred)
{
    for (; first1 != last1 && first2 != last2; ++first1, ++first2) {
        if (!pred(*first1, *first2)) {
            return false;
        }
    }
    return first1 == last1 && first2 == last2;
}

} // namespace ctl

#endif /* CTL_EQUAL_H_ */
