#ifndef COSMOPOLITAN_LIBC_TESTLIB_BENCHMARK_H_
#define COSMOPOLITAN_LIBC_TESTLIB_BENCHMARK_H_
#include "libc/calls/struct/timespec.h"
#include "libc/stdio/stdio.h"
COSMOPOLITAN_C_START_

#define BENCHMARK(ITERATIONS, WORK_PER_RUN, CODE)                             \
  do {                                                                        \
    struct timespec start = timespec_real();                                  \
    for (int __i = 0; __i < ITERATIONS; ++__i) {                              \
      asm volatile("" ::: "memory");                                          \
      CODE;                                                                   \
    }                                                                         \
    long long work = ((WORK_PER_RUN) ? (WORK_PER_RUN) : 1) * (ITERATIONS);    \
    double nanos =                                                            \
        (timespec_tonanos(timespec_sub(timespec_real(), start)) + work - 1) / \
        (double)work;                                                         \
    if (nanos < 1000) {                                                       \
      printf("%10g ns %2dx %s\n", nanos, (ITERATIONS), #CODE);                \
    } else {                                                                  \
      printf("%10lld ns %2dx %s\n", (long long)nanos, (ITERATIONS), #CODE);   \
    }                                                                         \
  } while (0)

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_TESTLIB_BENCHMARK_H_ */
