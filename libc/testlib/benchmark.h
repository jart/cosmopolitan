#ifndef COSMOPOLITAN_LIBC_TESTLIB_BENCHMARK_H_
#define COSMOPOLITAN_LIBC_TESTLIB_BENCHMARK_H_
#include "libc/calls/struct/timespec.h"
#include "libc/stdio/stdio.h"
COSMOPOLITAN_C_START_

#define X(x) __expropriate(x)
#define V(x) __veil("r", x)

#define BENCHMARK(ITERATIONS, WORK_PER_RUN, CODE)                     \
  do {                                                                \
    struct timespec start = timespec_real();                          \
    for (int __i = 0; __i < ITERATIONS; ++__i) {                      \
      asm volatile("" ::: "memory");                                  \
      CODE;                                                           \
    }                                                                 \
    long ns = timespec_tonanos(timespec_sub(timespec_real(), start)); \
    _print_benchmark_result(ns, WORK_PER_RUN, ITERATIONS, #CODE);     \
  } while (0)

static void _print_benchmark_result(double total_nanos, double work_per_run,
                                    int iterations, const char* code) {
  double time_per_op = total_nanos / (work_per_run * iterations);
  double throughput = work_per_run / (total_nanos / iterations * 1e-9);

  const char* throughput_unit;
  const char* time_unit;
  double time_value;

  // Determine throughput unit
  if (throughput >= 1e9) {
    throughput /= 1e9;
    throughput_unit = "G";
  } else if (throughput >= 1e6) {
    throughput /= 1e6;
    throughput_unit = "M";
  } else if (throughput >= 1e3) {
    throughput /= 1e3;
    throughput_unit = "K";
  } else {
    throughput_unit = " ";
  }

  // Determine time unit
  if (time_per_op >= 1e6) {
    time_value = time_per_op / 1e6;
    time_unit = "ms";
  } else if (time_per_op >= 1e3) {
    time_value = time_per_op / 1e3;
    time_unit = "µs";
  } else if (time_per_op >= .01) {
    time_value = time_per_op;
    time_unit = "ns";
  } else {
    time_value = time_per_op * 1e3;
    time_unit = "ps";
  }

  // Determine work unit
  const char* work_unit;
  if (work_per_run >= 1e9) {
    work_per_run /= 1e9;
    work_unit = "G";
  } else if (work_per_run >= 1e6) {
    work_per_run /= 1e6;
    work_unit = "M";
  } else if (work_per_run >= 1e3) {
    work_per_run /= 1e3;
    work_unit = "K";
  } else {
    work_unit = " ";
  }

  printf("%8.2f %-2s %8.2f %s/s %6.2f %s %3dx %s\n", time_value, time_unit,
         throughput, throughput_unit, work_per_run, work_unit, iterations,
         code);
}

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_TESTLIB_BENCHMARK_H_ */
