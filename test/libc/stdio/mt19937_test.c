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
#include "libc/errno.h"
#include "libc/log/check.h"
#include "libc/macros.internal.h"
#include "libc/mem/gc.h"
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
