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
#include "libc/str/blake2.h"
#include "libc/assert.h"
#include "libc/calls/struct/timespec.h"
#include "libc/mem/mem.h"
#include "libc/stdio/rand.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/str/tab.h"
#include "libc/testlib/benchmark.h"
#include "libc/testlib/hyperion.h"
#include "libc/testlib/testlib.h"

__static_yoink("libc/testlib/blake2b256_tests.txt");
__static_yoink("zipos");

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
  p = malloc(n + 1);
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
}

TEST(BLAKE2B256Test, vectors) {
  char *line = NULL;
  size_t cap = 0;
  ssize_t n;
  FILE *f = fopen("/zip/libc/testlib/blake2b256_tests.txt", "r");
  uint8_t *digest = 0;
  while ((n = getline(&line, &cap, f)) != -1) {
    if (n < 2 || line[0] == '#')
      continue;
    line[n - 1] = 0;
    if (!strncmp(line, "IN:", 3)) {
      digest = HEXBLAKE2B256(line + 4);
    }
    if (!strncmp(line, "HASH: ", 6)) {
      EXPECT_BINEQ(line + 6, digest);
    }
  }
  fclose(f);
  free(line);
}

BENCH(blake2, benchmark) {
  char fun[256];
  rngset(fun, 256, _rand64, -1);
  BENCHMARK(100, 0, __expropriate(EZBLAKE2B256(0, 0)));
  BENCHMARK(100, 1, __expropriate(EZBLAKE2B256("h", 1)));
  BENCHMARK(100, 8, __expropriate(EZBLAKE2B256("helloooo", 8)));
  BENCHMARK(100, 31, __expropriate(EZBLAKE2B256(fun, 31)));
  BENCHMARK(100, 32, __expropriate(EZBLAKE2B256(fun, 32)));
  BENCHMARK(100, 63, __expropriate(EZBLAKE2B256(fun, 63)));
  BENCHMARK(100, 64, __expropriate(EZBLAKE2B256(fun, 64)));
  BENCHMARK(100, 128, __expropriate(EZBLAKE2B256(fun, 128)));
  BENCHMARK(100, 256, __expropriate(EZBLAKE2B256(fun, 256)));
  BENCHMARK(100, kHyperionSize,
            __expropriate(EZBLAKE2B256(kHyperion, kHyperionSize)));
}
