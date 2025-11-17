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
#include "libc/stdio/rand.h"
#include "libc/str/str.h"
#include "libc/testlib/benchmark.h"
#include "libc/testlib/hyperion.h"
#include "libc/testlib/testlib.h"
#include "third_party/haclstar/haclstar.h"

uint8_t private_key[32];
uint8_t public_key[32];
uint8_t expanded_key[96];
uint8_t signature[64];
uint8_t signature2[64];

TEST(ed25519, test) {
  getentropy(private_key, 32);
  Hacl_Ed25519_secret_to_public(public_key, private_key);
  Hacl_Ed25519_sign(signature, private_key, 1,
                    (Hacl_Slice[]){{kHyperionSize, kHyperion}});
  ASSERT_TRUE(Hacl_Ed25519_verify(
      public_key, 1, (Hacl_Slice[]){{kHyperionSize, kHyperion}}, signature));
}

void Ed25519DerivePublicKey(void) {
  Hacl_Ed25519_secret_to_public(public_key, private_key);
}

void Ed25519Sign(void) {
  Hacl_Ed25519_sign(signature, private_key, 1,
                    (Hacl_Slice[]){{kHyperionSize, kHyperion}});
}

void Ed25519Expand(void) {
  Hacl_Ed25519_expand_keys(expanded_key, private_key);
}

void Ed25519SignExpanded(void) {
  Hacl_Ed25519_sign_expanded(signature, expanded_key, 1,
                             (Hacl_Slice[]){{kHyperionSize, kHyperion}});
}

void Ed25519Verify(void) {
  ASSERT_TRUE(Hacl_Ed25519_verify(
      public_key, 1, (Hacl_Slice[]){{kHyperionSize, kHyperion}}, signature));
}

void Ed25519VerifyChainLink(void) {
  ASSERT_TRUE(Hacl_Ed25519_verify(
      public_key, 1, (Hacl_Slice[]){{32, public_key}}, signature2));
}

BENCH(ed25519, bench) {
  arc4random_buf(private_key, 32);
  Ed25519DerivePublicKey();
  Ed25519Sign();
  BENCHMARK(400, 1, Ed25519DerivePublicKey());
  BENCHMARK(400, 1, Ed25519Sign());
  BENCHMARK(400, 1, Ed25519Verify());
  BENCHMARK(400, 1, Ed25519Expand());
  BENCHMARK(400, 1, Ed25519SignExpanded());
  Hacl_Ed25519_sign(signature2, private_key, 1,
                    (Hacl_Slice[]){{32, public_key}});
  BENCHMARK(400, 1, Ed25519VerifyChainLink());
}
