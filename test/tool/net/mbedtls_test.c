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
#include "libc/testlib/hyperion.h"
#include "libc/testlib/testlib.h"
#include "third_party/mbedtls/aes.h"
#include "third_party/mbedtls/base64.h"
#include "third_party/mbedtls/bignum.h"
#include "third_party/mbedtls/chacha20.h"
#include "third_party/mbedtls/chachapoly.h"
#include "third_party/mbedtls/ctr_drbg.h"
#include "third_party/mbedtls/des.h"
#include "third_party/mbedtls/dhm.h"
#include "third_party/mbedtls/ecp.h"
#include "third_party/mbedtls/entropy.h"
#include "third_party/mbedtls/gcm.h"
#include "third_party/mbedtls/hmac_drbg.h"
#include "third_party/mbedtls/md5.h"
#include "third_party/mbedtls/memory_buffer_alloc.h"
#include "third_party/mbedtls/nist_kw.h"
#include "third_party/mbedtls/pkcs5.h"
#include "third_party/mbedtls/poly1305.h"
#include "third_party/mbedtls/rsa.h"
#include "third_party/mbedtls/sha1.h"
#include "third_party/mbedtls/sha256.h"
#include "third_party/mbedtls/sha512.h"
#include "third_party/mbedtls/x509.h"

#ifdef MBEDTLS_SELF_TEST
TEST(mbedtls, selfTest) {
#ifdef MBEDTLS_DES_C
  EXPECT_EQ(0, mbedtls_des_self_test(0));
#endif
#ifdef MBEDTLS_CTR_DRBG_C
  EXPECT_EQ(0, mbedtls_ctr_drbg_self_test(0));
#endif
#ifdef MBEDTLS_HMAC_DRBG_C
  EXPECT_EQ(0, mbedtls_hmac_drbg_self_test(0));
#endif
#ifdef MBEDTLS_ENTROPY_C
  EXPECT_EQ(0, mbedtls_entropy_self_test(0));
#endif
#ifdef MBEDTLS_POLY1305_C
  EXPECT_EQ(0, mbedtls_poly1305_self_test(0));
#endif
#ifdef MBEDTLS_RSA_C
  EXPECT_EQ(0, mbedtls_rsa_self_test(0));
#endif
#ifdef MBEDTLS_BIGNUM_C
  EXPECT_EQ(0, mbedtls_mpi_self_test(0));
#endif
#ifdef MBEDTLS_PKCS5_C
  EXPECT_EQ(0, mbedtls_pkcs5_self_test(0));
#endif
#ifdef MBEDTLS_DHM_C
  EXPECT_EQ(0, mbedtls_dhm_self_test(0));
#endif
#ifdef MBEDTLS_GCM_C
  EXPECT_EQ(0, mbedtls_gcm_self_test(0));
#endif
#ifdef MBEDTLS_X509_USE_C
  EXPECT_EQ(0, mbedtls_x509_self_test(0));
#endif
#ifdef MBEDTLS_CHACHA20_C
  EXPECT_EQ(0, mbedtls_chacha20_self_test(0));
#endif
#ifdef MBEDTLS_CHACHAPOLY_C
  EXPECT_EQ(0, mbedtls_chachapoly_self_test(0));
#endif
#ifdef MBEDTLS_MEMORY_BUFFER_ALLOC_C
  EXPECT_EQ(0, mbedtls_memory_buffer_alloc_self_test(0));
#endif
#ifdef MBEDTLS_AES_C
  EXPECT_EQ(0, mbedtls_aes_self_test(0));
#endif
#ifdef MBEDTLS_BASE64_C
  EXPECT_EQ(0, mbedtls_base64_self_test(0));
#endif
#ifdef MBEDTLS_ECP_C
  EXPECT_EQ(0, mbedtls_ecp_self_test(0));
#endif
#ifdef MBEDTLS_MD5_C
  EXPECT_EQ(0, mbedtls_md5_self_test(0));
#endif
#ifdef MBEDTLS_SHA1_C
  EXPECT_EQ(0, mbedtls_sha1_self_test(0));
#endif
#ifdef MBEDTLS_SHA256_C
  EXPECT_EQ(0, mbedtls_sha256_self_test(0));
#endif
#ifdef MBEDTLS_SHA512_C
  EXPECT_EQ(0, mbedtls_sha512_self_test(0));
#endif
#ifdef MBEDTLS_NIST_KW_C
  EXPECT_EQ(0, mbedtls_nist_kw_self_test(0));
#endif
}
#endif /* MBEDTLS_SELF_TEST */

TEST(md5, test) {
  uint8_t d[16];
  uint8_t want[16] = {0x90, 0x01, 0x50, 0x98, 0x3C, 0xD2, 0x4F, 0xB0,
                      0xD6, 0x96, 0x3F, 0x7D, 0x28, 0xE1, 0x7F, 0x72};
  mbedtls_md5_ret("abc", 3, d);
  EXPECT_EQ(0, memcmp(want, d, 16));
}

