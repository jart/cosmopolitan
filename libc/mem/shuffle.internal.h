#ifndef COSMOPOLITAN_LIBC_RAND_SHUFFLE_H_
#define COSMOPOLITAN_LIBC_RAND_SHUFFLE_H_
#include "libc/intrin/xchg.internal.h"

/**
 * Fisher-Yates shuffle.
 *
 * @param R is a function like rand() → ≥0
 * @param A is a typed array
 * @param n is the number of items in A
 * @see ARRAYLEN()
 */
#define shuffle(R, A, n)                      \
  do {                                        \
    autotype(A) Array = (A);                  \
    for (size_t i = (n)-1; i >= 1; --i) {     \
      xchg(&Array[i], &Array[R() % (i + 1)]); \
    }                                         \
  } while (0)

#endif /* COSMOPOLITAN_LIBC_RAND_SHUFFLE_H_ */
