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
#include <algorithm>
#include <atomic>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <ctime>

#define PRECISION 2e-6
#define LV1DCACHE 49152
#define THRESHOLD 3000000

#if defined(__OPTIMIZE__) && !defined(__SANITIZE_ADDRESS__)
#define ITERATIONS 5
#else
#define ITERATIONS 1
#endif

#define OPTIMIZED __attribute__((__optimize__("-O3,-ffast-math")))
#define PORTABLE                         \
  __target_clones("arch=znver4,"         \
                  "arch=znver3,"         \
                  "arch=sapphirerapids," \
                  "arch=alderlake,"      \
                  "arch=rocketlake,"     \
                  "arch=cooperlake,"     \
                  "arch=tigerlake,"      \
                  "arch=cascadelake,"    \
                  "arch=skylake-avx512," \
                  "arch=skylake,"        \
                  "arch=znver1,"         \
                  "arch=tremont,"        \
                  "fma,"                 \
                  "avx")

static bool is_self_testing;

// m×n → n×m
template <typename TA, typename TB>
void transpose(long m, long n, const TA *A, long lda, TB *B, long ldb) {
#pragma omp parallel for collapse(2) if (m * n > THRESHOLD)
  for (long i = 0; i < m; ++i)
    for (long j = 0; j < n; ++j) {
      B[ldb * j + i] = A[lda * i + j];
    }
}

// m×k * k×n → m×n
// k×m * k×n → m×n if aT
// m×k * n×k → m×n if bT
// k×m * n×k → m×n if aT and bT
template <typename TC, typename TA, typename TB>
void dgemm(bool aT, bool bT, long m, long n, long k, float alpha, const TA *A,
           long lda, const TB *B, long ldb, float beta, TC *C, long ldc) {
#pragma omp parallel for collapse(2) if (m * n * k > THRESHOLD)
  for (long i = 0; i < m; ++i)
    for (long j = 0; j < n; ++j) {
      double sum = 0;
      for (long l = 0; l < k; ++l)
        sum = std::fma((aT ? A[lda * l + i] : A[lda * i + l]) * alpha,
                       (bT ? B[ldb * j + l] : B[ldb * l + j]), sum);
      C[ldc * i + j] = beta * C[ldc * i + j] + sum;
    }
}

template <typename T, typename TC, typename TA, typename TB>
struct Gemmlin {
 public:
  Gemmlin(bool aT, bool bT, float alpha, const TA *A, long lda, const TB *B,
          long ldb, float beta, TC *C, long ldc)
      : aT(aT),
        bT(bT),
        alpha(alpha),
        A(A),
        lda(lda),
        B(B),
        ldb(ldb),
        beta(beta),
        C(C),
        ldc(ldc) {
  }

  void gemm(long m, long n, long k) {
    if (!m || !n) return;
    for (long i = 0; i < m; ++i)
      for (long j = 0; j < n; ++j) {
        C[ldc * i + j] *= beta;
      }
    if (!k) return;
    cub = sqrt(LV1DCACHE) / sqrt(sizeof(T) * 3);
    mnpack(0, m, 0, n, 0, k);
  }

 private:
  void mnpack(long m0, long m,  //
              long n0, long n,  //
              long k0, long k) {
    long mc = rounddown(std::min(m - m0, cub), 4);
    long mp = m0 + (m - m0) / mc * mc;
    long nc = rounddown(std::min(n - n0, cub), 4);
    long np = n0 + (n - n0) / nc * nc;
    long kc = rounddown(std::min(k - k0, cub), 4);
    long kp = k0 + (k - k0) / kc * kc;
    kpack(m0, mc, mp, n0, nc, np, k0, kc, k, kp);
    if (m - mp) mnpack(mp, m, n0, np, k0, k);
    if (n - np) mnpack(m0, mp, np, n, k0, k);
    if (m - mp && n - np) mnpack(mp, m, np, n, k0, k);
  }

