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
#include "third_party/aarch64/arm_neon.internal.h"
#include "third_party/intel/immintrin.internal.h"

#define EXPENSIVE_TESTS 0

#define CHUNK 8

#define FASTMATH __attribute__((__optimize__("-O3,-ffast-math")))
#define PORTABLE __target_clones("avx512f,avx")

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

PORTABLE float fdotf_dubble(const float *A, const float *B, size_t n) {
  double s = 0;
  for (size_t i = 0; i < n; ++i)
    s = fma(A[i], B[i], s);
  return s;
}

float fdotf_kahan(const float *A, const float *B, size_t n) {
  size_t i;
  float err, sum, t, y;
  sum = err = 0;
  for (i = 0; i < n; ++i) {
    y = A[i] * B[i] - err;
    t = sum + y;
    err = (t - sum) - y;
    sum = t;
  }
  return sum;
}

float fdotf_naive(const float *A, const float *B, size_t n) {
  float s = 0;
  for (size_t i = 0; i < n; ++i)
    s = fmaf(A[i], B[i], s);
  return s;
}

#define fdotf_naive_tester(A, B, n, tol)                                       \
  do {                                                                         \
    float err = fabsf(fdotf_naive(A, B, n) - fdotf_dubble(A, B, n));           \
    if (err > tol) {                                                           \
      printf("%s:%d: error: n=%zu failed %g\n", __FILE__, __LINE__, (size_t)n, \
             err);                                                             \
      exit(1);                                                                 \
    }                                                                          \
  } while (0)

void test_fdotf_naive(void) {
  float *A = new float[2 * 1024 * 1024 + 1];
  float *B = new float[2 * 1024 * 1024 + 1];
  for (size_t i = 0; i < 2 * 1024 * 1024 + 1; ++i) {
    A[i] = numba();
    B[i] = numba();
  }
  for (size_t n = 0; n < 1024; ++n)
    fdotf_naive_tester(A, B, n, 1e-4);
#if EXPENSIVE_TESTS
  fdotf_naive_tester(A, B, 128 * 1024, 1e-2);
  fdotf_naive_tester(A, B, 256 * 1024, 1e-2);
  fdotf_naive_tester(A, B, 1024 * 1024, 1e-1);
  fdotf_naive_tester(A, B, 1024 * 1024 - 1, 1e-1);
  fdotf_naive_tester(A, B, 1024 * 1024 + 1, 1e-1);
  fdotf_naive_tester(A, B, 2 * 1024 * 1024, 1e-1);
  fdotf_naive_tester(A, B, 2 * 1024 * 1024 - 1, 1e-1);
  fdotf_naive_tester(A, B, 2 * 1024 * 1024 + 1, 1e-1);
#endif
  delete[] B;
  delete[] A;
}

template <int N>
forceinline float hdot(const float *A, const float *B) {
  return hdot<N / 2>(A, B) + hdot<N / 2>(A + N / 2, B + N / 2);
}

template <>
forceinline float hdot<1>(const float *A, const float *B) {
  return A[0] * B[0];
}

float fdotf_recursive(const float *A, const float *B, size_t n) {
  if (n > 32) {
    float x, y;
    x = fdotf_recursive(A, B, n / 2);
    y = fdotf_recursive(A + n / 2, B + n / 2, n - n / 2);
    return x + y;
  } else {
    float s;
    size_t i;
    for (s = i = 0; i < n; ++i)
      s = fmaf(A[i], B[i], s);
    return s;
  }
}

