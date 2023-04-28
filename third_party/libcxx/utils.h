// -*- c++ -*-
// clang-format off
#ifndef COSMOPOLITAN_THIRD_PARTY_LEARNED_SORT_UTILS_H_
#define COSMOPOLITAN_THIRD_PARTY_LEARNED_SORT_UTILS_H_
#include "third_party/libcxx/iterator"

namespace learned_sort {
namespace utils {

// Helper functions for working with unsigned types
template <typename T>
inline T _get_key(T a) {
  return a;
}

template <class RandomIt>
void insertion_sort(RandomIt begin, RandomIt end) {
  // Determine the data type
  typedef typename std::iterator_traits<RandomIt>::value_type T;

  // Determine the input size
  const size_t input_sz = std::distance(begin, end);

  if (input_sz <= 0) return;

  RandomIt cmp_idx;
  T key;
  for (auto i = begin + 1; i != end; ++i) {
    key = i[0];
    cmp_idx = i - 1;
    while (cmp_idx >= begin && cmp_idx[0] > key) {
      cmp_idx[1] = cmp_idx[0];
      --cmp_idx;
    }
    cmp_idx[1] = key;
  }
}

}  // namespace utils
}  // namespace learned_sort

#endif /* COSMOPOLITAN_THIRD_PARTY_LEARNED_SORT_UTILS_H_ */
