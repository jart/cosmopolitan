/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2025 Justine Alexandra Roberts Tunney                              │
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
#include "libc/bsdstdlib.h"
#include "libc/str/str.h"
#include "libc/testlib/benchmark.h"
#include "libc/testlib/testlib.h"
#include "third_party/haclstar/haclstar.h"
#include "third_party/mbedtls/everest.h"

// the proper way to make a secret key for curve25519
void make_secret_key(uint8_t secret[32]) {
  arc4random_buf(secret, 32);
  secret[0] &= 248;
  secret[31] &= 127;
  secret[31] |= 64;
}

TEST(hacl, test) {
  uint8_t secret[32] = {0};
  uint8_t basepoint[32] = {0};
  for (int i = 0; i < 100; ++i) {
    uint8_t public[32];
    curve25519(public, secret, basepoint);
    uint8_t other[32];
    Hacl_Curve25519_scalarmult(other, secret, basepoint);
    ASSERT_EQ(0, memcmp(other, public, 32));
    make_secret_key(secret);
    arc4random_buf(basepoint, 32);
  }
}

uint8_t secret[32];
uint8_t basepoint[32];

void everest(void) {
  uint8_t public[32];
  curve25519(public, secret, basepoint);
}

void hacl51(void) {
  uint8_t other[32];
  Hacl_Curve25519_scalarmult(other, secret, basepoint);
}

BENCH(hacl, bench) {
  make_secret_key(secret);
  arc4random_buf(basepoint, 32);
  BENCHMARK(1000, 1, everest());
  BENCHMARK(30000, 1, hacl51());
}
