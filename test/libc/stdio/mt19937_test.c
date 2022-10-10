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
#include "libc/errno.h"
#include "libc/intrin/bits.h"
#include "libc/log/check.h"
#include "libc/macros.internal.h"
#include "libc/mem/gc.internal.h"
#include "libc/mem/mem.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/rand.h"
#include "libc/stdio/stdio.h"
#include "libc/sysv/consts/grnd.h"
#include "libc/testlib/ezbench.h"
#include "libc/testlib/testlib.h"
#include "third_party/mbedtls/ctr_drbg.h"

void GetRandom(void *p, size_t n) {
  ssize_t rc;
  size_t i, m;
  for (i = 0; i < n; i += rc) {
    m = MIN(n - i, 256);
    rc = getrandom((char *)p + i, m, 0);
    if (rc == -1 && errno == EINTR) continue;
    if (rc <= 0) abort();
  }
}

int GetEntropy1(void *c, unsigned char *p, size_t n) {
  rngset(p, n, rdseed, 0);
  return 0;
}

void ctrdrbg1(void *p, size_t n) {
  int rc;
  size_t i, m;
  mbedtls_ctr_drbg_context rng;
  mbedtls_ctr_drbg_init(&rng);
  DCHECK_EQ(0, mbedtls_ctr_drbg_seed(&rng, GetEntropy1, 0, "justine", 7));
  for (i = 0; i < n; i += m) {
    m = MIN(n - i, MBEDTLS_CTR_DRBG_MAX_REQUEST);
    DCHECK_EQ(0, mbedtls_ctr_drbg_random(&rng, (unsigned char *)p + i, m));
  }
  mbedtls_ctr_drbg_free(&rng);
}

int GetEntropy2(void *c, unsigned char *p, size_t n) {
  rngset(p, n, rdseed, -1);
  return 0;
}

void ctrdrbg2(void *p, size_t n) {
  int rc;
  size_t i, m;
  mbedtls_ctr_drbg_context rng;
  mbedtls_ctr_drbg_init(&rng);
  DCHECK_EQ(0, mbedtls_ctr_drbg_seed(&rng, GetEntropy2, 0, "justine", 7));
  for (i = 0; i < n; i += m) {
    m = MIN(n - i, MBEDTLS_CTR_DRBG_MAX_REQUEST);
    DCHECK_EQ(0, mbedtls_ctr_drbg_random(&rng, (unsigned char *)p + i, m));
  }
  mbedtls_ctr_drbg_free(&rng);
}

dontinline uint64_t xorshift(void) {
  static uint64_t s = 88172645463325252;
  uint64_t x = s;
  x ^= x << 13;
  x ^= x >> 7;
  x ^= x << 17;
  return (s = x);
}

dontinline void xorshifta(char *p, size_t n) {
  static uint64_t s = 88172645463325252;
  uint64_t x = s;
  while (n >= 8) {
    x ^= x << 13;
    x ^= x >> 7;
    x ^= x << 17;
    __builtin_memcpy(p, &x, 8);
    n -= 8;
    p += 8;
  }
  s = x;
  while (n--) {
    *p++ = x;
    x >>= 8;
  }
}

dontinline uint64_t knuth(void) {
  uint64_t a, b;
  static uint64_t x = 1;
  x *= 6364136223846793005;
  x += 1442695040888963407;
  a = x >> 32;
  x *= 6364136223846793005;
  x += 1442695040888963407;
  b = x >> 32;
  return a | b << 32;
}

dontinline void knutha(char *p, size_t n) {
  static uint64_t s = 1;
  uint32_t u;
  uint64_t x = s;
  while (n >= 4) {
    x *= 6364136223846793005;
    x += 1442695040888963407;
    u = x >> 32;
    p[0] = (0x000000FF & u) >> 000;
    p[1] = (0x0000FF00 & u) >> 010;
    p[2] = (0x00FF0000 & u) >> 020;
    p[3] = (0xFF000000 & u) >> 030;
    n -= 4;
    p += 4;
  }
  s = x;
  while (n--) {
    *p++ = x;
    x >>= 8;
  }
}

