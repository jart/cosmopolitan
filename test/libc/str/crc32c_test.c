/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
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
#include "libc/intrin/maps.h"
#include "libc/nexgen32e/crc32.h"
#include "libc/stdio/rand.h"
#include "libc/stdio/stdio.h"
#include "libc/testlib/benchmark.h"
#include "libc/testlib/hyperion.h"
#include "libc/testlib/testlib.h"
#include "third_party/zlib/zlib.h"

uint32_t crc32c_reference(uint32_t init, const void *data, size_t size) {
  uint32_t hash = ~init;
  for (size_t i = 0; i < size; ++i) {
    hash ^= ((const uint8_t *)data)[i];
    for (int j = 0; j < 8; ++j) {
      if (hash & 1) {
        hash >>= 1;
        hash ^= 0x82f63b78u;
      } else {
        hash >>= 1;
      }
    }
  }
  return ~hash;
}

TEST(crc32c, test) {
  char data[4000];
  for (int i = 0; i < 4000; ++i)
    data[i] = rand();
  for (int size = 0; size < 4000; ++size) {
    uint32_t seed = lemur64();
    uint32_t got = crc32c(seed, data, size);
    uint32_t want = crc32c_reference(seed, data, size);
    ASSERT_EQ(want, got);
  }
}

dontinline uint64_t fnv(char *s, size_t len) {
  uint64_t hash = 0xcbf29ce484222325;
  for (size_t i = 0; i < len; i++) {
    hash *= 0x100000001b3;
    hash ^= ((const uint8_t *)s)[i];
  }
  return hash;
}

dontinline unsigned kmh(const void *p, size_t n) {
  unsigned h = 0;
  for (size_t i = 0; i < n; i++) {
    h += ((const uint8_t *)p)[i];
    h *= 0x9e3779b1;
  }
  return h;
}

TEST(crc32c, bench) {

  printf("small\n");
  BENCHMARK(100, 5, X(crc32c(0, "hello", 5)));
  BENCHMARK(100, 5, X(crc32_z(0, "hello", 5)));
  BENCHMARK(100, 5, X(fnv("hello", 5)));
  BENCHMARK(100, 5, X(kmh("hello", 5)));

  printf("medium\n");
  BENCHMARK(100, 64, X(crc32c(0, kHyperion, 64)));
  BENCHMARK(100, 64, X(crc32_z(0, kHyperion, 64)));
  BENCHMARK(100, 64, X(fnv(kHyperion, 64)));
  BENCHMARK(100, 64, X(kmh(kHyperion, 64)));

  printf("enormous\n");
  BENCHMARK(100, kHyperionSize, X(crc32c(0, kHyperion, kHyperionSize)));
  BENCHMARK(100, kHyperionSize, X(crc32_z(0, kHyperion, kHyperionSize)));
  BENCHMARK(100, kHyperionSize, X(fnv(kHyperion, kHyperionSize)));
  BENCHMARK(100, kHyperionSize, X(kmh(kHyperion, kHyperionSize)));
}
