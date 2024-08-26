#include "libc/assert.h"
#include "libc/calls/struct/timespec.h"
#include "libc/intrin/bsr.h"
#include "libc/macros.h"
#include "libc/math.h"
#include "libc/mem/gc.h"
#include "libc/mem/leaks.h"
#include "libc/mem/mem.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/testlib/benchmark.h"
#include "libc/x/xasprintf.h"

#define EXPENSIVE_TESTS 0

#define CHUNK 8

#define FASTMATH __attribute__((__optimize__("-O3,-ffast-math")))
#define PORTABLE __target_clones("avx512f,avx")

static unsigned long long lcg = 1;

int rand32(void) {
  /* Knuth, D.E., "The Art of Computer Programming," Vol 2,
     Seminumerical Algorithms, Third Edition, Addison-Wesley, 1998,
     p. 106 (line 26) & p. 108 */
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

FASTMATH PORTABLE float fsumf_dubble(const float *p, size_t n) {
  double s = 0;
  for (size_t i = 0; i < n; ++i)
    s += p[i];
  return s;
}

PORTABLE float fsumf_kahan(const float *p, size_t n) {
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

FASTMATH PORTABLE float fsumf_naive(const float *p, size_t n) {
  float s = 0;
  for (size_t i = 0; i < n; ++i)
    s += p[i];
  return s;
}

#define fsumf_naive_tester(A, n, tol)                                          \
  do {                                                                         \
    float err = fabsf(fsumf_naive(A, n) - fsumf_dubble(A, n));                 \
    if (err > tol) {                                                           \
      printf("%s:%d: error: n=%zu failed %g\n", __FILE__, __LINE__, (size_t)n, \
             err);                                                             \
      exit(1);                                                                 \
    }                                                                          \
  } while (0)

void test_fsumf_naive(void) {
  float *A = new float[2 * 1024 * 1024 + 1];
  for (size_t i = 0; i < 2 * 1024 * 1024 + 1; ++i)
    A[i] = numba();
  for (size_t n = 0; n < 1024; ++n)
    fsumf_naive_tester(A, n, 1e-4);
#if EXPENSIVE_TESTS
  fsumf_naive_tester(A, 128 * 1024, 1e-2);
  fsumf_naive_tester(A, 256 * 1024, 1e-2);
  fsumf_naive_tester(A, 1024 * 1024, 1e-1);
  fsumf_naive_tester(A, 1024 * 1024 - 1, 1e-1);
  fsumf_naive_tester(A, 1024 * 1024 + 1, 1e-1);
  fsumf_naive_tester(A, 2 * 1024 * 1024, 1e-1);
  fsumf_naive_tester(A, 2 * 1024 * 1024 - 1, 1e-1);
  fsumf_naive_tester(A, 2 * 1024 * 1024 + 1, 1e-1);
#endif
  delete[] A;
}

template <int N>
forceinline float hsum(const float *p) {
  return hsum<N / 2>(p) + hsum<N / 2>(p + N / 2);
}

template <>
forceinline float hsum<1>(const float *p) {
  return *p;
}

FASTMATH PORTABLE float fsumf_recursive(const float *p, size_t n) {
  if (n > 32) {
    float x, y;
    x = fsumf_recursive(p, n / 2);
    y = fsumf_recursive(p + n / 2, n - n / 2);
    return x + y;
  } else {
    float s;
    size_t i;
    for (s = i = 0; i < n; ++i)
      s += p[i];
    return s;
  }
}

FASTMATH PORTABLE float fsumf_ruler(const float *p, size_t n) {
  size_t i, sp = 0;
  int rule, step = 2;
  float stack[bsr(n / CHUNK + 1) + 1];
  for (i = 0; i + CHUNK * 4 <= n; i += CHUNK * 4, step += 2) {
    float sum = 0;
    for (size_t j = 0; j < CHUNK * 4; ++j)
      sum += p[i + j];
    for (rule = bsr(step & -step); --rule;)
      sum += stack[--sp];
    stack[sp++] = sum;
  }
  float res = 0;
  while (sp)
    res += stack[--sp];
  while (i < n)
    res += p[i++];
  return res;
}

#define fsumf_ruler_tester(A, n, tol)                                          \
  do {                                                                         \
    float err = fabsf(fsumf_ruler(A, n) - fsumf_dubble(A, n));                 \
    if (err > tol) {                                                           \
      printf("%s:%d: error: n=%zu failed %g\n", __FILE__, __LINE__, (size_t)n, \
             err);                                                             \
      exit(1);                                                                 \
    }                                                                          \
  } while (0)

void test_fsumf_ruler(void) {
  float *A = new float[10 * 1024 * 1024 + 1];
  for (size_t i = 0; i < 10 * 1024 * 1024 + 1; ++i)
    A[i] = numba();
  fsumf_ruler_tester(A, 96, 1e-6);
  for (size_t n = 0; n < 1024; ++n)
    fsumf_ruler_tester(A, n, 1e-5);
#if EXPENSIVE_TESTS
  fsumf_ruler_tester(A, 128 * 1024, 1e-4);
  fsumf_ruler_tester(A, 256 * 1024, 1e-4);
  fsumf_ruler_tester(A, 1024 * 1024, 1e-3);
  fsumf_ruler_tester(A, 1024 * 1024 - 1, 1e-3);
  fsumf_ruler_tester(A, 1024 * 1024 + 1, 1e-3);
  fsumf_ruler_tester(A, 2 * 1024 * 1024, 1e-3);
  fsumf_ruler_tester(A, 2 * 1024 * 1024 - 1, 1e-3);
  fsumf_ruler_tester(A, 2 * 1024 * 1024 + 1, 1e-3);
  fsumf_ruler_tester(A, 8 * 1024 * 1024, 1e-3);
  fsumf_ruler_tester(A, 10 * 1024 * 1024, 1e-3);
#endif
  delete[] A;
}

FASTMATH PORTABLE float fsumf_hefty(const float *p, size_t n) {
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

#define fsumf_hefty_tester(A, n, tol)                                          \
  do {                                                                         \
    float err = fabsf(fsumf_hefty(A, n) - fsumf_dubble(A, n));                 \
    if (err > tol) {                                                           \
      printf("%s:%d: error: n=%zu failed %g\n", __FILE__, __LINE__, (size_t)n, \
             err);                                                             \
      exit(1);                                                                 \
    }                                                                          \
  } while (0)

void test_fsumf_hefty(void) {
  float *A = new float[10 * 1024 * 1024 + 1];
  for (size_t i = 0; i < 10 * 1024 * 1024 + 1; ++i)
    A[i] = numba();
  for (size_t n = 0; n < 1024; ++n)
    fsumf_hefty_tester(A, n, 1e-5);
#if EXPENSIVE_TESTS
  fsumf_hefty_tester(A, 128 * 1024, 1e-4);
  fsumf_hefty_tester(A, 256 * 1024, 1e-4);
  fsumf_hefty_tester(A, 1024 * 1024, 1e-3);
  fsumf_hefty_tester(A, 1024 * 1024 - 1, 1e-3);
  fsumf_hefty_tester(A, 1024 * 1024 + 1, 1e-3);
  fsumf_hefty_tester(A, 2 * 1024 * 1024, 1e-3);
  fsumf_hefty_tester(A, 2 * 1024 * 1024 - 1, 1e-3);
  fsumf_hefty_tester(A, 2 * 1024 * 1024 + 1, 1e-3);
  fsumf_hefty_tester(A, 8 * 1024 * 1024, 1e-3);
  fsumf_hefty_tester(A, 10 * 1024 * 1024, 1e-3);
#endif
  delete[] A;
}

float nothing(float x) {
  return x;
}

float (*barrier)(float) = nothing;

int main() {
  ShowCrashReports();

#if EXPENSIVE_TESTS
  size_t n = 4 * 1024 * 1024;
#else
  size_t n = 1024;
#endif

  float *p = new float[n];
  for (size_t i = 0; i < n; ++i)
    p[i] = numba();
  float kahan, naive, dubble, recursive, hefty, ruler;
  test_fsumf_naive();
  test_fsumf_hefty();
  test_fsumf_ruler();
  BENCHMARK(20, 1, (kahan = barrier(fsumf_kahan(p, n))));
  BENCHMARK(20, 1, (dubble = barrier(fsumf_dubble(p, n))));
  BENCHMARK(20, 1, (naive = barrier(fsumf_naive(p, n))));
  BENCHMARK(20, 1, (recursive = barrier(fsumf_recursive(p, n))));
  BENCHMARK(20, 1, (ruler = barrier(fsumf_ruler(p, n))));
  BENCHMARK(20, 1, (hefty = barrier(fsumf_hefty(p, n))));
  printf("dubble    = %f (%g)\n", dubble, fabs(dubble - dubble));
  printf("kahan     = %f (%g)\n", kahan, fabs(kahan - dubble));
  printf("naive     = %f (%g)\n", naive, fabs(naive - dubble));
  printf("recursive = %f (%g)\n", recursive, fabs(recursive - dubble));
  printf("ruler     = %f (%g)\n", ruler, fabs(ruler - dubble));
  printf("hefty     = %f (%g)\n", hefty, fabs(hefty - dubble));
  delete[] p;

  CheckForMemoryLeaks();
}