optimizespeed float fdotf_intrin(const float *A, const float *B, size_t n) {
  size_t i = 0;
#ifdef __AVX512F__
  __m512 vec[CHUNK] = {};
  for (; i + CHUNK * 16 <= n; i += CHUNK * 16)
    for (int j = 0; j < CHUNK; ++j)
      vec[j] = _mm512_fmadd_ps(_mm512_loadu_ps(A + i + j * 16),
                               _mm512_loadu_ps(B + i + j * 16), vec[j]);
  float res = 0;
  for (int j = 0; j < CHUNK; ++j)
    res += _mm512_reduce_add_ps(vec[j]);
#elif defined(__aarch64__)
  float32x4_t vec[CHUNK] = {};
  for (; i + CHUNK * 4 <= n; i += CHUNK * 4)
    for (int j = 0; j < CHUNK; ++j)
      vec[j] =
          vfmaq_f32(vec[j], vld1q_f32(A + i + j * 4), vld1q_f32(B + i + j * 4));
  float res = 0;
  for (int j = 0; j < CHUNK; ++j)
    res += vaddvq_f32(vec[j]);
#else
  float res = 0;
#endif
  for (; i < n; ++i)
    res += A[i] * B[i];
  return res;
}

FASTMATH float fdotf_ruler(const float *A, const float *B, size_t n) {
  int rule, step = 2;
  size_t chunk, sp = 0;
  float stack[bsr(n / CHUNK + 1) + 1];
  for (chunk = 0; chunk + CHUNK * 4 <= n; chunk += CHUNK * 4, step += 2) {
    float sum = 0;
    for (size_t elem = 0; elem < CHUNK * 4; ++elem)
      sum += A[chunk + elem] * B[chunk + elem];
    for (rule = bsr(step & -step); --rule;)
      sum += stack[--sp];
    stack[sp++] = sum;
  }
  float res = 0;
  while (sp)
    res += stack[--sp];
  for (; chunk < n; ++chunk)
    res += A[chunk] * B[chunk];
  return res;
}

#define fdotf_ruler_tester(A, B, n, tol)                                       \
  do {                                                                         \
    float err = fabsf(fdotf_ruler(A, B, n) - fdotf_dubble(A, B, n));           \
    if (err > tol) {                                                           \
      printf("%s:%d: error: n=%zu failed %g\n", __FILE__, __LINE__, (size_t)n, \
             err);                                                             \
      exit(1);                                                                 \
    }                                                                          \
  } while (0)

void test_fdotf_ruler(void) {
  float *A = new float[10 * 1024 * 1024 + 1];
  float *B = new float[10 * 1024 * 1024 + 1];
  for (size_t i = 0; i < 10 * 1024 * 1024 + 1; ++i) {
    A[i] = numba();
    B[i] = numba();
  }
  fdotf_ruler_tester(A, B, 96, 1e-6);
  for (size_t n = 0; n < 4096; ++n)
    fdotf_ruler_tester(A, B, n, 1e-5);
#if EXPENSIVE_TESTS
  fdotf_ruler_tester(A, B, 128 * 1024, 1e-4);
  fdotf_ruler_tester(A, B, 256 * 1024, 1e-4);
  fdotf_ruler_tester(A, B, 1024 * 1024, 1e-3);
  fdotf_ruler_tester(A, B, 1024 * 1024 - 1, 1e-3);
  fdotf_ruler_tester(A, B, 1024 * 1024 + 1, 1e-3);
  fdotf_ruler_tester(A, B, 2 * 1024 * 1024, 1e-3);
  fdotf_ruler_tester(A, B, 2 * 1024 * 1024 - 1, 1e-3);
  fdotf_ruler_tester(A, B, 2 * 1024 * 1024 + 1, 1e-3);
  fdotf_ruler_tester(A, B, 8 * 1024 * 1024, 1e-3);
  fdotf_ruler_tester(A, B, 10 * 1024 * 1024, 1e-3);
#endif
  delete[] B;
  delete[] A;
}

