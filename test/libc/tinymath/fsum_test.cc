#include "libc/assert.h"
#include "libc/calls/struct/timespec.h"
#include "libc/intrin/bsr.h"
#include "libc/macros.internal.h"
#include "libc/math.h"
#include "libc/mem/gc.h"
#include "libc/mem/mem.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/x/xasprintf.h"

int rand32(void) {
  /* Knuth, D.E., "The Art of Computer Programming," Vol 2,
     Seminumerical Algorithms, Third Edition, Addison-Wesley, 1998,
     p. 106 (line 26) & p. 108 */
  static unsigned long long lcg = 1;
  lcg *= 6364136223846793005;
  lcg += 1442695040888963407;
  return lcg >> 32;
}

float float01(unsigned x) {  // (0,1)
  return 1.f / 8388608 * ((x >> 9) + .5f);
}

float numba(void) {  // (-1,1)
  return float01(rand32()) * 2 - 1;
}

double fsumf_gold(const float *p, size_t n) {
  size_t i;
  double s;
  if (n > 8)
    return fsumf_gold(p, n / 2) + fsumf_gold(p + n / 2, n - n / 2);
  for (s = i = 0; i < n; ++i)
    s += p[i];
  return s;
}

float fsumf_linear(const float *p, size_t n) {
  float s = 0;
  for (size_t i = 0; i < n; ++i)
    s += p[i];
  return s;
}

float fsumf_kahan(const float *p, size_t n) {
  size_t i;
  float err, sum, t, y;
  sum = err = 0;
  for (i = 0; i < n; ++i) {
    y = p[i] - err;
    t = sum + y;
    err = (t - sum) - y;
    sum = t;
  }
  return sum;
}

float fsumf_logarithmic(const float *p, size_t n) {
  size_t i;
  float s;
  if (n > 32)
    return fsumf_logarithmic(p, n / 2) +
           fsumf_logarithmic(p + n / 2, n - n / 2);
  for (s = i = 0; i < n; ++i)
    s += p[i];
  return s;
}

template <int N>
inline float hsum(const float *p) {
  return hsum<N / 2>(p) + hsum<N / 2>(p + N / 2);
}

template <>
inline float hsum<1>(const float *p) {
  return *p;
}

#define CHUNK 8

#define OPTIMIZE __attribute__((__optimize__("-O3")))
#define PORTABLE __target_clones("avx512f,avx")

OPTIMIZE PORTABLE float fsumf_nonrecursive(const float *p, size_t n) {
  unsigned i, par, len = 0;
  float sum, res[n / CHUNK + 1];
  for (res[0] = i = 0; i + CHUNK <= n; i += CHUNK)
    res[len++] = hsum<CHUNK>(p + i);
  if (i < n) {
    for (sum = 0; i < n; i++)
      sum += p[i];
    res[len++] = sum;
  }
  for (par = len >> 1; par; par >>= 1, len >>= 1) {
    for (i = 0; i < par; ++i)
      res[i] += res[par + i];
    if (len & 1)
      res[par - 1] += res[len - 1];
  }
  return res[0];
}

void test_fsumf_nonrecursive(void) {
  float A[CHUNK * 3];
  for (int i = 0; i < CHUNK * 3; ++i)
    A[i] = numba();
  for (int n = 0; n < CHUNK * 3; ++n)
    if (fabsf(fsumf_nonrecursive(A, n) - fsumf_kahan(A, n)) > 1e-3)
      exit(7);
}

float nothing(float x) {
  return x;
}

float (*barrier)(float) = nothing;

#define BENCH(ITERATIONS, WORK_PER_RUN, CODE)                                 \
  do {                                                                        \
    struct timespec start = timespec_real();                                  \
    for (int __i = 0; __i < ITERATIONS; ++__i) {                              \
      asm volatile("" ::: "memory");                                          \
      CODE;                                                                   \
    }                                                                         \
    long long work = (WORK_PER_RUN) * (ITERATIONS);                           \
    long nanos =                                                              \
        (timespec_tonanos(timespec_sub(timespec_real(), start)) + work - 1) / \
        (double)work;                                                         \
    printf("%8ld ns %2dx %s\n", nanos, (ITERATIONS), #CODE);                  \
  } while (0)

int main() {
  size_t n = 1024;
  float *p = (float *)malloc(sizeof(float) * n);
  for (size_t i = 0; i < n; ++i)
    p[i] = numba();
  float kahan, gold, linear, logarithmic, nonrecursive;
  test_fsumf_nonrecursive();
  BENCH(100, 1, (kahan = barrier(fsumf_kahan(p, n))));
  BENCH(100, 1, (gold = barrier(fsumf_gold(p, n))));
  BENCH(100, 1, (linear = barrier(fsumf_linear(p, n))));
  BENCH(100, 1, (logarithmic = barrier(fsumf_logarithmic(p, n))));
  BENCH(100, 1, (nonrecursive = barrier(fsumf_nonrecursive(p, n))));
  printf("gold = %.12g (%.12g)\n", gold, fabs(gold - gold));
  printf("linear = %.12g (%.12g)\n", linear, fabs(linear - gold));
  printf("kahan = %.12g (%.12g)\n", kahan, fabs(kahan - gold));
  printf("logarithmic = %.12g (%.12g)\n", logarithmic,
         fabs(logarithmic - gold));
  printf("nonrecursive = %.12g (%.12g)\n", nonrecursive,
         fabs(nonrecursive - gold));
  free(p);
}
