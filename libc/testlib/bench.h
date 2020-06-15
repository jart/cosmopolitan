#ifndef COSMOPOLITAN_LIBC_BENCH_H_
#define COSMOPOLITAN_LIBC_BENCH_H_
#include "libc/bits/safemacros.h"
#include "libc/nexgen32e/bench.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

/**
 * @fileoverview Microbenchmarking tools.
 */

#define BENCHLOOP(START, STOP, N, INIT, EXPR)                          \
  ({                                                                   \
    int Iter, Count;                                                   \
    long double Average, Sample, Time1, Time2;                         \
    for (Average = 1.0, Iter = 1, Count = (N); Iter < Count; ++Iter) { \
      INIT;                                                            \
      Time1 = START();                                                 \
      EXPR;                                                            \
      Time2 = STOP();                                                  \
      Sample = Time2 - Time1;                                          \
      Average += (Sample - Average) / Iter;                            \
    }                                                                  \
    Average;                                                           \
  })

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_BENCH_H_ */