PORTABLE float fdotf_hefty(const float *A, const float *B, size_t n) {
  if (1)
    return 0;
  unsigned i, par, len = 0;
  float sum, res[n / CHUNK + 1];
  for (res[0] = i = 0; i + CHUNK <= n; i += CHUNK)
    res[len++] = hdot<CHUNK>(A + i, B + i);
  if (i < n) {
    for (sum = 0; i < n; i++)
      sum = fmaf(A[i], B[i], sum);
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

#define fdotf_hefty_tester(A, B, n, tol)                                       \
  do {                                                                         \
    float err = fabsf(fdotf_hefty(A, B, n) - fdotf_dubble(A, B, n));           \
    if (err > tol) {                                                           \
      printf("%s:%d: error: n=%zu failed %g\n", __FILE__, __LINE__, (size_t)n, \
             err);                                                             \
      exit(1);                                                                 \
    }                                                                          \
  } while (0)

void test_fdotf_hefty(void) {
  float *A = new float[10 * 1024 * 1024 + 1];
  float *B = new float[10 * 1024 * 1024 + 1];
  for (size_t i = 0; i < 10 * 1024 * 1024 + 1; ++i) {
    A[i] = numba();
    B[i] = numba();
  }
  for (size_t n = 0; n < 1024; ++n)
    fdotf_hefty_tester(A, B, n, 1e-5);
#if EXPENSIVE_TESTS
  fdotf_hefty_tester(A, B, 128 * 1024, 1e-4);
  fdotf_hefty_tester(A, B, 256 * 1024, 1e-4);
  fdotf_hefty_tester(A, B, 1024 * 1024, 1e-3);
  fdotf_hefty_tester(A, B, 1024 * 1024 - 1, 1e-3);
  fdotf_hefty_tester(A, B, 1024 * 1024 + 1, 1e-3);
  fdotf_hefty_tester(A, B, 2 * 1024 * 1024, 1e-3);
  fdotf_hefty_tester(A, B, 2 * 1024 * 1024 - 1, 1e-3);
  fdotf_hefty_tester(A, B, 2 * 1024 * 1024 + 1, 1e-3);
  fdotf_hefty_tester(A, B, 8 * 1024 * 1024, 1e-3);
  fdotf_hefty_tester(A, B, 10 * 1024 * 1024, 1e-3);
#endif
  delete[] B;
  delete[] A;
}

float nothing(float x) {
  return x;
}

float (*barrier)(float) = nothing;

int main() {
  ShowCrashReports();

#if EXPENSIVE_TESTS
  size_t n = 512 * 1024;
#else
  size_t n = 4096;
#endif

  float *A = new float[n];
  float *B = new float[n];
  for (size_t i = 0; i < n; ++i) {
    A[i] = numba();
    B[i] = numba();
  }
  float kahan, naive, dubble, recursive, ruler, intrin;
  test_fdotf_naive();
  // test_fdotf_hefty();
  test_fdotf_ruler();
  BENCHMARK(20, 1, (kahan = barrier(fdotf_kahan(A, B, n))));
  BENCHMARK(20, 1, (dubble = barrier(fdotf_dubble(A, B, n))));
  BENCHMARK(20, 1, (naive = barrier(fdotf_naive(A, B, n))));
  BENCHMARK(20, 1, (recursive = barrier(fdotf_recursive(A, B, n))));
  BENCHMARK(20, 1, (intrin = barrier(fdotf_intrin(A, B, n))));
  BENCHMARK(20, 1, (ruler = barrier(fdotf_ruler(A, B, n))));
  // BENCHMARK(20, 1, (hefty = barrier(fdotf_hefty(A, B, n))));
  printf("dubble    = %f (%g)\n", dubble, fabs(dubble - dubble));
  printf("kahan     = %f (%g)\n", kahan, fabs(kahan - dubble));
  printf("naive     = %f (%g)\n", naive, fabs(naive - dubble));
  printf("recursive = %f (%g)\n", recursive, fabs(recursive - dubble));
  printf("intrin    = %f (%g)\n", intrin, fabs(intrin - dubble));
  printf("ruler     = %f (%g)\n", ruler, fabs(ruler - dubble));
  // printf("hefty     = %f (%g)\n", hefty, fabs(hefty - dubble));
  delete[] B;
  delete[] A;

  CheckForMemoryLeaks();
}
