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
#include "third_party/mbedtls/config.h"
#include "third_party/mbedtls/ssl_ciphersuites.h"

int GetCipherId(const char *s) {
  const mbedtls_ssl_ciphersuite_t *c;
  if ((c = GetCipherSuite(s))) {
    return c->id;
  } else {
    return -1;
  }
}

#ifdef MBEDTLS_CIPHER_MODE_CBC
TEST(GetCipherSuite, theOlde) {
  EXPECT_EQ(0x002F, GetCipherId("RSA-AES128-CBC-SHA"));            // Cosmo
  EXPECT_EQ(0x002F, GetCipherId("TLS_RSA_AES_128_CBC_SHA1"));      // GnuTLS
  EXPECT_EQ(0x002F, GetCipherId("TLS_RSA_WITH_AES_128_CBC_SHA"));  // IANA
  // EXPECT_EQ(0x002F, GetCipherId("AES128-SHA"));                 // OpenSSL
}
#endif

#ifdef MBEDTLS_DES_C
TEST(GetCipherSuite, theAncient) {
  EXPECT_EQ(0x000A, GetCipherId("RSA-3DES-EDE-CBC-SHA"));           // Cosmo
  EXPECT_EQ(0x000A, GetCipherId("TLS_RSA_3DES_EDE_CBC_SHA1"));      // GnuTLS
  EXPECT_EQ(0x000A, GetCipherId("TLS_RSA_WITH_3DES_EDE_CBC_SHA"));  // IANA
  // EXPECT_EQ(0x000A, GetCipherId("DES-CBC3-SHA"));
}
#endif

#ifdef MBEDTLS_KEY_EXCHANGE_ECDHE_ECDSA_ENABLED
TEST(GetCipherSuite, theUltimo) {
  EXPECT_EQ(0xC02C, GetCipherId("ECDHE-ECDSA-AES256-GCM-SHA384"));
  EXPECT_EQ(0xC02C, GetCipherId("ECDHE-ECDSA-WITH-AES-256-GCM-SHA384"));
  EXPECT_EQ(0xC02C, GetCipherId("TLS-ECDHE-ECDSA-WITH-AES-256-GCM-SHA384"));
  EXPECT_EQ(0xC02C, GetCipherId("TLS_ECDHE_ECDSA_WITH_AES_256_GCM_SHA384"));
}
#endif

#ifdef MBEDTLS_CHACHAPOLY_C
TEST(GetCipherSuite, arcfourReborn) {
  EXPECT_EQ(0xCCA8, GetCipherId("ECDHE-RSA-CHACHA20-POLY1305-SHA256"));
  EXPECT_EQ(0xCCA8, GetCipherId("TLS_ECDHE_RSA_WITH_CHACHA20_POLY1305_SHA256"));
  // EXPECT_EQ(0xCCA8, GetCipherId("TLS_ECDHE_RSA_CHACHA20_POLY1305"));
  // EXPECT_EQ(0xCCA8, GetCipherId("ECDHE-RSA-CHACHA20-POLY1305"));
}
#endif

TEST(GetCipherSuite, forTheeNotForMe) {
  EXPECT_EQ(0x0004, GetCipherId("RSA-RC4-128-MD5"));           // Cosmo
  EXPECT_EQ(0x0004, GetCipherId("TLS_RSA_WITH_RC4_128_MD5"));  // IANA
  // EXPECT_EQ(0x0004, GetCipherId("TLS_RSA_ARCFOUR_128_MD5"));
}
