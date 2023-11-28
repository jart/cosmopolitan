#ifndef COSMOPOLITAN_LIBC_ALG_REVERSE_H_
#define COSMOPOLITAN_LIBC_ALG_REVERSE_H_
#include "libc/intrin/xchg.internal.h"

/**
 * Reverses array.
 *
 * @param ARRAY is a typed array or a pointer to one
 * @param COUNT is the number of items
 * @return pointer to start of array
 * @see ARRAYLEN()
 */
#define reverse(ARRAY, COUNT)              \
  ({                                       \
    autotype(&(ARRAY)[0]) Array = (ARRAY); \
    size_t Count = (COUNT);                \
    if (Count) {                           \
      size_t Start = 0;                    \
      size_t End = Count - 1;              \
      while (Start < End) {                \
        xchg(&Array[Start], &Array[End]);  \
        ++Start;                           \
        --End;                             \
      }                                    \
    }                                      \
    Array;                                 \
  })

#endif /* COSMOPOLITAN_LIBC_ALG_REVERSE_H_ */
