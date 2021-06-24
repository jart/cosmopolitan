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

TEST(mbedtls, test) {
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
