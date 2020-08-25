#ifndef COSMOPOLITAN_LIBC_BENCH_H_
#define COSMOPOLITAN_LIBC_BENCH_H_
#include "libc/nexgen32e/bench.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

/**
 * @fileoverview Microbenchmarking tools.
 */

#define BENCHLOOP(START, STOP, N, INIT, EXPR)                        \
  ({                                                                 \
    unsigned long Iter, Count;                                       \
    double Average, Sample, Time1, Time2;                            \
    for (Average = 1, Iter = 1, Count = (N); Iter < Count; ++Iter) { \
      INIT;                                                          \
      Time1 = START();                                               \
      EXPR;                                                          \
      Time2 = STOP();                                                \
      Sample = Time2 - Time1;                                        \
      Average += 1. / Iter * (Sample - Average);                     \
    }                                                                \
    Average;                                                         \
  })

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_BENCH_H_ */
