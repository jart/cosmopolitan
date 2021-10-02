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
#include "libc/testlib/ezbench.h"
#include "libc/testlib/testlib.h"
#include "net/https/https.h"
#include "third_party/argon2/argon2.h"

TEST(argon2, test) {
  /* consistent with https://argon2.online/ */
  uint8_t hash[32];
  uint8_t salt[8] = {'s', 'a', 'l', 't', 's', 'a', 'l', 't'};
  uint8_t pass[8] = {'p', 'a', 's', 's', 'w', 'o', 'r', 'd'};
  uint32_t parallelism = 1;
  uint32_t memorycost = 65536;
  uint32_t iterations = 2;
  argon2i_hash_raw(iterations, memorycost, parallelism, pass, sizeof(pass),
                   salt, sizeof(salt), hash, sizeof(hash));
  ASSERT_BINEQ(
      "b5d0818d1c0c05684e68aef02de8b4ed14bfe6c60800e0aa36a5c18f846a297b", hash);
}

void specimen(void) {
  uint8_t hash[32];
  uint8_t salt[8] = {'s', 'a', 'l', 't', 's', 'a', 'l', 't'};
  uint8_t pass[8] = {'p', 'a', 's', 's', 'w', 'o', 'r', 'd'};
  uint32_t parallelism = 1;
  uint32_t memorycost = 65536 / 4;
  uint32_t iterations = 2;
  argon2i_hash_raw(iterations, memorycost, parallelism, pass, sizeof(pass),
                   salt, sizeof(salt), hash, sizeof(hash));
}

BENCH(argon2, bench) {
  EZBENCH2("argon2", donothing, specimen());
}
