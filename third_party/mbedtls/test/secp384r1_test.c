/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2021 Justine Alexandra Roberts Tunney                              │
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
#include "libc/rand/rand.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/testlib/ezbench.h"
#include "libc/testlib/testlib.h"
#include "third_party/mbedtls/bignum.h"
#include "third_party/mbedtls/ecp.h"
#include "third_party/mbedtls/ecp_internal.h"
#include "third_party/mbedtls/math.h"
#ifdef MBEDTLS_ECP_C

int ecp_mod_p384_old(mbedtls_mpi *);

int GetEntropy(void *c, unsigned char *p, size_t n) {
  rngset(p, n, rand64, -1);
  return 0;
}

TEST(secp384r1, testIsTheSame) {
  int i;
  mbedtls_mpi A, B;
  mbedtls_mpi_init(&A);
  mbedtls_mpi_init(&B);
  mbedtls_mpi_fill_random(&A, 12 * 8, GetEntropy, 0);
  mbedtls_mpi_copy(&B, &A);
  ecp_mod_p384(&A);
  ecp_mod_p384_old(&B);
  for (i = 0; i < 1000; ++i) {
    if (memcmp(A.p, B.p, 12 * 8)) {
      for (i = 0; i < 12; ++i) {
        printf("0x%016lx vs. 0x%016lx %d\n", A.p[i], B.p[i], A.p[i] == B.p[i]);
      }
      exit(1);
    }
  }
  mbedtls_mpi_free(&B);
  mbedtls_mpi_free(&A);
}

static inline bool mbedtls_p384_gte(uint64_t p[7]) {
  return (((int64_t)p[6] > 0 ||
           (p[5] > 0xffffffffffffffff ||
            (p[5] == 0xffffffffffffffff &&
             (p[4] > 0xffffffffffffffff ||
              (p[4] == 0xffffffffffffffff &&
               (p[3] > 0xffffffffffffffff ||
                (p[3] == 0xffffffffffffffff &&
                 (p[2] > 0xfffffffffffffffe ||
                  (p[2] == 0xfffffffffffffffe &&
                   (p[1] > 0xffffffff00000000 ||
                    (p[1] == 0xffffffff00000000 &&
                     (p[0] > 0x00000000ffffffff ||
                      (p[0] == 0x00000000ffffffff))))))))))))));
}

static inline void mbedtls_p384_gro(uint64_t p[7]) {
#if defined(__x86_64__) && !defined(__STRICT_ANSI__)
  asm("addq\t%1,%0\n\t"
      "adcq\t%2,8+%0\n\t"
      "adcq\t%3,16+%0\n\t"
      "adcq\t%4,24+%0\n\t"
      "adcq\t%4,32+%0\n\t"
      "adcq\t%4,40+%0\n\t"
      "adcq\t$0,48+%0"
      : "+o"(*p)
      : "r"(0x00000000ffffffffl), "r"(0xffffffff00000000),
        "i"(0xfffffffffffffffel), "i"(0xffffffffffffffff)
      : "memory", "cc");
#else
  uint64_t c;
  ADC(p[0], p[0], 0x00000000ffffffff, 0, c);
  ADC(p[1], p[1], 0xffffffff00000000, c, c);
  ADC(p[2], p[2], 0xfffffffffffffffe, c, c);
  ADC(p[3], p[3], 0xffffffffffffffff, c, c);
  ADC(p[4], p[4], 0xffffffffffffffff, c, c);
  ADC(p[5], p[5], 0xffffffffffffffff, c, c);
  ADC(p[6], p[6], 0, c, c);
#endif
}

static inline void mbedtls_p384_red(uint64_t p[7]) {
#if defined(__x86_64__) && !defined(__STRICT_ANSI__)
  asm("subq\t%1,%0\n\t"
      "sbbq\t%2,8+%0\n\t"
      "sbbq\t%3,16+%0\n\t"
      "sbbq\t%4,24+%0\n\t"
      "sbbq\t%4,32+%0\n\t"
      "sbbq\t%4,40+%0\n\t"
      "sbbq\t$0,48+%0"
      : "+o"(*p)
      : "r"(0x00000000ffffffffl), "r"(0xffffffff00000000),
        "i"(0xfffffffffffffffel), "i"(0xffffffffffffffff)
      : "memory", "cc");
#else
  uint64_t c;
  SBB(p[0], p[0], 0x00000000ffffffff, 0, c);
  SBB(p[1], p[1], 0xffffffff00000000, c, c);
  SBB(p[2], p[2], 0xfffffffffffffffe, c, c);
  SBB(p[3], p[3], 0xffffffffffffffff, c, c);
  SBB(p[4], p[4], 0xffffffffffffffff, c, c);
  SBB(p[5], p[5], 0xffffffffffffffff, c, c);
  SBB(p[6], p[6], 0, c, c);
#endif
}

static inline void mbedtls_p384_rum(uint64_t p[7]) {
  while (mbedtls_p384_gte(p)) mbedtls_p384_red(p);
}

static inline void mbedtls_p384_mod(uint64_t X[12]) {
  secp384r1(X);
  if ((int64_t)X[6] < 0) {
    do {
      mbedtls_p384_gro(X);
    } while ((int64_t)X[6] < 0);
  } else {
    while (mbedtls_p384_gte(X)) {
      mbedtls_p384_red(X);
    }
  }
}