  void kpack(long m0, long mc, long m,  //
             long n0, long nc, long n,  //
             long k0, long kc, long k,  //
             long kp) {
    rpack(m0, mc, m, n0, nc, n, k0, kc, kp);
    if (k - kp) rpack(m0, mc, m, n0, nc, n, kp, k - kp, k);
  }

  void rpack(long m0, long mc, long m,  //
             long n0, long nc, long n,  //
             long k0, long kc, long k) {
    if (!(mc % 4) && !(nc % 4))
      bgemm<4, 4>(m0, mc, m, n0, nc, n, k0, kc, k);
    else
      bgemm<1, 1>(m0, mc, m, n0, nc, n, k0, kc, k);
  }

  template <int mr, int nr>
  void bgemm(long m0, long mc, long m,  //
             long n0, long nc, long n,  //
             long k0, long kc, long k) {
    ops = (m - m0) * (n - n0) * (k - k0);
    ml = (m - m0) / mc;
    nl = (n - n0) / nc;
    locks = new lock[ml * nl];
    there_will_be_blocks<mr, nr>(m0, mc, m, n0, nc, n, k0, kc, k);
    delete[] locks;
  }

  template <int mr, int nr>
  void there_will_be_blocks(long m0, volatile long mc, long m, long n0, long nc,
                            long n, long k0, long kc, long k) {
#pragma omp parallel for collapse(2) if (ops > THRESHOLD && mc * kc > 16)
    for (long ic = m0; ic < m; ic += mc)
      for (long pc = k0; pc < k; pc += kc)
        gizmo<mr, nr>(m0, mc, ic, n0, nc, k0, kc, pc, n);
  }

  template <int mr, int nr>
  PORTABLE OPTIMIZED void gizmo(long m0, long mc, long ic, long n0, long nc,
                                long k0, long kc, long pc, long n) {
    T Ac[mc / mr][kc][mr];
    for (long i = 0; i < mc; ++i)
      for (long j = 0; j < kc; ++j)
        Ac[i / mr][j][i % mr] = alpha * (aT ? A[lda * (pc + j) + (ic + i)]
                                            : A[lda * (ic + i) + (pc + j)]);
    for (long jc = n0; jc < n; jc += nc) {
      T Bc[nc / nr][nr][kc];
      for (long j = 0; j < nc; ++j)
        for (long i = 0; i < kc; ++i)
          Bc[j / nr][j % nr][i] =
              bT ? B[ldb * (jc + j) + (pc + i)] : B[ldb * (pc + i) + (jc + j)];
      T Cc[nc / nr][mc / mr][nr][mr];
      memset(Cc, 0, nc * mc * sizeof(float));
      for (long jr = 0; jr < nc / nr; ++jr)
        for (long ir = 0; ir < mc / mr; ++ir)
          for (long pr = 0; pr < kc; ++pr)
            for (long j = 0; j < nr; ++j)
              for (long i = 0; i < mr; ++i)
                Cc[jr][ir][j][i] += Ac[ir][pr][i] * Bc[jr][j][pr];
      const long lk = nl * ((ic - m0) / mc) + ((jc - n0) / nc);
      locks[lk].acquire();
      for (long ir = 0; ir < mc; ir += mr)
        for (long jr = 0; jr < nc; jr += nr)
          for (long i = 0; i < mr; ++i)
            for (long j = 0; j < nr; ++j)
              C[ldc * (ic + ir + i) + (jc + jr + j)] +=
                  Cc[jr / nr][ir / mr][j][i];
      locks[lk].release();
    }
  }

  inline long rounddown(long x, long r) {
    if (x < r)
      return x;
    else
      return x & -r;
  }

  class lock {
   public:
    lock() = default;
    void acquire() {
      while (lock_.exchange(true, std::memory_order_acquire)) {
      }
    }
    void release() {
      lock_.store(false, std::memory_order_release);
    }

