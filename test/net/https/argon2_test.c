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
#include "third_party/argon2/argon2.h"
#include "libc/dce.h"
#include "libc/testlib/ezbench.h"
#include "libc/testlib/testlib.h"
#include "net/https/https.h"

TEST(argon2, testArgon2ReadmeExample) {
  uint8_t hash[24];
  uint8_t salt[8] = {'s', 'o', 'm', 'e', 's', 'a', 'l', 't'};
  uint8_t pass[8] = {'p', 'a', 's', 's', 'w', 'o', 'r', 'd'};
  uint32_t parallelism = 4;
  uint32_t memorycost = 1 << 16;
  uint32_t iterations = 2;
  argon2i_hash_raw(iterations, memorycost, parallelism, pass, sizeof(pass),
                   salt, sizeof(salt), hash, sizeof(hash));
  ASSERT_BINEQ("45d7ac72e76f242b20b77b9bf9bf9d5915894e669a24e6c6", hash);
}

TEST(argon2, testArgon2ReadmeExampleEncoded) {
  char hash[128];
  uint8_t salt[8] = {'s', 'o', 'm', 'e', 's', 'a', 'l', 't'};
  uint8_t pass[8] = {'p', 'a', 's', 's', 'w', 'o', 'r', 'd'};
  uint32_t parallelism = 4;
  uint32_t memorycost = 1 << 16;
  uint32_t iterations = 2;
  argon2i_hash_encoded(iterations, memorycost, parallelism, pass, sizeof(pass),
                       salt, sizeof(salt), 24, hash, sizeof(hash));
  ASSERT_STREQ("$argon2i$v=19$m=65536,t=2,p=4$c29tZXNhbHQ$RdescudvJCsgt3ub+b+"
               "dWRWJTmaaJObG",
               hash);
}

TEST(argon2i, testOnlineGeneratorExample) {
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

TEST(argon2id, testOnlineGeneratorExample) {
  /* consistent with https://argon2.online/ */
  uint8_t hash[32];
  uint8_t salt[8] = {'s', 'a', 'l', 't', 's', 'a', 'l', 't'};
  uint8_t pass[8] = {'p', 'a', 's', 's', 'w', 'o', 'r', 'd'};
  uint32_t parallelism = 1;
  uint32_t memorycost = 65536;
  uint32_t iterations = 2;
  argon2id_hash_raw(iterations, memorycost, parallelism, pass, sizeof(pass),
                    salt, sizeof(salt), hash, sizeof(hash));
  ASSERT_BINEQ(
      "360da2d90fd9d6f52923f293d142131a13909b780698daf09e6756422ebd1045", hash);
}

void BenchmarkArgon2(void) {
  uint8_t hash[24];
  uint8_t salt[8] = {'s', 'o', 'm', 'e', 's', 'a', 'l', 't'};
  uint8_t pass[8] = {'p', 'a', 's', 's', 'w', 'o', 'r', 'd'};
  uint32_t parallelism = 4;
  uint32_t memorycost = (1 << 16) / 8;
  uint32_t iterations = 2;
  argon2i_hash_raw(iterations, memorycost, parallelism, pass, sizeof(pass),
                   salt, sizeof(salt), hash, sizeof(hash));
}

BENCH(argon2, bench) {
  EZBENCH2("argon2", donothing, BenchmarkArgon2());
}
