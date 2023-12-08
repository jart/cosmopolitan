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
#include "libc/assert.h"
#include "libc/mem/mem.h"
#include "libc/stdio/rand.h"
#include "libc/str/blake2.h"
#include "libc/str/str.h"
#include "libc/str/tab.internal.h"
#include "libc/testlib/ezbench.h"
#include "libc/testlib/hyperion.h"
#include "libc/testlib/testlib.h"

uint8_t *EZBLAKE2B256(const char *s, size_t n) {
  static uint8_t digest[BLAKE2B256_DIGEST_LENGTH];
  BLAKE2B256(s, n, digest);
  return digest;
}

uint8_t *HEXBLAKE2B256(const char *s) {
  char *p;
  int a, b;
  size_t i, n;
  static uint8_t digest[BLAKE2B256_DIGEST_LENGTH];
  n = strlen(s);
  assert(!(n & 1));
  n /= 2;
  p = malloc(n);
  for (i = 0; i < n; ++i) {
    a = kHexToInt[s[i * 2 + 0] & 255];
    b = kHexToInt[s[i * 2 + 1] & 255];
    assert(a != -1);
    assert(b != -1);
    p[i] = a << 4 | b;
  }
  BLAKE2B256(p, n, digest);
  free(p);
  return digest;
}

TEST(BLAKE2B256Test, ABC) {
  EXPECT_BINEQ(
      "bddd813c634239723171ef3fee98579b94964e3bb1cb3e427262c8c068d52319",
      EZBLAKE2B256("abc", 3));
  EXPECT_BINEQ(
      "0e5751c026e543b2e8ab2eb06099daa1d1e5df47778f7787faab45cdf12fe3a8",
      HEXBLAKE2B256(""));
  EXPECT_BINEQ(
      "03170a2e7597b7b7e3d84c05391d139a62b157e78786d8c082f29dcf4c111314",
      HEXBLAKE2B256("00"));
  /* TODO(jart): do rest */
}

BENCH(blake2, bench) {
  char fun[256];
  rngset(fun, 256, _rand64, -1);
  EZBENCH_N("blake2b256", 0, EZBLAKE2B256(0, 0));
  EZBENCH_N("blake2b256", 8, EZBLAKE2B256("helloooo", 8));
  EZBENCH_N("blake2b256", 31, EZBLAKE2B256(fun, 31));
  EZBENCH_N("blake2b256", 32, EZBLAKE2B256(fun, 32));
  EZBENCH_N("blake2b256", 63, EZBLAKE2B256(fun, 63));
  EZBENCH_N("blake2b256", 64, EZBLAKE2B256(fun, 64));
  EZBENCH_N("blake2b256", 128, EZBLAKE2B256(fun, 128));
  EZBENCH_N("blake2b256", 256, EZBLAKE2B256(fun, 256));
  EZBENCH_N("blake2b256", kHyperionSize,
            EZBLAKE2B256(kHyperion, kHyperionSize));
}