TEST(sha1, test) {
  uint8_t d[20];
  uint8_t want[20] = {0xA9, 0x99, 0x3E, 0x36, 0x47, 0x06, 0x81,
                      0x6A, 0xBA, 0x3E, 0x25, 0x71, 0x78, 0x50,
                      0xC2, 0x6C, 0x9C, 0xD0, 0xD8, 0x9D};
  mbedtls_sha1_ret("abc", 3, d);
  EXPECT_EQ(0, memcmp(want, d, 20));
}

TEST(sha224, test) {
  uint8_t d[28];
  uint8_t want[28] = {0x23, 0x09, 0x7D, 0x22, 0x34, 0x05, 0xD8,
                      0x22, 0x86, 0x42, 0xA4, 0x77, 0xBD, 0xA2,
                      0x55, 0xB3, 0x2A, 0xAD, 0xBC, 0xE4, 0xBD,
                      0xA0, 0xB3, 0xF7, 0xE3, 0x6C, 0x9D, 0xA7};
  mbedtls_sha256_ret("abc", 3, d, 1);
  EXPECT_EQ(0, memcmp(want, d, 28));
}

TEST(sha256, test) {
  uint8_t d[32];
  uint8_t want[32] = {0xBA, 0x78, 0x16, 0xBF, 0x8F, 0x01, 0xCF, 0xEA,
                      0x41, 0x41, 0x40, 0xDE, 0x5D, 0xAE, 0x22, 0x23,
                      0xB0, 0x03, 0x61, 0xA3, 0x96, 0x17, 0x7A, 0x9C,
                      0xB4, 0x10, 0xFF, 0x61, 0xF2, 0x00, 0x15, 0xAD};
  mbedtls_sha256_ret("abc", 3, d, 0);
  EXPECT_EQ(0, memcmp(want, d, 32));
}

TEST(sha384, test) {
  uint8_t d[48];
  uint8_t want[48] = {
      0xCB, 0x00, 0x75, 0x3F, 0x45, 0xA3, 0x5E, 0x8B, 0xB5, 0xA0, 0x3D, 0x69,
      0x9A, 0xC6, 0x50, 0x07, 0x27, 0x2C, 0x32, 0xAB, 0x0E, 0xDE, 0xD1, 0x63,
      0x1A, 0x8B, 0x60, 0x5A, 0x43, 0xFF, 0x5B, 0xED, 0x80, 0x86, 0x07, 0x2B,
      0xA1, 0xE7, 0xCC, 0x23, 0x58, 0xBA, 0xEC, 0xA1, 0x34, 0xC8, 0x25, 0xA7};
  mbedtls_sha512_ret("abc", 3, d, 1);
  EXPECT_EQ(0, memcmp(want, d, 48));
}

TEST(sha512, test) {
  uint8_t d[64];
  uint8_t want[64] = {
      0xDD, 0xAF, 0x35, 0xA1, 0x93, 0x61, 0x7A, 0xBA, 0xCC, 0x41, 0x73,
      0x49, 0xAE, 0x20, 0x41, 0x31, 0x12, 0xE6, 0xFA, 0x4E, 0x89, 0xA9,
      0x7E, 0xA2, 0x0A, 0x9E, 0xEE, 0xE6, 0x4B, 0x55, 0xD3, 0x9A, 0x21,
      0x92, 0x99, 0x2A, 0x27, 0x4F, 0xC1, 0xA8, 0x36, 0xBA, 0x3C, 0x23,
      0xA3, 0xFE, 0xEB, 0xBD, 0x45, 0x4D, 0x44, 0x23, 0x64, 0x3C, 0xE8,
      0x0E, 0x2A, 0x9A, 0xC9, 0x4F, 0xA5, 0x4C, 0xA4, 0x9F};
  mbedtls_sha512_ret("abc", 3, d, 0);
  EXPECT_EQ(0, memcmp(want, d, 64));
}

BENCH(mbedtls, bench) {
  uint8_t d[64];
  EZBENCH2("md5", donothing, mbedtls_md5_ret(kHyperion, kHyperionSize, d));
  EZBENCH2("sha1", donothing, mbedtls_sha1_ret(kHyperion, kHyperionSize, d));
  EZBENCH2("sha256", donothing,
           mbedtls_sha256_ret(kHyperion, kHyperionSize, d, 0));
  EZBENCH2("sha384", donothing,
           mbedtls_sha512_ret(kHyperion, kHyperionSize, d, 1));
  EZBENCH2("sha512", donothing,
           mbedtls_sha512_ret(kHyperion, kHyperionSize, d, 0));
}
