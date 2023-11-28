#ifndef COSMOPOLITAN_LIBC_BENCH_H_
#define COSMOPOLITAN_LIBC_BENCH_H_
#include "libc/nexgen32e/bench.h"
COSMOPOLITAN_C_START_

/**
 * @fileoverview Microbenchmarking Toolz.
 */

#define BENCHLOOPER(START, STOP, N, EXPR) \
  ({                                      \
    long Iter = 1;                        \
    long Toto = (N);                      \
    uint64_t Time1 = START();             \
    asm volatile("" ::: "memory");        \
    for (; Iter < Toto; ++Iter) {         \
      asm volatile("" ::: "memory");      \
      EXPR;                               \
      asm volatile("" ::: "memory");      \
    }                                     \
    asm volatile("" ::: "memory");        \
    uint64_t Time2 = STOP();              \
    (double)(long)(Time2 - Time1) / Iter; \
  })

#ifndef BENCHLOOP
/* TODO(jart): DELETE */
#define BENCHLOOP(START, STOP, N, INIT, EXPR)                        \
  ({                                                                 \
    double Average;                                                  \
    uint64_t Time1, Time2;                                           \
    unsigned long Iter, Count;                                       \
    for (Average = 1, Iter = 1, Count = (N); Iter < Count; ++Iter) { \
      INIT;                                                          \
      Time1 = START();                                               \
      asm volatile("" ::: "memory");                                 \
      EXPR;                                                          \
      asm volatile("" ::: "memory");                                 \
      Time2 = STOP();                                                \
      Average += 1. / Iter * ((int)(Time2 - Time1) - Average);       \
    }                                                                \
    Average;                                                         \
  })
#endif /* BENCHLOOP */

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_BENCH_H_ */
