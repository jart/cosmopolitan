/*-*-mode:c++;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8-*-│
│ vi: set et ft=c++ ts=2 sts=2 sw=2 fenc=utf-8                             :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2024 Justine Alexandra Roberts Tunney                              │
│                                                                              │
│ Permission to use, copy, modify, and/or distribute this software for         │
│ any purpose with or without fee is hereby granted, provided that the         │
│ above copyright notice and this permission notice appear in all copies.      │
│                                                                              │
│ THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL                │
│ WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED                │
│ WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE             │
│ AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL         │
│ DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR        │
│ PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER               │
│ TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR             │
│ PERFORMANCE OF THIS SOFTWARE.                                                │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/calls/calls.h"
#include "libc/calls/struct/timespec.h"
#include "libc/fmt/itoa.h"
#include "libc/inttypes.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/clock.h"
#include "third_party/double-conversion/double-to-string.h"
#include "third_party/double-conversion/utils.h"
#include "third_party/openmp/omp.h"

#ifndef __FAST_MATH__
#define FLAWLESS 0
#else
#define FLAWLESS 1e-05
#endif

#if defined(__OPTIMIZE__) && !defined(__SANITIZE_ADDRESS__)
#define ITERATIONS 10
#else
#define ITERATIONS 1
#endif

// m×n → (m×n)ᵀ
template <typename T>
void transpose(long m, long n, const T *A, long sa, T *B, long sb) {
#pragma omp parallel for collapse(2)
  for (long i = 0; i < m; ++i) {
    for (long j = 0; j < n; ++j) {
      B[sb * j + i] = A[sa * i + j];
    }
  }
}

// m×k * k×n → m×n
template <typename T>
void matmul(long m, long n, long k, const T *A, long sa, const T *B, long sb,
            T *C, long sc) {
#pragma omp parallel for collapse(2)
  for (long i = 0; i < m; ++i) {
    for (long j = 0; j < n; ++j) {
      T sum = 0;
      for (long l = 0; l < k; ++l) {
        sum += A[sa * i + l] * B[sb * l + j];
      }
      C[sc * i + j] = sum;
    }
  }
}

template <long BM, long BN, typename T>
void gemmk(long k, const T *A, long sa, const T *B, long sb, T *C, long sc) {
  T S[BM][BN] = {0};
  for (long l = 0; l < k; ++l) {
    for (long i = 0; i < BM; ++i) {
      for (long j = 0; j < BN; ++j) {
        S[i][j] += A[sa * l + i] * B[sb * l + j];
      }
    }
  }
  for (long i = 0; i < BM; ++i) {
    for (long j = 0; j < BN; ++j) {
      C[sc * i + j] = S[i][j];
    }
  }
}

// (m×k)ᵀ * k×n → m×n
template <long BM, long BN, typename T>
void gemm(long m, long n, long k, const T *A, long sa, const T *B, long sb,
          T *C, long sc) {
#pragma omp parallel for collapse(2)
  for (long i = 0; i < m; i += BM) {
    for (long j = 0; j < n; j += BN) {
      gemmk<BM, BN>(k, A + i, sa, B + j, sb, C + sc * i + j, sc);
    }
  }
}

template <typename T>
void show(long m, long n, const T *A, long sa) {
  long max = 4;
  printf("{");
  for (long i = 0; i < m; ++i) {
    if (i) {
      if (i == max) {
        printf(", ...");
        break;
      } else {
        printf(", ");
      }
    }
    printf("{");
    for (long j = 0; j < n; ++j) {
      if (j) {
        if (j == max) {
          printf(", ...");
          break;
        } else {
          printf(", ");
        }
      }
      printf("%g", static_cast<double>(A[j + i * sa]));
    }
    printf("}");
  }
  printf("}");
}

template <typename T>
double diff(long m, long n, const T *A, long sa, const T *B, long sb) {
  double s = 0;
  for (long i = 0; i < m; ++i) {
    for (long j = 0; j < n; ++j) {
      s += fabs(A[sa * i + j] - B[sb * i + j]);
    }
  }
  return s / m / n;
}

template <typename T>
void check(double tol, long m, long n, const T *A, long sa, const T *B, long sb,
           const char *file, long line) {
  double sad = diff(m, n, A, sa, B, sb);
  if (sad > tol) {
    printf("%s:%d: sad %g exceeds %g\n\twant ", file, line, sad, tol);
    show(m, n, A, sa);
    printf("\n\t got ");
    show(m, n, B, sb);
    printf("\n");
    exit(1);
  }
}

#define check(tol, m, n, A, sa, B, sb) \
  check(tol, m, n, A, sa, B, sb, __FILE__, __LINE__)

long micros(void) {
  struct timespec ts;
  clock_gettime(CLOCK_REALTIME, &ts);
  return ts.tv_sec * 1000000 + (ts.tv_nsec + 999) / 1000;
}

#define bench(x)                                                            \
  do {                                                                      \
    long t1 = micros();                                                     \
    for (long i = 0; i < ITERATIONS; ++i) {                                 \
      asm volatile("" ::: "memory");                                        \
      x;                                                                    \
      asm volatile("" ::: "memory");                                        \
    }                                                                       \
    long t2 = micros();                                                     \
    printf("%8" PRId64 " µs %s\n", (t2 - t1 + ITERATIONS - 1) / ITERATIONS, \
           #x);                                                             \
  } while (0)

unsigned long rando(void) {
  static unsigned long s;
  unsigned long z = (s += 0x9e3779b97f4a7c15);
  z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9;
  z = (z ^ (z >> 27)) * 0x94d049bb133111eb;
  return z ^ (z >> 31);
}

double real01(unsigned long x) {  // (0,1)
  return 1. / 4503599627370496. * ((x >> 12) + .5);
}

double numba(void) {  // (-1,1)
  return real01(rando()) * 2 - 1;
}

template <typename T>
void fill(T *A, long n) {
  for (long i = 0; i < n; ++i) {
    A[i] = numba();
  }
}

void check_reference_gemm_is_ok(void) {
  constexpr long m = 2;
  constexpr long n = 2;
  constexpr long k = 2;
  float A[m][k] = {{1, 2}, {3, 4}};
  float B[k][n] = {{5, 6}, {7, 8}};
  float C[m][n] = {{666, 666}, {666, 666}};
  float G[m][n] = {{19, 22}, {43, 50}};
  bench(matmul(m, n, k, (float *)A, k, (float *)B, n, (float *)C, n));
  check(FLAWLESS, m, n, (float *)G, n, (float *)C, n);
}

void check_transposed_blocking_gemm_is_ok(void) {
  long m = 1024;
  long k = 512;
  long n = 80;
  float *A = new float[m * k];
  float *B = new float[k * n];
  float *C = new float[m * n];
  float *D = new float[m * n];
  fill(A, m * k);
  fill(B, k * n);
  bench(matmul(m, n, k, A, k, B, n, C, n));
  float *At = new float[k * m];
  bench(transpose(m, k, A, k, At, m));
  bench((gemm<8, 4>(m, n, k, At, m, B, n, D, n)));
  check(FLAWLESS, m, n, C, n, D, n);
  delete[] At;
  delete[] D;
  delete[] C;
  delete[] B;
  delete[] A;
}

int main(int argc, char *argv[]) {
  check_reference_gemm_is_ok();
  check_transposed_blocking_gemm_is_ok();
}