uint64_t urandom(void) {
  return random();
}

TEST(mt19937, test) {
  uint64_t init[] = {0x12345ULL, 0x23456ULL, 0x34567ULL, 0x45678ULL};
  uint64_t want[] = {
      7266447313870364031ull,  4946485549665804864ull, 16945909448695747420ull,
      16394063075524226720ull, 4873882236456199058ull,
  };
  _Smt19937(init, ARRAYLEN(init));
  for (int i = 0; i < ARRAYLEN(want); i++) {
    ASSERT_EQ(want[i], _mt19937());
  }
}

BENCH(mt19937, bench8) {
  volatile uint64_t x;
  EZBENCH2("lemur64", donothing, x = lemur64());
  EZBENCH2("_rand64", donothing, x = _rand64());
  EZBENCH2("vigna", donothing, x = vigna());
  EZBENCH2("vigna_r", donothing, vigna_r(&x));
  EZBENCH2("xorshift", donothing, x = xorshift());
  EZBENCH2("knuth", donothing, x = knuth());
  EZBENCH2("random", donothing, x = urandom());
  EZBENCH2("mt19937", donothing, x = _mt19937());
  EZBENCH2("rand64char", donothing, x = _rand64());
  size_t i = 0;
  volatile uint8_t *p = gc(malloc(3 * 2048 * 2 * 8));
  EZBENCH3("rdrand", 2048, donothing, p[i++] = rdrand());
  EZBENCH3("rdseed", 2048, donothing, p[i++] = rdseed());
  EZBENCH3("getrandom", 2048, donothing, GetRandom(p + i++, 8));
}

BENCH(mt19937, bench32k) {
  volatile char *p = gc(malloc(32768));
  EZBENCH_N("rngset(_rand64,-1)", 32768, rngset(p, 32768, _rand64, -1));
  EZBENCH_N("rngset(rdseed,512)", 32768, rngset(p, 32768, rdseed, 512));
  EZBENCH_N("ctrdrbg+rdseed [blk]", 32768, ctrdrbg1(p, 32768));
  EZBENCH_N("getrandom [block]", 32768, GetRandom(p, 32768));
  EZBENCH_N("vigna [word]", 32768, rngset(p, 32768, vigna, 0));
  EZBENCH_N("xorshift [word]", 32768, xorshifta(p, 32768));
  EZBENCH_N("knuth [word]", 32768, knutha(p, 32768));
  EZBENCH_N("random [word]", 32768, rngset(p, 32768, urandom, 0));
  EZBENCH_N("mt19937 [word]", 32768, rngset(p, 32768, _mt19937, 0));
  EZBENCH_N("_rand64 [word]", 32768, rngset(p, 32768, _rand64, 0));
  EZBENCH_N("rdrand [word]", 32768, rngset(p, 32768, rdrand, 0));
  EZBENCH_N("rdseed [word]", 32768, rngset(p, 32768, rdseed, 0));
}

BENCH(mt19937, bench48) {
  volatile char *p = gc(malloc(48));
  EZBENCH_N("rngset(rdrand,0)", 48, rngset(p, 48, rdrand, 0));
  EZBENCH_N("rngset(rdseed,0)", 48, rngset(p, 48, rdseed, 0));
  EZBENCH_N("getrandom", 48, GetRandom(p, 48));
}

#if 0
TEST(mt19937, test) {
  int i;
  uint64_t init[4] = {0x12345ULL, 0x23456ULL, 0x34567ULL, 0x45678ULL};
  uint64_t length = 4;
  mt19937_init_by_array64(init, length);
  printf("1000 outputs of genrand64_int64()\n");
  for (i = 0; i < 1000; i++) {
    printf("%20llu ", mt19937_genrand64_int64());
    if (i % 5 == 4) printf("\n");
  }
  printf("\n1000 outputs of genrand64_real2()\n");
  for (i = 0; i < 1000; i++) {
    printf("%10.8f ", mt19937_genrand64_real2());
    if (i % 5 == 4) printf("\n");
  }
}
#endif
