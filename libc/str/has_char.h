// -*- c++ -*-
#ifndef COSMOPOLITAN_LIBC_STR_HAS_CHAR_H_
#define COSMOPOLITAN_LIBC_STR_HAS_CHAR_H_
#ifdef __cplusplus

template <typename T>
static bool has_char(const T (*ranges)[2], size_t n, T c) {
  unsigned l = 0;
  unsigned r = n;
  while (l < r) {
    unsigned m = (l & r) + ((l ^ r) >> 1);  // floor((a+b)/2)
    if (c < ranges[m][0]) {
      r = m;
    } else if (c > ranges[m][1]) {
      l = m + 1;
    } else {
      return true;
    }
  }
  return false;
}

#endif /* __cplusplus */
#endif /* COSMOPOLITAN_LIBC_STR_HAS_CHAR_H_ */
