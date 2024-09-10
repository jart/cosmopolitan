#ifndef COSMOPOLITAN_LIBC_TESTLIB_BENCHMARK_H_
#define COSMOPOLITAN_LIBC_TESTLIB_BENCHMARK_H_
#include "libc/calls/struct/timespec.h"
#include "libc/stdio/stdio.h"
COSMOPOLITAN_C_START_

#define X(x) __expropriate(x)
#define V(x) __veil("r", x)

#define BENCHMARK(ITERATIONS, WORK_PER_RUN, CODE)                        \
  do {                                                                   \
    struct timespec start = timespec_mono();                             \
    for (int __i = 0; __i < ITERATIONS; ++__i) {                         \
      asm volatile("" ::: "memory");                                     \
      CODE;                                                              \
    }                                                                    \
    double total_nanos =                                                 \
        (double)timespec_tonanos(timespec_sub(timespec_mono(), start));  \
    double total_work =                                                  \
        (double)((WORK_PER_RUN) ? (WORK_PER_RUN) : 1) * (ITERATIONS);    \
    _print_benchmark_result(total_nanos, total_work, ITERATIONS, #CODE); \
  } while (0)

static void _print_benchmark_result(double total_nanos, double total_work,
                                    int iterations, const char* code) {
  double time_per_op = total_nanos / iterations;
  double throughput = total_work / (total_nanos * 1e-9);
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
  } else {
    time_value = time_per_op;
    time_unit = "ns";
  }

  // Determine work unit
  const char* work_unit;
  double work_value = total_work / iterations;
  if (work_value >= 1e9) {
    work_value /= 1e9;
    work_unit = "G";
  } else if (work_value >= 1e6) {
    work_value /= 1e6;
    work_unit = "M";
  } else if (work_value >= 1e3) {
    work_value /= 1e3;
    work_unit = "K";
  } else {
    work_unit = " ";
  }

  printf("%8.2f %-2s %6.2f %s/s %6.2f %s %2dx %s\n", time_value, time_unit,
         throughput, throughput_unit, work_value, work_unit, iterations, code);
}

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_TESTLIB_BENCHMARK_H_ */
