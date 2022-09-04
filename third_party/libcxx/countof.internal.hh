#ifndef COSMOPOLITAN_THIRD_PARTY_LIBCXX_COUNTOF_H_
#define COSMOPOLITAN_THIRD_PARTY_LIBCXX_COUNTOF_H_
#include "third_party/libcxx/__config"

namespace {

template <typename T, size_t N>
inline _LIBCPP_CONSTEXPR size_t countof(const T (&)[N]) {
  return N;
}

template <typename T>
inline _LIBCPP_CONSTEXPR size_t countof(const T* const begin,
                                        const T* const end) {
  return static_cast<size_t>(end - begin);
}

} // namespace

#endif /* COSMOPOLITAN_THIRD_PARTY_LIBCXX_COUNTOF_H_ */
