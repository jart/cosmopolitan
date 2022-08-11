#ifndef COSMOPOLITAN_LIBC_BENCH_H_
#define COSMOPOLITAN_LIBC_BENCH_H_
#include "libc/intrin/safemacros.internal.h"
#include "libc/nexgen32e/bench.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

/**
 * @fileoverview Microbenchmarking Toolz.
 */

#ifndef BENCHLOOP
#define BENCHLOOP(START, STOP, N, INIT, EXPR)                                 \
  ({                                                                          \
    unsigned long Iter, Count;                                                \
    uint64_t Time1, Time2;                                                    \
    double Average;                                                           \
    for (Average = 1, Iter = 1, Count = (N); Iter < Count; ++Iter) {          \
      INIT;                                                                   \
      Time1 = START();                                                        \
      asm volatile("" ::: "memory");                                          \
      EXPR;                                                                   \
      asm volatile("" ::: "memory");                                          \
      Time2 = STOP();                                                         \
      Average += 1. / Iter * ((int)unsignedsubtract(Time2, Time1) - Average); \
    }                                                                         \
    Average;                                                                  \
  })
#endif /* BENCHLOOP */

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_BENCH_H_ */