TEST(secp384r1, needsDownwardCorrection) {
  int i;
  uint64_t P[6] = {
      0x00000000ffffffff,  //
      0xffffffff00000000,  //
      0xfffffffffffffffe,  //
      0xffffffffffffffff,  //
      0xffffffffffffffff,  //
      0xffffffffffffffff,  //
  };
  uint64_t X[12] = {
      0xffffffffffffffff,  //
      0xffffffffffffffff,  //
      0xffffffffffffffff,  //
      0xffffffffffffffff,  //
      0xffffffffffffffff,  //
      0xffffffffffffffff,  //
      0xffffffffffffffff,  //
      0xffffffffffffffff,  //
      0xffffffffffffffff,  //
      0xffffffffffffffff,  //
      0xffffffffffffffff,  //
      0xffffffffffffffff,  //
  };
  uint64_t W[12] /* == X mod P */ = {
      0xfffffffe00000000,  //
      0x0000000200000000,  //
      0xfffffffe00000000,  //
      0x0000000200000000,  //
      0x0000000000000001,  //
  };
  mbedtls_p384_mod(X);
  if (memcmp(W, X, 12 * 8)) {
    for (i = 0; i < 12; ++i) {
      printf("0x%016lx vs. 0x%016lx %d\n", W[i], X[i], W[i] == X[i]);
    }
    exit(1);
  }
}

TEST(secp384r1, needsUpwardCorrection) {
  int i;
  uint64_t P[6] = {
      0x00000000ffffffff,  //
      0xffffffff00000000,  //
      0xfffffffffffffffe,  //
      0xffffffffffffffff,  //
      0xffffffffffffffff,  //
      0xffffffffffffffff,  //
  };
  uint64_t X[12] = {
      0x0000000000000000,  //
      0x0000000000000000,  //
      0x0000000000000000,  //
      0x0000000000000000,  //
      0x0000000000000000,  //
      0x0000000000000000,  //
      0x0000000000000000,  //
      0x0000000000000000,  //
      0x0000000000000000,  //
      0x0000000000000000,  //
      0x0000000000000000,  //
      0x00000000ffffffff,  //
  };
  uint64_t W[12] /* == X mod P */ = {
      0xffffffffffffffff,  //
      0x0000000000000000,  //
      0xfffffffefffffffd,  //
      0x0000000100000000,  //
      0x0000000000000000,  //
      0x00000001ffffffff,  //
  };
  mbedtls_p384_mod(X);
  if (memcmp(W, X, 12 * 8)) {
    for (i = 0; i < 12; ++i) {
      printf("0x%016lx vs. 0x%016lx %d\n", W[i], X[i], W[i] == X[i]);
    }
    exit(1);
  }
}

BENCH(secp384r1, bench) {
  mbedtls_mpi A;
  mbedtls_mpi_init(&A);
  mbedtls_mpi_fill_random(&A, 12 * 8, GetEntropy, 0);
  EZBENCH2("secp384r1", donothing, secp384r1(A.p));
  EZBENCH2("ecp_mod_p384", donothing, ecp_mod_p384(&A));
  EZBENCH2("ecp_mod_p384_old", donothing, ecp_mod_p384_old(&A));
  mbedtls_mpi_free(&A);
}

void mbedtls_p384_shl_a(uint64_t p[7]) {
  asm("shlq\t%0\n\t"
      "rclq\t8+%0\n\t"
      "rclq\t16+%0\n\t"
      "rclq\t24+%0\n\t"
      "rclq\t32+%0\n\t"
      "rclq\t40+%0\n\t"
      "rclq\t48+%0\n\t"
      : "+o"(*p)
      : /* no inputs */
      : "memory", "cc");
  mbedtls_p384_rum(p);
}

void mbedtls_p384_shl_b(uint64_t p[7]) {
  p[6] = p[5] >> 63;
  p[5] = p[5] << 1 | p[4] >> 63;
  p[4] = p[4] << 1 | p[3] >> 63;
  p[3] = p[3] << 1 | p[2] >> 63;
  p[2] = p[2] << 1 | p[1] >> 63;
  p[1] = p[1] << 1 | p[0] >> 63;
  p[0] = p[0] << 1;
  mbedtls_p384_rum(p);
}

BENCH(shl, bench) {
  uint64_t A[7] = {0};
  EZBENCH2("mbedtls_p384_shl_a", donothing, mbedtls_p384_shl_a(A));
  EZBENCH2("mbedtls_p384_shl_b", donothing, mbedtls_p384_shl_b(A));
}

void mbedtls_p384_red_a(uint64_t p[7]) {
  asm("subq\t%1,%0\n\t"
      "sbbq\t%2,8+%0\n\t"
      "sbbq\t%3,16+%0\n\t"
      "sbbq\t%4,24+%0\n\t"
      "sbbq\t%4,32+%0\n\t"
      "sbbq\t%4,40+%0\n\t"
      "sbbq\t$0,48+%0"
      : "+o"(*p)
      : "r"(0x00000000ffffffffl), "r"(0xffffffff00000000),
        "i"(0xfffffffffffffffel), "i"(0xffffffffffffffff)
      : "memory", "cc");
}

void mbedtls_p384_red_b(uint64_t p[7]) {
  uint64_t c;
  SBB(p[0], p[0], 0x00000000ffffffff, 0, c);
  SBB(p[1], p[1], 0xffffffff00000000, c, c);
  SBB(p[2], p[2], 0xfffffffffffffffe, c, c);
  SBB(p[3], p[3], 0xffffffffffffffff, c, c);
  SBB(p[4], p[4], 0xffffffffffffffff, c, c);
  SBB(p[5], p[5], 0xffffffffffffffff, c, c);
  SBB(p[6], p[6], 0, c, c);
}

BENCH(red, bench) {
  uint64_t A[7] = {0};
  EZBENCH2("mbedtls_p384_red_a", donothing, mbedtls_p384_red_a(A));
  EZBENCH2("mbedtls_p384_red_b", donothing, mbedtls_p384_red_b(A));
}

#endif /* MBEDTLS_ECP_C */
