#include <assert.h>
#include <cxxabi.h>
#include <errno.h>
#include <stdckdint.h>
#include <stdio.h>
#include <string.h>
#include "libc/assert.h"
#include "libc/calls/struct/timespec.h"
#include "libc/cosmo.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/internal.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/time.h"

void nothing(void) {
}

void (*barrier)(void) = nothing;

struct timespec now(void) {
  struct timespec ts;
  timespec_get(&ts, TIME_UTC);
  return ts;
}

long long tonanos(struct timespec x) {
  return x.tv_sec * 1000000000ll + x.tv_nsec;
}

struct timespec tub(struct timespec a, struct timespec b) {
  a.tv_sec -= b.tv_sec;
  if (a.tv_nsec < b.tv_nsec) {
    a.tv_nsec += 1000000000;
    a.tv_sec--;
  }
  a.tv_nsec -= b.tv_nsec;
  return a;
}

#define BENCH(ITERATIONS, WORK_PER_RUN, CODE)                              \
  do {                                                                     \
    struct timespec start = now();                                         \
    for (int i = 0; i < ITERATIONS; ++i) {                                 \
      barrier();                                                           \
      CODE;                                                                \
    }                                                                      \
    long long work = WORK_PER_RUN * ITERATIONS;                            \
    double nanos = (tonanos(tub(now(), start)) + work - 1) / (double)work; \
    printf("%10g ns %2dx %s\n", nanos, ITERATIONS, #CODE);                 \
  } while (0)

char got[5632];
char huge[262144];

__static_yoink("PrintBacktraceUsingSymbols");
__static_yoink("GetSymbolTable");

int main(int argc, char *argv[]) {
  ShowCrashReports();
  const char *sym =
      "_ZN12_GLOBAL__N_116tinyBLAS_Q0_AVX2ILi0E10block_q4_010block_"
      "q8_0fE4gemmILi3ELi2ELi0EEEvllll";
  __demangle(got, sym, sizeof(got));
  printf("%s\n", got);
  BENCH(1000, 1, __demangle(got, sym, sizeof(got)));
}