   private:
    std::atomic_bool lock_ = false;
  };

  bool aT;
  bool bT;
  float alpha;
  const TA *A;
  long lda;
  const TB *B;
  long ldb;
  float beta;
  TC *C;
  long ldc;
  long ops;
  long nl;
  long ml;
  lock *locks;
  long cub;
};

template <typename TC, typename TA, typename TB>
void sgemm(bool aT, bool bT, long m, long n, long k, float alpha, const TA *A,
           long lda, const TB *B, long ldb, float beta, TC *C, long ldc) {
  Gemmlin<float, TC, TA, TB> g{aT, bT, alpha, A, lda, B, ldb, beta, C, ldc};
  g.gemm(m, n, k);
}

template <typename TA, typename TB>
void show(FILE *f, long max, long m, long n, const TA *A, long lda, const TB *B,
          long ldb) {
  flockfile(f);
  fprintf(f, "      ");
  for (long j = 0; j < n; ++j) {
    fprintf(f, "%13ld", j);
  }
  fprintf(f, "\n");
  for (long i = 0; i < m; ++i) {
    if (i == max) {
      fprintf(f, "...\n");
      break;
    }
    fprintf(f, "%5ld ", i);
    for (long j = 0; j < n; ++j) {
      if (j == max) {
        fprintf(f, " ...");
        break;
      }
      char ba[16], bb[16];
      sprintf(ba, "%13.7f", static_cast<double>(A[lda * i + j]));
      sprintf(bb, "%13.7f", static_cast<double>(B[ldb * i + j]));
      for (long k = 0; ba[k] && bb[k]; ++k) {
        if (ba[k] != bb[k]) fputs_unlocked("\33[31m", f);
        fputc_unlocked(ba[k], f);
        if (ba[k] != bb[k]) fputs_unlocked("\33[0m", f);
      }
    }
    fprintf(f, "\n");
  }
  funlockfile(f);
}

inline unsigned long GetDoubleBits(double f) {
  union {
    double f;
    unsigned long i;
  } u;
  u.f = f;
  return u.i;
}

inline bool IsNan(double x) {
  return (GetDoubleBits(x) & (-1ull >> 1)) > (0x7ffull << 52);
}

template <typename TA, typename TB>
double diff(long m, long n, const TA *Want, long lda, const TB *Got, long ldb) {
  double s = 0;
  int got_nans = 0;
  int want_nans = 0;
  for (long i = 0; i < m; ++i)
    for (long j = 0; j < n; ++j)
      if (IsNan(Want[ldb * i + j]))
        ++want_nans;
      else if (IsNan(Got[ldb * i + j]))
        ++got_nans;
      else
        s += std::fabs(Want[lda * i + j] - Got[ldb * i + j]);
  if (got_nans) printf("WARNING: got %d NaNs!\n", got_nans);
  if (want_nans) printf("WARNING: want array has %d NaNs!\n", want_nans);
  return s / (m * n);
}

template <typename TA, typename TB>
void show_error(FILE *f, long max, long m, long n, const TA *A, long lda,
                const TB *B, long ldb, const char *file, int line, double sad,
                double tol) {
  fprintf(f, "%s:%d: sad %.17g exceeds %g\nwant\n", file, line, sad, tol);
  show(f, max, m, n, A, lda, B, ldb);
  fprintf(f, "got\n");
  show(f, max, m, n, B, ldb, A, lda);
  fprintf(f, "\n");
}

template <typename TA, typename TB>
void check(double tol, long m, long n, const TA *A, long lda, const TB *B,
           long ldb, const char *file, int line) {
  double sad = diff(m, n, A, lda, B, ldb);
  if (sad <= tol) {
    if (!is_self_testing) {
      printf("         %g error\n", sad);
    }
  } else {
    show_error(stderr, 16, m, n, A, lda, B, ldb, file, line, sad, tol);
    const char *path = "/tmp/openmp_test.log";
    FILE *f = fopen(path, "w");
    if (f) {
      show_error(f, 10000, m, n, A, lda, B, ldb, file, line, sad, tol);
      printf("see also %s\n", path);
    }
    exit(1);
  }
}

