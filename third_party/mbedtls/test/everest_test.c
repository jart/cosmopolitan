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
#include "third_party/mbedtls/config.h"
#include "third_party/mbedtls/endian.h"

void Hacl_Curve25519_crypto_scalarmult(uint8_t *, uint8_t *, uint8_t *);
void curve25519(uint8_t[32], uint8_t[32], uint8_t[32]);

const uint64_t kNumbers[] = {
    0x0000000000000000,  //
    0x0000000000000001,  //
    0x0000000000001000,  //
    0x0000000002000000,  //
    0x0000004000000000,  //
    0x0008000000000000,  //
    0x8000000000000000,  //
    0x0007ffffffffffff,  //
    0x0000003fffffffff,  //
    0x0000000001ffffff,  //
    0x0000000000000fff,  //
    0xffffffffffffffff,  //
    0xfff8000000000000,  //
};

TEST(everest, tinierVersionBehavesTheSame) {
  size_t i;
  uint8_t secret[32], bpoint[32], public[2][32];
  for (i = 0; i < 500; ++i) {
    rngset(secret, sizeof(secret), _rand64, -1);
    rngset(bpoint, sizeof(bpoint), _rand64, -1);
    Hacl_Curve25519_crypto_scalarmult(public[0], secret, bpoint);
    curve25519(public[1], secret, bpoint);
    ASSERT_EQ(0, memcmp(public[0], public[1], sizeof(public[0])));
  }
  for (i = 0; i < 500; ++i) {
    Write64le(secret + 000, kNumbers[rand() % ARRAYLEN(kNumbers)]);
    Write64le(secret + 010, kNumbers[rand() % ARRAYLEN(kNumbers)]);
    Write64le(secret + 020, kNumbers[rand() % ARRAYLEN(kNumbers)]);
    Write64le(secret + 030, kNumbers[rand() % ARRAYLEN(kNumbers)]);
    Write64le(bpoint + 000, kNumbers[rand() % ARRAYLEN(kNumbers)]);
    Write64le(bpoint + 010, kNumbers[rand() % ARRAYLEN(kNumbers)]);
    Write64le(bpoint + 020, kNumbers[rand() % ARRAYLEN(kNumbers)]);
    Write64le(bpoint + 030, kNumbers[rand() % ARRAYLEN(kNumbers)]);
    Hacl_Curve25519_crypto_scalarmult(public[0], secret, bpoint);
    curve25519(public[1], secret, bpoint);
    ASSERT_EQ(0, memcmp(public[0], public[1], sizeof(public[0])));
  }
}

BENCH(everest, bench) {
  uint8_t secret[32], bpoint[32], public[32];
  rngset(secret, sizeof(secret), _rand64, -1);
  rngset(bpoint, sizeof(bpoint), _rand64, -1);
  EZBENCH2("everest", donothing,
           Hacl_Curve25519_crypto_scalarmult(public, secret, bpoint));
  EZBENCH2("mariana", donothing, curve25519(public, secret, bpoint));
}
