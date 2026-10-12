// -*-mode:c++;indent-tabs-mode:nil;c-basic-offset:4;tab-width:8;coding:utf-8-*-
// vi: set et ft=cpp ts=4 sts=4 sw=4 fenc=utf-8 :vi
#ifndef CTL_SORT_H_
#define CTL_SORT_H_
#include "is_same.h"
#include "iterator_traits.h"
#include "less.h"
#include "new.h"
#include "utility.h"

namespace ctl {

namespace detail {

void
sort_r(void*, size_t, size_t, int (*)(const void*, const void*, void*), void*);

template<typename T, typename Compare>
int
sort_compare(const T& x, const T& y, Compare& comp)
{
    if (comp(x, y))
        return -1;
    if (comp(y, x))
        return 1;
    return 0;
}

template<typename T, typename Compare>
int
sort_direct(const void* a, const void* b, void* arg)
{
    return sort_compare(*static_cast<const T*>(a),
                        *static_cast<const T*>(b),
                        *static_cast<Compare*>(arg));
}

template<typename RandomIt, typename Compare>
struct sort_context
{
    RandomIt first;
    Compare& comp;
};

template<typename RandomIt, typename Compare>
int
sort_indirect(const void* a, const void* b, void* arg)
{
    auto* ctx = static_cast<sort_context<RandomIt, Compare>*>(arg);
    return sort_compare(ctx->first[*static_cast<const size_t*>(a)],
                        ctx->first[*static_cast<const size_t*>(b)],
                        ctx->comp);
}

} // namespace detail

template<typename RandomIt, typename Compare>
void
sort(RandomIt first, RandomIt last, Compare comp)
{
    using T = typename ctl::iterator_traits<RandomIt>::value_type;
    size_t n = last - first;
    if (n < 2)
        return;
    if constexpr (ctl::is_same_v<RandomIt, T*> && __is_trivially_copyable(T)) {
        detail::sort_r(
          first, n, sizeof(T), detail::sort_direct<T, Compare>, &comp);
        return;
    }
    size_t* idx = new size_t[n];
    for (size_t i = 0; i < n; ++i)
        idx[i] = i;
    detail::sort_context<RandomIt, Compare> ctx{ first, comp };
    detail::sort_r(
      idx, n, sizeof(size_t), detail::sort_indirect<RandomIt, Compare>, &ctx);
    for (size_t i = 0; i < n; ++i) {
        if (idx[i] == i)
            continue;
        T tmp(ctl::move(first[i]));
        size_t j = i;
        for (;;) {
            size_t k = idx[j];
            idx[j] = j;
            if (k == i) {
                first[j] = ctl::move(tmp);
                break;
            }
            first[j] = ctl::move(first[k]);
            j = k;
        }
    }
    delete[] idx;
}

template<typename RandomIt>
void
sort(RandomIt first, RandomIt last)
{
    sort(first,
         last,
         ctl::less<typename ctl::iterator_traits<RandomIt>::value_type>());
}

} // namespace ctl

#endif // CTL_SORT_H_