#define check(tol, m, n, A, lda, B, ldb) \
  check(tol, m, n, A, lda, B, ldb, __FILE__, __LINE__)

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

void test_gemm(long m, long n, long k) {
  float *A = new float[m * k];
  float *At = new float[k * m];
  float *B = new float[k * n];
  float *Bt = new float[n * k];
  float *C = new float[m * n];
  float *GOLD = new float[m * n];
  float alpha = 1;
  float beta = 0;
  fill(A, m * k);
  fill(B, k * n);
  dgemm(0, 0, m, n, k, 1, A, k, B, n, 0, GOLD, n);
  transpose(m, k, A, k, At, m);
  transpose(k, n, B, n, Bt, k);
  sgemm(0, 0, m, n, k, alpha, A, k, B, n, beta, C, n);
  check(PRECISION, m, n, GOLD, n, C, n);
  sgemm(1, 0, m, n, k, alpha, At, m, B, n, beta, C, n);
  check(PRECISION, m, n, GOLD, n, C, n);
  sgemm(0, 1, m, n, k, alpha, A, k, Bt, k, beta, C, n);
  check(PRECISION, m, n, GOLD, n, C, n);
  sgemm(1, 1, m, n, k, alpha, At, m, Bt, k, beta, C, n);
  check(PRECISION, m, n, GOLD, n, C, n);
  delete[] GOLD;
  delete[] C;
  delete[] Bt;
  delete[] B;
  delete[] At;
  delete[] A;
}

void check_gemm_works(void) {
  static long kSizes[] = {1, 2, 3, 4, 5, 6, 7, 17, 31, 33, 63, 128, 129};
  is_self_testing = true;
  long c = 0;
  long N = sizeof(kSizes) / sizeof(kSizes[0]);
  for (long i = 0; i < N; ++i) {
    long m = kSizes[i];
    for (long j = 0; j < N; ++j) {
      long n = kSizes[N - 1 - i];
      for (long k = 0; k < N; ++k) {
        long K = kSizes[i];
        if (c++ % 13 == 0) {
          printf("testing %2ld %2ld %2ld\r", m, n, K);
        }
        test_gemm(m, n, K);
      }
    }
  }
  printf("\r");
  is_self_testing = false;
}

long m = 2333 / 3;
long k = 577 / 3;
long n = 713 / 3;

void check_sgemm(void) {
  float *A = new float[m * k];
  float *At = new float[k * m];
  float *B = new float[k * n];
  float *Bt = new float[n * k];
  float *C = new float[m * n];
  double *GOLD = new double[m * n];
  fill(A, m * k);
  fill(B, k * n);
  transpose(m, k, A, k, At, m);
  transpose(k, n, B, n, Bt, k);
  bench(dgemm(0, 0, m, n, k, 1, A, k, B, n, 0, GOLD, n));
  bench(sgemm(0, 0, m, n, k, 1, A, k, B, n, 0, C, n));
  check(PRECISION, m, n, GOLD, n, C, n);
  bench(sgemm(1, 0, m, n, k, 1, At, m, B, n, 0, C, n));
  check(PRECISION, m, n, GOLD, n, C, n);
  bench(sgemm(0, 1, m, n, k, 1, A, k, Bt, k, 0, C, n));
  check(PRECISION, m, n, GOLD, n, C, n);
  bench(sgemm(1, 1, m, n, k, 1, At, m, Bt, k, 0, C, n));
  check(PRECISION, m, n, GOLD, n, C, n);
  delete[] GOLD;
  delete[] C;
  delete[] Bt;
  delete[] B;
  delete[] At;
  delete[] A;
}

int main(int argc, char *argv[]) {
  check_gemm_works();
  check_sgemm();
}
