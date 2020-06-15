#ifndef COSMOPOLITAN_LIBC_ALG_INSERTIONSORT_H_
#define COSMOPOLITAN_LIBC_ALG_INSERTIONSORT_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)

#define siftbackwards(C, X, V, i)                                    \
  do {                                                               \
    autotype(V) V_ = (V);                                            \
    for (autotype(i) j = (i); j && C(&V_[j - 1], &V_[j]) > 0; --j) { \
      X(&V_[j - 1], &V_[j]);                                         \
    }                                                                \
  } while (0)

#if 0
/**
 * Tiny in-place quadratic sorting algorithm.
 *
 * The only advantage to insertion sort is saving on code size when
 * there's a strong level of certainty the array won't have more than
 * sixteen items. Sometimes Insertion Sort is favored for sorting data
 * that's almost sorted. SmoothSort should be a better choice (see
 * qsort()) since it has that advantage and a linearithmic worst-case.
 */
#endif
#define INSERTIONSORT(C, X, A, n)          \
  do {                                     \
    autotype(A) A_ = (A);                  \
    autotype(n) n_ = (n);                  \
    for (autotype(n) i = 1; i < n_; ++i) { \
      siftbackwards(C, X, A_, i);          \
    }                                      \
  } while (0)

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_ALG_INSERTIONSORT_H_ */
