/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
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
#include "libc/stdio/rand.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/testlib/ezbench.h"
#include "libc/testlib/testlib.h"
#include "third_party/mbedtls/bignum.h"
#include "third_party/mbedtls/ecp.h"
#include "third_party/mbedtls/ecp_internal.h"
#include "third_party/mbedtls/math.h"
#ifdef MBEDTLS_ECP_C

/*P=0xfffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffeffffffff0000000000000000ffffffff*/

int ecp_mod_p384_old(mbedtls_mpi *);

int GetEntropy(void *c, unsigned char *p, size_t n) {
  rngset(p, n, _rand64, -1);
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
      ASSERT_TRUE(false);
    }
  }
  mbedtls_mpi_free(&B);
  mbedtls_mpi_free(&A);
}

TEST(secp384r1, needsDownwardCorrection) {
  int i;
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
    ASSERT_TRUE(false);
  }
}

TEST(secp384r1, needsUpwardCorrection) {
  int i;
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
    ASSERT_TRUE(false);
  }
}

TEST(secp384r1, largestInput_quasiModNeedsTwoDownwardCorrections) {
  int i;
  uint64_t X[12] = {
      // X = (P-1)*(P-1)
      0xfffffffc00000004,  //
      0x0000000400000000,  //
      0xfffffffe00000002,  //
      0x0000000200000000,  //
      0x0000000000000001,  //
      0x0000000000000000,  //
      0x00000001fffffffc,  //
      0xfffffffe00000000,  //
      0xfffffffffffffffd,  //
      0xffffffffffffffff,  //
      0xffffffffffffffff,  //
      0xffffffffffffffff,  //
  };
  uint64_t W[12] /* W = X mod P */ = {
      0x0000000000000001,  //
  };
  mbedtls_p384_mod(X);
  if (memcmp(W, X, 12 * 8)) {
    for (i = 0; i < 12; ++i) {
      printf("0x%016lx vs. 0x%016lx %d\n", W[i], X[i], W[i] == X[i]);
    }
    ASSERT_TRUE(false);
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

#endif /* MBEDTLS_ECP_C */
