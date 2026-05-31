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
#include "libc/testlib/testlib.h"
#include "third_party/mbedtls4/include/mbedtls/ssl_ciphersuites.h"

static int GetCipherId(const char *name) {
  const mbedtls_ssl_ciphersuite_t *c = mbedtls_ssl_ciphersuite_from_string(name);
  return c ? mbedtls_ssl_ciphersuite_get_id(c) : -1;
}

#ifdef MBEDTLS_KEY_EXCHANGE_ECDHE_ECDSA_ENABLED
TEST(GetCipherSuite, ecdhEcdsa) {
  EXPECT_EQ(0xC02C, GetCipherId("TLS-ECDHE-ECDSA-WITH-AES-256-GCM-SHA384"));
  EXPECT_EQ(0xC02B, GetCipherId("TLS-ECDHE-ECDSA-WITH-AES-128-GCM-SHA256"));
}
#endif

#ifdef MBEDTLS_CHACHAPOLY_C
TEST(GetCipherSuite, chacha20poly1305) {
  EXPECT_EQ(0xCCA8, GetCipherId("TLS-ECDHE-RSA-WITH-CHACHA20-POLY1305-SHA256"));
  EXPECT_EQ(0xCCA9, GetCipherId("TLS-ECDHE-ECDSA-WITH-CHACHA20-POLY1305-SHA256"));
}
#endif

TEST(GetCipherSuite, unknownReturnsNull) {
  /* Suites removed in mbedTLS 4 (RSA key exchange, RC4, 3DES) */
  EXPECT_EQ(-1, GetCipherId("TLS-RSA-WITH-AES-128-CBC-SHA"));
  EXPECT_EQ(-1, GetCipherId("TLS-RSA-WITH-RC4-128-MD5"));
  EXPECT_EQ(-1, GetCipherId("TLS-RSA-WITH-3DES-EDE-CBC-SHA"));
  EXPECT_EQ(-1, GetCipherId("unknown-nonsense"));
}
