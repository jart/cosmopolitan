// Copyright 2024 Justine Alexandra Roberts Tunney
//
// Permission to use, copy, modify, and/or distribute this software for
// any purpose with or without fee is hereby granted, provided that the
// above copyright notice and this permission notice appear in all copies.
//
// THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL
// WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE
// AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
// DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
// PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
// TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
// PERFORMANCE OF THIS SOFTWARE.

#include <unistd.h>
#include <cassert>
#include <cinttypes>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <ctime>

// high performance high accuracy matrix multiplication in ansi c

#define MATH __target_clones("avx512f,fma,avx")

namespace {
namespace ansiBLAS {

static constexpr int KN = 8;

struct Vector {
  double v[KN];
};

Vector load(const float *p) {
  Vector x;
  for (int i = 0; i < KN; ++i)
    x.v[i] = p[i];
  return x;
}

Vector madd(Vector x, Vector y, Vector s) {
  for (int i = 0; i < KN; ++i)
    s.v[i] = fma(x.v[i], y.v[i], s.v[i]);
  return s;
}

float hsum(Vector x) {
  double s = 0;
  for (int i = 0; i < KN; ++i)
    s += x.v[i];
  return s;
}

struct ansiBLAS {
 public:
  ansiBLAS(int k, const float *A, int lda, const float *B, int ldb, float *C,
           int ldc, int ith, int nth)
      : k(k),
        A(A),
        lda(lda),
        B(B),
        ldb(ldb),
        C(C),
        ldc(ldc),
        ith(ith),
        nth(nth) {
  }

  void matmul(int m, int n) {
    mnpack(0, m, 0, n);
  }

 private:
  void mnpack(int m0, int m, int n0, int n) {
    int mc, nc, mp, np;
    if (m - m0 <= 0 || n - n0 <= 0)
      return;
    if (m - m0 >= 4 && n - n0 >= 3) {
      mc = 4;
      nc = 3;
      gemm<4, 3>(m0, m, n0, n);
    } else {
      mc = 1;
      nc = 1;
      gemm<1, 1>(m0, m, n0, n);
    }
    mp = m0 + (m - m0) / mc * mc;
    np = n0 + (n - n0) / nc * nc;
    mnpack(mp, m, n0, np);
    mnpack(m0, m, np, n);
  }

  template <int RM, int RN>
  MATH void gemm(int m0, int m, int n0, int n) {
    int ytiles = (m - m0) / RM;
    int xtiles = (n - n0) / RN;
    int tiles = xtiles * ytiles;
    int duty = (tiles + nth - 1) / nth;
    int start = duty * ith;
    int end = start + duty;
    if (end > tiles)
      end = tiles;
    for (int job = start; job < end; ++job) {
      int ii = m0 + job / xtiles * RM;
      int jj = n0 + job % xtiles * RN;
      Vector Cv[RN][RM] = {};
      for (int l = 0; l < k; l += KN)
        for (int j = 0; j < RN; ++j)
          for (int i = 0; i < RM; ++i)
            Cv[j][i] = madd(load(A + lda * (ii + i) + l),  //
                            load(B + ldb * (jj + j) + l),  //
                            Cv[j][i]);
      for (int j = 0; j < RN; ++j)
        for (int i = 0; i < RM; ++i)
          C[ldc * (jj + j) + (ii + i)] = hsum(Cv[j][i]);
    }
  }

  const int k;
  const float *const A;
  const int lda;
  const float *const B;
  const int ldb;
  float *const C;
  const int ldc;
  const int ith;
  const int nth;
};

void sgemm(int m, int n, int k,      //
           const float *A, int lda,  //
           const float *B, int ldb,  //
           float *C, int ldc) {
  int nth = sysconf(_SC_NPROCESSORS_ONLN);
#pragma omp parallel for schedule(static)
  for (int ith = 0; ith < nth; ++ith) {
    ansiBLAS tb{k, A, lda, B, ldb, C, ldc, ith, nth};
    tb.matmul(m, n);
  }
}

}  // namespace ansiBLAS

long micros(void) {
  struct timespec ts;
  clock_gettime(CLOCK_REALTIME, &ts);
  return ts.tv_sec * 1000000 + (ts.tv_nsec + 999) / 1000;
}

unsigned rand32(void) {
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

void fill(int m, int n, float *A, int lda) {
  for (int j = 0; j < n; ++j)
    for (int i = 0; i < m; ++i)
      A[lda * j + i] = numba();
}

float *new_matrix(int m, int n, int *lda) {
  void *ptr = 0;
  int b = 64 / sizeof(float);
  *lda = (n + b - 1) & -b;
  posix_memalign(&ptr, 4096, sizeof(float) * m * *lda);
  return (float *)ptr;
}

}  // namespace

void barrier(void) {
}
void (*pBarrier)(void) = barrier;

#define BENCH(x)                                                           \
  do {                                                                     \
    x;                                                                     \
    int N = 10;                                                            \
    long t1 = micros();                                                    \
    for (long i = 0; i < N; ++i) {                                         \
      pBarrier();                                                          \
      x;                                                                   \
    }                                                                      \
    long t2 = micros();                                                    \
    printf("%8" PRId64 " µs %s %g gigaflops\n", (t2 - t1 + N - 1) / N, #x, \
           1e6 / ((t2 - t1 + N - 1) / N) * m * n * k * 2 * 1e-9);          \
  } while (0)

int main() {
  int m = 1024;
  int n = 1024;
  int k = 1024;
  int lda, ldb, ldc;
  float *A = new_matrix(m, k, &lda);
  float *B = new_matrix(n, k, &ldb);
  float *C = new_matrix(n, m, &ldc);
  fill(k, n, A, lda);
  fill(k, m, B, ldb);
  BENCH(ansiBLAS::sgemm(m, n, k, A, lda, B, ldb, C, ldc));
  assert(C[0] == -0x1.20902ap+4);
  assert(C[1] == -0x1.bf7726p+4);
  free(C);
  free(B);
  free(A);
}
